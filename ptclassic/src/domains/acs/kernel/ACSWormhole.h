#ifndef _ACSWormhole_h
#define _ACSWormhole_h

/*  Version @(#)ACSWormhole.h	1.9 08/15/00

@Copyright (c) 1998 The Regents of the University of California.
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

    Programmer:		T.M. Parks, J. Buck
    Date of creation:	17 January 1992
*/

#ifdef __GNUG__
#pragma interface
#endif

#include "Wormhole.h"
#include "ACSStar.h"
#include "EventHorizon.h"
#include "ACSPortHole.h"

// JMS
//#include "HWSchedule.h"
//#include "acs_starconsts.h"
#include "ACSCGFPGACore.h"

//class Capability;
//class Port_Id;

class ACSWormhole : public Wormhole, public ACSStar {
public:
	// Constructor
	ACSWormhole(Galaxy& g, Target* t=0);

	// Destructor
	~ACSWormhole();

	void begin() { Wormhole::begin(); }
	void wrapup();

	Scheduler* scheduler() const { return myTarget()->scheduler(); }

	// clone -- allows interpreter/pigi to make copies
	Block* clone() const;
	Block* makeNew() const;

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

	StringList print(int verbose) const {
		return Wormhole::print(verbose);
	}

protected:
	void setup();
	void go();

	// return stopTime
	double getStopTime();
};

class ACStoUniversal : public ToEventHorizon, public InACSPort {
public:
	// constructor
	ACStoUniversal() : ToEventHorizon(this) {}

	// redefine
	void receiveData();

	void initialize();

        int isItInput() const;
        int isItOutput() const;

	// as EventHorizon
	EventHorizon* asEH();

	/*virtual*/ Geodesic* allocateGeodesic()
	{ return ToEventHorizon::allocateGeodesic(); }
};

class ACSfromUniversal : public FromEventHorizon, public OutACSPort {
public:
	// constructor
	ACSfromUniversal() : FromEventHorizon(this) {}

	// redefine
	void sendData();

	void initialize();

        int isItInput() const;
        int isItOutput() const;

	// as EventHorizon
	EventHorizon* asEH();

	/*virtual*/ Geodesic* allocateGeodesic()
	{ return FromEventHorizon::allocateGeodesic(); }
};

#endif
