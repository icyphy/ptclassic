#ifndef _Scheduler_h
#define _Scheduler_h 1

#include "SpaceWalk.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 1/17/90
 Revisions: 5/29/90 -- SDF-specific stuff moved to SDFScheduler.h

 Scheduler sets up the execution by determining the order in which
 Blocks are executed. This is done in two phases -- setup and
 run

 This file just defines the virtual base class.

**************************************************************************/

	////////////////////////////
	// Scheduler
	////////////////////////////

class Block;

class Scheduler {
public:
	// setup sets up the schedule; it returns TRUE for success
	// and FALSE for failure.
	virtual int setup(Block& galaxy) = 0;

	// run runs the simulation.
	virtual int run(Block& galaxy) = 0;

	// hack method to set stopping time
	virtual void setStopTime(float limit) = 0;

	// display schedule
	virtual StringList displaySchedule() { return "not implemented\n";}

	// make destructor virtual
	virtual ~Scheduler() {}

	// allow errorhandler to ask for a halt
	static void requestHalt() { haltRequestFlag++;}

	// read the halt flag
	static int haltRequested() { return haltRequestFlag;}
protected:
	// The following member is used to visit all atomic blocks
	// (stars and wormholes) in the galaxy exacly once each
	// It must be initialized in the setup() method.
	SpaceWalk alanShepard;

	// request flag for halting -- schedulers must poll and reset
	static int haltRequestFlag;
};

#endif
