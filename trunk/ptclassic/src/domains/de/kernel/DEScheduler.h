#ifndef _DEScheduler_h
#define _DEScheduler_h 1

#include "type.h"
#include "Galaxy.h"
#include "SpaceWalk.h"
#include "DEStar.h"
#include "PriorityQueue.h"
#include "Scheduler.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 5/30/90
 Revisions: 1. add a set of properties related to the "timed" nature of
	       the domain (amITimed(), nextTime(), stopAfterOutput,
	       stopBeforeDeadlock, processQ) (10/24/90).

 DE Scheduler for single computer is implemented.

 ********************************************************************/
 

	////////////////////////////
	// DEScheduler
	////////////////////////////


class DEScheduler : public Scheduler {

	// stoping condition of the scheduler
	float stopTime;

	// set the depth of the DEStars.
	int setDepth(DEStar*);
public:
	// The global event queue is implemented as a priority queue
	// in DE scheduler.
	PriorityQueue eventQ;

	// process queue 
	PriorityQueue processQ; // If a timed domain is in the DEWormhole,
				// the wormhole is regarded as a process.
				// Then, without input events, it will be
				// fired at the specified time.

	// Set up the stopping condition.
	void setStopTime(float limit) {stopTime = limit ;}

	// The setup function initialize the global event queue
	// and notify the blockes of the queue name.
	int setup(Block& galaxy);

	// The run function resumes the run where it was left off.
	int run(Block& galaxy);

	// Display the static schedule for SDF wormholes...
	StringList displaySchedule(); 

	// Constructor sets default options
	DEScheduler () { stopTime = 100.0;
			 stopAfterOutput = FALSE;
			 stopBeforeDeadlocked = FALSE ;}

	// output the stopTime
	float whenStop() { return stopTime ;}

	// relative time scale to the outer domain when it is a inner domain.
	float relTimeScale;

	// special methods since DE is a "timed" domain
	int amITimed();
	float nextTime();	// return when the next event is scheduled.
	int stopAfterOutput;	// flag, if set, stop when it generates an
				// output inside a Wormhole even before
				// the stopTime. 
				// (for timed-timed domain interface).
	int stopBeforeDeadlocked; // flag, set if the scheduler stops while
				// events are stored in the eventQ.
};

#endif
