/******************************************************************
Version identification:
 $Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: Soonhoi Ha 
 Date of creation: 6/15/90
 Date of revision: 6/20/90

*******************************************************************/

#include "SDFWormhole.h"
#include "SDFScheduler.h"
#include "StringList.h"
#include "Output.h"
#include <stream.h>

extern Error errorHandler;

/*******************************************************************

	class SDFWormhole methods

********************************************************************/


void SDFWormhole :: go() {
	// set the currentTime of the inner domain.
	scheduler->setCurrentTime(arrivalTime);

	// run
	run();
}

// return the next time Stamp for the stopping condition of the inner timed
// domain.

float SDFWormhole :: getStopTime() {
	// set the next token arrival time
	arrivalTime += space;
	return arrivalTime;
}

// return the token's arrival time.
// At the beginning, token is arrived at the currentTime of the SDF
// scheduler. It is incremented by "space" afterwards.

float SDFWormhole :: getArrivalTime() {
	// At the very first call, we initialize "arrivalTime".
	if (!mark) {
		SDFScheduler* sched = (SDFScheduler*) parent()->mySched();
		arrivalTime = sched->currentTime;
		space = sched->schedulePeriod / double(repetitions) ;
		mark = 1;
	}
	return arrivalTime;
}

// Constructor
SDFWormhole :: SDFWormhole(Galaxy& g) : Wormhole(*this,g)
{
	buildEventHorizons ();
}


// cloner -- clone the inside and make a new wormhole from that.
Block* SDFWormhole :: clone() {
	return new SDFWormhole(gal.clone()->asGalaxy());
}

