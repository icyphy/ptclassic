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

	void start();
	void go();
	void wrapup() { endSimulation(); }

	// constructor
	DEWormhole(Galaxy &g) : Wormhole(*this,g) {
		buildEventHorizons ();
	}

	// return my scheduler
	Scheduler* mySched() { return scheduler ;}

	// print methods
	StringList printVerbose() { return Wormhole :: print(0);}
	StringList printRecursive() { return Wormhole :: print(1);}

	// clone -- allows interpreter to make copies
	Block* clone();
};
	
#endif
