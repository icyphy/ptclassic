#ifndef _BDFWormhole_h
#define _BDFWormhole_h 1
#ifdef __GNUG__
#pragma interface
#endif


#include "StringList.h"
#include "Wormhole.h"
#include "EventHorizon.h"
#include "BDFStar.h"

/*******************************************************************
 SCCS Version identification :
 $Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer : Soonhoi Ha
 Date of Creation : 6/15/90
	
********************************************************************/

	//////////////////////////////
	// BDFWormhole
	//////////////////////////////

class BDFWormhole : public Wormhole, public BDFStar {

private:
	// time interval between samples.
	double space;

	// flag to be set after fired once.
	int mark;

	// token's arrival time.
	double arrivalTime;

protected:
	// redefine the getStopTime() 
	double getStopTime();

	// no necessary of sumUp method.

public:
	void setup();
	void go();
	void wrapup();

	// Constructor
	BDFWormhole(Galaxy& g,Target* t = 0);

	// Destructor
	~BDFWormhole() { freeContents();}

	// return my scheduler
	Scheduler* scheduler() const { return target->scheduler();}

	// print methods
	StringList printVerbose() const;
	StringList printRecursive() const;

	// clone -- allows interpreter/pigi to make copies
	Block* clone() const;
	Block* makeNew() const;

	// get the token's arrival time to the wormhole
	double getArrivalTime();

	// identify myself as a wormhole
	int isItWormhole() const { return TRUE;}

	// use statelist for inner galaxy for stateWithName
	State* stateWithName (const char* name) {
		return gal.stateWithName(name);
	}
	
	// state initialize
	void initState() { gal.initState() ;}
};

        //////////////////////////////////////////
        // class BDFtoUniversal
        //////////////////////////////////////////

// Input Boundary of ??inBDF_Wormhole
class BDFtoUniversal : public ToEventHorizon, public InBDFPort
{
public:
	// constructor
	BDFtoUniversal() : ToEventHorizon(this) {}

	// redefine
	void receiveData() ;

	void initialize();

        int isItInput() const;
        int isItOutput() const;

	// as EventHorizon
	EventHorizon* asEH();
};

        //////////////////////////////////////////
        // class BDFfromUniversal
        //////////////////////////////////////////

// Output Boundary of ??inBDF_Wormhole
class BDFfromUniversal : public FromEventHorizon, public OutBDFPort
{
protected:
	// redefine the "ready" method for implicit synchronization.
	int ready() ;

public:
	// constructor
	BDFfromUniversal() : FromEventHorizon(this) {}

	// redefine
	void sendData() ;	

	void initialize();

        int isItInput() const;
        int isItOutput() const;

	// as EventHorizon
	EventHorizon* asEH();
};
	
#endif
