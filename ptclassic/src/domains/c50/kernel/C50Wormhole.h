#ifndef _C50Wormhole_h
#define _C50Wormhole_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "StringList.h"
#include "EventHorizon.h"
#include "SDFScheduler.h"
#include "C50PortHole.h"
#include "CGWormBase.h"
#include "C50Star.h"

/*******************************************************************
Version:
@(#)C50Wormhole.h	1.2	8/15/96

@Copyright (c) 1990-1996 The Regents of the University of California.
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

Programmer: Andreas Baensch
Date of creation: 30 April 1995

Modeled after code by Soonhoi Ha.

********************************************************************/
// C50Wormhole can not contain a timed-domain. It may contain another
// type of CGdomain.

	//////////////////////////////
	// C50Wormhole
	//////////////////////////////

class C50Wormhole : public CGWormBase, public C50Star {

public:
	void setup();
	void go();
	void wrapup() { /* target->wrapup();  */ }

	// Constructor
	C50Wormhole(Galaxy& g, Target* t = 0);
	~C50Wormhole();

	// return my scheduler
	Scheduler* scheduler() const { return myTarget()->scheduler() ;}

	// execution time which is the average of the workload inside 
	// the wormhole with 1 processor.
	int myExecTime() { return execTime; }

	// print methods
	StringList print(int verbose = 0) const {
		return Wormhole::print(verbose);
	}

	// clone -- allows interpreter/pigi to make copies
	Block* clone() const;

	// identify myself as a wormhole
	int isItWormhole() const { return TRUE;}

	// use statelist for inner galaxy for stateWithName
	State* stateWithName (const char* name) {
		return gal.stateWithName(name);
	}
	
	// state initialize
	void initState() { gal.initState() ;}

	// FIXME: what should this do?
	double getStopTime() { return 0.0;}

	// return myself
	/*virtual*/ Wormhole* asWormhole();

};

        //////////////////////////////////////////
        // class C50toUniversal
        //////////////////////////////////////////

// Input Boundary of ??inC50_Wormhole
class C50toUniversal : public ToEventHorizon, public InC50Port
{
public:
	// constructor
	C50toUniversal() : ToEventHorizon(this) {}

	// redefine
	void receiveData() ;

	void initialize();

	int isItInput() const;
	int isItOutput() const;

	// as EventHorizon
	EventHorizon* asEH();
};

        //////////////////////////////////////////
        // class C50fromUniversal
        //////////////////////////////////////////

// Output Boundary of ??inC50_Wormhole
class C50fromUniversal : public FromEventHorizon, public OutC50Port
{
protected:
	// redefine the "ready" method for implicit synchronization.
	int ready() ;

public:
	// constructor
	C50fromUniversal() : FromEventHorizon(this) {}

	// redefine
	void sendData() ;	

	void initialize();

	int isItInput() const;
	int isItOutput() const;

	// as EventHorizon
	EventHorizon* asEH();
};
	
#endif




