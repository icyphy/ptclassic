#ifndef _Block_h
#define _Block_h 1

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
 Revisions:
	3/19/90 - J. Buck
		Changes required by the interpreter:
		1) add saveMPH to save a pointer to a contained MultiPortHole.
		   addPort(MultiPortHole& p) sets it.
		2) add the virtual method "clone".
		3) add the method "portWithName" (defined in Block.cc)

	3/23/90 - J. Buck
		Make a Block a type of NamedObj

        5/26/90 - I. Kuroda and J. Buck
                Add StateList states and methods for State

	5/29/90	- Change StringList cast to printVerbose function

 Block is the basic unit of computation...it is an abstraction that has
 inputs and outputs and certain generic methods common to all
 computational units

**************************************************************************/

class Star;
class Galaxy;
class Scheduler;

class Block : public NamedObj
{
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
	PortHole& nextPort() {return ports++;}

	// Method setting internal data in the Block
	// If the parent pointer is not provied, it defaults to NULL
	virtual Block& setBlock(const char* s, Block* parent = NULL);

	// Constructor
	Block() {}

	// Another constructor
	Block(const char* n,Block* p,const char* d) : NamedObj(n,p,d) {}

	// destructor
	~Block();

	// Method to reply "false" if the block contains component
	// blocks that can be seen from the outside.
	// i.e., it is true for stars and wormholes, false for galaxies.
	virtual int isItAtomic () const; // {return TRUE;}

	// virtual method to make a new object of the same type.  This
	// version should never be called; stars and galaxies should 
	// redefine it as
	// virtual Block* clone () {return new MyType;}

	virtual Block* clone ();

	// Method to print out a description of the block
	// Note that this function flattens the profile of a galaxy,
	// which may not always be wanted.  It produces a great deal
	// of data.
	StringList printVerbose();

	// Add elements to the to the lists
	// Made public for the benefit of MultiPortHole and
	// classes derived therefrom.
	addPort(PortHole& p) {ports.put(p);}

	// Retrieve the PortHole with the given name
	PortHole *portWithName(const char* name);

	// Retrieve the MultiPortHole with the given name
	MultiPortHole *multiPortWithName(const char* name);

	// Get a list of contained PortHole names
	int portNames (const char** names, int* io, int nMax);

	// Get a list of contained MultiPortHole names
	int multiPortNames (const char** names, int* io, int nMax);

	// print portholes as part of the info-printing method
	StringList printPorts(const char* type);

	// return the scheduler under which it is in.
	virtual Scheduler* mySched() ;

        // Add  State to the block
        addState(State& s) {states.put(s);}

	// Initialize the State
        virtual void initState();

        // Return number of states 
        int numberStates() const {return states.size();}

        State& nextState() {return states++;}

        // print states as part of the info-printing method
        StringList printStates(const char* type);

        // Retrieve the State with the given name
        State *stateWithName(const char* name);

        // Re-Define State
        setState(const char* stateName, const char* expression) {
                        stateWithName(stateName)->setValue(expression);}

	// Return reference to Block as a Star.  Error if it's not.
	virtual Star& asStar() const;

	// Return reference to Block as a Galaxy.  Error if it's not.
	virtual Galaxy& asGalaxy() const;

	// Return my domain (e.g. SDF, DE, etc.)
	virtual const char* domain() const;
protected:
	// Database for this block

	// The following are set from within the Block; hence, protected
	PortList ports;
	
	// This function saves the given MultiPortHole so portWithName
	// can find it.
	addPort(MultiPortHole& p) {multiports.put(p);}

        // stateWithName can find a state.
        StateList states;

private:
	// This is a list of multiportholes in the block.
	MPHList multiports;
};

#endif
