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

	// Start the simulation; i.e. do everything necessary
	// to start over, like initializing state variables
	virtual void start() {};

	// Run the simulation, accessing Particles on the
	// input PortHoles, generating Particles on the output PortHoles
        virtual void go() {}; 

	// Perform functions at wrapup, such as collecting stats
        virtual void wrapup() {};

	// Method to print out description
	operator StringList ();
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

// The following methods are provided by the system and accessed
// by the Scheduler

	// Methods called by the Scheduler before go() and
	//  after go(); they call methods of the same name for each
	//  input or output PortHole
	// They can be used to do things like get Particles from
	//  input Geodesics, put Particles onto output Geodesics, etc.
	// In general behavior can be different for different types
	//  of PortHoles
	void beforeGo();
	void afterGo();

	// Redefine method setting internal data in the Block
	// so that various SDF-specific initilizations can be performed.
	// If the parent pointer is not provied, it defaults to NULL
	Block& setBlock(char* starName, Block* parent = NULL);
};

#endif
