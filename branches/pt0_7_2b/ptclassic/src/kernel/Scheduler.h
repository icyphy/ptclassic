#ifndef _Scheduler_h
#define _Scheduler_h 1

#ifdef __GNUG__
#pragma interface
#endif
#include "StringList.h"
#include "SimControl.h"

/**************************************************************************
Version identification:
@(#)Scheduler.h	2.20    02/22/99

Copyright (c) 1990-1999 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

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
	Scheduler() :  stopBeforeDeadFlag(0), myTarget(0), myGalaxy(0) {}

	// connect a galaxy to the scheduler
	virtual void setGalaxy(Galaxy& g);

	Galaxy* galaxy() { return myGalaxy;}

	// setup sets up the schedule; it calls requestHalt for failure.

	virtual void setup() = 0;

	// run runs the simulation.
	virtual int run() = 0;

	// hack method to set stopping time
	virtual void setStopTime(double limit) = 0;

	// method to retrieve stopping time
	virtual double getStopTime() = 0;

	// hack method to set stopping time of wormhole
	virtual void resetStopTime(double limit);

	// set the currentTime 
	virtual void setCurrentTime(double val);

	// display schedule
	virtual StringList displaySchedule();

	// make destructor virtual
	virtual ~Scheduler() {}

	// current time of the schedule
	double now() const { return currentTime;}

	// flag set if stop before deadlocked.
	// for untimed domain, it is always FALSE.
	int stopBeforeDeadlocked() const { return stopBeforeDeadFlag;}

	// identify itself
	virtual const char* domain() const;

	// set the target
	void setTarget(Target& t);

	// get the target
	Target& target ();

	// Call code-generation functions in the Target to generate
	// code for a run.
	// In the base class, this just causes an error.
	virtual void compileRun();

	// The following functions now forward requests to SimControl,
	// which is responsible for controlling the simulation.

	// requestHalt, for backward compatibility
	// NOTE: SimControl::requestHalt only sets the halt bit,
	// not the error bit.
	static void requestHalt() { SimControl::declareErrorHalt();}

	// haltRequested, for backward compatibilty
	static int haltRequested() { return SimControl::haltRequested();}

	// For a target within a Wormhole, if this method returns TRUE
	// and the outside domain is timed, then the target will be
	// fired again at the time returned by the nextFiringTime method.
	// In this base class, always return FALSE.
	virtual int selfFiringRequested() { return FALSE; }
	
	// If selfFiringRequested returns TRUE, return the time at which
	// this firing is requested.
	virtual double nextFiringTime() { return 0.0; }

        // class identification
        virtual int isA(const char*) const;
protected:
	// the current time
	double currentTime;

	// flag used if stopped before deadlocked.
	int stopBeforeDeadFlag;

	// clearHalt, for backward compatibilty
	static void clearHalt() { SimControl::clearHalt();}

private:
	// Target pointer
	Target* myTarget;
	Galaxy* myGalaxy;
};

#endif
