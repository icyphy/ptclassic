static const char file_id[] = "DEScheduler.cc";

// FIXME put the copyright back

#ifdef __GNUG__
#pragma implementation
#endif

#include "type.h"
#include "DEScheduler.h"
#include "StringList.h"
#include "FloatState.h"
#include "GalIter.h"
#include "IntState.h"

extern const char DEdomainName[];

/*******************************************************************
		Main DE scheduler routines
*******************************************************************/


	////////////////////////////
	// displaySchedule
	////////////////////////////

StringList DEScheduler :: displaySchedule () {
	return "DE schedule is computed at run-time\n";
}


extern int warnIfNotConnected (Galaxy&);

	////////////////////////////
	// setup
	////////////////////////////

void DEScheduler :: setup () {
	clearHalt();
	currentTime = 0;

	if (!galaxy()) {
		Error::abortRun("DEScheduler: no galaxy!");
		return;
	}
	GalStarIter next(*galaxy());

	// initialize the global event queue and process queue.
	eventQ.initialize();

	// check connectivity
	if (warnIfNotConnected (*galaxy())) return;

	// Notify each star of the global event queue, 
	Star* s;
	while ((s = next++) != 0) {
		if (strcmp (s->domain(), DEdomainName) != 0) {
			Error::abortRun (*s, " is not a DE star");
			return;
		}
		// set up the block event queue.
		DEStar* p = (DEStar*) s;
		p->eventQ = &eventQ;

	}

	galaxy()->initialize();
	
	// Fire source stars to initialize the global event queue.
	initialFire();

	if (!checkDelayFreeLoop() || !computeDepth()) return;

	if (!relTimeScale) {
		Error::abortRun(*galaxy(),
				": zero timeScale is not allowed in DE.");
	}
}

//  If output events are generated during the "start" phase, send them
//  to the global event queue.
void DEScheduler :: initialFire() {
	GalStarIter nextStar(*galaxy());
	DEStar* s;
	while ((s = (DEStar*) nextStar++) != 0 )
		s->sendOutput();
}

// detect the delay free loop.
int DEScheduler :: checkDelayFreeLoop() {
	GalStarIter next(*galaxy());
	DEStar* s;
	while ((s = (DEStar*) next++) != 0) {
		BlockPortIter nextp(*s);
		for (int k = s->numberPorts(); k > 0; k--) {
			if(!checkLoop(nextp++, s)) return FALSE;
		}
	}
	return TRUE;
}

// set the depth of the stars...
int DEScheduler :: computeDepth() {
	GalStarIter next(*galaxy());
	DEStar* s;
	while ((s = (DEStar*) next++) != 0) {
		BlockPortIter nextp(*s);
		for (int k = s->numberPorts(); k > 0; k--) {
			PortHole* p = nextp++;
			if (p->isItInput())
			   if(setDepth(p, s) >= 0) {
				Error::abortRun(*p, "has an undefined depth");
				return FALSE;
			   }
		}
	}
	return TRUE;
}

	////////////////////////////
	// run
	////////////////////////////

// pop the top element (earliest event) from the global queue
// and fire the destination star. Check all simultaneous events to the star.
// Run until StopTime.
// EWK this is where the biggest changes are made

