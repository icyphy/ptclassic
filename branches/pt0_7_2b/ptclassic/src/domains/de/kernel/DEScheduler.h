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
 Revisions:

 DE Scheduler for single computer is implemented.

 ********************************************************************/
 

	////////////////////////////
	// DEScheduler
	////////////////////////////


class DEScheduler : public Scheduler {

	// stoping condition of the scheduler
	float stopTime;
public:
	// The global event queue is implemented as a priority queue
	// in DE scheduler.
	PriorityQueue eventQ;

	// Set up the stopping condition.
	void setStopTime(float limit) {stopTime = limit ;}

	// The setup function initialize the global event queue
	// and notify the blockes of the queue name.
	int setup(Block& galaxy);

	// The run function resumes the run where it was left off.
	int run(Block& galaxy);

	// The wrapup function runs the termination routines of all the
	// atomic blocks
	int wrapup(Block& galaxy);

	// Display the static schedule for SDF wormholes...
	StringList displaySchedule(Block& galaxy); 

	// Constructor sets default options
	DEScheduler () { stopTime = 100.0;}
};

#endif