#ifndef _Galaxy_h
#define _Galaxy_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Star.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 1/17/90

Definition of the Galaxy class, together with the BlockList class.

**************************************************************************/

	////////////////////////////////////
	// class BlockList
	////////////////////////////////////

// This class is used to store a list of component blocks in a Galaxy

class BlockList : public SequentialList
{
public:
	// Add Block to list
	void put(Block* b) {SequentialList::put(b);}

	// Return size of list
	int size() const {return SequentialList::size();}

	// Return first Block on list (a const method)
	Block& head () const {return *(Block*) SequentialList::head();}

	// Remove a Block from the list.  Note: block is not deleted
	int remove (Block* b) { return SequentialList::remove(b);}
};

	////////////////////////////////////
	// class Galaxy
	////////////////////////////////////

class Galaxy : public Block  {
	friend class GalTopBlockIter;
	friend class CGalTopBlockIter;
private:
	// Keep a list of component Blocks
	BlockList	blocks;

protected:

	// Add blocks to the list
	void addBlock(Block& b) {blocks.put(&b);}

	// Connect sub-blocks with a delay (default to zero delay)
	void connect(GenericPort& source, GenericPort& destination,
			  int numberDelays = 0) {
		source.connect(destination,numberDelays);
	}

	// TO BE DONE:
	// Allow delays to be initialized to something reasonable.
	// Geodesic& connect(PortHole& source, PortHole& destination,
			// int numberDelays, Particle& delayValue);

	// Connect a Galaxy PortHole to a PortHole of a sub-block
	void alias(PortHole& galPort, PortHole& blockPort) {
		galPort.setAlias(blockPort);
	}

	// Overload to alias MultiPortHoles
	void alias(MultiPortHole& galPort, MultiPortHole& blockPort) {
		galPort.setAlias(blockPort);
	}

	// support blockWithName message to access internal block list
	Block* blockWithName (const char* name);

	// initialize subblocks only.
	void initSubblocks();

	// initialize states in subblocks only.
	void initStateSubblocks();

public:

	// Constructor
	Galaxy ();

	// destructor
	~Galaxy ();

	// class identification
	/* virtual */ int isA(const char*) const;
	/* virtual */ const char* className() const {return "Galaxy";}

	// system initialize method.  Derived Galaxies should not
	// redefine initialize; they should write a start() method
	// to do any class-specific startup.
	void initialize();

	// system wrapup method.  Recursively calls wrapup in subsystems
	void wrapup();

        // Define States of component Blocks by States of this Block
        int stateAlias(Block& b, char* stateName, char* expression)  {
		return b.setState(stateName,expression);
        }

	// Add block and call setBlock for it
	void addBlock(Block& b,const char* bname) {
		addBlock(b.setBlock(bname,this));
	}

	// Remove a block from the list
	int removeBlock(Block& b) { return blocks.remove(&b);}

        // States initialize
        virtual void initState();
	// Return the number of blocks in the galaxy.
	int numberBlocks() const {return blocks.size();}

	// Print a description of the galaxy
	/* virtual */ StringList print(int verbose) const;

	// Method replies FALSE to indicate that component blocks
	// can be seen from outside.
	virtual int isItAtomic () const; // {return FALSE;}

	// Return myself as a Galaxy.  Overrides Block::asGalaxy.
	Galaxy& asGalaxy(); // { return *this;}
	const Galaxy& asGalaxy() const; // { return *this;}

	// return my domain
	const char* domain () const;

	// my domain
	const char* myDomain;
};

// Iterator classes associated with Galaxy
class GalTopBlockIter : private ListIter {
public:
	GalTopBlockIter(Galaxy& g) : ListIter(g.blocks) {}
	Block* next() { return (Block*)ListIter::next();}
	Block* operator++() { return next();}
	ListIter::reset;
};
#endif
