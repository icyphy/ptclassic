#ifndef _Block_h
#define _Block_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "NamedObj.h"
#include "PortHole.h"
#include "State.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee, D. G. Messerschmitt, J. Buck
 Date of creation: 1/17/90

 Block is the basic unit of computation...it is an abstraction that has
 inputs and outputs and certain generic methods common to all
 computational units

**************************************************************************/

class Star;
class Galaxy;
class Scheduler;

class Block : public NamedObj
{
	// Give iterator classes special access.
	friend class BlockPortIter;
	friend class BlockStateIter;
	friend class BlockMPHIter;

	// These are defined in ConstIters.h.
	friend class CBlockPortIter;
	friend class CBlockStateIter;
	friend class CBlockMPHIter;
public:
	// Initialize the data structures
	/* virtual */ void initialize();

	// execute the block <for executable blocks, otherwise this is error>
	virtual int run();

	// User-specified end of simulation
	// again, does nothing by default.
	virtual void wrapup();

	// Methods making ports available on the outside;
	// can be read but not set
	int numberPorts() const {return ports.size();}

	int numberMPHs() const {return multiports.size();}

	// Method setting internal data in the Block
	// If the parent pointer is not provied, it defaults to NULL
	virtual Block& setBlock(const char* s, Block* parent = NULL);

	// Constructor
	Block();

	// Another constructor
	Block(const char* name, Block* parent, const char* desc);

	// destructor
	~Block();

	// Method to reply "false" if the block contains component
	// blocks that can be seen from the outside.
	// i.e., it is true for stars and wormholes, false for galaxies.
	virtual int isItAtomic () const; // {return TRUE;}

	// Method to reply "true" if the block is a wormhole
	// (contains a sub-universe of a different domain inside)
	// and false otherwise.
	virtual int isItWormhole () const; // {return FALSE;}

	// virtual method to make a new object of the same type.
	// Stars and galaxies should redefine it as, say
	//
	// virtual Block* makeNew () const {return new MyType;}
	virtual Block* makeNew () const;

	// make a duplicate object.  By default, this does makeNew
	// and copyStates -- it can be redefined to copy any additional
	// cruft.
	virtual Block* clone () const;

	// Method to print out a description of the block
	// Note that this function flattens the profile of a galaxy,
	// which may not always be wanted.  It produces a great deal
	// of data.
	/* virtual */ StringList print(int verbose) const;

	// Add elements to the to the lists
	// Made public for the benefit of MultiPortHole and
	// classes derived therefrom.
	void addPort(PortHole& p) {ports.put(p);}

	// remove a porthole from the list
	int removePort(PortHole& p) { return ports.remove(&p);}

	// This function saves the given MultiPortHole so portWithName
	// can find it.
	void addPort(MultiPortHole& p) {multiports.put(p);}

	// Retrieve the PortHole with the given name
	// The first method returns a generic port, the second returns
	// a real PortHole with all aliases resolved
	GenericPort *genPortWithName (const char* name);
	PortHole *portWithName(const char* name);

	// Retrieve the MultiPortHole with the given name
	MultiPortHole *multiPortWithName(const char* name) {
		return multiports.multiPortWithName(name);
	}

	// Get a list of contained PortHole names
	int portNames (const char** names, const char** types,
		       int* io, int nMax) const;

	// Get a list of contained MultiPortHole names
	int multiPortNames (const char** names, const char** types,
			    int* io, int nMax) const;

	// print portholes as part of the info-printing method
	StringList printPorts(const char* type, int verbose) const;

	// return the scheduler under which it is in.
	virtual Scheduler* scheduler() const;

        // Add  State to the block
        void addState(State& s) {states.put(s);}

	// Initialize the State
        virtual void initState();

        // Return number of states 
        int numberStates() const {return states.size();}

        // print states as part of the info-printing method
        StringList printStates(const char* type,int verbose) const;

        // Retrieve the State with the given name
        virtual State *stateWithName(const char* name);

        // Re-Define setState -- returns false if no state named stateName
	int setState(const char* stateName, const char* expression);

	// Return reference to Block as a Star.  Error if it's not.
	virtual const Star& asStar() const;
	virtual Star& asStar();

	// Return reference to Block as a Galaxy.  Error if it's not.
	virtual Galaxy& asGalaxy();
	virtual const Galaxy& asGalaxy() const;

	// Return my domain (e.g. SDF, DE, etc.)
	virtual const char* domain() const;

	// class identification
	int isA(const char*) const;
	const char* className() const;

protected:
	// User-specified additional initialization
	// By default, it does nothing.  It is called by initialize
	// (and should also be called if initialize is redefined)
	virtual void setup();

	// method for copying states during cloning.  It is designed for use
	// by clone methods, and it assumes that the src argument has the same
	// state list as the "this" object.  
        Block* copyStates(const Block&);

	// delete all States.  This must not be called unless the
	// States are on the heap!
	void deleteAllStates() { states.deleteAll();}

	// same for ports and multiports.  Order is important:
	// delete multiports first.
	void deleteAllGenPorts() {
		multiports.deleteAll();
		ports.deleteAll();
	}
private:
	// Database for this block

	// The following are set from within the Block; hence, protected
	PortList ports;
	
        // stateWithName can find a state.
        StateList states;

	// This is a list of multiportholes in the block.
	MPHList multiports;
};

// Iterator classes associated with Block
class BlockPortIter : public PortListIter {
public:
	BlockPortIter(Block& b) : PortListIter (b.ports) {}
};

class BlockStateIter : public StateListIter {
public:
	BlockStateIter(Block& b) : StateListIter (b.states) {}
};

class BlockMPHIter : public MPHListIter {
public:
	BlockMPHIter(Block& b) : MPHListIter (b.multiports) {}
};

#endif
