/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 5/30/90
 Date of revision: 8/28/90

 Definition of DEStar and DERepeatStar (which fires itself repeatedly).
 Revisoin : add DEPriorityStar.

*******************************************************************/
#ifndef _DEStar_h
#define _DEStar_h 1

#include "type.h"
#include "DEConnect.h"
#include "Particle.h"
#include "Output.h"
#include "Star.h"
#include "PriorityQueue.h"
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

};

	////////////////////////////////////
	// class DERepeatStar
	////////////////////////////////////

class DERepeatStar : public DEStar {

protected:
	// specify the feedback links to fire itself.
	InDEPort feedbackIn;
	OutDEPort feedbackOut;

	// access the feedback arc.
	void refireAtTime(float when);	// send next event.
	int canGetFired();		// can it be fired?

public:
	// define the common start function for this class
	// completionTime should be setup beforehand.
	void start();

	// constructor
	DERepeatStar();
};

	////////////////////////////////////
	// class DEPriorityStar
	////////////////////////////////////

class DEPriorityStar : public DERepeatStar {

protected:
	int numEvent;		// number of queued event

	Queue* inQueue;		// queue for each input ports
	IntState queueSize;	// size of inQueue

	PriorityQueue priorityList;	// priority list of input ports

public:
	// setup priorityList
	void priority(InDEPort& p, int level)
		{priorityList.levelput(&p, float(level)) ;}

	// define start() method to initialize queues, etc.
	void start();

	// when an event is coming while the star is executing the previous
	// event, queue that event and return FALSE.
	// If the incoming event is runnable, return TRUE.
	int isItRunnable();

	// get the particle associated with an input port
	Particle* fetchData(InDEPort& p);

	// go to the next execution. Usually called at the end of go().
	void goToNext() {if (numEvent > 0) refireAtTime(completionTime) ;}

	// constructor
	DEPriorityStar();
};

#endif
