static const char file_id[] = "DynamicGalaxy.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha and J. Buck
 Date of creation: 1/14/92

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "DynamicGalaxy.h"

// Destructor body (has a different name so it can be called
// in other contexts)

void DynamicGalaxy :: zero () {
	// delete component blocks
	deleteAllBlocks();
	// delete component ports and multiports.
	deleteAllGenPorts();
	// delete states.
	deleteAllStates();
}

// isa
ISA_FUNC(DynamicGalaxy,Galaxy);

// clone function: return error

Block* DynamicGalaxy::makeNew() const {
	Error::abortRun(*this, " cannot clone object of type 'DynamicGalaxy'");
	return 0;
}
