/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 1/17/90

Methods for class Galaxy
***************************************************************************/
#include "Star.h"
#include "StringList.h"
#include "Galaxy.h"
#include "Block.h"
#include "Scheduler.h"

// small virtual functions

// atomic check
int Galaxy :: isItAtomic () const { return FALSE;}

// return myself as a galaxy
Galaxy& Galaxy :: asGalaxy () const { return *this;}

// destructor.  Not really do-nothing because members are deleted
Galaxy :: ~ Galaxy () {}

	////////////////////////////////////
	// Galaxy::printRecursive()
	////////////////////////////////////

StringList
Galaxy :: printRecursive () const {
	StringList out;
	out = "GALAXY: ";
	out += readFullName();
	out += "\n";
	out += "Descriptor: ";
	out += readDescriptor();
	out += "\n";
	out += "Number of blocks: ";
	out += numberBlocks();
	out += "\n";
	out += printPorts("Galaxy");
	out += printStates("Galaxy");
	out += "Blocks in the Galaxy:----------------------------------\n";
	Block* b;
	GalTopBlockIter next(*this);
	while ((b = next++) != 0)
		out += b->printRecursive();
	return out;
}

	////////////////////////////////////
	// Galaxy::printVerbose()
	////////////////////////////////////

StringList
Galaxy :: printVerbose () const {
	StringList out;
	out = "GALAXY: ";
	out += readFullName();
	out += "\n";
	out += "Descriptor: ";
	out += readDescriptor();
	out += "\n";
	out += "Contained blocks: ";
	Block* b;
	GalTopBlockIter next(*this);
	while ((b = next++) != 0) {
		out += b->readName();
		out += " ";
	}
	out += "\n";
	out += printPorts("Galaxy");
	out += printStates("Galaxy");
	return out;
}


        ////////////////////////////////////
        // initialize()
        ////////////////////////////////////

// Initialize myself and all sub-blocks.  Note that
// Block::initialize calls start().
// Stop the initialization if an error occurs.

void Galaxy :: initialize() {
	Block::initialize();
	GalTopBlockIter next(*this);
	Block* b;
	while ((b = next++) != 0 && !Scheduler::haltRequested())
		b->initialize();
}

        ////////////////////////////////////
        // initState()
        ////////////////////////////////////

// like initialize but for states only.

void Galaxy :: initState() {
        Block::initState();
	GalTopBlockIter next(*this);
	Block* b;
	while ((b = next++) != 0 && !Scheduler::haltRequested())
		b->initState();
}

        ////////////////////////////////////
        // Block* blockWithName()
        ////////////////////////////////////

// return ptr to sub-block with given name, NULL if not found
Block *
Galaxy::blockWithName(const char* name) const {
	Block* b;
	GalTopBlockIter next(*this);
	while ((b = next++) != 0) {
		if (strcmp(name,b->readName()) == 0)
			return b;
	}
	// not found
	return 0;
}

        ////////////////////////////////////
        // const char* domain()
        ////////////////////////////////////


// return myDomain if set, else use domain of first sub-block.
const char*
Galaxy::domain() const {
	if (myDomain) return myDomain;
	if (numberBlocks() == 0)
		return "UNKNOWN";
	else return blocks.head().domain();
}
