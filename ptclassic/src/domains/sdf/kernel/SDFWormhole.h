#ifndef _SDFWormhole_h
#define _SDFWormhole_h 1

#include "Wormhole.h"
#include "SDFStar.h"

/*******************************************************************
 SCCS Version identification :
 $Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer : Soonhoi Ha
 Date of Creation : 6/15/90
	
********************************************************************/

	//////////////////////////////
	// SDFWormhole
	//////////////////////////////

class SDFWormhole : public Wormhole, public SDFStar {

private:
	// time interval between samples.
	float space;

	// flag to be set after fired once.
	int mark;

	// token's arrival time.
	float arrivalTime;

protected:
	// redefine the getStopTime() 
	float getStopTime();

	// no necessary of sumUp method.

public:
	void start() { Wormhole :: setup();
		       mark = 0; }
	void go();
	void wrapup() { endSimulation();}

	// Constructor
	SDFWormhole(Galaxy& g);

	// return my scheduler
	Scheduler* mySched() { return scheduler ;}

	// print methods
	StringList printVerbose() { return Wormhole :: print(0);}
	StringList printRecursive() { return Wormhole :: print(1);}

	// clone -- allows interpreter to make copies
	Block* clone();

	// get the token's arrival time to the wormhole
	float getArrivalTime();

};
	
#endif
