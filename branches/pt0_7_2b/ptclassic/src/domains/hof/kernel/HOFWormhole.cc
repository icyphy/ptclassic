static const char file_id[] = "HOFWormhole.cc";

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

#include "HOFWormhole.h"

/*******************************************************************

	class HOFWormhole methods

********************************************************************/

void HOFWormhole :: setup() {
	Wormhole :: setup();
}

void HOFWormhole :: go() {
	// set the currentTime of the inner domain.
	// (somehow)

	// run
	Wormhole::run();
}

void HOFWormhole :: wrapup() {
	target->wrapup();
}

HOFWormhole :: ~HOFWormhole() { freeContents();}

// Constructor
HOFWormhole :: HOFWormhole(Galaxy& g,Target* t) : Wormhole(*this,g,t)
{
	buildEventHorizons ();
}

StringList HOFWormhole :: printVerbose() const {
	return Wormhole :: print(0);
}

StringList HOFWormhole :: printRecursive() const {
	return Wormhole :: print(1);
}


// cloner -- clone the inside and make a new wormhole from that.
Block* HOFWormhole :: clone() const {
	LOG_NEW; return new HOFWormhole(gal.clone()->asGalaxy(),
					target->cloneTarget());
}

Block* HOFWormhole :: makeNew() const {
	LOG_NEW; return new HOFWormhole(gal.makeNew()->asGalaxy(),
					target->cloneTarget());
}

// return stop time
double HOFWormhole :: getStopTime() {
	// return correct value!
	return 0.0;
}

/**************************************************************************

	methods for HOFtoUniversal

**************************************************************************/

void HOFtoUniversal :: receiveData ()
{
	// fill in
}

void HOFtoUniversal :: initialize() {
	InHOFPort :: initialize();
	ToEventHorizon :: initialize();		
}

int HOFtoUniversal :: isItInput() const { return EventHorizon :: isItInput(); }
int HOFtoUniversal :: isItOutput() const {return EventHorizon:: isItOutput();}

EventHorizon* HOFtoUniversal :: asEH() { return this; }
	
/**************************************************************************

	methods for HOFfromUniversal

**************************************************************************/

void HOFfromUniversal :: sendData ()
{
	// fill in
}

void HOFfromUniversal :: initialize() {
	OutHOFPort :: initialize();
	FromEventHorizon :: initialize();
}

int HOFfromUniversal :: isItInput() const {return EventHorizon::isItInput();}
int HOFfromUniversal :: isItOutput() const {return EventHorizon::isItOutput();}

EventHorizon* HOFfromUniversal :: asEH() { return this; }
