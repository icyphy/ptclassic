static const char file_id[] = "FSMWormhole.cc";

/*  $Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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
    return new FSMWormhole(gal.clone()->asGalaxy(),
			   myTarget()->cloneTarget());
}

Block* FSMWormhole :: makeNew() const {
    return new FSMWormhole(gal.makeNew()->asGalaxy(),
			   myTarget()->cloneTarget());
}

/**************************************************************************

	methods for FSMtoUniversal

**************************************************************************/

void FSMtoUniversal :: receiveData ()
{
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

    if (!strcmp(ghostDomain,"DE")) {
	if (int((*this)%0) == 0) {
	    // In pure FSM, 0 means absent, 
	    // so we shouldn't send event to DE.
	    tokenNew = FALSE;
	}
    }

    // transfer Data
    transferData();
}

void FSMtoUniversal :: initialize() {
    InFSMPort :: initialize();
    ToEventHorizon :: initialize();
    if (FSMtoUniversal::isItInput()) {
      // When FSM is an outer domain. Use the connection topology to find
      // out the inside domain because innerSched() is not setup yet.
      // (Scheduler is allocated in Target::setup())
      ghostDomain = asEH()->ghostAsPort()->far()->parent()->domain();

    } else {
      // When FSM is an inner domain.
      ghostDomain = outerSched()->domain();
    }
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

    if (FSMfromUniversal::isItOutput()) {
      // When FSM is an outer domain.
      if (!tokenNew) {
	// If there is no token: The only possibility is that there is 
	// no event from inner DE domain, then it is considered absent.
	(*this)%0 << 0;
	tokenNew = TRUE;

      } else {
	// There exist tokens.
	if (!strcmp(ghostDomain,"DE")) {
	  // If inner domain is DE, it is considered present as long as
	  // there is a token, and we don't care about its value.
	  (*this)%0 << 1;
	}	
      }
    }

    // Note: When FSM is an inner domain, if ghost domain is DE, this method
    // won't even have the chance to be invoked when there is no event in the
    // ghost DE port. Hence, the translation of signals for this case is 
    // handled by FSMScheduler::run() instead of here.

    if (tokenNew) {
      // 2. put data to the FSM geodesic
      putData();
    } else {
      Error::abortRun(*this,"Internal error: Should not happen this way!");
      return;
    }
}

void FSMfromUniversal :: initialize() {
    OutFSMPort :: initialize();
    FromEventHorizon :: initialize();
    if (FSMfromUniversal::isItOutput()) {
      // When FSM is an outer domain. Use the connection topology to find
      // out the inside domain because innerSched() is not setup yet.
      // (Scheduler is allocated in Target::setup())
      ghostDomain = asEH()->ghostAsPort()->far()->parent()->domain();

    } else {
      // When FSM is an inner domain.
      ghostDomain  = outerSched()->domain();
    }
}

int FSMfromUniversal :: isItInput() const {return EventHorizon::isItInput();}
int FSMfromUniversal :: isItOutput() const {return EventHorizon::isItOutput();}

EventHorizon* FSMfromUniversal :: asEH() { return this; }
