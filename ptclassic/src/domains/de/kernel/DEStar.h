/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 5/30/90

 Revised 10/2/90 -- DERepeatStar now have its own .h files.
 Revised 11/6/90 -- add two properties : depth, delayType.

*******************************************************************/
#ifndef _DEStar_h
#define _DEStar_h 1

#include "type.h"
#include "DEConnect.h"
#include "Particle.h"
#include "Output.h"
#include "Star.h"
#include "IntState.h"

class PriorityQueue;

	////////////////////////////////////
	// class DEStar
	////////////////////////////////////

class DEStar : public Star {
friend class DEScheduler;
protected:
	// Flag to indicate whether it is a delay-type star or not.
	// If a star simulates the latency, it is called delay-type.
	// Examples : Delay, UniDelay, Server, etc...
	// The proper place to set this flag will be "constructor".
	// For wormhole case, it is set in the start() method depending on
	// whether the inside domain is timed or untimed.
	int delayType;

public:
	// initialize domain-specific members
	void prepareForScheduling();

	// define firing
	void fire();

	// my domain
	const char* domain() const;

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
	PriorityQueue *eventQ;

	// The depth is the longest path from a star to any terminating star.
	// Terminating stars are those that have no output PortHoles, or
	// delay-type Stars, or those that are connected to the output
	// of the Wormhole.  To use depth as a fineLevel for PriorityQueue,
	// we do sign-reversal : depth = - longest path.  Therefore,
	// stars with smallest depth has the highest priority for scheduling
	// at the same global time.
 	int depth;
};

#endif
