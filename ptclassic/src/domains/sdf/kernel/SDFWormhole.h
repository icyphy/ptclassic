#ifndef _SDFWormhole_h
#define _SDFWormhole_h 1

#include "Wormhole.h"
#include "SDFStar.h"
#include "Fraction.h"

/*******************************************************************
 SCCS Version identification :
 $Id$

 Programmer : Soonhoi Ha
 Date of Creation : 6/15/90
	
********************************************************************/

	//////////////////////////////
	// SDFWormhole
	//////////////////////////////

class SDFWormhole : public Wormhole, public SDFStar {

public:
	// Wormhole specific.
	FloatState iterationPeriod;

	void start() {Wormhole :: setup() ;}
	void go();

	// Constructor
	SDFWormhole(Scheduler* s, const char* typeDesc, Galaxy* g)
		: Wormhole(s, typeDesc, g) {
		addState(iterationPeriod.setState("iterationPeriod", this,
			"100.0", "simulated iteration Period of SDF system"));}

	// printVerbose
	StringList printVerbose() { Star :: printVerbose();
				    Wormhole :: printVerbose(); }
};
	
#endif
