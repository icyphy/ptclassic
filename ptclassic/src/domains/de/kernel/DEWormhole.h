#ifndef _DEWormhole_h
#define _DEWormhole_h 1
#ifdef __GNUG__
#pragma once
#pragma interface
#endif


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

	// prepare for a new phase of firing
	void startNewPhase();

	// constructor
	DEWormhole(Galaxy &g) : Wormhole(*this,g) {
		buildEventHorizons ();
	}

	// return my scheduler
	Scheduler* mySched() const { return scheduler ;}

	// print methods
	StringList printVerbose() const { return Wormhole :: print(0);}
	StringList printRecursive() const { return Wormhole :: print(1);}

	// clone -- allows interpreter to make copies
	Block* clone() const;

	// identify myself as a wormhole
	int isItWormhole() const { return TRUE;}

	// use statelist for inner galaxy for stateWithName
	State* stateWithName (const char* name) const {
		return gal.stateWithName(name);
	}

	// state initialize
	void initState() { gal.initState() ;}

protected:
	// redefine getStopTime() : 
	// return the currentTime if syncMode of the scheduler is set (default)
	// otherwise     the stopTime.
	float getStopTime();

	// redefine sumUp()
	void sumUp();
};
	
#endif
