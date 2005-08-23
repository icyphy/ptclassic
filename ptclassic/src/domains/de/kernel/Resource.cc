static const char file_id[] = "Resource.cc";
/**************************************************************************
Version identification:	@(#)Resource.cc	1.24 11/10/99

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


**************************************************************************/


/* 
 Programmer:  Neil Smyth
              Mudit Goel 
              Claudio Passerone
 Date of creation: 11/11/97
 Revisions: 4/15/98 Revised strategy to store the pending event in the stars
 
A Resource object is used to control access to a simulated resource (eg CPU,
data bus etc.) in a DE simulation with resource contention. It is designed 
to be used in conjunction with a DERCScheduler.
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
#include "DERCStar.h"


//////////////////////////////////////
// class Resource
//////////////////////////////////////

Resource::Resource(const char* n, int policy, DERCScheduler* sched) 
    : name(n), schedPolicy(policy), mysched(sched)
{
    intStarList = new SequentialList();
    eventQ = (CQEventQueue*) mysched->queue();
    timeWhenFree = -1;
}

////////////////////////////////////////
// Returns the scheduling policy code
// schedpolicy is case insensitive
////////////////////////////////////////

int Resource :: getSchedPolicy(const char* n)
{
    char st[256];
    int policy = -1;
    
    if (!strcasecmp("RoundRobin", n)) policy = DE_ROUNDROBIN;
    else if (!strcasecmp("NonPreemptive", n)) policy = DE_NONPREEMPTIVE;
    else if (!strcasecmp("Preemptive", n)) policy = DE_PREEMPTIVE;
    else if (!strcasecmp("FIFO", n)) policy = DE_FIFO;
    else if (!strcasecmp("FIFONonPreemptive", n)) policy = DE_FIFONONPREEMPTIVE;
    else if (!strcasecmp("FIFOPreemptive", n)) policy = DE_FIFOPREEMPTIVE;
    else {
        strcpy(st, n);
        strcat(st, ": not a valid scheduling policy");
        Error::abortRun( st );
    }
    if (schedPolicy != -1 && schedPolicy != policy) {
        strcpy(st, name);
        strcat(st, ": stars with different scheduling policies");
        Error::abortRun( st );
    }
    return (schedPolicy = policy);
}

////////////////////////////////////////////////////////////////
// Registers the priority to update the lastFired HashTable
////////////////////////////////////////////////////////////////

int * Resource :: registerPriority(const char *prkey)
{
    int * intPointer;
    char prkeyfixed[] = "0";
    
    if (schedPolicy == DE_ROUNDROBIN || schedPolicy == DE_FIFO) prkey = prkeyfixed;
    if (lastFired.hasKey(prkey)) {
        intPointer = (int *) lastFired.lookup(prkey);
        *intPointer = 0;
        return(intPointer);
    }
    intPointer = new int(0);
    lastFired.insert(prkey, (Pointer) intPointer);
    return(intPointer);
}

////////////////////////////////////////////////////////////////
// Registers the clock frequency (check if same for all stars)
////////////////////////////////////////////////////////////////

void Resource :: registerClock(double clock)
{
    char st[256];
    
    if (clkfreq == 0.0) {
        clkfreq = clock;
    } else if (clkfreq != clock) {
        strcpy(st, name);;
        strcat(st, ": stars with different clock frequencies");
        Error::abortRun( st );
    }
}

/////////////////////////////////////////////////////////////////////////
// For the purposes of simulation, each Hardware unit can be considered
// also as a resource, but with only one Star using it.
// For Resources which have more than one Star using them, 
// it is a bit more complicated, also have to resolve issue of 
// resource contention.
// 
// Note that each hardware resource is used by exactly one Star (CFSM)
// Any events passed to this function by the scheduler must have a RC
// star as destination (this method is only called by DERCScheduler)
/////////////////////////////////////////////////////////////////////////

int Resource :: newEventFromEventQ(CqLevelLink* link, double now) {
    double nextTry, updateDelay = 0;
    int access;

    // Each link contains an event, the destination star of the event, and
    // the time of the event. In the following code we refer to links and
    // events interchangably.

    // ASSUMING that DERCStars use the feedbackIn port *only* for 
    // controling the emitting of events, if its value is 0.0.
    // Otherwise it is a refire event, and should be treated as
    // all the other events.
    if ( ((Event*)link->e)->dest == getDERCStar(link)->feedbackIn) {
        if ( int(*((((Event*)link->e)->p))) == 0 ) {
            // fire the star. This firing is signifying that an event is ready 
            // for output or that a Star is finished using this Resource
            DERCStar* starUsingResource = getDERCStar(link);
            starUsingResource->arrivalTime = now;
            starUsingResource->startNewPhase();

            // need to place the particle in the dest port
            InDEPort* dest = (InDEPort*)((Event*)link->e)->dest;
            dest->getFromQueue(((Event*)link->e)->p);
            if (!starUsingResource->run()) return FALSE;
            eventQ->putFreeLink(link);
            return TRUE;
        }
    }
        
    

    // first get all links from the eventQ with the current time, 
    // with events for stars for this Resource
    SequentialList* sortList = getOtherLinks(link, now);
        
    // Move these links to an array for more efficent processing    
    const int dimen = sortList->size();
    LOG_NEW; CqLevelLink** sortArray = new CqLevelLink*[dimen];
    int i;
    for (i=0 ; i<dimen ; i++) {
        sortArray[i] = (CqLevelLink*)sortList->getAndRemove();
    }

    sortList->~SequentialList();
	LOG_DEL; delete sortList;

    // Check to see if getOtherEvents only returned an event on a
    // feedbackIn port. If so, then the event is signaling that one
    // of the events stored in the star is ready to be emitted.
    if ((dimen == 1) &&  (((Event*)sortArray[0]->e)->dest == getDERCStar(sortArray[0])->feedbackIn)) {
        if ( int(*((((Event*)sortArray[0]->e)->p))) == 0 ) {
            // fire the star. This firing is signifying that an event is ready 
            // for output
            DERCStar* starUsingResource = getDERCStar(sortArray[0]);
            starUsingResource->arrivalTime = now;
            starUsingResource->startNewPhase();

            // need to place the particle in the dest port
            InDEPort* dest = (InDEPort*)((Event*)sortArray[0]->e)->dest;
            dest->getFromQueue(((Event*)sortArray[0]->e)->p);
            CqLevelLink *l = sortArray[0];
	      LOG_DEL; delete [] sortArray;
            if (!starUsingResource->run()) return FALSE;
            eventQ->putFreeLink(l);
            return TRUE;
        }
    }

    // Must have an event that is trying to fire the star and use the resource
    
    if (getDERCStar(link)->needsSharedResource) {
        //////////////////////////////////////
        // Star is SW!!!
        //////////////////////////////////////
              
        // set the timeOfArrival parameter of each Star if it is 
        // not already set
        for (i=0 ; i<dimen ; i++){
            if (getDERCStar(sortArray[i])->timeOfArrival == -1) {
                getDERCStar(sortArray[i])->timeOfArrival = now;	
            }
        }     
        
        // Now obtain the "top" link in the array using the links
        // events destination, according to priority, index,
        // timeOfArrival and depth depending on the scheduling
        // policy selected.
        //
        // Modified by Claudio Passerone 09/25/1998
        CqLevelLink* topLink = sortArray[0];
        int topPrio = getDERCStar(topLink)->priority;
        double earliestToA = getDERCStar(topLink)->timeOfArrival;
        int topDist = getDERCStar(topLink)->index() - (*(getDERCStar(topLink)->lastFiredPr));
        int topDepth = ((DEPortHole*)(((Event*)topLink->e)->dest))->depth;
        int indexOftop = 0;
        int tempDist = 0;
        for (i=1; i<dimen; i++) {
            switch (schedPolicy) {
                case DE_PREEMPTIVE:
                case DE_NONPREEMPTIVE:
                case DE_FIFOPREEMPTIVE:
                case DE_FIFONONPREEMPTIVE:
                    if (getDERCStar(sortArray[i])->priority > topPrio) {
                        topLink = sortArray[i];
                        topPrio = getDERCStar(topLink)->priority;
                        earliestToA = getDERCStar(topLink)->timeOfArrival;
                        topDist = getDERCStar(topLink)->index() - (*(getDERCStar(topLink)->lastFiredPr));
                        topDepth = ((DEPortHole*)(((Event*)topLink->e)->dest))->depth;
                        indexOftop = i;
                        break;
                    }
                    if (getDERCStar(sortArray[i])->priority < topPrio) {
                        break;
                    }
                case DE_ROUNDROBIN:
                case DE_FIFO:
                    switch (schedPolicy) {
                        case DE_PREEMPTIVE:
                        case DE_NONPREEMPTIVE:
                        case DE_ROUNDROBIN:
                            tempDist = getDERCStar(sortArray[i])->index() - (*(getDERCStar(sortArray[i])->lastFiredPr));
                            if (( topDist <= 0 && tempDist > 0 ) ||
                                ( tempDist < topDist && !(topDist > 0 && tempDist <= 0))) {
                                topLink = sortArray[i];
                                topPrio = getDERCStar(topLink)->priority;
                                earliestToA = getDERCStar(topLink)->timeOfArrival;
                                topDist = getDERCStar(topLink)->index() - (*(getDERCStar(topLink)->lastFiredPr));
                                topDepth = ((DEPortHole*)(((Event*)topLink->e)->dest))->depth;
                                indexOftop = i;
                            }
                            break;
                        case DE_FIFOPREEMPTIVE:
                        case DE_FIFONONPREEMPTIVE:
                        case DE_FIFO:
                            if (getDERCStar(sortArray[i])->timeOfArrival < earliestToA) {
                                topLink = sortArray[i];
                                topPrio = getDERCStar(topLink)->priority;
                                earliestToA = getDERCStar(topLink)->timeOfArrival;
                                topDist = getDERCStar(topLink)->index() - (*(getDERCStar(topLink)->lastFiredPr));
                                topDepth = ((DEPortHole*)(((Event*)topLink->e)->dest))->depth;
                                indexOftop = i;
                                break;
                            }
                            if (getDERCStar(sortArray[i])->timeOfArrival == earliestToA && 
                               ((DEPortHole*)(((Event*)sortArray[i]->e)->dest))->depth < topDepth) {
                                topLink = sortArray[i];
                                topPrio = getDERCStar(topLink)->priority;
                                earliestToA = getDERCStar(topLink)->timeOfArrival;
                                topDist = getDERCStar(topLink)->index() - (*(getDERCStar(topLink)->lastFiredPr));
                                topDepth = ((DEPortHole*)(((Event*)topLink->e)->dest))->depth;
                                indexOftop = i;
                            }
                            break;
                        default:
                            Error::abortRun("Invalid scheduling policy!");
                            return FALSE;
                    }
                    break;
                default:
                    Error::abortRun("Invalid scheduling policy!");
                    return FALSE;
            }
        }

        // Now have the current link which should get the resource
        // See if this link can access the Resource
        access = canAccessResource(topLink);
        if (access) {
            // Can access Resource!
            // Need to make available any other Events for the Star
            
            DERCStar* starUsingResource = getDERCStar(topLink);
            starUsingResource->arrivalTime = now;
            DERCStar* dest;
            
            starUsingResource->startNewPhase();
            
            for (i=0; i<dimen; i++){
                DEPortHole* port = (DEPortHole*)(((Event*)sortArray[i]->e)->dest);
                dest = getDERCStar(sortArray[i]);
                if (starUsingResource == dest) {
                    if (port->isItOutput()) {
                        Error::abortRun("Terminal goes to wormhole!!!");
                        return FALSE;
                    } else {
                        //success is True only for the first event at the given
                        //time for this port
                        int success = ((InDEPort*) port)->getFromQueue(((Event*)sortArray[i]->e)->p);
                        // Flag this Event from the Array & free memory
                        eventQ->putFreeLink(sortArray[i]);
                        sortArray[i] = 0;
                        
                        // Now write to the overflow file if any events have been missed
                        if (!success) {
                            char stemp[1024];
                            DERCStar* s = starUsingResource;
                            sprintf( stemp, "%s: %f %s\n", (const char*)((DEStar*)s)->fullName(), now, (const char *) ((DEPortHole*)port)->name());
                            ((DERCStar*)starUsingResource)->Printoverflow( stemp );
                        }
                    }
                }
            }
         
            // Fire the star. The Star will store its output events 
            if (!starUsingResource->run()) {
		LOG_DEL; delete [] sortArray;
		return FALSE;
	      }
            // Now update the Resources SequentialList of executing stars
            updateDelay = starUsingResource->getDelay();
        
            if (updateDelay > 0.0) {
                // First update stars waiting to use this resource with the 
                // delay of the current Star
                ListIter oldStarList(*intStarList);
                ResLLCell* p;
                while ((p = (ResLLCell*)oldStarList++) != 0) {
                    double oldTime = p->ECT;
                    p->ECT = oldTime + updateDelay;
                    // Update also all the output events of the
                    // preempted star.
                    ListIter listEvents(*((p->star)->emittedEvents));
                    StarLLCell *q;
                    while (( q = (StarLLCell*)listEvents++) != 0 ) {
                        q->time = q->time + updateDelay;
                    }
                }    
                this->timeWhenFree = ((StarLLCell*)(starUsingResource)->emittedEvents->tail())->time;

                // now add the firing star to the resources list of executing stars
                LOG_NEW; p = new ResLLCell(starUsingResource, (this->timeWhenFree), starUsingResource->priority);
                intStarList->prepend(p);
            }
        }

        // Now reschedule the links remained in the sorted Array
        if (access && updateDelay == 0.0) {
            for (i=0; i<dimen; i++) {
                if (sortArray[i] != 0) {
                    eventQ->pushBack(sortArray[i]);
                }
            }
        } else {
            for (i=0; i<dimen; i++) {
                // Events which have been already delivered are marked with 0
                if (sortArray[i] != 0) {
                    // Reschedule each event at the appropriate time by 
                    // looking at the Resources Linked List to see all 
                    // the interrupted Stars ahead of it
                    nextTry = -1;
                    switch (schedPolicy) {
                        case DE_FIFO:
                        case DE_FIFONONPREEMPTIVE:
                        case DE_ROUNDROBIN:
                        case DE_NONPREEMPTIVE:
                            nextTry = ((ResLLCell*) intStarList->head())->ECT;
                            break;
                        case DE_FIFOPREEMPTIVE:
                        case DE_PREEMPTIVE:
                            ListIter getAppECT(*intStarList);
                            ResLLCell* p;
                            while ((p = (ResLLCell*) getAppECT++) != 0) {
                                if (p->priority >= getDERCStar(sortArray[i])->priority) {
                                    nextTry = p->ECT;
                                } else {
                                    break;
                                }
                                if (nextTry == -1) {
                                    Error::abortRun("SW Event never got rescheduled in NewEventFromEventQ()");
                                    return FALSE;
                                }
                            }
                    }
                    sortArray[i]->level = nextTry;
                    eventQ->pushBack(sortArray[i]);
                }
            }
        }
    } else {
        //////////////////////////////////////////////////////
        // Star is HW!!!
        //////////////////////////////////////////////////////
        if (this->timeWhenFree > now) {
            // Resource is not free
            // Reschedule the Links. Remember all such Events use Star
            // for same time (same freq)
            for (i=0 ; i<dimen ; i++) {
                sortArray[i]->level = timeWhenFree;
                eventQ->pushBack(sortArray[i]);
            }
        } else {
            // Can access Resource!
            // Need to make available any other Events for the Star
            DERCStar* starUsingResource = getDERCStar(link);
            starUsingResource->arrivalTime = now;
            DERCStar* destStar;
            
            starUsingResource->startNewPhase();
            for (i=0 ; i<dimen ; i++){
                DEPortHole* port = (DEPortHole*)(((Event*)sortArray[i]->e)->dest);
                destStar = getDERCStar(sortArray[i]);
                assert(starUsingResource == destStar);
                if (port->isItOutput()) {
                    Error::abortRun("Terminal goes to wormhole!!!");
                    return FALSE;
                } else {
                    int success = ((InDEPort*)port)->getFromQueue(((Event*)sortArray[i]->e)->p);
                    eventQ->putFreeLink(sortArray[i]);
                    if (!success) {
                        char stemp[1024];
                        DERCStar* s = starUsingResource;
                        sprintf( stemp, "%s: %f %s\n", (const char*)((DEStar*)s)->fullName(), now, (const char *) ((DEPortHole*)port)->name());
                        ((DERCStar*)starUsingResource)->Printoverflow( stemp );
                    }
                }
            }
            
            // fire the star. A HW Star writes its Events directly to the eventQ
            if (!starUsingResource->run()) {
		LOG_DEL; delete [] sortArray;
		return FALSE;
	      }

            nextTry = now + starUsingResource->getDelay();
            this->timeWhenFree = nextTry;
        }
    }
    LOG_DEL; delete [] sortArray;
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////
// This method returns a SequentialList of all other current Links in the
// eventQ, and the passed link, who also want to access this resource at
// this time. However, if one of the links at the current time are on a 
// feedbackIn port and its value is 0.0, it is the only link returned. This
// is because we assume the feebbackIn port with value 0.0 is *only* used in
// DERCStars for signalling that an event is ready to be emitted.
///////////////////////////////////////////////////////////////////////////

SequentialList* Resource :: getOtherLinks(CqLevelLink* f, double now) {
  
    LOG_NEW; SequentialList* sortList = new SequentialList();
    sortList->prepend(f);

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
        // Now add these events to a linked list temporarily      
        sortList->append(h);
    }
    // Now go through this SequentialList and pushback any events not looking 
    // for this resource. sortList is used for this in a circular fashion,
    // ie set a pointer to the top event and go through list until reach it 
    // again. Any events using this resource get appended to the tail of the 
    // list. However if any link has an event on a feedbackIn port, this is 
    // the only link returned.
    CqLevelLink* topLink = (CqLevelLink*)sortList->getAndRemove();
    sortList->append(topLink); // Set marker to event at end of list
    CqLevelLink* nextLink;  
    CqLevelLink* feedbackInLink = NULL;
    do {
        nextLink = (CqLevelLink*)sortList->getAndRemove();
        if (((DERCStar*)&((Event*)nextLink->e)->dest->parent()->asStar())->resourcePointer != this){
            // not for this resource, so put back in queue
            eventQ->pushBack(nextLink); 
        } else {  
            if (getDERCStar(nextLink)->feedbackIn == ((Event*)nextLink->e)->dest) {
                if ( int(*((((Event*)nextLink->e)->p))) == 0 ) {
                    feedbackInLink = nextLink;
                    break;
                }
            }
            sortList->append(nextLink);
        }
    } while ( nextLink != topLink );

    // Now check to see if any of the links are for an event on a 
    // feedbackIn port
    if (feedbackInLink != NULL) {
        while ((nextLink = (CqLevelLink*)sortList->getAndRemove()) != 0) {
            eventQ->pushBack(nextLink);
        }
        sortList->append(feedbackInLink);
    }
   
    return sortList;
}


//////////////////////////////////////////////////////////////
// Used to check if the current Link can gain access to
// the Resource. Returns 1 if it can, else 0
//////////////////////////////////////////////////////////////

int Resource :: canAccessResource(CqLevelLink* newLink) {
    // if resource is not currently being used, can access it!
    if (intStarList->size() == 0) {
        return 1;
    }
    
    if (timeWhenFree == -1) {
        return 1;
    }   
    if (schedPolicy == DE_FIFO || schedPolicy == DE_FIFONONPREEMPTIVE ||
        schedPolicy == DE_ROUNDROBIN || schedPolicy == DE_NONPREEMPTIVE) {
            return 0;
    }
    // Resource is not free, so see if new Event can preempt the  currently
    // executing one
    if (schedPolicy != DE_PREEMPTIVE && schedPolicy != DE_FIFOPREEMPTIVE) {
        Error::abortRun("invalid scheduling policy!");
        return 0;
    }
    ResLLCell* topCell = (ResLLCell*)intStarList->head();

    // newPriority > oldPriority ?
    if (getDERCStar(newLink)->priority > topCell->priority) {
        return 1; //  pre-empting!
    } else {
        return 0;
    }
}


/* Called when a star is finished using this resource. Primarily
 * called when when a DERCStar sees a dummy event.
 * It removes the star from the intStarList and also updates the
 * timeWhenFree variable of the REsource
 */
