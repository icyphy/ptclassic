static const char file_id[] = "BDFWormhole.cc";
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

#include "BDFWormhole.h"
#include "BDFScheduler.h"
#include "StringList.h"
#include "Error.h"

/*******************************************************************

	class BDFWormhole methods

********************************************************************/

void BDFWormhole :: setup() {
	Wormhole :: setup();
	mark = 0;
}

void BDFWormhole :: go() {
	// set the currentTime of the inner domain.
	myTarget()->setCurrentTime(arrivalTime);

	// run
	Wormhole::run();
}

void BDFWormhole :: wrapup() {
	myTarget()->wrapup();
}

// return the next time Stamp for the stopping condition of the inner timed
// domain.

double BDFWormhole :: getStopTime() {
	// set the next token arrival time
	arrivalTime += space;
	return arrivalTime;
}

// return the token's arrival time.
// At the beginning, token is arrived at the currentTime of the BDF
// scheduler. It is incremented by "space" afterwards.

double BDFWormhole :: getArrivalTime() {
	// At the very first call, we initialize "arrivalTime".
	if (!mark) {
		BDFScheduler* sched = (BDFScheduler*) parent()->scheduler();
		arrivalTime = sched->now();
		space = sched->schedulePeriod / double(repetitions) ;
		mark = 1;
	}
	return arrivalTime;
}

// Constructor
BDFWormhole :: BDFWormhole(Galaxy& g,Target* t) : Wormhole(*this,g,t)
{
	space = arrivalTime = 0.0;
	mark = 0;
	buildEventHorizons ();
}

StringList BDFWormhole :: printVerbose() const {
	return Wormhole :: print(0);
}

StringList BDFWormhole :: printRecursive() const {
	return Wormhole :: print(1);
}


// cloner -- clone the inside and make a new wormhole from that.
Block* BDFWormhole :: clone() const {
	LOG_NEW; return new BDFWormhole(gal.clone()->asGalaxy(),
					myTarget()->cloneTarget());
}

Block* BDFWormhole :: makeNew() const {
	LOG_NEW; return new BDFWormhole(gal.makeNew()->asGalaxy(),
					myTarget()->cloneTarget());
}

/**************************************************************************

	methods for BDFtoUniversal

**************************************************************************/

void BDFtoUniversal :: receiveData ()
{
	// 1. get data
	getData();

	// Check it is an input or output.
	// BUG: g++ 2.2.2 is screwup up the vtbl for fns inherited
	// from two different baseclasses.
	if (BDFtoUniversal::isItInput()) {
		
		// 2. set the timeMark from token's arrival Time to wormhole.
		BDFWormhole* worm = (BDFWormhole*) wormhole;
		timeMark = worm->getArrivalTime();

	} else {
		// 2. annul increment of currentTime at the end of run.
		BDFScheduler* sched = (BDFScheduler*) parent()->scheduler();
		timeMark = sched->now() - sched->schedulePeriod;
	}

	// transfer Data
	transferData();
}

void BDFtoUniversal :: initialize() {
	InBDFPort :: initialize();
	ToEventHorizon :: initialize();		
}

int BDFtoUniversal :: isItInput() const { return EventHorizon :: isItInput(); }
int BDFtoUniversal :: isItOutput() const {return EventHorizon:: isItOutput();}

EventHorizon* BDFtoUniversal :: asEH() { return this; }
	
/**************************************************************************

	methods for BDFfromUniversal

**************************************************************************/

void BDFfromUniversal :: sendData ()
{
	// 1. transfer data
	transferData();

	if (tokenNew) {
		// 2. put data
		putData();

	} else if (BDFfromUniversal::isItOutput()) {
		// 2. inside domain does not generate enough number of tokens,
		//    which is error.
		Error::abortRun(*this, "not enough output tokens ",
				"at BDF wormhole boundary");
	}

	// no timeMark business since BDF is "untimed" domain.
}

// wait until has enough number of tokens to fire the inside star
// (#repetitions) times from BDF scheduler.

int BDFfromUniversal :: ready() {
	DFPortHole *pFar = (DFPortHole*)far();
	int target = pFar->numXfer() * pFar->parentReps();
	if (numTokens() >= target) {
		// yes, enough tokens
		return TRUE;
	}
	return FALSE;
}

void BDFfromUniversal :: initialize() {
	OutBDFPort :: initialize();
	FromEventHorizon :: initialize();
}

int BDFfromUniversal :: isItInput() const {return EventHorizon::isItInput();}
int BDFfromUniversal :: isItOutput() const {return EventHorizon::isItOutput();}

EventHorizon* BDFfromUniversal :: asEH() { return this; }
