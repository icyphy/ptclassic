/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 1/17/89
 Revisions:
 	3/19/90 - J. Buck
	Add method BlockList::blockWithName
	Use newConnection method in Galaxy::connect: we no longer have
	to know at compile time whether an argument is a PortHole or
	MultiPortHole.

	5/26/90 - I. Kuroda 
	Add method Galaxy::initState

	5/29/90 - J. Buck
	Change operator StringList to printVerbose.  Galaxy::connect
	is gone; it is now done by GenericPort class instead.

Methods for class Galaxy
***************************************************************************/
#include "Star.h"
#include "StringList.h"
#include "Galaxy.h"
#include "Block.h"

// small virtual functions

// atomic check
int Galaxy :: isItAtomic () const { return FALSE;}

// return myself as a galaxy
Galaxy& Galaxy :: asGalaxy () const { return *this;}

// destructor.  Not really do-nothing because members are deleted
Galaxy :: ~ Galaxy () {}

	////////////////////////////////////
	// Galaxy::printRecursive()
	////////////////////////////////////

StringList
Galaxy :: printRecursive () {
	StringList out;
	out = "GALAXY: ";
	out += readFullName();
	out += "\n";
	out += "Descriptor: ";
	out += readDescriptor();
	out += "\n";
	out += "Number of blocks: ";
	out += numberBlocks();
	out += "\n";
	out += printPorts("Galaxy");
	out += printStates("Galaxy");
	out += "Blocks in the Galaxy:----------------------------------\n";
	for(int i = numberBlocks(); i>0; i--)
		out += nextBlock().printRecursive();
	return out;
}

	////////////////////////////////////
	// Galaxy::printVerbose()
	////////////////////////////////////

StringList
Galaxy :: printVerbose () {
	StringList out;
	out = "GALAXY: ";
	out += readFullName();
	out += "\n";
	out += "Descriptor: ";
	out += readDescriptor();
	out += "\n";
	out += "Contained blocks: ";
	for (int i = numberBlocks(); i>0; i--) {
		out += nextBlock().readName();
		if (i > 1) out += ", ";
	}
	out += "\n";
	out += printPorts("Galaxy");
	out += printStates("Galaxy");
	return out;
}


        ////////////////////////////////////
        // initialize()
        ////////////////////////////////////

// Initialize myself and all sub-blocks.  Note that
// Block::initialize calls start().

void Galaxy :: initialize() {
	Block::initialize();
	for(int i=blocks.size(); i>0; i--)
		blocks++.initialize();
}

        ////////////////////////////////////
        // initState()
        ////////////////////////////////////

// like initialize but for states only.

void Galaxy :: initState() {
        Block::initState();
        for(int i=blocks.size(); i>0; i--)
		blocks++.initState();
}

        ////////////////////////////////////
        // Block* blockWithName()
        ////////////////////////////////////

// return ptr to sub-block with given name, NULL if not found
Block *
BlockList::blockWithName (const char* ident) {
	Block *p;
	for (int i = size();i>0;i--) {
		p = (Block *)next();
		if (strcmp(ident,p->readName()) == 0)
			return p;
	}
	return NULL;
}

// return myDomain if set, else use domain of first sub-block.
const char*
Galaxy::domain() const {
	if (myDomain) return myDomain;
	if (numberBlocks() == 0)
		return "UNKNOWN";
	else return blocks.head().domain();
}
