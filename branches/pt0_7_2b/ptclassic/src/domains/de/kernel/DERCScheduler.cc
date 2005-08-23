static const char file_id[] = "DERCScheduler.cc";
/******************************************************************** 
Version identification:  @(#)DERCScheduler.cc	1.18 09/21/99
 
Author: Mudit Goel
        Neil Smyth
        Claudio Passerone

Copyright (c) 1997-1999 The Regents of the University of California.
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


#ifdef __GNUG__
#pragma implementation
#endif

#include "type.h"
#include "DERCScheduler.h"
#include "StringList.h"
#include "FloatState.h"
#include "GalIter.h"
#include "IntState.h"
#include <assert.h>
#include "checkConnect.h"
#include "DataStruct.h"
#include "DERCStar.h"

extern const char DEdomainName[];

/*******************************************************************
		Main DE scheduler routines
*******************************************************************/


////////////////////////////
// displaySchedule
////////////////////////////
StringList DERCScheduler :: displaySchedule () {
    return "{ { scheduler \"Calendar queue for Resource Contention(RC) run-time scheduler\" } }";
}


////////////////////////////
// setup
////////////////////////////
void DERCScheduler :: setup () {
    clearHalt();
    currentTime = 0;

    if (! galaxy()) {
        Error::abortRun("DERC scheduler has no galaxy defined");
        return;
    }

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

    // Added by Claudio Passerone 09/24/1998
    // Set indexValue of stars
    setStarIndices(*galaxy());
    resourceList = getResources();
}



	///////////////////////////////////////////////////
	// The following are the DERC specific methods.
	///////////////////////////////////////////////////

// Gets the list of all distinct resources and sets the resourcePointer in all
// stars to the appropriate new Resource object. Currently each Star is 
// restricted to accessing one resource
SequentialList* DERCScheduler :: getResources() {

    GalStarIter nextStar(*galaxy());
    DEStar* star;
    DERCStar *rcStar;
    char* newResourceName;
    int found = 0;
    Resource* reslink;
	
    SequentialList* resList = new SequentialList();
    while ((star = (DEStar*) nextStar++) != 0) {
	    if (star->isRCStar && ((DERCStar*)star)->needsSharedResource < 2) {
	        rcStar = (DERCStar*)star;
	        newResourceName = rcStar->resource;
            if (rcStar->needsSharedResource) {	 //Not a HW star
		        found = 0;
		        ListIter nextResource(*resList);
		        while ((found == 0) && ((reslink = (Resource*)nextResource++) != NULL)) {
		            if (strcmp (newResourceName, reslink->name) == 0) {
    			        found = 1;   // Resource already exists
	    		        rcStar->resourcePointer = reslink;

                        // Now check that this Star has the same scheduling
                        // policy as all the other Stars using this resource
//                        Deleted by Claudio Passerone
//                        if (rcStar->schedPolicy != reslink->schedPolicy) {
//                            Error::abortRun("Stars with different scheduling policies executing on same resource");
//                            return FALSE;
//                        }
                    }
		        }
		        if (found == 0) {
//                    Deleted by Claudio Passerone
//                    int schedPolicy = (rcStar->schedPolicy);
                    int schedPolicy = -1;
                    Resource* newResource = new Resource(newResourceName, schedPolicy, this); 
                    resList->append(newResource);
    		        rcStar->resourcePointer = newResource;
                }
	        } else {
                // This is a HW star
		        Resource* newResource = new Resource("HW", -1, this);
                rcStar->resourcePointer = newResource;	
	        }
	    }
    }
    return resList;
}


////////////////////////////
// run
////////////////////////////

// pop the top element (earliest event) from the global queues
// and fire the destination star. Check all simultaneous events to the star.
// Run until StopTime.

int DERCScheduler :: run () {
//    Deleted by Claudio Passerone  9/24/1998
//    Added in the setup() method
//    resourceList = getResources();

    if (SimControl::haltRequested() || !galaxy()) {
        Error::abortRun("Calendar Queue scheduler has no galaxy to run");
        return FALSE;
    }
    
    double level = 0;
    
    while (TRUE) {      
	int bFlag = FALSE;  // flag = TRUE when the terminal is on the
        // boundary of a wormhole of DE domain.
        
	// fetch the earliest event.
	CqLevelLink* f  = eventQ.get();
        
	// Choosing the one with the lowest level
	if (f == NULL) break;
        level    = f->level;
        //printf("\thave event in scheduler for star %s at time %f\n\n", ((Event*)f->e)->dest->parent()->asStar().name(), f->level);fflush(0);
     
	// if level > stopTime, RETURN...
	if (level > stopTime)	{
            eventQ.pushBack(f);		// push back
            // set currentTime = next event time.
            // FIXME : need to resolve time issue (real time vs instants)
            currentTime = stopTime/relTimeScale;
            stopBeforeDeadFlag = TRUE;  // there is extra events.
            return TRUE;
        }
        // If the event time is less than the global clock,
        // it is an error...
	else if (level < currentTime) {
	    // The event is in the past!  Argh!
	    // Try to give a good error message.
            Event* ent = (Event*) f->e;
            PortHole* src = ent->dest->far();
            //printf("now is %f, event at time %f\n", currentTime, level);fflush(0);
            Error::abortRun(*src, ": event from this porthole is in the past!");
            return FALSE;  
	}        
	// set currentTime
	currentTime = level;
       
        //A new event, never seen before
	if (((DEStar*)f->dest)->isRCStar && ((DERCStar*)f->dest)->needsSharedResource < 2) {
            DERCStar* pStar = (DERCStar*)(f->dest);
            //pStar->arrivalTime = level;
            //printf("updating arrivalTime of Star  %s, to time %f\n", ((Event*)f->e)->dest->parent()->asStar().name(), level);fflush(0);
 	    // eventQ.putFreeLink(f);
            // this method will only return false if star fails to run(error)
            if (!(pStar->resourcePointer->newEventFromEventQ(f, currentTime))) return FALSE;
        }
        else {
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
   	    // LL: uncommented 9/16/99
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
		        int success = ((InDEPort*) tl)->getFromQueue(ee->p);
		        if (success) {
			   // LL: uncommented 9/16/99
		           eventQ.putFreeLink(h);
			}
		        else {
			    h->next = store;
			    store = h;
		        }
		    } else if (!tl) {
			 // LL: uncommented 9/16/99
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
            
        } // end of else
   }// end while
    
    if (haltRequested()) return FALSE;
    
    stopBeforeDeadFlag = FALSE;	// yes, no more events...
    return TRUE;
}

////////////////////////////
// Miscellanies
////////////////////////////

// fetch an event on request.
int DERCScheduler :: fetchEvent(InDEPort* p, double timeVal) 
{
    CqLevelLink *store = NULL;
    eventQ.DisableResize();
    while (1) {
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
		      // LL: uncommented 9/16/99
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

// isA
ISA_FUNC(DERCScheduler,DEBaseSched);
