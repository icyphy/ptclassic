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
	// DEWormhole
	//////////////////////////////

class DEWormhole : public Wormhole, public DEStar {

public:

	void setup();
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

	// print method
	StringList print(int verbose) const { return Wormhole::print(verbose);}

	// clone -- allows interpreter to make copies
	Block* clone() const;
	Block* makeNew() const;

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
