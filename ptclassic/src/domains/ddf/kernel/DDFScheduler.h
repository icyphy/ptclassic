#ifndef _DDFScheduler_h
#define _DDFScheduler_h 1

#include "Scheduler.h"
#include "Galaxy.h"
#include "DDFStar.h"
#include "DataStruct.h"
#include "IntState.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 8/9/90

 Methods for the DDF Scheduler

**************************************************************************/


	////////////////////////////
	// DDFScheduler
	////////////////////////////

class DDFScheduler : public Scheduler {

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
public:
	// my domain
	const char* domain() const ;

	// The setup function computes an DDF schedule
	// and initializes all the blocks.
	int setup(Block& galaxy);

	// The run function resumes the run where it was left off.
	int run(Block& galaxy);

	StringList displaySchedule() ;

	// Constructor sets default options
	DDFScheduler () { stopTime = 1;
			 numOverlapped = 1; 
			 schedulePeriod = 10000.0; }

	// setStopTime, for compatibility with DE scheduler.
	// for now, we assume each schedule interation takes 1.0
	// time units.  (Argh).  Deal with roundoff problems.
	void setStopTime (float limit) { stopTime = int(limit) ;}
	void resetStopTime () { stopTime = 1; numFiring = 0; }

	// declare "untimed"
	int amITimed();

	// scheduler Period : interface with timed domain.
	float schedulePeriod;
};

#endif
