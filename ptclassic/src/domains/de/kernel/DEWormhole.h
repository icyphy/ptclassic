#ifndef _DEWormhole_h
#define _DEWormhole_h 1

#include "Wormhole.h"
#include "DEStar.h"
#include "PriorityQueue.h"

/*******************************************************************
 SCCS Version identification :
 $Id$

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
	DEWormhole(Scheduler* s, const char* typeDesc, Galaxy* g)
		: Wormhole(s, typeDesc, g) {}

	// printVerbose
	StringList printVerbose() { Star :: printVerbose();
				    Wormhole :: printVerbose(); }
};
	
#endif
