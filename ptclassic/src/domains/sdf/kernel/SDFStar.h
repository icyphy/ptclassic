#ifndef _SDFStar_h
#define _SDFStar_h 1
#ifdef __GNUG__
#pragma interface
#endif

/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 5/29/90

 SDFStar is a Star that runs under the SDF Scheduler.
 Formerly defined in Star.h

*******************************************************************/

#include "Star.h"
#include "Fraction.h"
#include "SDFConnect.h"
#include "Particle.h"

class EGNode;

	////////////////////////////////////
	// class DataFlowStar
	////////////////////////////////////

// This abstract class includes all dataflow stars.  Eventually more
// will be here.

class DataFlowStar : public Star {
public:
	int isA(const char*) const;
};

	////////////////////////////////////
	// class SDFStar
	////////////////////////////////////

class SDFStar : public DataFlowStar  {
private:
        // pointer to master of instances for an expanded graph
	// TEMPORARY: these kinds of things don't belong here.
        EGNode *master;

protected:
	// During scheduling, the scheduler must keep track of how
	// many times it has scheduled a star.  This is a convenient
	// place to do that.
	unsigned noTimes;

public:
	// constructor
	SDFStar() {
		repetitions = 0;
		noTimes = 0;
	}

	// my expanded graph master (temporary)
	EGNode *myMaster() { return master;}

        // set the expanded graph master
        void setMaster(EGNode *m) {master = m;}

	// my domain
	const char* domain() const;

	// domain specific initialization
	void prepareForScheduling();

	// functions for use by scheduler

	// notRunnable: returns 0 if the star can be run now,
	// 1 if it can't be run now but should be runnable later,
	// and 2 if it has been run as often as it needs to be.
	int notRunnable();

	// deferrable: returns TRUE if we can defer the star,
	// FALSE if we cannot.
	int deferrable();

	// runCost: cost of executing the star.  Can be used in deciding
	// which eligible star to run.
	virtual unsigned runCost();

	// simulate execution of the star, as part of schedule computation.
	// returns 0 if the star was run successfully;
	// 1 if it cannot be run at present;
	// 2 if has already been run as often as it needs to be;
	// 3 if its execution is to be deferred (only if deferFiring is true)
	virtual int simRunStar(int deferFiring);

	// move data and execute the go function
	void fire();

	// class identification
	int isA(const char*) const;

	// The number of repetitions of the star in a periodic
	// schedule.  Initialized to 0 by constructor.  Set to correct
	// value by an SDF scheduler.  It is represented as a fraction
	// for the convenience of the scheduler.
	Fraction repetitions;

	// Execution time, for schedulers that use it
	virtual int myExecTime();
};

class SDFStarPortIter : public BlockPortIter {
public:
	SDFStarPortIter(SDFStar& s) : BlockPortIter(s) {}
	SDFPortHole* next() { return (SDFPortHole*)BlockPortIter::next();}
	SDFPortHole* operator++() { return next();}
};

#endif
