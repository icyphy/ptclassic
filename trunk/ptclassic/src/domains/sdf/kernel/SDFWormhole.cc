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

	// inform sampler stars about sampling time.
	gal.setState("samplingTime", 
		form("%f",mySched()->currentTime));
	gal.initState();

	run();

}

// Constructor
SDFWormhole :: SDFWormhole(Galaxy& g) : Wormhole(*this,g)
{
	buildEventHorizons ();
	gal.addState(samplingTime.setState("samplingTime",&gal,"10.0",
		     "sampling time for sampler stars"));
}


// cloner -- clone the inside and make a new wormhole from that.
Block* SDFWormhole :: clone() {
	return new SDFWormhole(gal.clone()->asGalaxy());
}

