#ifndef _Galaxy_h
#define _Galaxy_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Star.h"

/**************************************************************************
Version identification:
@(#)Galaxy.h	2.36 11/13/97

Copyright (c) 1990-1997 The Regents of the University of California.
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

 Programmer:  E. A. Lee, D. G. Messerschmitt, J. Buck
 Date of creation: 1/17/90

Definition of the Galaxy class, together with the BlockList class.

**************************************************************************/

class Target;

// class for a list of Geodesics (or nodes).
class NodeList : private NamedObjList
{
	friend class NodeListIter;
public:
	NodeList() {}
	~NodeList() { deleteAll();}
	void put(Geodesic& g);
	inline Geodesic* nodeWithName (const char* name) {
		return (Geodesic*) NamedObjList::objWithName(name);
	}
	int remove(Geodesic* g);
	// pass along baseclass methods.
	NamedObjList::size;
	NamedObjList::deleteAll;
	NamedObjList::initialize;
};

class NodeListIter : private NamedObjListIter {
public:
	NodeListIter(NodeList& n) : NamedObjListIter(n) {}
	inline Geodesic* next() { return (Geodesic*) NamedObjListIter::next();}
	inline Geodesic* operator++(POSTFIX_OP) { return next();}
	NamedObjListIter::reset;
};

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
	inline void put(Block& b) {NamedObjList::put(b);}

	// Return first Block on list (const, non-const forms)
	inline Block* head () {return (Block*) NamedObjList::head();}
	inline const Block* head () const {
		return (const Block*) NamedObjList::head();
	}

	// Remove a Block from the list.  Note: block is not deleted
	inline int remove (Block* b) { return NamedObjList::remove(b);}

	// See if Block is present in list
	int isInList (const Block& b) const;

	// find Block with given name (const and non-const forms)
	inline Block* blockWithName(const char* name) {
		return (Block*)objWithName(name);
	}

	inline const Block* blockWithName(const char* name) const {
		return (const Block*)objWithName(name);
	}

	// pass along baseclass methods.
	NamedObjList::size;
	NamedObjList::deleteAll;
	NamedObjList::initialize;
};

///////////////////////////////////////////
// class BlockListIter
///////////////////////////////////////////

// an iterator for BlockList
class BlockListIter : private NamedObjListIter {
public:
	BlockListIter(BlockList& sl) : NamedObjListIter (sl) {}
	inline Block* next() { return (Block*)NamedObjListIter::next();}
	inline Block* operator++(POSTFIX_OP) { return next();}
	NamedObjListIter::reset;
	NamedObjListIter::remove;
};

	////////////////////////////////////
	// class Galaxy
	////////////////////////////////////

class Galaxy : public Block  {
	friend class GalTopBlockIter;
	friend class CGalTopBlockIter;
	friend class Scope;	// nt4.vc needs this.
public:

	// Constructor
	Galaxy ();

	// destructor
	~Galaxy ();

	// class identification
	/* virtual */ int isA(const char*) const;
	/* virtual */ const char* className() const;

	// system initialize method.  Derived Galaxies should not
        // redefine initialize; they should write a setup() method
	// to do any class-specific startup.
	void initialize();

	// system preinitialize method.
	// Recursively calls preinitialize in subsystems
	void preinitialize();

	// system wrapup method.  Recursively calls wrapup in subsystems
	void wrapup();

	// Add block and call setBlock for it
	inline void addBlock(Block& b,const char* bname) {
		b.setBlock(bname,this);
		blocks.put(b);
	}

	// Remove a block from the list
	inline int removeBlock(Block& b) { return blocks.remove(&b);}

	// Allow blocks to be looked up by name --- const and non-const forms
	inline Block* blockWithName (const char* name) {
		return blocks.blockWithName(name);
	}
	inline const Block* blockWithName (const char* name) const {
		return blocks.blockWithName(name);
	}

	// Get the head of the blocks list--useful when there
	// are Clusters with only one star in them.  By default, when
	// a cluster hierarchy is created, every star is a Cluster.
	// For such clusters, calling head() will give access to the star
	// immediately rather than having to set up an iterator to get it.
	inline Block* head() {return blocks.head();}

        // States initialize
        virtual void initState();

        // PortHoles initialize
        virtual void initPorts();

        // resetPortBuffers in each Block.
        virtual void resetPortBuffers();

	// Return the number of blocks in the galaxy.
	inline int numberBlocks() const {return blocks.size();}

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
	inline void setDomain(const char* dom) { myDomain = dom;}

	// Add a block to the list of blocks to be deleted after initialization
	void deleteBlockAfterInit(Block& b);

    	/* virtual */ int setTarget(Target*);

        // empty all the blocks from the galaxy - they are not
        // deallocated. 
        void empty();
    
	// Move blocks to parent(default) or specified galaxy and
        // We can optionally remove and delete the
        // galaxy from the parent list (TRUE by default).  We may
        // not want to do this for effeciency purposes - instead it is
        // possible to use ListIter::remove.   Be careful, if FALSE is
        // specified, we'll be left with a dangling pointer.
	virtual int flatten(Galaxy* = NULL, int = TRUE);

	// Return the largest KnownBlock serial number found within
	// the galaxy.  Optionally scan subgalaxies.
	long maxContainedSerialNumber (int recurse = TRUE);

protected:

        // A list of nodes (or geodesics) of the galaxy.  This list
        // will only contain members as class Cluster or InterpGalaxy 
    	NodeList nodes;

        // Add blocks to the list
	inline void addBlock(Block& b) {blocks.put(b);}

	// Connect sub-blocks with an initial delay values string
	inline void connect(GenericPort& source, GenericPort& destination,
		     int numberDelays = 0, const char* initDelayValues = 0) {
		source.connect(destination, numberDelays, initDelayValues);
	}

	// Connect a Galaxy PortHole to a PortHole of a sub-block
	inline void alias(PortHole& galPort, PortHole& blockPort) {
		galPort.setAlias(blockPort);
	}

	// Overload to alias MultiPortHoles
	inline void alias(MultiPortHole& galPort, MultiPortHole& blockPort) {
		galPort.setAlias(blockPort);
	}

	// initialize subblocks only.
	void initSubblocks();

	// initialize states in subblocks only.
	void initStateSubblocks();

	// initialize portholes in subblocks only.
	void initPortsSubblocks();

	// delete sub-blocks
	inline void deleteAllBlocks() { blocks.deleteAll();}
    
private:
	// Keep a list of component Blocks
	BlockList	blocks;

	// List of blocks to delete after initialization
	BlockList blocksToDelete;

	// my domain
	const char* myDomain;
};

// Iterator class associated with Galaxy
class GalTopBlockIter : public BlockListIter {
public:
	GalTopBlockIter(Galaxy& g) : BlockListIter(g.blocks) {}
};
#endif
