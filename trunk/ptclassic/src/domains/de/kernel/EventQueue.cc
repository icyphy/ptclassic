static const char file_id[] = "EventQueue.cc";
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 10/11/91
 Revisions:

This file contains member functions for EventQueue..

******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "EventQueue.h"

Event* EventQueue:: getEvent(Particle* p, PortHole* ph) {
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

void EventQueue:: clearFreeEvents() {
	if (!freeEventHead) return;
	Event* temp;
	while (freeEventHead->next) {
		temp = freeEventHead;
		freeEventHead = freeEventHead->next;
		LOG_DEL; delete temp;
	}
	LOG_DEL; delete freeEventHead;
}

void EventQueue:: putFreeLink(LevelLink* p) {
	if (p->fineLevel) {
		Event* temp = (Event*) p->e;
		putEvent(temp);
	}
	PriorityQueue:: putFreeLink(p);
}
