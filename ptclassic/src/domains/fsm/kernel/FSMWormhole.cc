static const char file_id[] = "FSMWormhole.cc";

/*  $Id$

@Copyright (c) 1992-%Q% The Regents of the University of California.
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

    Programmer:		Bilung Lee
    Date of creation:	3/3/96
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "FSMWormhole.h"
#include "FSMScheduler.h"


/*******************************************************************

	class FSMWormhole methods

********************************************************************/

void FSMWormhole :: setup() {
	Wormhole :: setup();
}

void FSMWormhole :: go() {
	// set the currentTime of the inner domain.
	// (somehow)

	// run
	Wormhole::run();
}

void FSMWormhole :: wrapup() {
	myTarget()->wrapup();
}

double FSMWormhole :: getStopTime() {
    FSMScheduler* sched = (FSMScheduler*) outerSched();
    return sched->now();
}

// get token's arrival time from outerSched()->now()
// to set the timeMark.
double FSMWormhole :: getArrivalTime() {
    FSMScheduler* sched = (FSMScheduler*) outerSched();
    return sched->now();
}

FSMWormhole :: ~FSMWormhole() { freeContents();}

// Constructor
FSMWormhole :: FSMWormhole(Galaxy& g,Target* t) : Wormhole(*this,g,t) {
	buildEventHorizons ();
}

// cloner -- clone the inside and make a new wormhole from that.
Block* FSMWormhole :: clone() const {
	LOG_NEW; return new FSMWormhole(gal.clone()->asGalaxy(),
					myTarget()->cloneTarget());
}

Block* FSMWormhole :: makeNew() const {
	LOG_NEW; return new FSMWormhole(gal.makeNew()->asGalaxy(),
					myTarget()->cloneTarget());
}

/**************************************************************************

	methods for FSMtoUniversal

**************************************************************************/

void FSMtoUniversal :: receiveData ()
{
    while (myGeodesic->size()) {
      // If there is still a particle in geodesic, get and transfer it.
 
      // 1. get data from the FSM geodesic.
      getData();

      // Check it is an input or output.
      // BUG: g++ 2.2.2 is screwup up the vtbl for fns inherited
      // from two different baseclasses.
      if (FSMtoUniversal::isItInput()) {

	// 2. set the timeMark from token's arrival Time to wormhole.
	FSMWormhole* worm = (FSMWormhole*) wormhole;
	timeMark = worm->getArrivalTime();
      } else {

	// 2. set the timeMark from the innerSched()->now().
	// reactive system takes zero time to output.
	FSMScheduler* sched = (FSMScheduler*) innerSched();
	timeMark = sched->now();
      }

      // transfer Data
      transferData();
    }
}

void FSMtoUniversal :: initialize() {
	InFSMPort :: initialize();
	ToEventHorizon :: initialize();		
}

int FSMtoUniversal :: isItInput() const { return EventHorizon :: isItInput(); }
int FSMtoUniversal :: isItOutput() const {return EventHorizon:: isItOutput();}

EventHorizon* FSMtoUniversal :: asEH() { return this; }
	
/**************************************************************************

	methods for FSMfromUniversal

**************************************************************************/

void FSMfromUniversal :: sendData () {
    // 1. transfer data from the other side of the event horizon
    transferData();

    while (tokenNew) {
      // 2. put data to the FSM geodesic
      putData();
      
      // 3. repeat as long as new data exists.
      tokenNew = FALSE;
      transferData();
    } 
}

void FSMfromUniversal :: initialize() {
	OutFSMPort :: initialize();
	FromEventHorizon :: initialize();
}

int FSMfromUniversal :: isItInput() const {return EventHorizon::isItInput();}
int FSMfromUniversal :: isItOutput() const {return EventHorizon::isItOutput();}

EventHorizon* FSMfromUniversal :: asEH() { return this; }
