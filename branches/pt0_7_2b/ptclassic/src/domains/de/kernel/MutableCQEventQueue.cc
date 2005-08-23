static const char file_id[] = "MutableCQEventQueue.cc";
/**************************************************************************
Version identification:
@(#)MutableCQEventQueue.cc	1.3 12/16/97

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

 Programmer:  Soonhoi Ha
 Date of creation: 10/11/91
 Revisions:

This file contains member functions for EventQueue..

******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "MutableCQEventQueue.h"
#include "Particle.h"

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

void MutableCQEventQueue :: initialize() {
	// first maintain free links and free events.
	clearFreeEvents();
	MutableCalendarQueue :: initialize();
	clearParticles();
}

MutableCQEventQueue :: ~MutableCQEventQueue() {
	initialize();
	clearFreeEvents();
}
