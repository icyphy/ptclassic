/******************************************************************
Version identification:
 $Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: Soonhoi Ha 
 Date of creation: 6/15/90
 Revision : set up the flag (delayType) depending on the scheduler in start().

*******************************************************************/
#include "DEWormhole.h"

/*******************************************************************

	class DEWormhole methods

********************************************************************/

void DEWormhole :: start()
{
	Wormhole :: setup();
	if (mySched()->amITimed())	delayType = TRUE;
	else				delayType = FALSE;
}
	
void DEWormhole :: go()
{
	run();
}

// cloner -- clone the inside and make a new wormhole from that.
Block* DEWormhole :: clone()
{
	return new DEWormhole(gal.clone()->asGalaxy());
}
