#ifndef _SDFWormhole_h
#define _SDFWormhole_h 1

#include "StringList.h"
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
	Scheduler* mySched() const { return scheduler ;}

	// print methods
	StringList printVerbose() const;
	StringList printRecursive() const;

	// clone -- allows interpreter/pigi to make copies
	Block* clone() const;

	// get the token's arrival time to the wormhole
	float getArrivalTime();

	// identify myself as a wormhole
	int isItWormhole() const { return TRUE;}

	// use statelist for inner galaxy for stateWithName
	State* stateWithName (const char* name) const {
		return gal.stateWithName(name);
	}
	
	// state initialize
	void initState() { gal.initState() ;}
};
	
#endif
