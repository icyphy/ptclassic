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

Methods for class Galaxy
***************************************************************************/
#include "Star.h"
#include "StringList.h"
#include "Galaxy.h"
#include "Block.h"


	////////////////////////////////////
	// connect
	////////////////////////////////////

// Need to add error checking

Geodesic& Galaxy :: connect (GenericPort& source, GenericPort& destination,
			int numberDelays = 0) {

	// Resolve any aliases and MultiPortHole stuff:
	// newConnection is a virtual function that does the right
	// thing for all types of PortHoles.
	PortHole& realSource = source.newConnection();
	PortHole& realDest = destination.newConnection();

	Geodesic* geo = realSource.allocateGeodesic();
	geo->originatingPort = &realSource;
	geo->destinationPort = &realDest;
	realSource.myGeodesic = geo;
	realDest.myGeodesic = geo;

	// Set the farSidePort pointers in both blocks
	// This information is redundant, since it also appears in the
	// Geodesic, but to get it from the Geodesic, you have to know
	// which PortHole is an input, and which is an output.
	realSource.farSidePort = &realDest;
	realDest.farSidePort = &realSource;

	// Set the number of delays
	geo->numInitialParticles = numberDelays;

	return *geo;
}


// TO BE DONE:  need another connect method that accepts a Particle
// reference as a fourth parameter giving the value of the initial Particle
// put on the Geodesic.



	////////////////////////////////////
	// operator StringList ()
	////////////////////////////////////


Galaxy :: operator StringList () {
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
		out += StringList(nextBlock());
	return out;
}


        ////////////////////////////////////
        // initState()
        ////////////////////////////////////

void Galaxy :: initState() {
        Block::initState();
        for(int i=blocks.size(); i>0; i--) {
                Block& b = blocks++;
                b.initState();}
}

        ////////////////////////////////////
        // Block* blockWithName()
        ////////////////////////////////////

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
