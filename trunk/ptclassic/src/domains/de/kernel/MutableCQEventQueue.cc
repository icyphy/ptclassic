static const char file_id[] = "MutableCQEventQueue.cc";
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1997 The Regents of the University of California.
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

 Programmer: John Davis
 Date: 5/28/97
        The MutableCQEventQueue class parallels the CQEventQueue
        with the addition of mutable semantics.

**************************************************************************/


#ifdef __GNUG__
#pragma implementation
#endif

#include "MutableCQEventQueue.h"
// #include "Particle.h"

Event* MutableCQEventQueue:: getEvent(Particle* p, PortHole* ph) {
	Event* temp;
	// a free event instance
	if (freeEventHead) {
		temp = freeEventHead;
		freeEventHead  = temp->next;
	} else {
		LOG_NEW; temp = new Event;
	}
	// set the event members.
	temp->dest = ph;
	temp->p = p;
	return temp;
}

// delete the free events.
void MutableCQEventQueue:: clearFreeEvents() {
	if (!freeEventHead) return;
	while (freeEventHead->next) {
		Event* temp = freeEventHead;
		freeEventHead = freeEventHead->next;
		LOG_DEL; delete temp; 
	}
	LOG_DEL; delete freeEventHead;
	freeEventHead = 0;
}

// Put the unused particles into the plasmas.
void MutableCQEventQueue:: clearParticles() {
	Event* temp = freeEventHead;
	while (temp) {
		if (temp->p) temp->p->die();
		temp = temp->next;
	}
}

void MutableCQEventQueue:: putFreeLink(CqLevelLink* p) {
	if (p->fineLevel) {
		Event* temp = (Event*) p->e;
		putEvent(temp);
	}
	MutableCalendarQueue:: putFreeLink(p);
}

CqLevelLink * MutableCQEventQueue::createRefInDestinedEventList( CqLevelLink * link )
{
	// Create reference in DestinedEventList and point CqLevelLink
	// this reference. 
	// FIXME: Will there be problems with the semantics of this
	// statement; i.e., circular dependencies??
	DEStar *destinationStar = (DEStar*) (link->dest);
/* FIXME
	link->starPendingEventRef = 
                destinationStar->addRefToDestinedEventList( link ); 
*/
	return link;
}

void MutableCQEventQueue :: initialize() {
	int dummy = 0;
	// first maintain free links and free events.
	clearFreeEvents();
	while( dummy < 10 ) {
	    dummy++;
	}
	MutableCalendarQueue :: initialize();
	dummy = 0;
	while( dummy < 10 ) {
	    dummy++;
	}
	clearParticles();
}

MutableCQEventQueue :: ~MutableCQEventQueue() {
	initialize();
	clearFreeEvents();
}
