/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY

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
