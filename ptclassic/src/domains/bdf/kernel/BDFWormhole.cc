static const char file_id[] = "BDFWormhole.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: Soonhoi Ha , J. Buck

Mostly copied from SDFWormhole.cc

*******************************************************************/

#include "BDFWormhole.h"
#include "BDFScheduler.h"
#include "StringList.h"
#include "Error.h"
#include <stream.h>

extern Error errorHandler;

/*******************************************************************

	class BDFWormhole methods

********************************************************************/


void BDFWormhole :: go() {
	// set the currentTime of the inner domain.
	target->setCurrentTime(arrivalTime);

	// run
	run();
}

// return the next time Stamp for the stopping condition of the inner timed
// domain.

float BDFWormhole :: getStopTime() {
	// set the next token arrival time
	arrivalTime += space;
	return arrivalTime;
}

// return the token's arrival time.
// At the beginning, token is arrived at the currentTime of the BDF
// scheduler. It is incremented by "space" afterwards.

float BDFWormhole :: getArrivalTime() {
	// At the very first call, we initialize "arrivalTime".
	if (!mark) {
		BDFScheduler* sched = (BDFScheduler*) parent()->mySched();
		arrivalTime = sched->currentTime;
		double reps = sched->repetitions(*this);
		space = sched->schedulePeriod / reps;
		mark = 1;
	}
	return arrivalTime;
}

// Constructor
BDFWormhole :: BDFWormhole(Galaxy& g, Target* t) : Wormhole(*this,g,t)
{
	buildEventHorizons ();
}

StringList BDFWormhole :: printVerbose() const {
	return Wormhole :: print(0);
}

StringList BDFWormhole :: printRecursive() const {
	return Wormhole :: print(1);
}


// cloner -- clone the inside and make a new wormhole from that.
Block* BDFWormhole :: clone() const {
LOG_NEW; return new BDFWormhole(gal.clone()->asGalaxy(),target->cloneTarget());
}

