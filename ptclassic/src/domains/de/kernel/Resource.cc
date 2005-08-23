static const char file_id[] = "Resource.cc";
/**************************************************************************
Version identification:
@(#)Resource.cc	1.55     12/12/97

Copyright (c) 1990- The Regents of the University of California.
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


**************************************************************************/


/* 
 Programmer:  Neil Smyth
              Mudit Goel 
 Date of creation: 11/11/97
 Revisions:
 
A Resource object is used to control access to a simulated resource (eg CPU,
data bus etc.) during a POLIS simulation. It is designed to be used in 
conjunction with a PolisScheduler.
*/



#ifdef __GNUG__
#pragma interface
#endif

#include <math.h>
#include <assert.h>
#include "type.h"
#include "DataStruct.h"
#include "Resource.h"
#include "PolisScheduler.h"
#include "PolisEventQ.h"
#include "DEPolis.h"


//////////////////////////////////////
// class Resource
//////////////////////////////////////

Resource::Resource(const char* n, int policy, PolisScheduler* sched) 
    : name(n), schedPolicy(policy), mysched(sched)
{
    intEventList = new SequentialList();
    eventQ = (PolisEventQ*) mysched->queue();
    interruptQ = (PolisEventQ*) mysched->interruptQueue();
    timeWhenFree = -1;
}

/* When the next event to be executed comes from the interruptQ,
   then either 
   - the event is signalling that the current Star is 
   finished using the resource
   - or it is an output event of the currently executing Star, which 
   must be placed in the appropriate other queue ie eventQ or eventQ.
*/
void Resource::newEventFromInterruptQ(PolisEvent* e, double now) {
    // first get the priority of the received events source star
    double prio = e->src->priority;

    // Now go through the intEventList and remove this event
    SequentialList* store = new SequentialList();
    ResLLCell* p = (ResLLCell*)intEventList->getAndRemove();
    
    if (p->priority != prio) {
        Error::abortRun("unequal priorities");
        return;
    }
    while (p->priority == prio) {        
        if (p->event == e) {
            break;
        } else {
            store->prepend((ResLLCell*)p);
        }
        p = (ResLLCell*)intEventList->getAndRemove();
    }
	
    ListIter storeEvents(*store);   
    ResLLCell *q;
    while((q = (ResLLCell*)storeEvents++)!=0) {
        intEventList->prepend(q);
    }
    store->~SequentialList();

    if (p->event != e) {
        Error::abortRun("Event from IntQ not on Resources Linked List");
        return;
    }
    if (e->isDummy) {
        // resource is finished with the current Star, so update 
        if (intEventList->size() == 0) {
            timeWhenFree = -1;
        } 
    } else {
        // The Event is an output Event and its time of output has
        // arrived without its Star getting interrupted, so the
        // Event can be emitted. This gets done via a call to the Star
        
        // Call Star to emit Event!!
        e->src->emitEvent(e, now); 
    }   
    return;
}

/* For the purposes of simulation, each Hardware unit can be considered
 * also as a resource, but with only one CFSM using it.
 * For Resources which have more than one CFSM using them, 
 * it is a bit more complicated, also have to resolve issue of 
 * resource contention.
 * 
 * Note that each hardware resource is used by exactly one Star (CFSM)
 * Any events passed to this function by the scheduler must have a Polis
 * star as destination
 */

