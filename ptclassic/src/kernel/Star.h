/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 12/15/89

 This header includes everything needed by a class implementing
 the Star function

*******************************************************************/
#ifndef _Star_h
#define _Star_h 1

#include "type.h"
#include "Connect.h"
#include "Block.h"
#include "Output.h"


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

	// Access the parameters by name, and store them
	// in the associated parameter variables
	// This function is virtual in base class Block, so put
	// in the Star only if needed
	setParameters() {};

	// Initialize the simulation; i.e. do everything necessary
	// to start over, like initializing state variables
        virtual int initialize() {};

	// Run the simulation, accessing Particles on the
	// input PortHoles, generating Particles on the output PortHoles
        virtual int go() {}; 

	// Perform functions at wrapup, such as collecting stats
        virtual int wrapup() {};

	// Method to print out description
	operator char* ();
};

#include "Fraction.h"

	////////////////////////////////////
	// class SDFStar
	////////////////////////////////////

class SDFStar : public Star  {

public:
	// The number of repetitions of the star in a periodic
	// schedule.  Initialized to 1 by setBlock.  Set to correct
	// value by an SDF scheduler.  It is represented as a fraction
	// for the convenience of the scheduler.
	Fraction repetitions;

	// During scheduling, the scheduler must keep track of how
	// many times it has scheduled a star.  This is a convenient
	// place to do that.
	unsigned noTimes;

	// Redefine method setting internal data in the Block
	// so that various SDF-specific initilizations can be performed.
	// If the parent pointer is not provied, it defaults to NULL
	Block& setBlock(char* starName, Block* parent = NULL);

	// The following method produces Particles on the geodesics
	// connected to output ports for the star to write its data.
	// This method is invoked automatically by the scheduler.
	void produceParticles();

	// The following method discards Particles that have been
	// consumed by the star.  It is automatically invoked
	// by the scheduler also.
	void consumeParticles();
};

#endif
