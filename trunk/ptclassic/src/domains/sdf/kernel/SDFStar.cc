/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 5/29/90
 Split off from Star.h by J. Buck

*******************************************************************/

#include "SDFStar.h"
#include "SDFConnect.h"

/*******************************************************************

	class SDFStar methods

********************************************************************/



// Redefine method setting internal data in the Block
// so that various SDF-specific initilizations can be performed.
// If the parent pointer is not provied, it defaults to NULL
Block& SDFStar :: setBlock(char* s, Block* parent = NULL) {
	// First invoke the generic setBlock
	Block::setBlock(s,parent);

	// Then perform SDF-specific initializations
	repetitions = 0;
	noTimes = 0;

	return *this;
}
