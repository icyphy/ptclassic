static const char file_id[] = "DDFWormhole.cc";
/******************************************************************
Version identification:
@(#)DDFWormhole.cc	1.2 01/06/99

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

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "DDFScheduler.h"
#include "DDFWormhole.h"
#include "StringList.h"
#include "Error.h"

extern const char DDFdomainName[];

/*******************************************************************

	class DDFWormhole methods

********************************************************************/

void DDFWormhole :: setup() {
	arrivalTime = 0;
	space = 0;
	Wormhole :: setup();
	mark = 0;
}

void DDFWormhole :: go() {
	// At the first call of each iteration, initialize arrivalTime
        // to the value of arriving tokens.  Subsequently in the
	// iteration, it is incremented by "space", which tries to
	// uniformly space the time stamps.  In a wormhole, normally
	// the schedulePeriod will be zero, so space will be zero, and
	// the arrival time will remain equal to the input arrival
	// time throughout the iteration.
        if (mark == 0) {
		DDFScheduler* sched = (DDFScheduler*) outerSched();
		arrivalTime = sched->now();
	}

	// run
	Wormhole::run();

	mark++;
	if (mark == reps()) mark = 0;
}

void DDFWormhole :: wrapup() {
	myTarget()->wrapup();
}
 
// Return the next time Stamp for the stopping condition of the inner
// timed domain.  This is used to set the stop time of the inner
// domain. Note that by default, "space" will have value zero, so the
// inner domain will not progress ahead of outer domains.  However,
// this can be overridden by the user by setting the schedulePeriod
// parameter of the Target.

double DDFWormhole :: getStopTime() {
    DDFScheduler* sched = (DDFScheduler*) outerSched();
    return sched->now() + mark*space;
}

// return the token's arrival time.
// At the beginning of a cycle through the DDF schedule,
// the arrival time is the currentTime of the outside scheduler.
// On subsequent cycles, it is incremented by space.

double DDFWormhole :: getArrivalTime() {
    DDFScheduler* sched = (DDFScheduler*) outerSched();
    return sched->now() + mark*space;
}

void DDFWormhole :: begin () {
    Wormhole::begin();

    // Compute the spacing between events for multirate systems.
    // When an DDF wormhole contains a timed system, and the DDF
    // system is a multirate system, then we would like to space out
    // the time stamps of the events passed to the timed system.
    // The spacing of the time stamps is the schedulePeriod divided
    // by the number of repetitions (number of firings) of the
    // wormhole in one schedule period.  Note that by default, the
    // schedulePeriod is zero, so this will have no effect unless the
    // user specifically sets the Target parameter schedulePeriod.

    DDFScheduler* sched = (DDFScheduler*) outerSched();
    // In case the repetitions is not set yet, set it to be 1.
    if (double(repetitions) == 0.0) repetitions = 1;
    space = sched->schedulePeriod / double(repetitions) ;
}

// Constructor
DDFWormhole :: DDFWormhole(Galaxy& g,Target* t) : Wormhole(*this,g,t)
{
	buildEventHorizons ();
}

// cloner -- clone the inside and make a new wormhole from that.
Block* DDFWormhole :: clone() const {
	LOG_NEW; return new DDFWormhole(gal.clone()->asGalaxy(),
					myTarget()->cloneTarget());
}

Block* DDFWormhole :: makeNew() const {
	LOG_NEW; return new DDFWormhole(gal.makeNew()->asGalaxy(),
					myTarget()->cloneTarget());
}

// my domain
const char* DDFWormhole :: domain () const { return DDFdomainName ;}

/**************************************************************************

	methods for DDFtoUniversal

**************************************************************************/

void DDFtoUniversal :: receiveData ()
{
	// 1. get data from the DDF geodesic.
	getData();

	// Check it is an input or output.
	// BUG: g++ 2.2.2 is screwup up the vtbl for fns inherited
	// from two different baseclasses.
	if (DDFtoUniversal::isItInput()) {
		
		// 2. set the timeMark from token's arrival Time to wormhole.
		DDFWormhole* worm = (DDFWormhole*) wormhole;
		timeMark = worm->getArrivalTime();

	} else {
		// 2. annul increment of currentTime at the end of run.
		DDFScheduler* sched = (DDFScheduler*) innerSched();
		timeMark = sched->now() - sched->schedulePeriod;
	}

	// transfer Data
	transferData();
}

void DDFtoUniversal :: initialize() {
	PortHole :: initialize();
	ToEventHorizon :: initialize();		
}

int DDFtoUniversal :: isItInput() const { return EventHorizon :: isItInput(); }
int DDFtoUniversal :: isItOutput() const {return EventHorizon:: isItOutput();}

EventHorizon* DDFtoUniversal :: asEH() { return this; }
	
// my domain
const char* DDFtoUniversal :: domain () const { return DDFdomainName ;}

/**************************************************************************

	methods for DDFfromUniversal

**************************************************************************/

void DDFfromUniversal :: sendData ()
{
	// 1. transfer data from the other side of the event horizon
	transferData();

	while (tokenNew) {
		// 2. put data to the DDF geodesic
		putData();
                
                // 3. repeat as long as new data exist.
                tokenNew = FALSE;
                transferData();
	}
}

// wait until has enough number of tokens to fire the inside star
// (#repetitions) times from DDF scheduler.

int DDFfromUniversal :: ready() {
	DFPortHole *pFar = (DFPortHole*)far();
	int target = pFar->numXfer() * pFar->parentReps();
	if (numTokens() >= target) {
		// yes, enough tokens
		return TRUE;
	}
	return FALSE;
}

void DDFfromUniversal :: initialize() {
	PortHole :: initialize();
	FromEventHorizon :: initialize();
}

int DDFfromUniversal :: isItInput() const {return EventHorizon::isItInput();}
int DDFfromUniversal :: isItOutput() const {return EventHorizon::isItOutput();}

EventHorizon* DDFfromUniversal :: asEH() { return this; }

// my domain
const char* DDFfromUniversal :: domain () const { return DDFdomainName ;}
