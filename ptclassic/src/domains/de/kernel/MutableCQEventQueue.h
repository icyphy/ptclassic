/* 
Version Identification:
@(#)MutableCQEventQueue.h	1.7 03/18/98

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

#ifndef _MutableCQEventQueue_h
#define _MutableCQEventQueue_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "MutableCalendarQueue.h"
#include "EventQueue.h"

class Particle;
class PortHole;
class PendingEventList;
class MutableCQScheduler;

        //////////////////////////////////////////
        // class Event and EventQueue
        //////////////////////////////////////////

class MutableCQEventQueue : public MutableCalendarQueue
{
	friend PendingEventList;
	friend MutableCQScheduler;

public:
	void pushHead(Particle* p, PortHole* ph, double v, double fv) {
            Event* temp = getEvent(p, ph);
	    Star *dest = NULL;
	    PortHole* tl = 0;
	    if (fv != 0) {
		tl = temp->dest;
		if (tl != NULL)
			dest = &tl->parent()->asStar();
		// ELSE dest IS ALREADY INITIALIZED TO NULL
	    } else {
		dest = (Star*) temp;
	    }
            levelput(temp, v, fv, dest);
	}
	void pushTail(Particle* p, PortHole* ph, double v, double fv) {
	    pushHead(p, ph, v, fv);
	}


 // The following takes care of the case where levelput is called
 // directly without going thru pushHead. In that case it is called
 // with three arguments and dest must be internally calculated.

	LevelLink* levelput(Pointer a, double v, double fv) {
	     Star *dest= NULL;
	     PortHole* tl = 0;
	     if (fv != 0) {
		 tl = ((Event *)a)->dest;
		 if (tl != NULL)
		       dest = &tl->parent()->asStar();
		 // else  dest is already NULL; 
	     } else {
		 dest = (Star*) a;
	     }
	     return levelput(a, v, fv, dest);
	}

	CqLevelLink* levelput(Pointer a, double v, double fv, Star* dest) {
	    CqLevelLink* link; 
	    
	    link = MutableCalendarQueue::levelput(a, v, fv, dest);

	    return link;
	}

	void decrementEventCount() {
	    cq_eventNum--;
	}

	void putFreeLink(CqLevelLink* p); // virtual

	// put the residual events and particles into the free stores.
	void initialize();

	MutableCQEventQueue() : freeEventHead(0) {}
	~MutableCQEventQueue();
private:
	Event*  freeEventHead;
	Event*  getEvent(Particle*, PortHole*);
	void putEvent(Event* e) {
	        e->next = freeEventHead;
	        freeEventHead = e;
             }
	void clearFreeEvents();
	void clearParticles();
};

#endif
