#ifndef _DEWormhole_h
#define _DEWormhole_h 1

#include "Wormhole.h"
#include "DEStar.h"
#include "PriorityQueue.h"

/*******************************************************************
 SCCS Version identification :
 $Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer : Soonhoi Ha
 Date of Creation : 6/15/90
	
********************************************************************/

	//////////////////////////////
	// DEWormhole
	//////////////////////////////

class DEWormhole : public Wormhole, public DEStar {

public:
	// redefine "go()". After running the inside scheduler,
	// it stamps the time for the OutDEEventHorizons.

	// Restriction 2 : all outputs are synchronized.

	void start() {Wormhole :: setup() ;}
	void go();

	// constructor
	DEWormhole(Galaxy &g) : Wormhole(*this,g) {
		buildEventHorizons ();
	}

	// print methods
	StringList printVerbose() { return Wormhole :: print(0);}
	StringList printRecursive() { return Wormhole :: print(1);}

	// clone -- allows interpreter to make copies
	Block* clone();
};
	
#endif
