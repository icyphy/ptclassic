#ifndef _Galaxy_h
#define _Galaxy_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Star.h"

/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY

 Programmer:  E. A. Lee, D. G. Messerschmitt, J. Buck
 Date of creation: 1/17/90

Definition of the Galaxy class, together with the BlockList class.

**************************************************************************/

	////////////////////////////////////
	// class BlockList
	////////////////////////////////////

// This class is used to store a list of component blocks in a Galaxy

class BlockList : private NamedObjList
{
	friend class BlockListIter;
	friend class CBlockListIter;
public:
	// Add Block to list
	void put(Block& b) {NamedObjList::put(b);}

	// Return first Block on list (const, non-const forms)
	Block* head () {return (Block*) NamedObjList::head();}
	const Block* head () const {
		return (const Block*) NamedObjList::head();
	}

	// Remove a Block from the list.  Note: block is not deleted
	int remove (Block* b) { return NamedObjList::remove(b);}

	// find Block with given name (const and non-const forms)
	Block* blockWithName(const char* name) {
		return (Block*)objWithName(name);
	}

	const Block* blockWithName(const char* name) const {
		return (const Block*)objWithName(name);
	}

	// pass along baseclass methods.
	NamedObjList::size;
	NamedObjList::deleteAll;
};

///////////////////////////////////////////
// class BlockListIter
///////////////////////////////////////////

// an iterator for BlockList
class BlockListIter : private NamedObjListIter {
public:
	BlockListIter(BlockList& sl) : NamedObjListIter (sl) {}
	Block* next() { return (Block*)NamedObjListIter::next();}
	Block* operator++() { return next();}
	NamedObjListIter::reset;
};

	////////////////////////////////////
	// class Galaxy
	////////////////////////////////////

class Galaxy : public Block  {
	friend class GalTopBlockIter;
	friend class CGalTopBlockIter;
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

	// Add block and call setBlock for it
	void addBlock(Block& b,const char* bname) {
		b.setBlock(bname,this);
		blocks.put(b);
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
	/* virtual */ int isItAtomic () const; // {return FALSE;}

	// Return myself as a Galaxy.  Overrides Block::asGalaxy.
	Galaxy& asGalaxy(); // { return *this;}
	const Galaxy& asGalaxy() const; // { return *this;}

	// return my domain
	const char* domain () const;

	// set my domain.  FIXME: should be protected, but
	// DDFAutoWorm needs access.
	void setDomain(const char* dom) { myDomain = dom;}

protected:

	// Add blocks to the list
	void addBlock(Block& b) {blocks.put(b);}

	// Connect sub-blocks with a delay (default to zero delay)
	void connect(GenericPort& source, GenericPort& destination,
			  int numberDelays = 0) {
		source.connect(destination,numberDelays);
	}

	// Connect a Galaxy PortHole to a PortHole of a sub-block
	void alias(PortHole& galPort, PortHole& blockPort) {
		galPort.setAlias(blockPort);
	}

	// Overload to alias MultiPortHoles
	void alias(MultiPortHole& galPort, MultiPortHole& blockPort) {
		galPort.setAlias(blockPort);
	}

	// support blockWithName message to access internal block list
	const Block* blockWithName (const char* name) const {
		return blocks.blockWithName(name);
	}

	Block* blockWithName (const char* name) {
		return blocks.blockWithName(name);
	}

	// initialize subblocks only.
	void initSubblocks();

	// initialize states in subblocks only.
	void initStateSubblocks();

	// delete sub-blocks
	void deleteAllBlocks() { blocks.deleteAll();}
private:
	// Keep a list of component Blocks
	BlockList	blocks;

	// my domain
	const char* myDomain;
};

// Iterator class associated with Galaxy
class GalTopBlockIter : public BlockListIter {
public:
	GalTopBlockIter(Galaxy& g) : BlockListIter(g.blocks) {}
};
#endif
