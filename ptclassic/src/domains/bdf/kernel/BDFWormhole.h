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
 @(#)BDFWormhole.h	2.11		08/31/96

Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

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
	void setup();
	void go();

	// redefine the getStopTime() 
	double getStopTime();

	// no need of sumUp method.

public:
	void begin() { Wormhole::begin(); }
	void wrapup();

	// Constructor
	BDFWormhole(Galaxy& g,Target* t = 0);

	// Destructor
	~BDFWormhole() { freeContents(); }

	// return my scheduler
	Scheduler* scheduler() const { return myTarget()->scheduler(); }

	// print methods
	StringList printVerbose() const;
	StringList printRecursive() const;

	// clone -- allows interpreter/pigi to make copies
	Block* clone() const;
	Block* makeNew() const;

	// get the token's arrival time to the wormhole
	double getArrivalTime();

	// identify myself as a wormhole
	int isItWormhole() const { return TRUE; }

	// Override Star::asWormhole definition
        Wormhole* asWormhole() { return this; }

	// use statelist for inner galaxy for stateWithName
	State* stateWithName (const char* name) {
		return gal.stateWithName(name);
	}
	
	// state initialize
	void initState() { gal.initState(); }
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

	/*virtual*/ Geodesic* allocateGeodesic()
	{ return ToEventHorizon::allocateGeodesic(); }
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

	/*virtual*/ Geodesic* allocateGeodesic()
	{ return FromEventHorizon::allocateGeodesic(); }
};
	
#endif
