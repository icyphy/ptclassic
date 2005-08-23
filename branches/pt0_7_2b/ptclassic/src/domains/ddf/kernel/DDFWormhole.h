#ifndef _DDFWormhole_h
#define _DDFWormhole_h 1
#ifdef __GNUG__
#pragma interface
#endif

/*******************************************************************
 SCCS Version identification :
@(#)DDFWormhole.h	1.2 01/06/99

Copyright (c) 1998-1999 The Regents of the University of California.
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

 Programmer : Bilung Lee
 Date of Creation : 10/28/98
	
********************************************************************/
#include "StringList.h"
#include "Wormhole.h"
#include "EventHorizon.h"
#include "SDFStar.h"

	//////////////////////////////
	// DDFWormhole
	//////////////////////////////

class DDFWormhole : public Wormhole, public SDFStar {
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
	void begin();
	void wrapup();

	// Constructor
	DDFWormhole(Galaxy& g,Target* t = 0);

	// Destructor
	~DDFWormhole() { freeContents(); }

	// return my scheduler
	Scheduler* scheduler() const { return myTarget()->scheduler(); }

	// print method
	StringList print(int verbose) const {
		return Wormhole::print(verbose);
	}

	// clone -- allows interpreter/pigi to make copies
	Block* clone() const;
	Block* makeNew() const;

	// get the token's arrival time to the wormhole
	double getArrivalTime();

	// identify myself as a wormhole
	int isItWormhole() const { return TRUE; }
	Wormhole* asWormhole() { return this; }

	// use statelist for inner galaxy for stateWithName
	State* stateWithName (const char* name) {
		return gal.stateWithName(name);
	}
	
	// state initialize
	void initState() { gal.initState(); }

	// my domain
        const char* domain() const;
};

        //////////////////////////////////////////
        // class DDFtoUniversal
        //////////////////////////////////////////

class DDFtoUniversal : public ToEventHorizon, public InSDFPort
{
public:
	// constructor
	DDFtoUniversal() : ToEventHorizon(this) {}

	// redefine
	void receiveData() ;

	void initialize();

        int isItInput() const;
        int isItOutput() const;

	// as EventHorizon
	EventHorizon* asEH();

	/*virtual*/ Geodesic* allocateGeodesic()
	{ return ToEventHorizon::allocateGeodesic(); }

	// my domain
        const char* domain() const;
};

        //////////////////////////////////////////
        // class DDFfromUniversal
        //////////////////////////////////////////

class DDFfromUniversal : public FromEventHorizon, public OutSDFPort
{
protected:
	// redefine the "ready" method for implicit synchronization.
	int ready() ;

public:
	// constructor
	DDFfromUniversal() : FromEventHorizon(this) {}

	// redefine
	void sendData() ;	

	void initialize();

        int isItInput() const;
        int isItOutput() const;

	// as EventHorizon
	EventHorizon* asEH();

	/*virtual*/ Geodesic* allocateGeodesic()
	{ return FromEventHorizon::allocateGeodesic(); }

	// my domain
        const char* domain() const;
};
	
#endif
