#ifndef _Block_h
#define _Block_h 1

#include "NamedObj.h"
#include "Connect.h"

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

 Block is the basic unit of computation...it is an abstraction that has
 inputs and outputs and certain generic methods common to all
 computational units

**************************************************************************/


class Block : public NamedObj
{
public:
	// Initialize the data structures
	void initialize();

	// Methods making ports available on the outside;
	// can be read but not set
	int numberPorts() {return ports.size();}
	PortHole& nextPort()  {return ports++;}

	// Method setting internal data in the Block
	// If the parent pointer is not provied, it defaults to NULL
	Block& setBlock(char* s, Block* parent = NULL) {
		setNameParent (s, parent);
		return *this;
		}

	// Constructor
	Block() { 
		 saveMPH = NULL;
	}

	// Method to set the parameters of the block
	// NOT CURRENTLY IMPLEMENTED
	virtual int initParameters() {};

	// Method to reply "false" if the block contains component
	// blocks that can be seen from the outside.
	// i.e., it is true for stars and wormholes, false for galaxies.
	virtual int isItAtomic () {return TRUE;}

	// virtual method to make a new object of the same type.  This
	// version should never be called; stars and galaxies should 
	// redefine it as
	// virtual Block* clone () {return new MyType;}
	// We return NULL instead of "new Block" here to avoid errors.
	virtual Block* clone () {return NULL;}

	// Method to print out a description of the block
	// Note that this function flattens the profile of a galaxy,
	// which may not always be wanted.  It produces a great deal
	// of data.
	virtual operator char* ();

	// Add elements to the to the lists
	// Made public for the benefit of MultiPortHole and
	// classes derived therefrom.
	addPort(PortHole& p) {ports.put(p);}

	// Retrieve the PortHole with the given name
	PortHole *portWithName(char* name);

protected:
	// Database for this block

	// The following are set from within the Block; hence, protected
	PortList ports;
	
	// This function saves the given MultiPortHole so portWithName
	// can find it; this current implementation only allows for
	// one MultiPortHole per block; fix later.
	addPort(MultiPortHole& p) {saveMPH = &p;}

private:
	// This may be changed to a structure holding multiple MultiPortHoles.
	MultiPortHole *saveMPH;
};

#endif
