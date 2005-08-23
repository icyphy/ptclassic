/******************************************************************
Version identification:
@(#)DEStar.h	1.31 03/27/98

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
 Date of creation: 5/30/90

 Revised 10/2/90 -- DERepeatStar now have its own .h files.

*******************************************************************/
#ifndef _DEStar_h
#define _DEStar_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "type.h"
#include "IntState.h"
#include "DEPortHole.h"
#include "Particle.h"
#include "Star.h"
#include "LinkedList.h"
#include "PendingEventList.h"
#include "CalendarQueue.h"

enum FiringMode {SIMPLE, PHASE};
class BasePrioQueue;
class MutableCQEventQueue;
class MutableCalendarQueue;
class DEDynMapBase;

	////////////////////////////////////
	// class DEStar
	////////////////////////////////////

class DEStar : public Star {

	friend MutableCQEventQueue;
	friend MutableCalendarQueue;
	friend DEDynMapBase;

public:
	// initialize domain-specific members (+ std initialize)
	/* virtual */ void initialize();

	// define firing
	/* virtual */ int run();

	// send output events to the global event queue.
        // declare inline so that it can be inlined into DEStar::run,
        // the primary call point
        inline void sendOutput() {
            BlockPortIter next(*this);
            PortHole* p;
            while ((p = next++) != 0) {
		p->sendData();
            }
        }

	// class identification
	/* virtual */ int isA(const char*) const;

	// my domain
	/* virtual */ const char* domain() const;

	// prepare a new phase of firing.
	virtual void startNewPhase();

	// Return TRUE if this star is mutable
	int isMutable();

	// Make this star mutable.
	void makeMutable();

	// constructor
	DEStar();

	// destructor
	~DEStar();

	// Store the completion time of the current execution, which in turn
	// the next free time. But, it may not be the start time of the next
	// execution since some users may allow several executions 
	// time-overlapped. The start time of the next execution will be
	// either event-arriveTime or MAX { availTime, event-arriveTime },
	// which is user's choice
	// It will be set by go() method in each  star definition.
	double completionTime;
	double arrivalTime;
	int isRCStar;

	// Pointer to the event queue of the universe or warmhole
	// in which the DEstar is. It is set by the Scheduler :
	// initialize().
	BasePrioQueue *eventQ;

        // Flag to indicate whether it is a delay-type star or not.
        // If a star simulates the latency, it is called delay-type.
        // Examples : Delay, UniDelay, Server, etc...
        // The proper place to set this flag will be "constructor".
        // For wormhole case, it is set in the start() method depending on
        // whether the inside domain is timed or untimed.
        int delayType;
protected:
	FiringMode mode;

	// set mode. If mode = PHASE, create inQue for its input portholes.
	// PHASE mode:
	// Get all simultaneous events in its input portholes at one time.
	// Then, the number of firing will be reduced as many as the number of
	// simultaneous input events on a porthole.
	// SIMPLE mode:
	// A special case of PHASE mode where the size of InQue of input
	// portholes is one, so unneccesary.
	void setMode(FiringMode m) { mode = m; }

private:
	// This variable is set to true if the star is mutable
	int _mutabilitySet;

	// List of Pending Events Destined for this Star
	PendingEventList *_pendingEventList;

	// Remove one of this star's pending events
	CqLevelLink * removePendingEvent(Link *);

	// Clear this Star's Pending Event List and Erase 
	// Events from the MutableCalendarQueue
	void clearAllPendingEvents();

	// Clear Pending Events that are destined for this
        // Star via a particular PortHole. Erase the corresponding
	// Events from the MutableCalendarQueue
	void clearPendingEventsOfPortHole(DEPortHole *);

	// Add an event to this stars's pending event list.
	// CqLevelLinks are used as the stored elements on
	// the list. Note that this method _appends_ the
	// pending event list.
	Link* addToPendingEventList(CqLevelLink*);

};

#endif



