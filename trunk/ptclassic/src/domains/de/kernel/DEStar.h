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

#include "type.h"
#include "DEConnect.h"
#include "Particle.h"
#include "Output.h"
#include "Star.h"
#include "IntState.h"

	////////////////////////////////////
	// class DEStar
	////////////////////////////////////

class DEStar : public Star {

public:
	// initialize domain-specific members
	void prepareForScheduling();

	// define firing
	void fire();

	// my domain
	const char* domain() const;

	// constructor
	DEStar() : delayType(FALSE) {}

	// Redefine method setting internal data in the Block
	// so that various DE-specific initilizations can be performed.
	// If the parent pointer is not provied, it defaults to NULL
	Block& setBlock(char* starName, Block* parent = NULL);

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
