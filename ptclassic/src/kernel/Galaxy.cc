static const char file_id[] = "Galaxy.cc";
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 1/17/90

Methods for class Galaxy
***************************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "Star.h"
#include "StringList.h"
#include "Galaxy.h"
#include "Block.h"
#include "Scheduler.h"
#include "ConstIters.h"

// Constructor
Galaxy :: Galaxy() : myDomain(0) {}

// small virtual functions

// atomic check
int Galaxy :: isItAtomic () const { return FALSE;}

// return myself as a galaxy
Galaxy& Galaxy :: asGalaxy () { return *this;}
const Galaxy& Galaxy :: asGalaxy () const { return *this;}

// destructor.  Not really do-nothing because members are deleted
Galaxy :: ~ Galaxy () {}

	////////////////////////////////////
	// Galaxy::printRecursive()
	////////////////////////////////////

StringList
Galaxy :: print (int verbose) const {
	StringList out;
	out = "GALAXY: ";
	out += fullName();
	out += "\n";
	out += "Descriptor: ";
	out += descriptor();
	out += "\n";
	const Block* b;
	CGalTopBlockIter next(*this);
	if (!verbose) {
		out += "Contained blocks: ";
		while ((b = next++) != 0) {
			out += b->name();
			out += " ";
		}
	}
	out += printPorts("Galaxy",verbose);
	out += printStates("Galaxy",verbose);
	if (verbose) {
		out += "Contained blocks: ";
		while ((b = next++) != 0)
			out += b->print(verbose);
	}
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
	initSubblocks();
}

// this is separate so derived galaxies can have more control.
void Galaxy :: initSubblocks() {
	GalTopBlockIter next(*this);
	Block* b;
	while ((b = next++) != 0 && !Scheduler::haltRequested())
		b->initialize();
}

        ////////////////////////////////////
        // wrapup()
        ////////////////////////////////////

// Wrapup myself and all sub-blocks.
// Stop the wrapup if an error occurs.

void Galaxy :: wrapup() {
	Block::wrapup();
	GalTopBlockIter next(*this);
	Block* b;
	while ((b = next++) != 0 && !Scheduler::haltRequested())
		b->wrapup();
}

        ////////////////////////////////////
        // initState()
        ////////////////////////////////////

// like initialize but for states only.

void Galaxy :: initState() {
        Block::initState();
	initStateSubblocks();
}

// separate for additional control in derived galaxies.

void Galaxy :: 	initStateSubblocks() {
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
Galaxy::blockWithName(const char* name) {
	Block* b;
	GalTopBlockIter next(*this);
	while ((b = next++) != 0) {
		if (strcmp(name,b->name()) == 0)
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

// isa
ISA_FUNC(Galaxy,Block);
