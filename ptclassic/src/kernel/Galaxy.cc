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

Geodesic& Galaxy :: connect (PortHole& source, PortHole& destination,
			int numberDelays = 0) {

	// Resolve any aliases and MultiPortHole stuff:
	// newConnection is a virtual function that does the right
	// thing for all types of PortHoles.
	PortHole& realSource = source.newConnection();
	PortHole& realDest = destination.newConnection();

	Geodesic* geo = source.allocateGeodesic();
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
	// operator char* ()
	////////////////////////////////////


Galaxy :: operator char* () {
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
	if (numberPorts()>0) out += "Ports in the Galaxy:\n";
	   for(int i = numberPorts(); i>0; i--)
		out += nextPort();

	out += "Blocks in the Galaxy:----------------------------------\n";
	for(i = numberBlocks(); i>0; i--)
		out += nextBlock();
	return out;
}

Block *
BlockList::blockWithName (char *ident) {
	Block *p;
	for (int i = size();i>0;i--) {
		p = (Block *)next();
		if (strcmp(ident,p->readName()) == 0)
			return p;
	}
	return NULL;
}
