static const char file_id[] = "DEScheduler.cc";
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 5/30/90
 Revisions:
	6/20/90 -- no longer call initialize() and start() on each
	Star.  Galaxy::initialize does this now.

These are the methods for the discrete event scheduler.

**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "type.h"
#include "DEScheduler.h"
#include "StringList.h"
#include "FloatState.h"
#include "GalIter.h"
#include "DEWormConnect.h"
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

int DEScheduler :: setup (Galaxy& galaxy) {
	clearHalt();
	currentTime = 0;

	GalStarIter next(galaxy);

	// initialize the global event queue and process queue.
	eventQ.initialize();

	// check connectivity
	if (warnIfNotConnected (galaxy)) return FALSE;

	// Notify each star of the global event queue, 
	Star* s;
	while ((s = next++) != 0) {
		if (strcmp (s->domain(), DEdomainName) != 0) {
			Error::abortRun (*s, " is not a DE star");
			return FALSE;
		}
		// set up the block event queue.
		DEStar* p = (DEStar*) s;
		p->eventQ = &eventQ;

		// reset data members
		p->prepareForScheduling();
	}

	galaxy.initialize();
	
	// Fire source stars to initialize the global event queue.
	initialFire(galaxy);

	if (!checkDelayFreeLoop(galaxy)) return FALSE;

	if (!computeDepth(galaxy)) return FALSE;

	if (!relTimeScale) {
		Error::abortRun(galaxy,
				": zero timeScale is not allowed in DE.");
		return FALSE;
	}

	return !haltRequested();
}

//  If output events are generated during the "start" phase, send them
//  to the global event queue.
void DEScheduler :: initialFire(Galaxy& g) {
	GalStarIter nextStar(g);
	DEStar* s;
	while ((s = (DEStar*) nextStar++) != 0 )
		s->sendOutput();
}

