/******************************************************************
Version identification:
 $Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: Soonhoi Ha 
 Date of creation: 6/15/90

*******************************************************************/
#include "DEWormhole.h"
#include "StringList.h"
#include "Output.h"

extern Error errorHandler;

/*******************************************************************

	class DEWormhole methods

********************************************************************/

void DEWormhole :: go()
{
// It notifies the output ports of the wormhole that data arrives and
// sets the timeStamp for them.
	run();

	completionTime = arrivalTime + double(messageProcessingTime);
}

// cloner -- clone the inside and make a new wormhole from that.
Block* DEWormhole :: clone()
{
	return new DEWormhole(gal.clone()->asGalaxy());
}
