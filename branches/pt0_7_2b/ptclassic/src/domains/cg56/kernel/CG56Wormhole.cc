static const char file_id[] = "CG56Wormhole.cc";
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
 Date of creation: 12/1/92

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CG56Wormhole.h"
#include "StringList.h"
#include "Error.h"


/**************************************************************************

	methods for CG56toUniversal

**************************************************************************/

void CG56toUniversal :: receiveData ()
{
	// 1. get data
	getData();

	// Check it is an input or output.
	// BUG: g++ 2.2.2 is screwup up the vtbl for fns inherited
	// from two different baseclasses.
	if (CG56toUniversal::isItOutput()) {
		// 2. annul increment of currentTime at the end of run.
		SDFScheduler* sched = (SDFScheduler*) innerSched();
		timeMark = sched->now() - sched->schedulePeriod;
	}

	// transfer Data
	transferData();
}

void CG56toUniversal :: initialize() {
	PortHole :: initialize();
	ToEventHorizon :: initialize();		
}

int CG56toUniversal :: isItInput() const { return EventHorizon :: isItInput(); }
int CG56toUniversal :: isItOutput() const {return EventHorizon:: isItOutput();}

EventHorizon* CG56toUniversal :: asEH() { return this; }
	
/**************************************************************************

	methods for CG56fromUniversal

**************************************************************************/

void CG56fromUniversal :: sendData ()
{
	// 1. transfer data
	transferData();

	if (tokenNew) {
		// 2. put data
		putData();

	} 

	// no timeMark business since CG56 is "untimed" domain.
}

// wait until has enough number of tokens to fire the inside star
// (#repetitions) times from CG56 scheduler.

int CG56fromUniversal :: ready() {
	DFPortHole *pFar = (DFPortHole*)far();
	int target = pFar->numXfer() * pFar->parentReps();
	if (numTokens() >= target) {
		// yes, enough tokens
		return TRUE;
	}
	return FALSE;
}

void CG56fromUniversal :: initialize() {
	PortHole :: initialize();
	FromEventHorizon :: initialize();
}

int CG56fromUniversal :: isItInput() const {return EventHorizon::isItInput();}
int CG56fromUniversal :: isItOutput() const {return EventHorizon::isItOutput();}

EventHorizon* CG56fromUniversal :: asEH() { return this; }
