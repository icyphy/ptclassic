#ifndef _DDFScheduler_h
#define _DDFScheduler_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "Scheduler.h"
#include "Galaxy.h"
#include "DDFStar.h"
#include "DataStruct.h"
#include "IntState.h"
#include "RecurScheduler.h"
#include "defConstructs.h"

class DDFTarget;

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 8/9/90
	 revision: 1/21/91  Detect special construct (case, for, dowhile,
			    recursion) for efficient scheduling.

 Methods for the DDF Scheduler

**************************************************************************/


	////////////////////////////
	// DDFScheduler
	////////////////////////////

class DDFScheduler : public Scheduler {

friend class DDFTarget;

	// explicit stopping condition
	int stopTime;

	// check whether a star is runnable. Also enable lazy evaluation.
	int isRunnable(Star&);

	// lazy evaluation
	int lazyEval(Star*);
	int checkLazyEval(Star *);

	// list of source blocks.
	SequentialList sourceBlocks;

	// the number of firing all sources so far
	int numFiring;

	// number of overlapped execution allowed.
	int numOverlapped;

	// capacity of arc
	int maxToken;

	// number of stars in the galaxy
	int galSize;

// Advanced features ......................
	// candidate domain
	int canDom;

	// real scheduler for other constructs
	Scheduler* realSched;

	// list of newly created wormholes of SDF domain.
	SequentialList sdfWorms;

	// cruft used for generating names for SDF galaxies.
	static int nW;
	const char* wormName();

	// decide the type of the construct.
	void detectConstruct(Galaxy&);

	// select scheduler
	void selectScheduler(Galaxy&);

	// modify the topology
	void makeSDFWormholes(Galaxy&);

	// flag to be set when restructured.
	int restructured;

// End of Advanced features ..................

public:
	// my domain
	const char* domain() const ;

	// The setup function computes an DDF schedule
	// and initializes all the blocks.
	int setup(Galaxy& galaxy);

	// The run function resumes the run where it was left off.
	int run(Galaxy& galaxy);

	StringList displaySchedule() ;

	// Constructor sets default options
	DDFScheduler ();
	~DDFScheduler ();

	// Timing control funcs
	void setStopTime (float limit);
	void resetStopTime (float v);
	float getStopTime() { return float(stopTime);}

	// scheduler Period : interface with timed domain.
	float schedulePeriod;

	// check whether the domain is predefined construct or not.
	int isSDFType()	;

	// reset "restructured" flag for DDFSelf star
	void resetFlag();
};

#endif
