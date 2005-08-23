static const char file_id[] = "ACSWormhole.cc";

/*  Version @(#)ACSWormhole.cc	1.6 09/08/99

@Copyright (c) 1998 The Regents of the University of California.
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

    Programmer:		T.M. Parks, J. Buck
    Date of creation:	17 January 1992
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "ACSWormhole.h"

/*******************************************************************

	class ACSWormhole methods

********************************************************************/

void ACSWormhole :: setup() {
	Wormhole :: setup();
}

void ACSWormhole :: go() {
	// set the currentTime of the inner domain.
	// (somehow)

	// run
	Wormhole::run();
}

void ACSWormhole :: wrapup() {
	myTarget()->wrapup();
}

ACSWormhole :: ~ACSWormhole() { freeContents();}

// Constructor
ACSWormhole :: ACSWormhole(Galaxy& g,Target* t) : Wormhole(*this,g,t)
{
	buildEventHorizons ();
}

// cloner -- clone the inside and make a new wormhole from that.
Block* ACSWormhole :: clone() const {
	LOG_NEW; return new ACSWormhole(gal.clone()->asGalaxy(),
					myTarget()->cloneTarget());
}

Block* ACSWormhole :: makeNew() const {
	LOG_NEW; return new ACSWormhole(gal.makeNew()->asGalaxy(),
					myTarget()->cloneTarget());
}

// return stop time
double ACSWormhole :: getStopTime() {
	// return correct value!
	return 0.0;
}

/**************************************************************************

	methods for ACStoUniversal

**************************************************************************/

void ACStoUniversal :: receiveData ()
{
	// fill in
}

void ACStoUniversal :: initialize() {
	InACSPort :: initialize();
	ToEventHorizon :: initialize();		
}

int ACStoUniversal :: isItInput() const { return EventHorizon :: isItInput(); }
int ACStoUniversal :: isItOutput() const {return EventHorizon:: isItOutput();}

EventHorizon* ACStoUniversal :: asEH() { return this; }
	
/**************************************************************************

	methods for ACSfromUniversal

**************************************************************************/

void ACSfromUniversal :: sendData ()
{
	// fill in
}

void ACSfromUniversal :: initialize() {
	OutACSPort :: initialize();
	FromEventHorizon :: initialize();
}

int ACSfromUniversal :: isItInput() const {return EventHorizon::isItInput();}
int ACSfromUniversal :: isItOutput() const {return EventHorizon::isItOutput();}

EventHorizon* ACSfromUniversal :: asEH() { return this; }
