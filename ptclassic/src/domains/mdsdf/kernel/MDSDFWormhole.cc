static const char file_id[] = "MDSDFWormhole.cc";

/*  Version $Id$

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

    Programmer:		T.M. Parks, J. Buck
    Date of creation:	17 January 1992
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "MDSDFWormhole.h"

/*******************************************************************

	class MDSDFWormhole methods

********************************************************************/

void MDSDFWormhole :: setup() {
	Wormhole :: setup();
}

void MDSDFWormhole :: go() {
	// set the currentTime of the inner domain.
	// (somehow)

	// run
	Wormhole::run();
}

void MDSDFWormhole :: wrapup() {
	target->wrapup();
}

MDSDFWormhole :: ~MDSDFWormhole() { freeContents();}

// Constructor
MDSDFWormhole :: MDSDFWormhole(Galaxy& g,Target* t) : Wormhole(*this,g,t)
{
	buildEventHorizons ();
}

StringList MDSDFWormhole :: printVerbose() const {
	return Wormhole :: print(0);
}

StringList MDSDFWormhole :: printRecursive() const {
	return Wormhole :: print(1);
}


// cloner -- clone the inside and make a new wormhole from that.
Block* MDSDFWormhole :: clone() const {
	LOG_NEW; return new MDSDFWormhole(gal.clone()->asGalaxy(),
					target->cloneTarget());
}

Block* MDSDFWormhole :: makeNew() const {
	LOG_NEW; return new MDSDFWormhole(gal.makeNew()->asGalaxy(),
					target->cloneTarget());
}

// return stop time
double MDSDFWormhole :: getStopTime() {
	// return correct value!
	return 0.0;
}

/**************************************************************************

	methods for MDSDFtoUniversal

**************************************************************************/

void MDSDFtoUniversal :: receiveData ()
{
	// fill in
}

void MDSDFtoUniversal :: initialize() {
	InMDSDFPort :: initialize();
	ToEventHorizon :: initialize();		
}

int MDSDFtoUniversal :: isItInput() const { return EventHorizon :: isItInput(); }
int MDSDFtoUniversal :: isItOutput() const {return EventHorizon:: isItOutput();}

EventHorizon* MDSDFtoUniversal :: asEH() { return this; }
	
/**************************************************************************

	methods for MDSDFfromUniversal

**************************************************************************/

void MDSDFfromUniversal :: sendData ()
{
	// fill in
}

void MDSDFfromUniversal :: initialize() {
	OutMDSDFPort :: initialize();
	FromEventHorizon :: initialize();
}

int MDSDFfromUniversal :: isItInput() const {return EventHorizon::isItInput();}
int MDSDFfromUniversal :: isItOutput() const {return EventHorizon::isItOutput();}

EventHorizon* MDSDFfromUniversal :: asEH() { return this; }
