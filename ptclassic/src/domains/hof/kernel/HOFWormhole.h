#ifndef _HOFWormhole_h
#define _HOFWormhole_h

/*  Version $Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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

    Programmer:		T.M. Parks, J. Buck
    Date of creation:	17 January 1992
*/

#ifdef __GNUG__
#pragma interface
#endif

#include "Wormhole.h"
#include "HOFStar.h"
#include "EventHorizon.h"
#include "HOFPortHole.h"


class HOFWormhole : public Wormhole, public HOFStar
{
public:
	// Constructor
	HOFWormhole(Galaxy& g, Target* t=0);

	// Destructor
	~HOFWormhole();

	void setup();
	void begin() { Wormhole::begin(); }
	void go();
	void wrapup();

	Scheduler* mySched() const { return myTarget()->scheduler(); }

	// clone -- allows interpreter/pigi to make copies
	Block* clone() const;
	Block* makeNew() const;

	// identify myself as a wormhole
	int isItWormhole() const { return TRUE; }

	// use statelist for inner galaxy for stateWithName
	State* stateWithName (const char* name) {
		return gal.stateWithName(name);
	}

	// return stopTime
	double getStopTime();

	// state initialize
	void initState() { gal.initState(); }

	StringList printVerbose() const;
	StringList printRecursive() const;
};

class HOFtoUniversal : public ToEventHorizon, public InHOFPort
{
public:
	// constructor
	HOFtoUniversal() : ToEventHorizon(this) {}

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

class HOFfromUniversal : public FromEventHorizon, public OutHOFPort
{
public:
	// constructor
	HOFfromUniversal() : FromEventHorizon(this) {}

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
