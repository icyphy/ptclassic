static const char file_id[] = "C50Wormhole.cc";
/******************************************************************
Version identification:
@(#)C50Wormhole.cc	1.2	8/15/96

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

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "C50Wormhole.h"
#include "StringList.h"
#include "Error.h"

/*******************************************************************

	class C50Wormhole methods

********************************************************************/

void C50Wormhole :: setup() {
	// main setup routine.
	CGWormBase :: setup();
}

void C50Wormhole :: go() { 
	scheduler()->compileRun();
}

// Constructor
C50Wormhole :: C50Wormhole(Galaxy& g, Target* t) : CGWormBase(*this,g,t)
	{ buildEventHorizons(); }

C50Wormhole :: ~C50Wormhole() { freeContents(); }

// cloner -- clone the inside and make a new wormhole from that.
Block* C50Wormhole :: clone() const {
	LOG_NEW; return new C50Wormhole(gal.clone()->asGalaxy(), myTarget()->cloneTarget());
}

Wormhole* C50Wormhole :: asWormhole() { return this; }


/**************************************************************************

	methods for C50toUniversal

**************************************************************************/

void C50toUniversal :: receiveData ()
{
	// 1. get data
	getData();

	// Check it is an input or output.
	// BUG: g++ 2.2.2 is screwup up the vtbl for fns inherited
	// from two different baseclasses.
	if (C50toUniversal::isItOutput()) {
		// 2. annul increment of currentTime at the end of run.
		SDFScheduler* sched = (SDFScheduler*) innerSched();
		timeMark = sched->now() - sched->schedulePeriod;
	}

	// transfer Data
	transferData();
}

void C50toUniversal :: initialize() {
	PortHole :: initialize();
	ToEventHorizon :: initialize();		
}

int C50toUniversal :: isItInput() const { return EventHorizon :: isItInput(); }
int C50toUniversal :: isItOutput() const {return EventHorizon:: isItOutput();}

EventHorizon* C50toUniversal :: asEH() { return this; }
	
/**************************************************************************

	methods for C50fromUniversal

**************************************************************************/

void C50fromUniversal :: sendData ()
{
	// 1. transfer data
	transferData();

	if (tokenNew) {
		// 2. put data
//		putData();

	} 

	// no timeMark business since C50 is "untimed" domain.
}

// wait until has enough number of tokens to fire the inside star
// (#repetitions) times from C50 scheduler.

int C50fromUniversal :: ready() {
	DFPortHole *pFar = (DFPortHole*)far();
	int target = pFar->numXfer() * pFar->parentReps();
	if (numTokens() >= target) {
		// yes, enough tokens
		return TRUE;
	}
	return FALSE;
}

void C50fromUniversal :: initialize() {
	PortHole :: initialize();
	FromEventHorizon :: initialize();
}

int C50fromUniversal :: isItInput() const {return EventHorizon::isItInput();}
int C50fromUniversal :: isItOutput() const {return EventHorizon::isItOutput();}

EventHorizon* C50fromUniversal :: asEH() { return this; }