int DEScheduler :: run () {

// FIXME we should be able to insert the destination stuff
// during put so that we don't have to search for it here.

fprintf(stderr,"DESC: started DEScheduler::run\n");
    if (haltRequested()) {
	    Error::abortRun(*galaxy(),
			    "Can't continue after run-time error");
	    return FALSE;
    }

    while (TRUE) {

	int bFlag = FALSE;  // flag = TRUE when the terminal is on the
			    // boundary of a wormhole of DE domain.

	// fetch the earliest event.
	CqLevelLink* f   = eventQ.get();
fprintf(stderr,"DESC: got event at top of outer loop\n");
	if (f == NULL) {
fprintf(stderr,"DESC: event is null\n");
	    break;
	}
	double level    = f->level;

	// if level > stopTime, RETURN...
	if (level > stopTime)	{
fprintf(stderr,"DESC: have to push it back\n");
		eventQ.pushBack(f);		// push back
fprintf(stderr,"DESC: pushed it back\n");
		// set currentTime = next event time.
		currentTime = level/relTimeScale;
		stopBeforeDeadFlag = TRUE;  // there is extra events.
		return TRUE;
	// If the event time is less than the global clock,
	// it is an error...
	} else if (level < currentTime) {
	    // The event is in the past!  Argh!
	    // Try to give a good error message.
		Event* ent = (Event*) f->e;
		PortHole* src = ent->dest->far();
		Error::abortRun(*src, ": event from this porthole",
				" is in the past!");
		return FALSE;
	}


	// set currentTime
	currentTime = level;

	// check if the normal event is fetched
	//
	// For normal events, the fineLevel of the CqLevelLink
	// class is negative (which is the minus of the depth of
	// the destination star)
	// But, we also put the process-star in the event queue
	// with zeroed fineLevel --> which will put the process-stars
	// at the end among simultaneous events as a side-effect.
	DEStar* ds;
	Star* s;
	PortHole* terminal = 0;
	if (f->fineLevel != 0) {
fprintf(stderr,"DESC: Doing stuff for fineLevel != 0\n");
		Event* ent = (Event*) f->e;
		terminal = ent->dest;
		s = &terminal->parent()->asStar();
		if (terminal->isItOutput()) {
			bFlag = TRUE;
		} else {
			ds = (DEStar*) s;
			ds->arrivalTime = level;

			// start a new phase.
			ds->startNewPhase();
		}

		// Grab the Particle from the queue to the terminal.
		// Record the arrival time, and flag existence of data.
		InDEPort* inp = (InDEPort*) terminal;
		inp->getFromQueue(ent->p);
	} else {
fprintf(stderr,"DESC: Doing stuff for fineLevel == 0\n");
	// process star is fetched
		ds = (DEStar*) f->e;
		ds->arrivalTime = level;
		s = ds;

		// start a new phase.
		ds->startNewPhase();
	}
	// put the LinkList into the free pool for memory management.

fprintf(stderr,"DESC: called putFreeLink\n");
	eventQ.putFreeLink(f);
			
	// Check if there is another event launching onto the
	// same star with the same time stamp (not the same port)...

	Star* dest;
	while (TRUE) {
fprintf(stderr,"DESC: Attempting another get\n");
            CqLevelLink *h = eventQ.get();
fprintf(stderr,"DESC: got event at top of inner loop\n");
	    if (h == NULL) {
fprintf(stderr,"DESC: null event, break\n");
		break;
	    }
	    if (h->level > level) {
fprintf(stderr,"DESC: event in future, break\n");
		break;
	    }
	    PortHole* tl = 0;
	    Event* ee = 0; 
// FIXME we shouldn't have to do this if we already sorted on dest
	    if (h->fineLevel != 0) {
		    ee = (Event*) h->e;
		    tl = ee->dest;
		    dest = &tl->parent()->asStar();
	    } else {
		    dest = (Star*) h->e;
	    }

	    // if same destination star with same time stamp..
// We don't need to extract since our get does an extract
fprintf(stderr,"DESC: check dest in inner loop\n");
	    if (dest == s) {
		if (tl) {
		     int success =
			((InDEPort*) tl)->getFromQueue(ee->p);
		     if (success) eventQ.putFreeLink(h);
		     else eventQ.pushBack(h);
		} else if (!tl) {
 		     eventQ.putFreeLink(h);
		} 
	    } else {
		// Added this, need to put back since we did a get
		eventQ.pushBack(h);
		// In the calendar queue, once you get a "bad"
		// event, you're done, don't need to go through
		// them all
		break;
	    }

        }   // while TRUE

// fire the star.
	if (!bFlag) {
fprintf(stderr,"DESC: call run\n");
	    if (!s->run()) return FALSE;
fprintf(stderr,"DESC: called run\n");
        }

    } // end of while

if (haltRequested()) return FALSE;

stopBeforeDeadFlag = FALSE;	// yes, no more events...
fprintf(stderr,"end of run()\n");
return TRUE;
}


////////////////////////////
// Miscellanies
////////////////////////////

// fetch an event on request.
int DEScheduler :: fetchEvent(InDEPort* p, double timeVal) {
return eventQ.fetchEvent(p, timeVal);
}

// detect any delay-free loop which may potentially generate an
// infinite loop at runtime.  If there is no delay-free loop, this
// routine automatically initialize the "depth" member of the portholes
// for later use from "setDepth()" method.
// By putting a initial delay on a feedback arc, we can intentionally
// create a delay-free loop: the programmer is in charge of the
// possibilities of an infinite loop.

