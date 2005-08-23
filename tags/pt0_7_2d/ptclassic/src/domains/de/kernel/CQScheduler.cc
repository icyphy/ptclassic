/* 
@(#)CQScheduler.cc	1.27 04/19/99

Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY
*/
static const char file_id[] = "CQScheduler.cc";


#ifdef __GNUG__
#pragma implementation
#endif

#include "type.h"
#include "CQScheduler.h"
#include "StringList.h"
#include "FloatState.h"
#include "GalIter.h"
#include "IntState.h"
#include <assert.h>
#include "checkConnect.h"

extern const char DEdomainName[];

/*******************************************************************
		Main DE scheduler routines
*******************************************************************/


	////////////////////////////
	// displaySchedule
	////////////////////////////

StringList CQScheduler :: displaySchedule () {
    return "{ { scheduler \"Calendar queue run-time scheduler\" } }";
}

	////////////////////////////
	// setup
	////////////////////////////

void CQScheduler :: setup () {
	clearHalt();
	currentTime = 0;

	if (! galaxy()) {
		Error::abortRun("Calendar Queue scheduler has no galaxy defined");
		return;
	}

	// initialize the global event queue and process queue.
	eventQ.initialize();

	// check connectivity
	if (warnIfNotConnected(*galaxy())) return;

	galaxy()->initialize();
	if(SimControl::haltRequested() || !galaxy()) return;

	// Check connectivity again because the galaxy may have
	// changed after initialization due to e.g. hof stars
	// performing block replacement.
	if (warnIfNotConnected(*galaxy())) return;
	
	if (!checkDelayFreeLoop() || !computeDepth()) return;

	if (!relTimeScale) {
		Error::abortRun(*galaxy(),
				"zero timeScale is not allowed in DE.");
	}
}

	////////////////////////////
	// run
	////////////////////////////

// pop the top element (earliest event) from the global queue
// and fire the destination star. Check all simultaneous events to the star.
// Run until StopTime.

