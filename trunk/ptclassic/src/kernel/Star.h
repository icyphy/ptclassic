/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 12/15/89

 Star is a Block that implements an atomic function...the
 Scheduler calls Stars at runtime to execute the simulation

*******************************************************************/
#ifndef _Star_h
#define _Star_h 1

#include "Block.h"

	////////////////////////////////////
	// class Star
	////////////////////////////////////

class Star : public Block  {

public:
// The writer of a Star code has the option to
// provide the methods start, go, and wrapup.
// Any Block, however, may have start and wrapup.

	// User supplied method.
	// Run the simulation, accessing Particles on the
	// input PortHoles, generating Particles on the output PortHoles
        virtual void go();

	// Method to print out description
	StringList printVerbose ();

	// Return myself as a Star.  Overrides Block::asStar.
	Star& asStar() const;

	// Return my domain (e.g. SDF, DE, etc.)
	virtual const char* domain() const;
};

#endif
