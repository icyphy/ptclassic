#ifndef _Block_h
#define _Block_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "NamedObj.h"
#include "Connect.h"
#include "State.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
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
	friend class BlockPortIter;
	friend class BlockStateIter;
	friend class BlockMPHIter;
	friend class BlockGenPortIter;
	friend class CBlockPortIter;
	friend class CBlockStateIter;
	friend class CBlockMPHIter;
	friend class CBlockGenPortIter;
public:
	// Initialize the data structures
	void initialize();

	// User-specified additional initialization
	// By default, it does nothing.
	virtual void start();

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
	// virtual Block* clone () const {return new MyType;}

	virtual Block* clone () const;

	// method for copying states during cloning.  It is designed for use
	// by clone methods, and it assumes that the src argument has the same
	// state list as me.  
	// This method will be protected after ptlang add this method when
	// cloning a star.
        Block& copyStates(const Block&);

	// Method to print out a description of the block
	// Note that this function flattens the profile of a galaxy,
	// which may not always be wanted.  It produces a great deal
	// of data.
	StringList printVerbose() const;

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
	MultiPortHole *multiPortWithName(const char* name);

	// Get a list of contained PortHole names
	int portNames (const char** names, const char** types,
		       int* io, int nMax) const;

	// Get a list of contained MultiPortHole names
	int multiPortNames (const char** names, const char** types,
			    int* io, int nMax) const;

	// print portholes as part of the info-printing method
	StringList printPorts(const char* type) const;

	// return the scheduler under which it is in.
	virtual Scheduler* mySched() const;

        // Add  State to the block
        void addState(State& s) {states.put(s);}

	// Initialize the State
        virtual void initState();

        // Return number of states 
        int numberStates() const {return states.size();}

        // print states as part of the info-printing method
        StringList printStates(const char* type) const;

        // Retrieve the State with the given name
        virtual State *stateWithName(const char* name);

        // Re-Define State
        void setState(const char* stateName, const char* expression) {
                        stateWithName(stateName)->setValue(expression);}

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
	const char* readClassName() const;

protected:
	// Database for this block

	// The following are set from within the Block; hence, protected
	PortList ports;
	
        // stateWithName can find a state.
        StateList states;

private:
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

class BlockMPHIter : private ListIter {
public:
	BlockMPHIter(Block& b) : ListIter (b.multiports) {}
	MultiPortHole* next() { return (MultiPortHole*)ListIter::next();}
	MultiPortHole* operator++() { return next();}
	ListIter::reset;
};

class BlockGenPortIter : private ListIter {
public:
	BlockGenPortIter(Block& b) : ListIter(b.ports), usedP(0),
			myBlock(b) {}
	GenericPort* next();
	GenericPort* operator++() { return next();}
	void reset() { reconnect(myBlock.ports);}
private:
	const Block& myBlock;
	int usedP;
};

#endif