// detect the delay free loop.
int DEScheduler :: checkDelayFreeLoop(Galaxy& g) {
	GalStarIter next(g);
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
int DEScheduler :: computeDepth(Galaxy& g) {
	GalStarIter next(g);
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
int
DEScheduler :: run (Galaxy& g) {


	if (haltRequested()) {
		Error::abortRun(g, "Can't continue after run-time error");
		return FALSE;
	}

	while (eventQ.length() > 0) {

		int bFlag = FALSE;  // flag = TRUE when the terminal is on the
		   		    // boundary of a wormhole of DE domain.

		// fetch the earliest event.
		LevelLink* f   = eventQ.get();
		float level    = f->level;

		// if level > stopTime, RETURN...
		if (level > stopTime)	{
			eventQ.pushBack(f);		// push back
			// set currentTime = next event time.
			currentTime = level/relTimeScale;
			stopBeforeDeadlocked = TRUE;  // there is extra events.
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
		// For normal events, the fineLevel of the LevelLink
		// class is negative (which is the minus of the depth of
		// the destination star)
		// But, we also put the process-star in the event queue
		// with zeroed fineLevel --> which will put the process-stars
		// at the end among simultaneous events as a side-effect.
		DEStar* ds;
		Star* s;
		PortHole* terminal = 0;
		if (f->fineLevel != 0) {
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
			if (!s->isItWormhole()) {
				InDEPort* inp = (InDEPort*) terminal;
				inp->getFromQueue(ent->p);
			} else {
				DEtoUniversal* ep = (DEtoUniversal*) terminal;
				ep->fetchData(ent->p);
			}
		} else {
		// process star is fetched
			ds = (DEStar*) f->e;
			ds->arrivalTime = level;
			s = ds;

			// start a new phase.
			ds->startNewPhase();
		}
		// put the LinkList into the free pool for memory management.
		eventQ.putFreeLink(f);
				
		// Check if there is another event launching onto the
		// same star with the same time stamp (not the same port)...
		int l;
		eventQ.reset();
		l = eventQ.length();
		Star* dest;
		while (l > 0) {
			l--;
			LevelLink* h = eventQ.next();
			if (h->level > level)	goto L1;
			PortHole* tl = 0;
			Event* ee = 0; 
			if (h->fineLevel != 0) {
				ee = (Event*) h->e;
				tl = ee->dest;
				dest = &tl->parent()->asStar();
			} else {
				dest = (Star*) h->e;
			}

			// if same destination star with same time stamp..
			if (dest == s) {
				if (tl) {
				     int success;
				     if (dest->isItWormhole()) {
success = ((DEtoUniversal*) tl)->fetchData(ee->p);
				     } else {
success = ((InDEPort*) tl)->getFromQueue(ee->p);
				     }
				     if (success) eventQ.extract(h);
				} else if (!tl) {
				     eventQ.extract(h);
				} 
			} 
		} // end of inside while

 		// fire the star.
 L1 :		if (!bFlag) {
	 		if (!s->fire()) return FALSE;
		}

	} // end of while

	if (haltRequested()) return FALSE;

	stopBeforeDeadlocked = FALSE;	// yes, no more events...
	return TRUE;
}


	////////////////////////////
	// Miscellanies
	////////////////////////////

// fetch an event on request.
int DEScheduler :: fetchEvent(InDEPort* p, float timeVal) {

	eventQ.reset();
	int l = eventQ.length();
	while (l > 0) {
		l--;
		LevelLink* h = eventQ.next();
		if (h->level > timeVal)	{
			Error :: abortRun (*p, " has no more data.");
			return FALSE;
		}
		InDEPort* tl = 0;
		if (h->fineLevel != 0) {
			Event* ent = (Event*) h->e;
			tl = (InDEPort*) ent->dest;

			// if same destination star with same time stamp..
			if (tl == p) {
				if (tl->getFromQueue(ent->p)) eventQ.extract(h);
				return TRUE;
			}
		} 
	}
	Error :: abortRun (*p, " has no more data.");
	return FALSE;
}

// detect any delay-free loop which may potentially generate an
// infinite loop at runtime.  If there is no delay-free loop, this
// routine automatically initialize the "depth" member of the portholes
// for later use from "setDepth()" method.
// By putting a initial delay on a feedback arc, we can intentionally
// create a delay-free loop: the programmer is in charge of the
// possibilities of an infinite loop.

int DEScheduler :: checkLoop(PortHole* p, DEStar* s) {

   if (s->isItWormhole()) {
	if (p->isItInput()) {
		DEtoUniversal* toP = (DEtoUniversal*) p;

		if (!toP->depth)	return TRUE;
		else if (s->delayType) toP->depth = 0;
		else if (toP->depth < 0) {
			BlockPortIter nextp(*s);
			toP->depth = 1;
			for (int i = s->numberPorts(); i > 0; i--) {
				PortHole* port = nextp++;
		   		if (port->isItOutput()) 
					if(!checkLoop(port, s)) return FALSE;
			}
			toP->depth = 0;
		}
		// If depth >0, detect the delay-free loop.
		else if (toP->depth > 0)  return errorDelayFree(p);

	} else if (!(s->delayType)) {
		DEfromUniversal* fromP = (DEfromUniversal*) p;

		if (!fromP->depth) 	return TRUE;
		else if (fromP->depth < 0) {
			if (fromP->numTokens()) fromP->depth = 0;
			else if (fromP->far()->isItInput()) {
			   	fromP->depth = 1;
				if(!checkLoop(fromP->far(),
				   (DEStar*) fromP->far()->parent()))
					return FALSE;
			   	fromP->depth = 0;
			}
		}
		// If depth >0, detect the delay-free loop.
		else if (fromP->depth > 0) return errorDelayFree(p);
	}
   } else {

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
	else if (dp->depth > 0) return errorDelayFree(p);
   }
   return TRUE;
}

// calculate the depth of portholes of DEStars.
// The depth of a porthole is set -1 if it is a porthole of a delay star.
// By putting a initial delay on a feedback arc, we can avoid
// a non-deterministic loop.

int DEScheduler :: setDepth(PortHole* p, DEStar* s) {

   if (s->isItWormhole()) {
	DEtoUniversal* toP = (DEtoUniversal*) p;
	if (toP->depth < 0) return toP->depth;
	else if (s->delayType) toP->depth = -1;
	else {
		BlockPortIter nextp(*s);
		int min = -1;
		for (int i = s->numberPorts(); i > 0; i--) {
			PortHole* port = nextp++;

	   		int val;
	   		if (port->isItOutput()) {
			    if (!port->numTokens() 
				&& port->far()->isItInput()) {
				val = setDepth(port->far(),
(DEStar*) port->far()->parent()) - 1;
	   			if (min >= val) min = val;
			    }
			}
		}
		toP->depth = min;
	}
	return toP->depth;

   } else {

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
}

int DEScheduler :: errorDelayFree(PortHole* p) {
	StringList msg;
	msg += p->readFullName();
	msg += " lies on a delay-free loop";
	Error::mark(*p);
	Error::abortRun(msg);
	return FALSE;
}

void DEScheduler :: errorUndefined(PortHole* p) {
	StringList msg;
	msg += p->readFullName();
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
