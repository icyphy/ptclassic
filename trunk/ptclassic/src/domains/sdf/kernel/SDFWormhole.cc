static const char file_id[] = "SDFWormhole.cc";
/******************************************************************
Version identification:
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
	Wormhole :: setup();
	mark = 0;
}

void SDFWormhole :: go() {
	// set the currentTime of the inner domain.
	target->setCurrentTime(arrivalTime);

	// run
	Wormhole::run();
}

void SDFWormhole :: wrapup() {
	target->wrapup();
}

// return the next time Stamp for the stopping condition of the inner timed
// domain.

double SDFWormhole :: getStopTime() {
	// set the next token arrival time
	arrivalTime += space;
	return arrivalTime;
}

// return the token's arrival time.
// At the beginning, token is arrived at the currentTime of the SDF
// scheduler. It is incremented by "space" afterwards.

double SDFWormhole :: getArrivalTime() {
	// At the very first call, we initialize "arrivalTime".
	if (!mark) {
		SDFScheduler* sched = (SDFScheduler*) outerSched();
		arrivalTime = sched->now();
		space = sched->schedulePeriod / double(repetitions) ;
		mark = 1;
	}
	return arrivalTime;
}

// Constructor
SDFWormhole :: SDFWormhole(Galaxy& g,Target* t) : Wormhole(*this,g,t)
{
	buildEventHorizons ();
}

StringList SDFWormhole :: printVerbose() const {
	return Wormhole :: print(0);
}

StringList SDFWormhole :: printRecursive() const {
	return Wormhole :: print(1);
}


// cloner -- clone the inside and make a new wormhole from that.
Block* SDFWormhole :: clone() const {
	LOG_NEW; return new SDFWormhole(gal.clone()->asGalaxy(),
					target->cloneTarget());
}

Block* SDFWormhole :: makeNew() const {
	LOG_NEW; return new SDFWormhole(gal.makeNew()->asGalaxy(),
					target->cloneTarget());
}

/**************************************************************************

	methods for SDFtoUniversal

**************************************************************************/

void SDFtoUniversal :: receiveData ()
{
	// 1. get data
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
	// 1. transfer data
	transferData();

	if (tokenNew) {
		// 2. put data
		putData();

	} else if (SDFfromUniversal::isItOutput()) {
		// 2. inside domain does not generate enough number of tokens,
		//    which is error.
		Error::abortRun(*this, "not enough output tokens ",
				"at SDF wormhole boundary");
	}

	// no timeMark business since SDF is "untimed" domain.
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
