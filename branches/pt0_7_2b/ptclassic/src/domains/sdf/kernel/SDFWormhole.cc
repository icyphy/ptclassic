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
#include "StringList.h"
#include "Output.h"
#include <stream.h>

extern Error errorHandler;

/*******************************************************************

	class SDFWormhole methods

********************************************************************/


void SDFWormhole :: go() {
	// The following three lines had better be moved between
	// start() and go(), since the state is adjusted after the
	// compile time scheduling. Currently, no method is calling
	// after scheduling before running. How about add adjustStates()
	// to Star class and call it after scheduling?

	// set delay states.
	messageProcessingTime = double(iterationPeriod) / double(repetitions);

	// inform sampler stars about samplingPeriod.
	gal.setState("samplingPeriod", 
		form("%f",float(messageProcessingTime)));
	gal.initState();

	run();

	// No time stamp setup is necessary for SDF system.
}