void Resource :: newEventFromEventQ(PolisEvent* e, double now) {
    double nextTry;    
    // first get all events form the eventQ with the current time, 
    // for this Resource
    SequentialList* sortList = getOtherEvents(e, now);
        
    // Move these events to an array for more efficent processing
    int dimen = sortList->size();
    PolisEvent* sortArray[dimen];
    int i;
    for (i=0 ; i<dimen ; i++) {
        sortArray[i] = (PolisEvent*)sortList->getAndRemove();
    }

    sortList->~SequentialList();
    
    if (getPolisStar(e)->needResource) {

        //////////////////////////////////////
        // Star is SW!!!
        //////////////////////////////////////
        // Now obtain the "top" event in the array according to priority,
        // then timeOfArrival and then precedence
        // FIXME : does this setup account correctly for RoundRobin scheduling? 

        PolisEvent* topEvent = sortArray[0];
        int topPrio = getPolisStar(topEvent)->priority;
        if ((getPolisStar(topEvent)->timeOfArrival) == -1) {
            (getPolisStar(topEvent))->timeOfArrival = now;
        }
        double earliestToA = getPolisStar(topEvent)->timeOfArrival;
        int indexOftop = 0;
        
        /* Now the Events are in an array, sort the events with the highest
        priority according to timeOfArrival  */
        for (i=1 ; i<dimen ; i++){
            if (getPolisStar(sortArray[i])->timeOfArrival == -1){
                getPolisStar(sortArray[i])->timeOfArrival = now;	
            }
            if (getPolisStar(sortArray[i])->priority > topPrio) {
                topEvent = sortArray[i];
                topPrio = getPolisStar(topEvent)->priority;
                earliestToA = getPolisStar(topEvent)->timeOfArrival;
                indexOftop = i;
            }
            else if (getPolisStar(sortArray[i])->priority == topPrio) {
                if (getPolisStar(sortArray[i])->timeOfArrival < earliestToA) {
                    earliestToA = getPolisStar(sortArray[i])->timeOfArrival;
                    topEvent = sortArray[i];
                    indexOftop = i;
                }
                else if (getPolisStar(sortArray[i])->timeOfArrival == earliestToA) {
                    int depth = ((DEPortHole*)(topEvent->dest))->depth;
                    if (depth > ((DEPortHole*)(sortArray[i]->dest))->depth) {
                        topEvent = sortArray[i];
                        indexOftop = i;
                    }       
                }
            }
        }

        /* Now have the current event of highest priority.    
        See if this event can access the Resource */
        if (!canAccessResource(topEvent)) {
            // Cannot access resource, so reschedule the Events
            for (i=0 ; i<dimen ; i++){
                if(getPolisStar(topEvent)->needResource) {
                    // Reschedule each event at the appropriate time by 
                    // looking at the Resources
                    // Linked List to see all the interrupted Stars ahead of it
                
                    double nextTry = -1;   
                    ListIter getAppECT(*intEventList);
                    ResLLCell* p;
                    while ((p= (ResLLCell*)getAppECT++)!= 0) {
                        if (p->priority < getPolisStar(sortArray[indexOftop])->priority) {
                            // resource MUST be operating in NonPreemptive mode, else have error
                            if (schedPolicy == Preemptive) {
                                Error::abortRun("if resource is using a Preemptive policy, it should not reach here");
                                return;
                            }
                            nextTry = timeWhenFree;
                            break;
                        }
                        if (p->priority >= getPolisStar(sortArray[i])->priority) {
                            nextTry = p->ECT;
                        } else {
                            break;
                        }
                    }
                    if (nextTry == -1) {
                        Error::abortRun("Event ner got rescheduled in NewEventFromEventQ()");
                        return;
                    }
                    getAppECT->~ListIter();
                    DEPortHole* destPort = (DEPortHole*)sortArray[i]->dest;
                    eventQ->levelput(sortArray[i], nextTry, destPort->depth);
                }  
            }          
        } else {
            // can access Resource!
            // Need to make available any other Events for the Star
            DEPolis* starUsingResource = getPolisStar(topEvent);
            DEPolis* dest;
        
            starUsingResource->startNewPhase();
            
            for (i=0 ; i<dimen ; i++){
                DEPortHole* tl = (DEPortHole*)(sortArray[i]->dest);
                dest = getPolisStar(sortArray[i]);
                if (starUsingResource == dest) {
                    if (tl->isItOutput()) {
                        Error::abortRun("Terminal goes to wormhole!!!");
                        return;
                    }
                    else {
                        int success = ((InDEPort*) tl)->getFromQueue(sortArray[i]->p);
			// Remove this Event from the Array
			if (success) sortArray[i] = 0; 
                    }
                }
            }
        
            // fire the star. The Star should write its events to the interruptQ.
            starUsingResource->run();
        
            // Now update the Resources SequentialList
            double updateDelay = starUsingResource->getDelay();
        
            // first update all old events with the delay of the curreent Star
            ListIter oldEventsList(*intEventList);
            ResLLCell* p;
            while ((p = (ResLLCell*)oldEventsList++) != 0) {
                double oldTime = p->ECT;
                p->ECT = oldTime + updateDelay;
                intQupdate(p->event, p->ECT, oldTime);
            }
            this->timeWhenFree = now + updateDelay;
            
            // then add the new events from that Star firing to the SequentialList
            ListIter newEventsList(*(starUsingResource->getEvents()));

            // this ListIter should have the events output by the star firing in
            // the order they were output ie the most recent event is at the head
            PolisEvent* ee;
	    ee = (PolisEvent*)newEventsList++;
            while (ee != 0) {
                ResLLCell* newCell = new ResLLCell(ee, ee->realTime, starUsingResource->priority);
                intEventList->prepend(newCell);
                ee = (PolisEvent*)newEventsList++;
            }
            oldEventsList->~ListIter();
        
            // Now reschedule the remaining events that were originally taken 
            // from the eventQ       
            for (i=0 ; i<dimen ; i++){
                if (sortArray[i] != 0 && indexOftop != i) {
                    // Reschedule each event at the appropriate time by 
                    // looking at the Resources
                    // Linked List to see all the interrupted Stars ahead of it
                    nextTry = -1;  
                    
                    ListIter getAppECT(*intEventList);
                    ResLLCell* p = (ResLLCell*)getAppECT++;
                    while (p != 0) {                      
                        if (p->priority >= getPolisStar(sortArray[i])->priority) {
                            nextTry = p->ECT;
                        } else {
                            break;
                        }
                        ResLLCell* p = (ResLLCell*)getAppECT++;
                    }
                    if (nextTry == -1) {
                        Error::abortRun("Event never got rescheduled in NewEventFromEventQ()");
                        return;
                    }
                    //getAppECT->~ListIter();
                    DEPortHole* destPort = (DEPortHole*)e->dest;
                    eventQ->levelput(sortArray[i], nextTry, destPort->depth);
                    
                }      
            } 
        }
    } else { 
        //////////////////////////////////////////////////////
        // Star is HW!!!
        //////////////////////////////////////////////////////
        if (this->timeWhenFree > now) {
            // Resource is not free
            // Reschedule the Events. Remember all such Events use Star
            // for same time (same freq)
            
            for (i=0 ; i<dimen ; i++) {
                DEPortHole* destPort = (DEPortHole*)sortArray[i]->dest;
                eventQ->levelput(sortArray[i], timeWhenFree, destPort->depth); 
            }
        } else { // Resource is FREE!     
            // Need to make available any other Events for the Star
            DEPolis* starUsingResource = getPolisStar(e);
            DEPolis* dest;
            
            starUsingResource->startNewPhase();
            for (i=0 ; i<dimen ; i++){
                DEPortHole* tl = (DEPortHole*)(sortArray[i]->dest);
                dest = getPolisStar(sortArray[i]);
                assert(starUsingResource == dest);
                if (tl->isItOutput()) {
                    Error::abortRun("Terminal goes to wormhole!!!");
                    return;
                }
                else {
                    int success=((InDEPort*)tl)->getFromQueue(sortArray[i]->p);
                    if (!success) {
                         eventQ->levelput(sortArray[i], now, tl->depth);
                     }
	 	}
            }
            
            // fire the star. A HW Star writes its Events directly to the eventQ
            starUsingResource->run();
            nextTry = now + starUsingResource->getDelay();
            this->timeWhenFree = nextTry;
        }
    }
}


