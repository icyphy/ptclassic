#ifndef _BDFWormhole_h
#define _BDFWormhole_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "StringList.h"
#include "Wormhole.h"
#include "BDFStar.h"

/*******************************************************************
 SCCS Version identification :
 $Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer : Soonhoi Ha, J. Buck

 For now this is a copy of SDFWormhole.  Needs work.
	
********************************************************************/

	//////////////////////////////
	// BDFWormhole
	//////////////////////////////

class BDFWormhole : public Wormhole, public BDFStar {

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
	BDFWormhole(Galaxy& g, Target* t = 0);

	// Destructor
	~BDFWormhole() { freeContents();}

	// return my scheduler
	Scheduler* mySched() const { return target->mySched() ;}

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
	State* stateWithName (const char* name) {
		return gal.stateWithName(name);
	}
};
#endif
