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
#include "KnownBlock.h"
#include "Scheduler.h"
#include "ConstIters.h"
#include "GraphUtils.h"

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

int Galaxy::flatten(Galaxy* g, int removeFlag) {
    if (!g && (g = (Galaxy*)parent()) == NULL) return FALSE;

    GalTopBlockIter nextBlock(*this);
    Block *block;
    while ((block = nextBlock++) != NULL)
	g->addBlock(*block,block->name());

    NodeListIter nextNodeToMove(nodes);
    Geodesic* nodeToMove;
    while ((nodeToMove = nextNodeToMove++) != NULL)
	g->nodes.put(*nodeToMove);

    nodes.initialize();
    empty();

    if (removeFlag && parent()) {
	parent()->asGalaxy().removeBlock(*this);
	delete this;		// must be LAST thing done here
    }

    return TRUE;
}

void Galaxy::empty() {
    nodes.initialize();
    blocks.initialize();
    deleteAllGenPorts();
}    

	////////////////////////////////////
	// Galaxy::printRecursive()
	////////////////////////////////////

StringList Galaxy::print(int verbose) const {
    StringList out;

    out << "GALAXY: " << fullName() << "\nDescriptor: "
	<< descriptor() << printPorts("Galaxy",verbose)
	<< printStates("Galaxy",verbose) << "\nContained blocks: ";

    const Block* b;
    CGalTopBlockIter next(*this);
    while ((b = next++) != 0)
	out << (b->isItAtomic()?
		(b->isItWormhole()?"Wormhole ":"Star "):"Galaxy ")
	    << b->name() << '\n';

    out << "_______________________________________"
	<< "_______________________________________\n\n";

    next.reset();
    while ((b = next++) != 0)
	out << b->print(verbose);

    return out;
}

int Galaxy :: setTarget(Target* t) {
    if (!Block::setTarget(t)) return FALSE;
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
// Block::initialize calls setup().
// Stop the initialization if an error occurs.

void Galaxy :: initialize() {
	Block::initialize();
	// Preinitialization of subblocks...
	preinitialize();
	// then regular initialization.
	initSubblocks();
}

// Pre-initialization pass.
// Blocks can change the galaxy topology during this pass.
//
// We actually do the preinitialization in two passes,
// one that preinits only atomic blocks and a second to preinit all blocks.
// This allows clean support of graphical recursion: for example,
// a HOFIfElseGr star can control a recursive reference to the current
// galaxy.  The IfElse star is guaranteed to get control before the
// subgalaxy does, so it can delete the subgalaxy to stop the recursion.
// The second pass must preinit all blocks in case a non-atomic block
// adds a block to the current galaxy.
//
// Also note that because this is called from Galaxy::initialize,
// blocks in subgalaxies will see a preinitialize call during this pass
// and then another one when the subgalaxy is actually initialized.
//
// For both this reason and the one above, blocks must act safely if
// preinitialized multiple times.  (HOF stars generally destroy themselves
// when preinitialized, so they can't see extra calls.)
//
// If Wormholes passed through preinitialize calls, it'd be necessary
// to discriminate against wormholes in the first pass to prevent infinite
// recursion through a wormhole.  But currently, they don't and it isn't.

void Galaxy :: preinitialize() {
	// Initialize galaxy's states in case stars refer to them
	initState();
	// Preinit stars only
	GalTopBlockIter next(*this);
	Block *b;
	while ((b = next++) && !Scheduler::haltRequested()) {
	    if (b->isItAtomic() && !blocksToDelete.isInList(*b))
		b->preinitialize();
	}
	// Preinit everyone
	GalTopBlockIter next2(*this);
	while ((b = next2++) && !Scheduler::haltRequested()) {
	    if (!blocksToDelete.isInList(*b))
		b->preinitialize();
	}
	if (Scheduler::haltRequested()) return;
	// Delete anything we've been told to delete.
	// We do this now, rather than allowing them to hang around until
	// initSubblocks is called, to ensure that InterpGalaxy::preinitialize
	// won't do something wrong if called again.
	BlockListIter bdel(blocksToDelete);
	Block *bd;
	while ( (bd = bdel++) ) {
	  removeBlock(*bd);
	  LOG_DEL; delete bd;
	}
	blocksToDelete.initialize();
}

// this is separate so derived galaxies can have more control.
void Galaxy :: initSubblocks() {
	GalTopBlockIter next(*this);
	Block *b;
	while ((b = next++) && !Scheduler::haltRequested()) {
	    if (!blocksToDelete.isInList(*b))
		b->initialize();
	}
	if (Scheduler::haltRequested()) return;
	// Delete anything we've been told to delete
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
  if (!blocksToDelete.isInList(b))
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
        // resetPortBuffers()
        ////////////////////////////////////

// resetPortBuffers in each Star.
void Galaxy :: resetPortBuffers() {
	GalStarIter next(*this);
	Star* s;
	while ((s = next++) != 0 && !Scheduler::haltRequested())
		s->resetPortBuffers();
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

// Return the largest KnownBlock serial number found within
// the galaxy.  Optionally scan subgalaxies.
// NOTE: the contents of wormholes are NOT recursively examined.
// Should they be?
long
Galaxy::maxContainedSerialNumber (int recurse)
{
  long maxSN = 0;
  GalTopBlockIter next(*this);
  Block* b;
  while ((b = next++) != 0) {
    long thisSN = KnownBlock::serialNumber(*b);
    if (thisSN > maxSN)
      maxSN = thisSN;
    if (recurse && ! b->isItAtomic()) {
      long innerSN = b->asGalaxy().maxContainedSerialNumber(recurse);
      if (innerSN > maxSN)
	maxSN = innerSN;
    }
  }
  return maxSN;
}

// isa
ISA_FUNC(Galaxy,Block);

// BlockList stuff

// See if Block is present in list
int BlockList :: isInList (const Block& b) const
{
	const Block *obj;
	CBlockListIter next(*this);
	while ((obj = next++) != 0) {
		if (obj == &b)
			return 1;
	}
	return 0;
}
