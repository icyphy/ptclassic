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
#include "SDFPortHole.h"
#include "Particle.h"

class EGNode;

	////////////////////////////////////
	// class DataFlowStar
	////////////////////////////////////

// This abstract class includes all dataflow stars.  Eventually more
// will be here.

class DataFlowStar : public Star {
public:
	// The number of repetitions of the star in a periodic
	// schedule.  Initialized to 0 by constructor.  Set to correct
	// value by an SDF scheduler.  It is represented as a fraction
	// for the convenience of the scheduler.
	// (can we make it protected)?
	Fraction repetitions;

	DataFlowStar();

	// notRunnable: returns 0 if the star can be run now,
	// 1 if it can't be run now but should be runnable later,
	// and 2 if it has been run as often as it needs to be.
	virtual int notRunnable();

	// deferrable: returns TRUE if we can defer the star,
	// FALSE if we cannot.
	virtual int deferrable();

	// runCost: cost of executing the star.  Can be used in deciding
	// which eligible star to run.
	virtual unsigned runCost();

	// Execution time, for schedulers that use it
	virtual int myExecTime();

	// initialize -- prepare for scheduling
	/* virtual */ void initialize();

	// Number of times this star is executed in the schedule
	int reps() const { return repetitions.num()/repetitions.den();}

	int isA(const char*) const;

	// tell if star is SDF in all contexts.  FALSE by default.
	virtual int isSDF() const;

	// tell if star is SDF in the current context
	// (e.g. it is BDF but conditions on all intra-cluster arcs match)
	// FALSE by default.
	virtual int isSDFinContext() const;

	// find source or sink stars quickly
	int isSource() const { return nInP == 0;}
	int isSink() const { return nInP == numberPorts();}
	int numInputs() const { return nInP;}
	int numOutputs() const { return numberPorts()-nInP;}

	// my expanded graph master (temporary)
	EGNode *myMaster() { return master;}

        // set the expanded graph master
        void setMaster(EGNode *m) {master = m;}

	// functions for use by scheduler.  These are for use in
	// SDF scheduling, e.g. when the stars are SDF or can be
	// treated as SDF for scheduling purposes.

	// simulate execution of the star, as part of schedule computation.
	// returns 0 if the star was run successfully;
	// 1 if it cannot be run at present;
	// 2 if has already been run as often as it needs to be;
	// 3 if its execution is to be deferred (only if deferFiring is true)
	virtual int simRunStar(int deferFiring);

	// move data and execute the go function
	int run();

protected:
	// During scheduling, the scheduler must keep track of how
	// many times it has scheduled a star.  This is a convenient
	// place to do that.
	unsigned noTimes;
private:
	// number of ports that are inputs
	int nInP;
        // pointer to master of instances for an expanded graph
	// TEMPORARY: these kinds of things don't belong here.
        EGNode *master;
};

	////////////////////////////////////
	// class SDFStar
	////////////////////////////////////

class SDFStar : public DataFlowStar  {
public:
	SDFStar() {}

	// identify self as SDF
	int isSDF() const;
	int isSDFinContext() const;

	// my domain
	const char* domain() const;

	// class identification
	int isA(const char*) const;

};

class SDFStarPortIter : public BlockPortIter {
public:
	SDFStarPortIter(SDFStar& s) : BlockPortIter(s) {}
	SDFPortHole* next() { return (SDFPortHole*)BlockPortIter::next();}
	SDFPortHole* operator++() { return next();}
};

class DFStarPortIter : public BlockPortIter {
public:
	DFStarPortIter(DataFlowStar& s) : BlockPortIter(s) {}
	DFPortHole* next() { return (DFPortHole*)BlockPortIter::next();}
	DFPortHole* operator++() { return next();}
};

#endif