/* This metheod returns a SequentialList of other Events in the eventQ who 
   also want to access this resource. The Linked List is returned
   in a form with the Events whose Stars have highest priority at the head.
*/
SequentialList* Resource :: getOtherEvents(PolisEvent* e, double now) {

    SequentialList* sortList = new SequentialList();
    sortList->prepend(e);
    while (TRUE) {
        CqLevelLink *h = eventQ->get();
        if (h == NULL) {
            break;
        }
        if (h->level > now) {
            eventQ->pushBack(h);
            break;
        }
	else assert(h->level == now);
        // Now add these events to a linked list in such a way that
        // the events of highest priority are at the head of the list
        
        sortList->prepend(h->e);
        //eventQ->putFreeLink(h);
    }
    // Now go through the SequentialList and pushback any events not looking 
    // for this resource. sortList is used for this in a circular fashion,
    // ie set a pointer to the top event and go through list until reach it 
    // again. Any events using this resource get appended to the tail of the 
    // list.
    PolisEvent* topEvent = (PolisEvent*)sortList->getAndRemove();
    sortList->append(topEvent); // Set marker to event at end of list
    PolisEvent* nextEvent;   
    do {
        nextEvent = (PolisEvent*)sortList->getAndRemove();
        if ((Resource*)(getPolisStar(nextEvent)->resourcePointer) != (Resource*)this){
            // not for this resource, so put back in queue
            DEPortHole* destPort = (DEPortHole*)nextEvent->dest;
            eventQ->levelput(nextEvent, now, destPort->depth);  
        } else {  
            sortList->append(nextEvent);
        }
    } while ( nextEvent != topEvent );

    return sortList;
}


//////////////////////////////////////////////////////////////
// Used to check if the current Event can gain access to
// the Resource. Returns 1 if it can, else 0
//////////////////////////////////////////////////////////////

int Resource :: canAccessResource(PolisEvent* newEvent) {
    if (timeWhenFree == -1) {
        return 1;
    }   
    if ( (schedPolicy == RoundRobin) || (schedPolicy == NonPreemptive) ) {
        return 0;
    }
    // Resource is not free, so see if new Event can preempt the  currently
    // executing one
    if (schedPolicy != Preemptive) {
        Error::abortRun("Not a valid policy!");
        return 0;
    }
    ResLLCell* topCell = (ResLLCell*)intEventList->getAndRemove();
    intEventList->prepend(topCell);
    
    //newPriority > oldPriority ?
    if (getPolisStar(newEvent)->priority > topCell->priority) {
        printf("preempting!!\n");
        return 1;
    } else {
        return 0;
    }
}


void Resource :: intQupdate(PolisEvent* eventPointer, double newTime, double oldTime) {    
    double fv;
    PolisEvent* event = interruptQ->getRandomEvent(eventPointer, oldTime);
      if (eventPointer->isDummy){
        fv = -0.5;
    } else {
        fv = ((DEPortHole*)eventPointer->dest)->depth;
    }    
    event->realTime = newTime;
    interruptQ->levelput(event, newTime, fv);
}


// used to get the destination of the event as a Polis Star
DEPolis* Resource :: getPolisStar(Event* e) {
    Star* temp = &(e->dest->parent()->asStar());
    DEPolis* tmp = (DEPolis*)temp;
    return tmp;
}


