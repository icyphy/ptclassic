static const char file_id[] = "XXXWormhole.cc";

/*  Version $Id$

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

    Programmer:		T.M. Parks, J. Buck
    Date of creation:	17 January 1992
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "XXXWormhole.h"

/*******************************************************************

	class XXXWormhole methods

********************************************************************/

void XXXWormhole :: setup() {
	Wormhole :: setup();
}

void XXXWormhole :: go() {
	// set the currentTime of the inner domain.
	// (somehow)

	// run
	Wormhole::run();
}

void XXXWormhole :: wrapup() {
	target->wrapup();
}

XXXWormhole :: ~XXXWormhole() { freeContents();}

// Constructor
XXXWormhole :: XXXWormhole(Galaxy& g,Target* t) : Wormhole(*this,g,t)
{
	buildEventHorizons ();
}

StringList XXXWormhole :: printVerbose() const {
	return Wormhole :: print(0);
}

StringList XXXWormhole :: printRecursive() const {
	return Wormhole :: print(1);
}


// cloner -- clone the inside and make a new wormhole from that.
Block* XXXWormhole :: clone() const {
	LOG_NEW; return new XXXWormhole(gal.clone()->asGalaxy(),
					target->cloneTarget());
}

Block* XXXWormhole :: makeNew() const {
	LOG_NEW; return new XXXWormhole(gal.makeNew()->asGalaxy(),
					target->cloneTarget());
}

// return stop time
double XXXWormhole :: getStopTime() {
	// return correct value!
	return 0.0;
}

/**************************************************************************

	methods for XXXtoUniversal

**************************************************************************/

void XXXtoUniversal :: receiveData ()
{
	// fill in
}

void XXXtoUniversal :: initialize() {
	InXXXPort :: initialize();
	ToEventHorizon :: initialize();		
}

int XXXtoUniversal :: isItInput() const { return EventHorizon :: isItInput(); }
int XXXtoUniversal :: isItOutput() const {return EventHorizon:: isItOutput();}

EventHorizon* XXXtoUniversal :: asEH() { return this; }
	
/**************************************************************************

	methods for XXXfromUniversal

**************************************************************************/

void XXXfromUniversal :: sendData ()
{
	// fill in
}

void XXXfromUniversal :: initialize() {
	OutXXXPort :: initialize();
	FromEventHorizon :: initialize();
}

int XXXfromUniversal :: isItInput() const {return EventHorizon::isItInput();}
int XXXfromUniversal :: isItOutput() const {return EventHorizon::isItOutput();}

EventHorizon* XXXfromUniversal :: asEH() { return this; }
