#ifndef _CGWormhole_h
#define _CGWormhole_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "CGStar.h"
#include "CGWormBase.h"
#include "EventHorizon.h"
#include "CGPortHole.h"

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
 Date of Creation : 5/21/91
	
********************************************************************/

// CGWormhole can not contain a timed-domain. It may contain another
// type of CGdomain.

	//////////////////////////////
	// CGWormhole
	//////////////////////////////

class CGWormhole : public CGWormBase, public CGStar {

public:
	void setup();
	void go();
	void wrapup() { /* target->wrapup();  */ }

	// Constructor
	CGWormhole(Galaxy& g, Target* t = 0);
	~CGWormhole();

	// return my scheduler
	Scheduler* scheduler() const { return target->scheduler() ;}

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

	// return myself
	CGWormBase* myWormhole();

	// FIXME: what should this do?
	double getStopTime() { return 0.0;}

/*******  methods for parallel scheduler. ********/

        // Redefine: return the profile when "pNum" processors are assigned.
        Profile* getProfile(int pNum); 
};


        //////////////////////////////////////////
        // class CGtoUniversal
        //////////////////////////////////////////

class CGtoUniversal : public ToEventHorizon, public InCGPort
{
public:
	// constructor
	CGtoUniversal(): ToEventHorizon(this) {}

	// redefine
	void initialize();

	int isItInput() const;
	int isItOutput() const;

	// as Eventhorizon
	EventHorizon* asEH();
};

        //////////////////////////////////////////
        // class CGfromUniversal
        //////////////////////////////////////////

class CGfromUniversal : public FromEventHorizon, public OutCGPort
{
public:
	// constructor
	CGfromUniversal(): FromEventHorizon(this) {}

	// redefine
	void initialize();

	int isItInput() const;
	int isItOutput() const;

	// as Eventhorizon
	EventHorizon* asEH();
};

#endif
