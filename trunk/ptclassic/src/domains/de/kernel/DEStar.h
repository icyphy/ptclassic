/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

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
#include "DEConnect.h"
#include "Particle.h"
#include "Star.h"

enum FiringMode {SIMPLE, PHASE};
class EventQueue;

	////////////////////////////////////
	// class DEStar
	////////////////////////////////////

class DEStar : public Star {
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

public:
	// initialize domain-specific members
	void prepareForScheduling();

	// define firing
	int fire();

	// send output events to the global event queue.
	void sendOutput();

	// class identification
	int isA(const char*) const;

	// my domain
	const char* domain() const;

	// prepare a new phase of firing.
	virtual void startNewPhase();

	// constructor
	DEStar() : delayType(FALSE), mode(SIMPLE) { arrivalTime = 0.0;
				      completionTime = 0.0; }
	
	// Store the completion time of the current execution, which in turn
	// the next free time. But, it may not be the start time of the next
	// execution since some users may allow several executions 
	// time-overlapped. The start time of the next execution will be
	// either event-arriveTime or MAX { availTime, event-arriveTime },
	// which is user's choice
	// It will be set by go() method in each  star definition.
	float completionTime;
	float arrivalTime;

	// Pointer to the event queue of the universe or warmhole
	// in which the DEstar is. It is set by the Scheduler :
	// initialize().
	EventQueue *eventQ;

        // Flag to indicate whether it is a delay-type star or not.
        // If a star simulates the latency, it is called delay-type.
        // Examples : Delay, UniDelay, Server, etc...
        // The proper place to set this flag will be "constructor".
        // For wormhole case, it is set in the start() method depending on
        // whether the inside domain is timed or untimed.
        int delayType;
};

#endif
