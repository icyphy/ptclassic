/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 10/2/90 (formerly part of DEStar)

DEPriorityStar.

*******************************************************************/

#ifndef _DEPriorityStar_h
#define _DEPriorityStar_h 1

#include "DERepeatStar.h"
#include "PriorityQueue.h"

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
