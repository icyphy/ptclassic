#ifndef _Scheduler_h
#define _Scheduler_h 1

#ifdef __GNUG__
#pragma once
#pragma interface
#endif
#include "StringList.h"

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

	// hack method to set stopping time of wormhole
	virtual void resetStopTime(float limit);

	// set the currentTime 
	virtual void setCurrentTime(float val);

	// display schedule
	virtual StringList displaySchedule();

	// make destructor virtual
	virtual ~Scheduler() {}

	// allow errorhandler to ask for a halt
	static void requestHalt() { haltRequestFlag++;}

	// read the halt flag
	static int haltRequested() { return haltRequestFlag;}

	// current time of the schedule
	float currentTime;

	// flag set if stop before deadlocked.
	// for untimed domain, it is always FALSE.
	int stopBeforeDeadlocked;

	// identify itself
	virtual const char* domain() const;
protected:

	// request flag for halting -- schedulers must poll and reset
	static int haltRequestFlag;
};

#endif