void Resource :: removeFinishedStar(DERCStar* star) {
    ResLLCell* p = (ResLLCell*) intStarList->getAndRemove();
    assert( star == p->star); // finished star must be at top of intStarList

    // Added by Claudio Passerone 09/24/1998
    // Update last fired star
    *(star->lastFiredPr) = star->index();
    if (intStarList->size() == 0 ) {
        timeWhenFree = -1;
    } else {
        p =  (ResLLCell*) intStarList->tail();
        timeWhenFree = p->ECT;
    }
    p->~ResLLCell();
	LOG_DEL; delete p;
}

ResLLCell* Resource :: getTopCell() {
    // warning: returns 0 if intStarList is empty
    ResLLCell* p = (ResLLCell*) intStarList->head();
    if ( p ==0 ) {
        Error::abortRun("resource does not currently have any stars using it");
    }
    return p;
}

double Resource :: getECT(DERCStar* star) {
    ListIter getAppECT(*intStarList);
    ResLLCell* p;
    while ((p = (ResLLCell*)getAppECT++) != 0) {
        if (p->star == star) {
            return p->ECT;
        }
    }
    Error::abortRun("star is not currently in the intStarList of the resource");
    return -1;
}

/////////////////////////////////////////////////////////////
// used to get the destination of the event as a DERC Star
/////////////////////////////////////////////////////////////
DERCStar* Resource :: getDERCStar(CqLevelLink* link) {
    DERCStar* temp = ((DERCStar*)&(((Event*)link->e)->dest->parent()->asStar()));
    return temp;
}

// For some reason, under Cygwin32, we need to the body in the .cc file
ResLLCell :: ResLLCell( DERCStar* s, double time, int prio) {
    star = s;
    ECT = time;    // Expected Completion Time
    priority = prio;
}
