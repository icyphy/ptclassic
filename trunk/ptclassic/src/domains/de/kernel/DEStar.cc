/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 5/30/90

 Class DERepeatStar's member functions, formerly here, now have
 their own file DERepeatStar.cc

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "DEStar.h"
#include "StringList.h"
#include "Output.h"
#include "PriorityQueue.h"

/*******************************************************************

	class DEStar methods

********************************************************************/

// Redefine method setting internal data in the Block
// so that various DE-specific initilizations can be performed.
// If the parent pointer is not provied, it defaults to NULL
Block& DEStar :: setBlock(char* s, Block* parent = NULL) {
	// First invoke the generic setBlock
	Block::setBlock(s,parent);

	// Then, perform the DE specific initialization.
	completionTime = 0.0;
	arrivalTime = 0.0;

	return *this;
}

// initialize DE-specific members.
void DEStar :: prepareForScheduling() {
	arrivalTime = 0.0;
	completionTime = 0.0;
}

void DEStar :: fire() {
	go();
	BlockPortIter next(*this);
	for (int k = numberPorts(); k > 0; k--) 
		(next++)->sendData();
}

// The following is defined in DEDomain.cc -- this forces that module
// to be included if any DE stars are linked in.
extern const char DEdomainName[];

const char* DEStar :: domain() const {
	return DEdomainName;
}



