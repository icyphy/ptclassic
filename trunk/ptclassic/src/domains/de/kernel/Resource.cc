static const char file_id[] = "Resource.cc";
/**************************************************************************
Version identification:	$Id$

Copyright (c) 1997- The Regents of the University of California.
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
conjunction with a DERCScheduler.
*/



#ifdef __GNUG__
#pragma interface
#endif

#include <math.h>
#include <assert.h>
#include "type.h"
#include "DataStruct.h"
#include "Resource.h"
#include "DERCScheduler.h"
#include "DERCEventQ.h"
#include "DERCStar.h"


//////////////////////////////////////
// class Resource
//////////////////////////////////////

Resource::Resource(const char* n, int policy, DERCScheduler* sched) 
    : name(n), schedPolicy(policy), mysched(sched)
{
    intEventList = new SequentialList();
    eventQ = (DERCEventQ*) mysched->queue();
    interruptQ = (DERCEventQ*) mysched->interruptQueue();
    timeWhenFree = -1;
}

////////////////////////////////////////////////////////////////////////
// When the next event to be executed comes from the interruptQ,
// then either 
// - the event is signalling that the current Star is 
// finished using the resource
// - or it is an output event of the currently executing Star whose time
// of emission has arrived, and so must be emitted by the Star
///////////////////////////////////////////////////////////////////////

