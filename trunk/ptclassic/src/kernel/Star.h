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

#include "type.h"
#include "Block.h"


	////////////////////////////////////
	// class Star
	////////////////////////////////////

class Star : public Block  {

// Have to make the following public or make every individual
// scheduler a friend.  It is not sufficient to make the base class
// Scheduler a friend (is this yet another language defect?).

public:
// The writer of a Star code has the option to
// provide the following methods

	// Start the simulation; i.e. do everything necessary
	// to start over, like initializing state variables
	virtual void start() {};

	// Run the simulation, accessing Particles on the
	// input PortHoles, generating Particles on the output PortHoles
        virtual void go() {}; 

	// Perform functions at wrapup, such as collecting stats
        virtual void wrapup() {};

	// Method to print out description
	StringList printVerbose ();

};

#endif