int CQScheduler :: run () {
#ifdef PT_SIP
    // wrapUpPML() is part of SiP.  If SiP is not present, then
    // we link with sipdummy.o
    extern void wrapUpPML(void);
#endif
    if (SimControl::haltRequested() || !galaxy()) {
        Error::abortRun("Calendar Queue scheduler has no galaxy to run");
        return FALSE;
    }

#ifdef PT_SIP
    wrapUpPML();
#endif
    while (TRUE) {

	int bFlag = FALSE;  // flag = TRUE when the terminal is on the
			    // boundary of a wormhole of DE domain.

	// fetch the earliest event.
	CqLevelLink* f   = eventQ.get();
	if (f == NULL) {
	    break;
	}
	double level    = f->level;

	// if level > stopTime, RETURN...
	if (level > stopTime)	{
		eventQ.pushBack(f);		// push back
		// set currentTime = next event time.
		currentTime = stopTime/relTimeScale;
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
		InDEPort* inp = (InDEPort*) terminal;
		inp->getFromQueue(ent->p);
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
        CqLevelLink *store = NULL;
	Star* dest;
	while (TRUE) {
            CqLevelLink *h = eventQ.get();
	    if (h == NULL) {
		break;
	    }
	    if (h->level > level) {
		eventQ.pushBack(h);
		break;
	    }
	    PortHole* tl = 0;
	    Event* ee = 0; 
	    if (h->fineLevel != 0) {
		    ee = (Event*) h->e;
		    tl = ee->dest;
		    dest = &tl->parent()->asStar();
	    } else {
		    dest = (Star*) h->e;
	    }

	    assert(dest == h->dest);
	    // if same destination star with same time stamp..
	    // We don't need to extract since our get does an extract
	    if (dest == s) {
		if (tl) {
		     int success =
			((InDEPort*) tl)->getFromQueue(ee->p);
		     if (success) eventQ.putFreeLink(h);
		     else {
			    h->next = store;
			    store = h;
		     }
		} else if (!tl) {
 		     eventQ.putFreeLink(h);
		} 
	    } else {
		// need to put back since we did a get
		eventQ.pushBack(h);
		// In the calendar queue, once you get a "bad"
		// event, you're done, don't need to go through
		// them all
		break;
	    }

        }   // while TRUE

        
        while (store != NULL) {
          CqLevelLink *temp = store;
          store = (CqLevelLink*)store->next;
          eventQ.pushBack(temp);
        }

// fire the star.
	if (!bFlag) {
	    if (!s->run()) return FALSE;
        }

    } // end of while

if (haltRequested()) return FALSE;

stopBeforeDeadFlag = FALSE;	// yes, no more events...
return TRUE;
}


////////////////////////////
// Miscellanies
////////////////////////////

/* 
Resort events according to new fine levels. The resorted 
events end up in eventQ (though during this process they will 
temporarily be stored in resortQ). This method is necessary
after a mutation occurs which might impact the porthole depths.
 */
void CQScheduler::resortEvents() {
	int allowResizing = 0;
	CQEventQueue resortQ;

	if( eventQ.cq_eventNum == 0 ) {
	    return;
	}

	if( eventQ.isResizeEnabled() == 1 ) {
	    allowResizing = 1;
	    eventQ.DisableResize();
	}

	resortQ.LocalInit( 0, eventQ.cq_bucketNum, 
			eventQ.cq_interval, eventQ.cq_lastTime );
	resortQ.DisableResize();

	while( eventQ.cq_eventNum > 0 ) {
	    CqLevelLink *cqLevelLink = eventQ.get();
	    // If the destination porthole is a DEPortHole, we need
	    // to update the fineLevel of this event.
	    if( ((Event *)(cqLevelLink->e))->dest->isA("DEPortHole") ) {
	        cqLevelLink->fineLevel = 
		    ((DEPortHole *)((Event *)(cqLevelLink->e))->dest)->depth;
	    }
	    resortQ.InsertCqLevelLink( cqLevelLink );
	}
	while( resortQ.cq_eventNum > 0 ) {
	    CqLevelLink *cqLevelLink = resortQ.get();
	    eventQ.InsertCqLevelLink( cqLevelLink );
	}

	if( allowResizing == 1 ) {
	    eventQ.EnableResize();
	}
}


// fetch an event on request.
int CQScheduler :: fetchEvent(InDEPort* p, double timeVal) 
{
	CqLevelLink *store = NULL;
	eventQ.DisableResize();
	while (1)
	{
		CqLevelLink *h = eventQ.get();
		if ((h == NULL) || (h->level > timeVal)) {
			Error :: abortRun (*p, " has no more data.");
			return FALSE;
		}
		InDEPort* tl = 0;
                if (h->fineLevel != 0) {
			Event* ent = (Event*) h->e;
                        tl = (InDEPort*) ent->dest;

                        // if same destination star with same time stamp..
                        if (tl == p) {
                                if (tl->getFromQueue(ent->p)){
					eventQ.putFreeLink(h);
				}
				else
					eventQ.pushBack(h);
				while (store != NULL) {
				      CqLevelLink *temp = store;
				      store = (CqLevelLink*)store->next;
				      eventQ.pushBack(temp);
				}
				eventQ.EnableResize();
                                return TRUE;
                        }
                }
		h->next = store;
		store = h;
	}
	Error :: abortRun (*p, " Should never get here.");
	return FALSE;
}

int CQScheduler :: selfFiringRequested() {
    CqLevelLink* f = eventQ.get();
    if (f == NULL) {
        // No event left in queue
        return FALSE;
    }
    
    eventQ.pushBack(f);	// push back
    return TRUE;
}
	
double CQScheduler :: nextFiringTime() {
    CqLevelLink* f = eventQ.get();
    if (f == NULL) {
        // No event left in queue
        return 0.0;
    }
    
    double level = f->level;
    eventQ.pushBack(f);	// push back
    return level;
}

// isA
ISA_FUNC(CQScheduler,DEBaseSched);
