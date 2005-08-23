static const char file_id[] = "CGCWormhole.cc";
/******************************************************************
Version identification:
 @(#)CGCWormhole.cc	1.10	01/01/96

Copyright (c) 1990-1996 The Regents of the University of California.
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
 Date of creation: 12/1/92

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGCWormhole.h"
#include "SDFScheduler.h"

/**************************************************************************

	class CGCWormhole methods

********************************************************************/
// Constructor
CGCWormhole :: CGCWormhole(Galaxy& g, Target* t) : CGWormBase(*this,g,t)
	{ buildEventHorizons(); }

CGCWormhole :: ~CGCWormhole() { freeContents(); }

// cloner -- clone the inside and make a new wormhole from that.
Block* CGCWormhole :: clone() const {
	LOG_NEW; return new CGCWormhole(gal.clone()->asGalaxy(), myTarget()->cloneTarget());
}

Wormhole* CGCWormhole :: asWormhole() { return this; }

void CGCWormhole :: go() {
    scheduler()->compileRun();
}

/**************************************************************************

	methods for CGCtoUniversal

**************************************************************************/

void CGCtoUniversal :: receiveData ()
{
	// 1. get data
	getData();

	// Check it is an input or output.
	// BUG: g++ 2.2.2 is screwup up the vtbl for fns inherited
	// from two different baseclasses.
	if (CGCtoUniversal::isItOutput()) {
		// 2. annul increment of currentTime at the end of run.
		SDFScheduler* sched = (SDFScheduler*) innerSched();
		timeMark = sched->now() - sched->schedulePeriod;
	}

	// transfer Data
	transferData();
}

void CGCtoUniversal :: initialize() {
	PortHole :: initialize();
	ToEventHorizon :: initialize();		
}

int CGCtoUniversal :: isItInput() const { return EventHorizon :: isItInput(); }
int CGCtoUniversal :: isItOutput() const {return EventHorizon:: isItOutput();}

EventHorizon* CGCtoUniversal :: asEH() { return this; }
	
/**************************************************************************

	methods for CGCfromUniversal

**************************************************************************/

void CGCfromUniversal :: sendData ()
{
	// 1. transfer data
	transferData();

	if (tokenNew) {
		// 2. put data
		putData();

	} 

	// no timeMark business since CGC is "untimed" domain.
}

// wait until has enough number of tokens to fire the inside star
// (#repetitions) times from CGC scheduler.

int CGCfromUniversal :: ready() {
	DFPortHole *pFar = (DFPortHole*)far();
	int target = pFar->numXfer() * pFar->parentReps();
	if (numTokens() >= target) {
		// yes, enough tokens
		return TRUE;
	}
	return FALSE;
}

void CGCfromUniversal :: initialize() {
	PortHole :: initialize();
	FromEventHorizon :: initialize();
}

int CGCfromUniversal :: isItInput() const {return EventHorizon::isItInput();}
int CGCfromUniversal :: isItOutput() const {return EventHorizon::isItOutput();}

EventHorizon* CGCfromUniversal :: asEH() { return this; }
