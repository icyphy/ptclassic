#ifndef _Block_h
#define _Block_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "NamedObj.h"
#include "PortHole.h"
#include "State.h"
#include "FlagArray.h"

/**************************************************************************
Version identification:
@(#)Block.h	2.32	11/13/97

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

 Block is the basic unit of computation...it is an abstraction that has
 inputs and outputs and certain generic methods common to all
 computational units

**************************************************************************/

class Star;
class Galaxy;
class Scheduler;
class Target;
class Scope;

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

	// Scope is defined Scope.h.  Scope needs
        // access to the block state list to clone it.
        friend class Scope;
    
public:
	// Pre-initialization pass
	// No-op in most Blocks, but stars that rearrange galaxy connections
	// (such as HOF stars) need to run before regular initialization
	virtual void preinitialize();

	// Initialize the data structures
	/* virtual */ void initialize();

	// execute the block <for executable blocks, otherwise this is error>
	virtual int run();

	// User-specified end of simulation
	// again, does nothing by default.
	virtual void wrapup();

	// Methods making ports available on the outside;
	// can be read but not set
	inline int numberPorts() const {return ports.size();}

	inline int numberMPHs() const {return multiports.size();}

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

	// Method to reply "false" if the block is a Cluster
        // and false otherwise.
        virtual int isItCluster () const; // {return FALSE;}

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
	inline void addPort(PortHole& p) {ports.put(p);}

	// remove a porthole from the list
	inline int removePort(PortHole& p) { return ports.remove(&p);}

	// This function saves the given MultiPortHole so portWithName
	// can find it.
	inline void addPort(MultiPortHole& p) {multiports.put(p);}

	// Retrieve the PortHole with the given name
	// The first method returns a generic port, the second returns
	// a real PortHole with all aliases resolved
	GenericPort *genPortWithName (const char* name);
	const GenericPort *genPortWithName (const char* name) const;
	PortHole *portWithName(const char* name);

	// Retrieve the MultiPortHole with the given name
	inline MultiPortHole *multiPortWithName(const char* name) {
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

	// return the scheduler under which it runs.
	virtual Scheduler* scheduler() const;

        // Add  State to the block
        inline void addState(State& s) {states.put(s);}

	// Initialize the State
        virtual void initState();

        // Initialize the PortHoles
        virtual void initPorts();

	// Reset the buffers in the PortHoles
	virtual void resetPortBuffers();

        // Return number of states 
        inline int numberStates() const {return states.size();}

        // print states as part of the info-printing method
        StringList printStates(const char* type,int verbose) const;

        // Retrieve the State with the given name
        virtual State *stateWithName(const char* name);

        // Set the value of a state - returns false if no state named stateName
	virtual int setState(const char* stateName, const char* expression);

        // Return the block which defines the lexical scope of our states
        Scope* scope() const;
 
        // Set the lexical scope for our states
        void setScope(Scope*);
 
        // Return the full name, using the scoping block hierarchy
        /*virtual*/ StringList fullName() const;

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

	virtual int setTarget(Target*);
	Target* target() const;

	// Many schedulers and targets need to be able to mark blocks
	// in various ways, to count invocations, or flag
	// that the block has been visited, or to classify it
	// as a particular type of block.  To support this,
	// we provide an array of flags that are not used
	// by class Block, and may be used in any way by a Target
	// or scheduler.  The array can be of any size, and the size
	// will be increased automatically as elements are referenced.
	// For readability and consistency, the user should define an enum
	// in the Target class to give the indices, so that mnemonic names
	// can be associated with flags, and so that multiple schedulers
	// for the same target are consistent.
	//
	// For efficiency, there is no checking to prevent
	// two different pieces of code (say a target and scheduler) from
	// using the same flags (which are indexed only by non-negative
	// integers) for different purposes.  The policy, therefore, is
	// that the target is in charge.  It is incumbent upon
	// the writer of the target to know what flags are used by schedulers
	// invoked by that target, and to avoid corrupting those flags
	// if the scheduler needs them preserved.
	FlagArray flags;

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
	inline void deleteAllStates() { states.deleteAll();}

	// same for ports and multiports.  Order is important:
	// delete multiports first.
	inline void deleteAllGenPorts() {
		multiports.deleteAll();
		ports.deleteAll();
	}
private:
	// Database for this block

  	Target* pTarget;

        // The scope our states belong.
        Scope* scp;

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

class RegisterBlock {
public:
	RegisterBlock(Block& prototype, const char* classname);
};

#endif
