/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 10/11/91
 Revisions:

This file contains definitions of two DE-specific classes:

	Event : entry for the global event queue.
		consists of destination PortHole and Particle.
	EventQueue : derived from PriorityQueue.

******************************************************************/
#ifndef _EventQueue_h
#define _EventQueue_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "PriorityQueue.h"
class Particle;
class PortHole;

        //////////////////////////////////////////
        // class Event and EventQueue
        //////////////////////////////////////////

class Event
{
public:	
	PortHole* dest;		// destination PortHole
	Particle* p;		// particle or event
	Event*	  next;		// form a event-list

	Event() : next(0) {}
};

class EventQueue : public PriorityQueue
{
public:
	void pushHead(Particle* p, PortHole* ph, double v, double fv) {
		Event* temp = getEvent(p, ph);
		leveltup(temp, v, fv);
	}

	void pushTail(Particle* p, PortHole* ph, double v, double fv) {
		Event* temp = getEvent(p, ph);
		levelput(temp, v, fv);
	}
	/* virtual */ void putFreeLink(LevelLink* p);

	// put the residual events and particles into the free stores.
	void initialize();

	EventQueue() : freeEventHead(0) {}
	~EventQueue();
private:
	Event*  freeEventHead;
	Event*  getEvent(Particle*, PortHole*);
	void    putEvent(Event* e) {
			e->next = freeEventHead;
			freeEventHead = e;
		}
	void clearFreeEvents();
	void clearParticles();
};

#endif
