#ifndef _SDFWormhole_h
#define _SDFWormhole_h 1
#ifdef __GNUG__
#pragma interface
#endif


#include "StringList.h"
#include "Wormhole.h"
#include "EventHorizon.h"
#include "SDFStar.h"

/*******************************************************************
 SCCS Version identification :
 $Id$

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
							COPYRIGHTENDKEY

 Programmer : Soonhoi Ha
 Date of Creation : 6/15/90
	
********************************************************************/

	//////////////////////////////
	// SDFWormhole
	//////////////////////////////

class SDFWormhole : public Wormhole, public SDFStar {

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
	SDFWormhole(Galaxy& g,Target* t = 0);

	// Destructor
	~SDFWormhole() { freeContents();}

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
        // class SDFtoUniversal
        //////////////////////////////////////////

// Input Boundary of ??inSDF_Wormhole
class SDFtoUniversal : public ToEventHorizon, public InSDFPort
{
public:
	// constructor
	SDFtoUniversal() : ToEventHorizon(this) {}

	// redefine
	void receiveData() ;

	void initialize();

        int isItInput() const;
        int isItOutput() const;

	// as EventHorizon
	EventHorizon* asEH();
};

        //////////////////////////////////////////
        // class SDFfromUniversal
        //////////////////////////////////////////

// Output Boundary of ??inSDF_Wormhole
class SDFfromUniversal : public FromEventHorizon, public OutSDFPort
{
protected:
	// redefine the "ready" method for implicit synchronization.
	int ready() ;

public:
	// constructor
	SDFfromUniversal() : FromEventHorizon(this) {}

	// redefine
	void sendData() ;	

	void initialize();

        int isItInput() const;
        int isItOutput() const;

	// as EventHorizon
	EventHorizon* asEH();
};
	
#endif
