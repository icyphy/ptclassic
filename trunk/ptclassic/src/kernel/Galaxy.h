#ifndef _Galaxy_h
#define _Galaxy_h 1

#include "type.h"
#include "Connect.h"
#include "Block.h"
#include "Star.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 1/17/89
 Revisions:
    3/19/90 - J. Buck
	Combine the four connect methods into one (since MultiPortHole
	is now a derived type of PortHole we can do this).
	Add Galaxy::blockWithName and BlockList::blockWithName.
 	Make KnownBlock (see KnownBlock.h) a friend of BlockList.

Definition of the Galaxy class, together with the BlockList class.

**************************************************************************/

	////////////////////////////////////
	// class BlockList
	////////////////////////////////////

// This class is used to store a list of component blocks in a Galaxy

class BlockList : SequentialList
{
	friend class Galaxy;
	friend class KnownBlock;

	// Add Block to list
	void put(Block* b) {SequentialList::put(b);}

	// Return size of list
	int size() {return SequentialList::size();}

	// Return next Block on list
	Block& operator ++ () {return *(Block*) next();}

	// Find a block with the given name and return pointer; NULL if not found
protected:
	Block* blockWithName (char* name);
};

	////////////////////////////////////
	// class Galaxy
	////////////////////////////////////

class Galaxy : public Block  {

private:
	// Keep a list of component Blocks
	BlockList	blocks;

protected:

	// Add blocks to the list
	void addBlock(Block& b) {blocks.put(&b);}

	// Connect sub-blocks with a delay (default to zero delay)
	// Returns a reference to the Geodesic it created.
	Geodesic& connect(PortHole& source, PortHole& destination,
			  int numberDelays = 0);

	// TO BE DONE:
	// Allow delays to be initialized to something reasonable.
	// Geodesic& connect(PortHole& source, PortHole& destination,
			// int numberDelays, Particle& delayValue);

	// Connect a Galaxy PortHole to a PortHole of a sub-block
	alias(PortHole& galPort, PortHole& blockPort) {
		galPort.setAlias(blockPort);
	}

	// Overload to alias MultiPortHoles
	alias(MultiPortHole& galPort, MultiPortHole& blockPort) {
		galPort.setAlias(blockPort);
	}

	// support blockWithName message to access internal block list
	Block* blockWithName (char* name) {return blocks.blockWithName(name);}
public:
	// Return the number of blocks in the galaxy.
	int numberBlocks() {return blocks.size();}

	// Return the next block in the list
	Block& nextBlock() {return blocks++;}

	// Print a description of the galaxy
	virtual operator char* ();

	// Method replies FALSE to indicate that component blocks
	// can be seen from outside.
	virtual int isItAtomic () {return FALSE;}
};


#endif
