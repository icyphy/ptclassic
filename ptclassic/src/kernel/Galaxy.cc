static const char file_id[] = "Galaxy.cc";
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

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

const char* Galaxy :: className() const {return "Galaxy";}

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

int Galaxy :: setTarget(Target* t) {
    GalTopBlockIter next(*this);
    Block* block;
    while((block = next++) != 0)
	if (!block->setTarget(t)) return FALSE;
    return TRUE;
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
	Block *b;
	while ((b = next++) && !Scheduler::haltRequested()) {
	    if (!blocksToDelete.blockWithName(b->name()))
		b->initialize();
	}
	if (Scheduler::haltRequested()) return;
	BlockListIter bdel(blocksToDelete);
	Block *bd;
	while ( (bd = bdel++) ) {
	  removeBlock(*bd);
	  LOG_DEL; delete bd;
	}
	blocksToDelete.initialize();
}

// Add a block to the list of blocks to be deleted after initialization
void Galaxy::deleteBlockAfterInit(Block& b) {
  // Check to make sure the block is not already there
  if(!blocksToDelete.blockWithName(b.name()))
     blocksToDelete.put(b);
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

}

// separate for additional control in derived galaxies.

void Galaxy :: 	initStateSubblocks() {
	GalTopBlockIter next(*this);
	Block* b;
	while ((b = next++) != 0 && !Scheduler::haltRequested())
		b->initState();
}

        ////////////////////////////////////
        // initPorts()
        ////////////////////////////////////

// like initialize but for portholes only.

void Galaxy :: initPorts() {
        Block::initPorts();
	initPortsSubblocks();
}

// separate for additional control in derived galaxies.

void Galaxy :: 	initPortsSubblocks() {
	GalTopBlockIter next(*this);
	Block* b;
	while ((b = next++) != 0 && !Scheduler::haltRequested())
		b->initPorts();
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
	else return blocks.head()->domain();
}

// isa
ISA_FUNC(Galaxy,Block);
