static const char file_id[] = "VHDLWormhole.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-1994 The Regents of the University of California.
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

 Programmer: Michael C. Williamson

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLWormhole.h"
#include "SDFScheduler.h"
#include "StringList.h"
#include "Error.h"

/*******************************************************************

	class VHDLWormhole methods

********************************************************************/

void VHDLWormhole :: setup() {
//	profile = myProfile;

//	// get the number of processors.
//	setOuterTarget(target());

	// main setup routine.
	CGWormBase :: setup();
}

void VHDLWormhole :: go() {
}

CGWormBase* VHDLWormhole :: myWormhole() { return selfWorm; }

//Profile* VHDLWormhole :: getProfile(int pNum) {
//	return CGWormBase :: getProfile(pNum);
//}

// Constructor
VHDLWormhole :: VHDLWormhole(Galaxy& g, Target* t) : CGWormBase(*this,g,t)
	{ buildEventHorizons(); }

VHDLWormhole :: ~VHDLWormhole() {
	freeContents();
}

// cloner -- clone the inside and make a new wormhole from that.
Block* VHDLWormhole :: clone() const {
	LOG_NEW; return new VHDLWormhole(gal.clone()->asGalaxy(), myTarget()->cloneTarget());
}

/**************************************************************************

	methods for VHDLtoUniversal

**************************************************************************/

void VHDLtoUniversal :: receiveData ()
{
	// 1. get data
	getData();

	// Check it is an input or output.
	// BUG: g++ 2.2.2 is screwup up the vtbl for fns inherited
	// from two different baseclasses.
	if (VHDLtoUniversal::isItOutput()) {
		// 2. annul increment of currentTime at the end of run.
		SDFScheduler* sched = (SDFScheduler*) innerSched();
		timeMark = sched->now() - sched->schedulePeriod;
	}

	// transfer Data
	transferData();
}

void VHDLtoUniversal :: initialize() {
	PortHole :: initialize();
	ToEventHorizon :: initialize();		
}

int VHDLtoUniversal :: isItInput() const { return EventHorizon :: isItInput(); }
int VHDLtoUniversal :: isItOutput() const {return EventHorizon:: isItOutput();}

EventHorizon* VHDLtoUniversal :: asEH() { return this; }
	
/**************************************************************************

	methods for VHDLfromUniversal

**************************************************************************/

void VHDLfromUniversal :: sendData ()
{
	// 1. transfer data
	transferData();

	if (tokenNew) {
		// 2. put data
		putData();

	} 
}

// wait until has enough number of tokens to fire the inside star
// (#repetitions) times from VHDL scheduler.

int VHDLfromUniversal :: ready() {
	DFPortHole *pFar = (DFPortHole*)far();
	int target = pFar->numXfer() * pFar->parentReps();
	if (numTokens() >= target) {
		// yes, enough tokens
		return TRUE;
	}
	return FALSE;
}

void VHDLfromUniversal :: initialize() {
	PortHole :: initialize();
	FromEventHorizon :: initialize();
}

int VHDLfromUniversal :: isItInput() const {return EventHorizon::isItInput();}
int VHDLfromUniversal :: isItOutput() const {return EventHorizon::isItOutput();}

EventHorizon* VHDLfromUniversal :: asEH() { return this; }
