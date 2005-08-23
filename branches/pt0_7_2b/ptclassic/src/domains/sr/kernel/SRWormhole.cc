static const char file_id[] = "SRWormhole.cc";

/*  Version $Id$

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

    Programmer:		T.M. Parks, J. Buck
    Date of creation:	17 January 1992
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "SRWormhole.h"

/*******************************************************************

	class SRWormhole methods

********************************************************************/

void SRWormhole :: setup() {
	Wormhole :: setup();
}

void SRWormhole :: go() {
	// set the currentTime of the inner domain.
	// (somehow)

	// run
	Wormhole::run();
}

void SRWormhole :: wrapup() {
	myTarget()->wrapup();
}

SRWormhole :: ~SRWormhole() { freeContents();}

// Constructor
SRWormhole :: SRWormhole(Galaxy& g,Target* t) : Wormhole(*this,g,t)
{
	buildEventHorizons ();
}

// cloner -- clone the inside and make a new wormhole from that.
Block* SRWormhole :: clone() const {
	LOG_NEW; return new SRWormhole(gal.clone()->asGalaxy(),
					myTarget()->cloneTarget());
}

Block* SRWormhole :: makeNew() const {
	LOG_NEW; return new SRWormhole(gal.makeNew()->asGalaxy(),
					myTarget()->cloneTarget());
}

// return stop time
double SRWormhole :: getStopTime() {
	// return correct value!
	return 0.0;
}

/**************************************************************************

	methods for SRtoUniversal

**************************************************************************/

void SRtoUniversal :: receiveData ()
{
	// fill in
}

void SRtoUniversal :: initialize() {
	InSRPort :: initialize();
	ToEventHorizon :: initialize();		
}

int SRtoUniversal :: isItInput() const { return EventHorizon :: isItInput(); }
int SRtoUniversal :: isItOutput() const {return EventHorizon:: isItOutput();}

EventHorizon* SRtoUniversal :: asEH() { return this; }
	
/**************************************************************************

	methods for SRfromUniversal

**************************************************************************/

void SRfromUniversal :: sendData ()
{
	// fill in
}

void SRfromUniversal :: initialize() {
	OutSRPort :: initialize();
	FromEventHorizon :: initialize();
}

int SRfromUniversal :: isItInput() const {return EventHorizon::isItInput();}
int SRfromUniversal :: isItOutput() const {return EventHorizon::isItOutput();}

EventHorizon* SRfromUniversal :: asEH() { return this; }