int DEScheduler :: checkLoop(PortHole* p, DEStar* s) {

DEPortHole* dp = (DEPortHole*) p;

if (!dp->depth || dp->depth == MINDEPTH) 	return TRUE;
else if (dp->depth < 0) {

if (dp->isItInput()) {
    InDEPort* inp = (InDEPort*) dp;

    // If it is delay-type, initialize depth.
    if (s->delayType == TRUE) {
	    inp->depth = 0;
    } else {

	    // Indicate that this port is under consideration.
	    // By depth > 0, can detect the delay-free loop
	    inp->depth = 1;

	    // Scan through all output connections. 
	    if (inp->complete) {
		    BlockPortIter nextp(*s);
		    for (int i = s->numberPorts(); i > 0; i--) {
			    PortHole* port = nextp++;
			    if (port->isItOutput()) 
				    if(!checkLoop(port, s))
					    return FALSE;
		    }
	    } else if (inp->triggerList) {
		    ListIter nextp(*(inp->triggerList));
		    GenericPort* port;
		    while ((port = (GenericPort*)nextp++)) {
			if (port->isItMulti()) {
			    MultiPortHole* pp = 
				    (MultiPortHole*) port;
			    MPHIter mp(*pp);
			    PortHole* tp;
			    while((tp = mp++)) {
			      if(!checkLoop(tp, s)) return FALSE;
			    }
			} else {
			    PortHole* pp = (PortHole*) port;
			    if(!checkLoop(pp, s)) return FALSE;
			}
		    }
	    }
	    dp->depth = 0;
    }
// 2. If it is an output, look at the connected input.
} else if (!s->delayType) {
    if (dp->numTokens()) dp->depth = 0;
    else if (dp->far()->isItInput()) {
	    dp->depth = 1;
	    if(!checkLoop(dp->far(),(DEStar*) dp->far()->parent()))
		    return FALSE;
	    dp->depth = 0;
    }
} else dp->depth = 0;
}
// If depth >0, detect the delay-free loop.
else return errorDelayFree(p);
return TRUE;
}

// calculate the depth of portholes of DEStars.
// The depth of a porthole is set -1 if it is a porthole of a delay star.
// By putting a initial delay on a feedback arc, we can avoid
// a non-deterministic loop.

int DEScheduler :: setDepth(PortHole* p, DEStar* s) {

InDEPort* inp = (InDEPort*) p;

if (inp->depth < 0) return inp->depth;
else if (inp->depth == 2) { errorUndefined(inp); return -1; }
else {
    // If it is delay-type, set depth = -1;
    if (s->delayType == TRUE) {
	    inp->depth = -1;
    } else {
	    // Scan through all output connections. 
	    // Get the minimum depth to set its depth.
	    int min = -1;

	    // If "beforeP" member is set, the minimum depth
	    // is determined by the depth of "beforeP".
	    if (inp->beforeP) {
		    inp->depth = 2;	// temporal setting.
		    PortHole* bp;
		    if (inp->beforeP->isItMulti()) {
		       MultiPortHole* pp = 
			    (MultiPortHole*) inp->beforeP;
		       MPHIter mp(*pp);
		       bp = mp++;
		    } else { bp = (PortHole*) inp->beforeP; }
		    min = setDepth(bp, s);
	    }
		       
	    // if it is complete-type, meaning that this input
	    // triggers all outputs.
	    if (inp->complete) {
		    BlockPortIter nextp(*s);
		    for (int i = s->numberPorts(); i > 0; i--) {
			    PortHole* port = nextp++;
			    int val;
			    if (port->isItOutput() &&
				!port->numTokens() &&
				port->far()->isItInput()) {
				val = setDepth(port->far(),
(DEStar*) port->far()->parent()) - 1;
				if (min >= val) min = val;
			    }
		    }
	    // else, it scans through the "triggerList".
	    } else if (inp->triggerList) {
		    ListIter nextp(*(inp->triggerList));
		    GenericPort* port;
		    while ((port = (GenericPort*)nextp++)) {
			if (port->isItMulti()) {
			    MultiPortHole* pp = 
				    (MultiPortHole*) port;
			    MPHIter mp(*pp);
			    PortHole* tp;
			    while((tp = mp++)) {
				if (!tp->numTokens() &&
				    tp->far()->isItInput()) {
				    int val = setDepth(tp->far(),
(DEStar*) tp->far()->parent()) - 1;
				    if (min >= val) min = val;
				}
			    }
			} else {
			    PortHole* pp = (PortHole*) port;
			    if (!pp->numTokens() &&
				    pp->far()->isItInput()) {
			       int val = setDepth(pp->far(),
(DEStar*) pp->far()->parent()) - 1;
			       if (min >= val) min = val;
			    }
			}
		    }
	    }
	    inp->depth = min;
    }
}
return inp->depth;
}

int DEScheduler :: errorDelayFree(PortHole* p) {
StringList msg;
msg += p->fullName();
msg += " lies on a delay-free loop";
Error::mark(*p);
Error::abortRun(msg);
return FALSE;
}

void DEScheduler :: errorUndefined(PortHole* p) {
StringList msg;
msg += p->fullName();
msg += " lies on a non-deterministic loop\n";
msg += "(execution order of simultaneous events is \
non-deterministic)\n";
msg += "Please put an infinitesimal delay on a feedback arc.\n";
msg += "Without the delay, sorry, the \
correct simulation may not be guaranteed.";
Error::warn(msg);
}

// my domain
const char* DEScheduler :: domain() const { return DEdomainName ;}
