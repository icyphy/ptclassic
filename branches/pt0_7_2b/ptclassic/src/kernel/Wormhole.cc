/******************************************************************
Version identification:
 $Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: Soonhoi Ha 
 Date of creation: 3/19/90
 Date of modification: 5/24/90
 Date of modification: 5/31/90
 Date of modification: 6/13/90

*******************************************************************/
#include "Wormhole.h"
#include "StringList.h"
#include "Output.h"
// #include "SDFUniverse.h"
// #include "SDFStar.h"

extern Error errorHandler;

/*******************************************************************

	class Wormhole methods

********************************************************************/

StringList Wormhole :: printVerbose () {
	StringList out;
	out += "CONTENTS:\n";

	out += gal.printVerbose ();

	return out;
}

/* void Wormhole :: checkSDF()
{
// check if the repetition member of the SDFstars at the wormhole
// boundary is one. If not, CURRENTLY, report as ERROR!
	if (!strcmp(type, SDFstring)) {
	   for (int i = numberPorts(); i > 0; i--) {
		PortHole* p = ((EventHorizon&) nextPort()).insidePort;
		SDFStar* s = (SDFStar*) p->parent();
		if (s->repetitions > 1)
			errorHandler.error("Repetition should be one:",
				s->readFullName());
	   }
	}
}
*/