int Resource::newEventFromInterruptQ(DERCEvent* e, double now) {
    // first get the priority of the received events source star
    double prio = e->src->priority;

    // Now go through the intEventList and remove this event
    SequentialList* store = new SequentialList();
    ResLLCell* p = (ResLLCell*)intEventList->getAndRemove();
    
    if (p->priority != prio) {
        Error::abortRun("unequal priorities");
        return FALSE;
    }
    while (p->priority == prio) {        
        if (p->event == e) {
            break;
        } else {
            store->prepend((ResLLCell*)p);
        }
        p = (ResLLCell*)intEventList->getAndRemove();
        assert(p != NULL);
    }
	
    ListIter storeEvents(*store);   
    ResLLCell *q;
    while((q = (ResLLCell*)storeEvents++)!=0) {
        intEventList->prepend(q);
    }
    store->~SequentialList();
    // storeEvents->~ListIter;

    if (p->event != e) {
        Error::abortRun("Event from IntQ not on Resources Linked List");
        return FALSE;
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
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////
// For the purposes of simulation, each Hardware unit can be considered
// also as a resource, but with only one CFSM using it.
// For Resources which have more than one CFSM using them, 
// it is a bit more complicated, also have to resolve issue of 
// resource contention.
// 
// Note that each hardware resource is used by exactly one Star (CFSM)
// Any events passed to this function by the scheduler must have a RC
// star as destination (this method is only called by DERCScheduler)
/////////////////////////////////////////////////////////////////////////

int Resource :: newEventFromEventQ(DERCEvent* e, double now) {
    double nextTry;    
    // first get all events from the eventQ with the current time, 
    // for this Resource
    SequentialList* sortList = getOtherEvents(e, now);
        
    // Move these events to an array for more efficent processing
    int dimen = sortList->size();
    DERCEvent* sortArray[dimen];
    int i;
    for (i=0 ; i<dimen ; i++) {
        sortArray[i] = (DERCEvent*)sortList->getAndRemove();
    }

    sortList->~SequentialList();
    
    if (getDERCStar(e)->needsSharedResource) {
        //////////////////////////////////////
        // Star is SW!!!
        //////////////////////////////////////
              
        // set the timeOfArrival parameter of each Star if it is 
        // not already set
        for (i=0 ; i<dimen ; i++){
            if (getDERCStar(sortArray[i])->timeOfArrival == -1){
                getDERCStar(sortArray[i])->timeOfArrival = now;	
            }
        }     
        
        // Now obtain the "top" event in the array, first according to 
        // priority, then timeOfArrival and finally port depth(from the 
        // topological sort)
        //
        // FIXME : does this setup account correctly for RoundRobin scheduling?
        // (perhaps need to switch timeOfArrival & priority)
        DERCEvent* topEvent = sortArray[0];
        int topPrio = getDERCStar(topEvent)->priority;
        double earliestToA = getDERCStar(topEvent)->timeOfArrival;
        int indexOftop = 0;
        for (i=1 ; i<dimen ; i++){
            if (getDERCStar(sortArray[i])->priority > topPrio) {
                topEvent = sortArray[i];
                topPrio = getDERCStar(topEvent)->priority;
                earliestToA = getDERCStar(topEvent)->timeOfArrival;
                indexOftop = i;
            }
            else if (getDERCStar(sortArray[i])->priority == topPrio) {
                if (getDERCStar(sortArray[i])->timeOfArrival < earliestToA) {
                    earliestToA = getDERCStar(sortArray[i])->timeOfArrival;
                    topEvent = sortArray[i];
                    indexOftop = i;
                }
                else if (getDERCStar(sortArray[i])->timeOfArrival == earliestToA) {
                    int depth = ((DEPortHole*)(topEvent->dest))->depth;
                    if (depth > ((DEPortHole*)(sortArray[i]->dest))->depth) {
                        topEvent = sortArray[i];
                        indexOftop = i;
                    }       
                }
            }
        }

        /* Now have the current event of highest priority.     */
        /* See if this event can access the Resource           */
        if (!canAccessResource(topEvent)) {
            // Cannot access resource, so reschedule the Events
            for (i=0 ; i<dimen ; i++){
                if(getDERCStar(topEvent)->needsSharedResource) {
                    // Reschedule each event at the appropriate time by 
                    // looking at the Resources Linked List to see all
                    // the interrupted Stars ahead of it
                
                    double nextTry = -1;   
                    ListIter getAppECT(*intEventList);
                    ResLLCell* p;
                    while ((p= (ResLLCell*)getAppECT++)!= 0) {
                        if (p->priority < getDERCStar(sortArray[indexOftop])->priority) {
                            // resource MUST be operating in NonPreemptive mode, else have error
                            if (schedPolicy == Preemptive) {
                                Error::abortRun("if resource is using a Preemptive policy, it should not reach here");
                                return FALSE;
                            }
                            // Try again some time in future, earlist time 
                            // being when currently executing star is finished
                            nextTry = timeWhenFree;
                            break;
                        }
                        if (p->priority >= getDERCStar(sortArray[i])->priority) {
                            nextTry = p->ECT;
                        } else {
                            break;
                        }
                    }
                    if (nextTry == -1) {
                        Error::abortRun(" SW Event never got rescheduled in NewEventFromEventQ()");
                        return FALSE;
                    }
                    // getAppECT->~ListIter();
                    DEPortHole* destPort = (DEPortHole*)sortArray[i]->dest;
                    eventQ->levelput(sortArray[i], nextTry, destPort->depth);
                }  
            }          
        } else {
            // can access Resource!
            // Need to make available any other Events for the Star
            DERCStar* starUsingResource = getDERCStar(topEvent);
            DERCStar* dest;
        
            starUsingResource->startNewPhase();
            
            for (i=0 ; i<dimen ; i++){
                DEPortHole* port = (DEPortHole*)(sortArray[i]->dest);
                dest = getDERCStar(sortArray[i]);
                if (starUsingResource == dest) {
                    if (port->isItOutput()) {
                        Error::abortRun("Terminal goes to wormhole!!!");
                        return FALSE;
                    }
                    else {
                        int success = ((InDEPort*) port)->getFromQueue(sortArray[i]->p);
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
            
            // get the new events from the fired Star
            ListIter newEventsList(*(starUsingResource->getEvents()));

            // this ListIter should have the events output by the star firing in
            // the order they were output ie the most recent event is at the head
            DERCEvent* ee;
	    ee = (DERCEvent*)newEventsList++;
            while (ee != 0) {
                ResLLCell* newCell = new ResLLCell(ee, ee->realTime, starUsingResource->priority);
                intEventList->prepend(newCell);
                ee = (DERCEvent*)newEventsList++;
            }
            // oldEventsList->~ListIter();
            // newEventsList->~ListIter();

            // Now reschedule the remaining events that were originally taken 
            // from the eventQ       
            for (i=0 ; i<dimen ; i++){
                if (sortArray[i] != 0 && indexOftop != i) {
                    // Reschedule each event at the appropriate time by 
                    // looking at the Resources Linked List to see all 
                    // the interrupted Stars ahead of it
                    nextTry = -1;  
                    
                    ListIter getAppECT(*intEventList);
                    ResLLCell* p = (ResLLCell*)getAppECT++;
                    while (p != 0) {                      
                        if (p->priority >= getDERCStar(sortArray[i])->priority) {
                            nextTry = p->ECT;
                        } else {
                            break;
                        }
                        p = (ResLLCell*)getAppECT++;
                    }
                    if (nextTry == -1) {
                        Error::abortRun("HW Event never got rescheduled in NewEventFromEventQ()");
                        return FALSE;
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
            printf("HW resource not free, timeWhenFree %f, now %f\n", timeWhenFree, now);fflush(0);
            for (i=0 ; i<dimen ; i++) {
                DEPortHole* destPort = (DEPortHole*)sortArray[i]->dest;
                eventQ->levelput(sortArray[i], timeWhenFree, destPort->depth); 
            }
        } else { // Resource is FREE!     
            // Need to make available any other Events for the Star
            DERCStar* starUsingResource = getDERCStar(e);
            DERCStar* dest;
            
            starUsingResource->startNewPhase();
            for (i=0 ; i<dimen ; i++){
                DEPortHole* port = (DEPortHole*)(sortArray[i]->dest);
                dest = getDERCStar(sortArray[i]);
                assert(starUsingResource == dest);
                if (port->isItOutput()) {
                    Error::abortRun("Terminal goes to wormhole!!!");
                    return FALSE;
                }
                else {
                    int success=((InDEPort*)port)->getFromQueue(sortArray[i]->p);
                    if (!success) {
                         eventQ->levelput(sortArray[i], now, port->depth);
                     }
	 	}
            }
            
            // fire the star. A HW Star writes its Events directly to the eventQ
            starUsingResource->run();
            nextTry = now + starUsingResource->getDelay();
            this->timeWhenFree = nextTry;
        }
    }
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////
// This method returns a SequentialList of all other current Events in the
// eventQ, and the passed event, who also want to access this resource. 
///////////////////////////////////////////////////////////////////////////

SequentialList* Resource :: getOtherEvents(DERCEvent* e, double now) {

    SequentialList* sortList = new SequentialList();
    sortList->prepend(e);

    // First get all current events from the eventQ
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
    // Now go through this SequentialList and pushback any events not looking 
    // for this resource. sortList is used for this in a circular fashion,
    // ie set a pointer to the top event and go through list until reach it 
    // again. Any events using this resource get appended to the tail of the 
    // list.
    DERCEvent* topEvent = (DERCEvent*)sortList->getAndRemove();
    sortList->append(topEvent); // Set marker to event at end of list
    DERCEvent* nextEvent;   
    do {
        nextEvent = (DERCEvent*)sortList->getAndRemove();
        if (getDERCStar(nextEvent)->resourcePointer != this){
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

int Resource :: canAccessResource(DERCEvent* newEvent) {
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
    if (getDERCStar(newEvent)->priority > topCell->priority) {
        printf("preempting!!\n");
        return 1;
    } else {
        return 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
// This method is used to reschedule an event in the interruptQ to a later time
///////////////////////////////////////////////////////////////////////////////

void Resource :: intQupdate(DERCEvent* eventPointer, double newTime, double oldTime) {    
    double fv;
    DERCEvent* event = interruptQ->getRandomEvent(eventPointer, oldTime);
      if (eventPointer->isDummy){
        fv = -0.5;
    } else {
        fv = ((DEPortHole*)eventPointer->dest)->depth;
    }    
    event->realTime = newTime;
    interruptQ->levelput(event, newTime, fv);
}

/////////////////////////////////////////////////////////////
// used to get the destination of the event as a DERC Star
/////////////////////////////////////////////////////////////
DERCStar* Resource :: getDERCStar(Event* e) {
    Star* temp = &(e->dest->parent()->asStar());
    DERCStar* tmp = (DERCStar*)temp;
    return tmp;
}


