#ifndef _DEWormhole_h
#define _DEWormhole_h 1
#ifdef __GNUG__
#pragma interface
#endif


#include "Wormhole.h"
#include "DEStar.h"
#include "PriorityQueue.h"
#include "EventHorizon.h"
#include "DEPortHole.h"

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
	void wrapup();

	// prepare for a new phase of firing
	void startNewPhase();

	// constructor
	DEWormhole(Galaxy &g,Target* t = 0) : Wormhole(*this,g,t) {
		buildEventHorizons ();
	}

	// destructor
	~DEWormhole() { freeContents();}

	// return my scheduler
	Scheduler* scheduler() const { return target->scheduler();}

	// print methods
	StringList printVerbose() const { return Wormhole :: print(0);}
	StringList printRecursive() const { return Wormhole :: print(1);}

	// clone -- allows interpreter to make copies
	Block* clone() const;

	// identify myself as a wormhole
	int isItWormhole() const { return TRUE;}

	// use statelist for inner galaxy for stateWithName
	State* stateWithName (const char* name) {
		return gal.stateWithName(name);
	}

	// state initialize
	void initState() { gal.initState() ;}

protected:
	// redefine getStopTime() : 
	// return the currentTime if syncMode of the scheduler is set (default)
	// otherwise     the stopTime.
	double getStopTime();

	// redefine sumUp()
	void sumUp();
};

        //////////////////////////////////////////
        // class DEtoUniversal
        //////////////////////////////////////////


// Input Boundary of ??inDE_Wormholes.
class DEtoUniversal : public ToEventHorizon, public InDEPort
{
public:
	// constructor
	DEtoUniversal(): ToEventHorizon(this) {}

	// redefine
	void receiveData(); 
	void cleanIt();

	// fetch the event from the qlobal queue
	int getFromQueue(Particle* p);

	void initialize();

	int isItInput() const;
	int isItOutput() const;
	
	// as eventhorizon
	EventHorizon* asEH();
};

        //////////////////////////////////////////
        // class DEfromUniversal
        //////////////////////////////////////////

// Output Boundary of ??inDE_Wormholes.
class DEfromUniversal : public FromEventHorizon, public OutDEPort
{
public:
	// constructor
	DEfromUniversal(): FromEventHorizon(this) {}

	void sendData();

	void initialize();

	int isItInput() const;
	int isItOutput() const;
	
	// as eventhorizon
	EventHorizon* asEH();
};

#endif
