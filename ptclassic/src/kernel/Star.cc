/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 12/15/89

*******************************************************************/
#include "Star.h"
#include "StringList.h"


// SCCS version identification
// @(#)Star.cc	1.4	1/14/90

Star :: operator char* () {
	StringList out;
	out = "Star: ";
	out += readFullName();
	out += "\n";
	out += "Descriptor: ";
	out += readDescriptor();
	out += "\n";
	out += "Ports in the star:\n";
	for(int i = numberPorts(); i>0; i--)
		out += nextPort().operator char* ();
	return out;
}

// Redefine method setting internal data in the Block
// so that various SDF-specific initilizations can be performed.
// If the parent pointer is not provied, it defaults to NULL
Block& SDFStar :: setBlock(char* s, Block* parent = NULL) {
	// First invoke the generic setBlock
	Block::setBlock(s,parent);

	// Then perform SDF-specific initializations
	repetitions = 0;
	noTimes = 0;

	return *this;
}

// Methods to consume and produce particles, invoked by the scheduler
void SDFStar :: getParticles() {
	SDFPortHole* port;
	for(int i = numberPorts(); i>0; i--) {
		port = &(SDFPortHole&)nextPort();
		port->increment(port->numberTokens);
	}
}
