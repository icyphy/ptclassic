#ifndef _Scheduler_h
#define _Scheduler_h 1

#ifdef __GNUG__
#pragma interface
#endif
#include "StringList.h"
#include "SimControl.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 1/17/90

 Scheduler sets up the execution by determining the order in which
 Blocks are executed. This is done in two phases -- setup and
 run

 This file just defines the virtual base class.

**************************************************************************/

	////////////////////////////
	// Scheduler
	////////////////////////////

class Galaxy;
class Target;

class Scheduler {
public:
	// Constructor
	Scheduler() : myTarget(0) {}

	// setup sets up the schedule; it returns TRUE for success
	// and FALSE for failure.
	virtual int setup(Galaxy&) = 0;

	// run runs the simulation.
	virtual int run(Galaxy&) = 0;

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

	// current time of the schedule
	float currentTime;

	// flag set if stop before deadlocked.
	// for untimed domain, it is always FALSE.
	int stopBeforeDeadlocked;

	// identify itself
	virtual const char* domain() const;

	// set the target
	void setTarget(Target& t);

	// get the target
	Target& getTarget ();

	// Return a StringList with code that can be executed to
	// effect a run.  In the base class, this just causes an error.
	virtual StringList compileRun();

	// The following functions now forward requests to SimControl,
	// which is responsible for controlling the simulation.

	// requestHalt, for backward compatibility
	// NOTE: SimControl::requestHalt only sets the halt bit,
	// not the error bit.
	static void requestHalt() { SimControl::declareErrorHalt();}

	// haltRequested, for backward compatibilty
	static int haltRequested() { return SimControl::haltRequested();}

protected:
	// clearHalt, for backward compatibilty
	static void clearHalt() { SimControl::clearHalt();}

private:
	// Target pointer
	Target* myTarget;
};

#endif
