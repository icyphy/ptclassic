static const char file_id[] = "SDFWormhole.cc";
/******************************************************************
Version identification:
 $Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

 Programmer: Soonhoi Ha 
 Date of creation: 6/15/90
 Date of revision: 6/20/90

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "SDFWormhole.h"
#include "SDFScheduler.h"
#include "StringList.h"
#include "Error.h"

/*******************************************************************

	class SDFWormhole methods

********************************************************************/

void SDFWormhole :: setup() {
	arrivalTime = 0;
	space = 0;
	Wormhole :: setup();
	mark = 0;
}

void SDFWormhole :: go() {
	// At the first call of each iteration, initialize arrivalTime
        // to the value of arriving tokens.  Subsequently in the
	// iteration, it is incremented by "space", which tries to
	// uniformly space the time stamps.  In a wormhole, normally
	// the schedulePeriod will be zero, so space will be zero, and
	// the arrival time will remain equal to the input arrival
	// time throughout the iteration.
        if (mark == 0) {
		SDFScheduler* sched = (SDFScheduler*) outerSched();
		arrivalTime = sched->now();
	}

	// run
	Wormhole::run();

	mark++;
	if (mark == reps()) mark = 0;
}

void SDFWormhole :: wrapup() {
	myTarget()->wrapup();
}
 
// Return the next time Stamp for the stopping condition of the inner
// timed domain.  This is used to set the stop time of the inner
// domain. Note that by default, "space" will have value zero, so the
// inner domain will not progress ahead of outer domains.  However,
// this can be overridden by the user by setting the schedulePeriod
// parameter of the Target.

double SDFWormhole :: getStopTime() {
    SDFScheduler* sched = (SDFScheduler*) outerSched();
    return sched->now() + mark*space;
}

// return the token's arrival time.
// At the beginning of a cycle through the SDF schedule,
// the arrival time is the currentTime of the outside scheduler.
// On subsequent cycles, it is incremented by space.

double SDFWormhole :: getArrivalTime() {
    SDFScheduler* sched = (SDFScheduler*) outerSched();
    return sched->now() + mark*space;
}

void SDFWormhole :: begin () {
    Wormhole::begin();

    // Compute the spacing between events for multirate systems.
    // When an SDF wormhole contains a timed system, and the SDF
    // system is a multirate system, then we would like to space out
    // the time stamps of the events passed to the timed system.
    // The spacing of the time stamps is the schedulePeriod divided
    // by the number of repetitions (number of firings) of the
    // wormhole in one schedule period.  Note that by default, the
    // schedulePeriod is zero, so this will have no effect unless the
    // user specifically sets the Target parameter schedulePeriod.

    SDFScheduler* sched = (SDFScheduler*) outerSched();
    space = sched->schedulePeriod / double(repetitions) ;
}

// Constructor
SDFWormhole :: SDFWormhole(Galaxy& g,Target* t) : Wormhole(*this,g,t)
{
	buildEventHorizons ();
}

// cloner -- clone the inside and make a new wormhole from that.
Block* SDFWormhole :: clone() const {
	LOG_NEW; return new SDFWormhole(gal.clone()->asGalaxy(),
					myTarget()->cloneTarget());
}

Block* SDFWormhole :: makeNew() const {
	LOG_NEW; return new SDFWormhole(gal.makeNew()->asGalaxy(),
					myTarget()->cloneTarget());
}

/**************************************************************************

	methods for SDFtoUniversal

**************************************************************************/

void SDFtoUniversal :: receiveData ()
{
	// 1. get data from the SDF geodesic.
	getData();

	// Check it is an input or output.
	// BUG: g++ 2.2.2 is screwup up the vtbl for fns inherited
	// from two different baseclasses.
	if (SDFtoUniversal::isItInput()) {
		
		// 2. set the timeMark from token's arrival Time to wormhole.
		SDFWormhole* worm = (SDFWormhole*) wormhole;
		timeMark = worm->getArrivalTime();

	} else {
		// 2. annul increment of currentTime at the end of run.
		SDFScheduler* sched = (SDFScheduler*) innerSched();
		timeMark = sched->now() - sched->schedulePeriod;
	}

	// transfer Data
	transferData();
}

void SDFtoUniversal :: initialize() {
	PortHole :: initialize();
	ToEventHorizon :: initialize();		
}

int SDFtoUniversal :: isItInput() const { return EventHorizon :: isItInput(); }
int SDFtoUniversal :: isItOutput() const {return EventHorizon:: isItOutput();}

EventHorizon* SDFtoUniversal :: asEH() { return this; }
	
/**************************************************************************

	methods for SDFfromUniversal

**************************************************************************/

void SDFfromUniversal :: sendData ()
{
	// 1. transfer data from the other side of the event horizon
	transferData();

	if (tokenNew) {
		// 2. put data to the SDF geodesic
		putData();

	} else if (SDFfromUniversal::isItOutput()) {
		// 2. inside domain does not generate enough number of tokens,
		//    which is error.
		Error::abortRun(*this, "not enough output tokens ",
				"at SDF wormhole boundary");
	}
}

// wait until has enough number of tokens to fire the inside star
// (#repetitions) times from SDF scheduler.

int SDFfromUniversal :: ready() {
	DFPortHole *pFar = (DFPortHole*)far();
	int target = pFar->numXfer() * pFar->parentReps();
	if (numTokens() >= target) {
		// yes, enough tokens
		return TRUE;
	}
	return FALSE;
}

void SDFfromUniversal :: initialize() {
	PortHole :: initialize();
	FromEventHorizon :: initialize();
}

int SDFfromUniversal :: isItInput() const {return EventHorizon::isItInput();}
int SDFfromUniversal :: isItOutput() const {return EventHorizon::isItOutput();}

EventHorizon* SDFfromUniversal :: asEH() { return this; }
