#ifndef _SDFStar_h
#define _SDFStar_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 5/29/90

 SDFStar is a Star that runs under the SDF Scheduler.
 Formerly defined in Star.h

*******************************************************************/

#include "Star.h"
#include "Fraction.h"
// next is included so SDFStar defns don't have to.
#include "SDFConnect.h"

// The following is defined in SDFScheduler.cc -- this forces that module
// to be included if any SDF stars are linked in.
extern const char SDFdomainName[];

	////////////////////////////////////
	// class SDFStar
	////////////////////////////////////

class SDFStar : public Star  {

public:
	// my domain
	const char* domain() { return SDFdomainName;}

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
};

#endif
