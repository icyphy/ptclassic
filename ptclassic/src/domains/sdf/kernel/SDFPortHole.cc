#include "Block.h"
#include "SDFConnect.h"
#include "CircularBuffer.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 5/29/90
 Revisions: 5/29/90: J. Buck split SDF-specific parts of Connect.cc
		     into this file.

Code for SDF-type portholes and multiportholes.

**************************************************************************/

int InSDFPort :: isItInput () { return TRUE;}

void InSDFPort :: beforeGo () { getParticle();}

int OutSDFPort :: isItOutput () { return TRUE;}

void OutSDFPort :: beforeGo () { clearParticle();}

void OutSDFPort :: afterGo () { putParticle();}

int MultiInSDFPort :: isItInput () { return TRUE;}
int MultiOutSDFPort :: isItOutput () { return TRUE;}

PortHole& SDFPortHole :: setPort (
			     const char* s,
                             Block* parent,
                             dataType t = FLOAT,
                             unsigned numTokens = 1,
			     unsigned delay = 0)
{
	// Initialize PortHole
        PortHole::setPort(s,parent,t);

	// Based on the delay, we allocate a Buffer assuming no
	// past Particles are allocated
        numberTokens = numTokens;
	// The number of Particles the buffer has to hold is:
	//		numberTokens current and future Particles
	//		delay past Particles
	bufferSize = numberTokens + delay;

        return *this;
}

// Function to alter only numTokens and delay.
// We re-do porthole initialization if bufferSize changes
PortHole& SDFPortHole :: setSDFParams(unsigned numTokens, unsigned delay) {
	numberTokens = numTokens;
	bufferSize = numberTokens + delay;
	if (myBuffer && myBuffer->size() != bufferSize)
		initialize();
	return *this;
}

MultiPortHole& MultiSDFPort :: setPort (const char* s,
                             Block* parent,
                             dataType t = FLOAT,
                             unsigned numTokens = 1) {
        MultiPortHole::setPort(s,parent,t);
        numberTokens = numTokens;
        return *this;
}

PortHole& MultiInSDFPort :: newPort () {
	PortHole& p = *new InSDFPort;
	p.numberTokens = numberTokens;
	return installPort(p);
}
 
 
PortHole& MultiOutSDFPort :: newPort () {
	PortHole& p = *new OutSDFPort;
	p.numberTokens = numberTokens;
	return installPort(p);
}

