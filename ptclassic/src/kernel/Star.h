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

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 12/15/89

 Star is a Block that implements an atomic function...the
 Scheduler calls Stars at runtime to execute the simulation

*******************************************************************/
#ifndef _Star_h
#define _Star_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Block.h"
class Galaxy;
class Target;

	////////////////////////////////////
	// class Star
	////////////////////////////////////

class Star : public Block  {

public:
// The writer of a Star code has the option to
// provide the methods setup, go, and wrapup.
// Any Block, however, may have setup and wrapup.

	// define firing.  Derived classes must call Star::run.
	/* virtual */ int run();

	// Method to print out description
	/* virtual */ StringList print (int verbose = 0) const;

	// Return myself as a Star.  Overrides Block::asStar.
	/* virtual */ Star& asStar();
	/* virtual */ const Star& asStar() const;

	// Return my indexValue.
	int index() const { return indexValue;}

	// set the Target
	virtual void setTarget(Target* t);

	// Check whether invocations are parallelizable or not.
	void noInternalState() { inStateFlag = FALSE; }
	int hasInternalState() { return inStateFlag; }

	// constructor
	Star() : targetPtr(0), indexValue(-1), inStateFlag(TRUE) {}

	// class identification
	/* virtual */ int isA(const char*) const;
	/* virtual */ const char* className() const;

protected:
	// User supplied method.
	// Run the simulation, accessing Particles on the
	// input PortHoles, generating Particles on the output PortHoles
        virtual void go();

	Target* targetPtr;
private:
	int indexValue;
	int inStateFlag; // indicate there are internal states (default)
	friend setStarIndices(Galaxy&);
};

int setStarIndices(Galaxy&);
#endif
