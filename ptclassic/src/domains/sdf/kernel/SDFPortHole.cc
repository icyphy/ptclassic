static const char file_id[] = "SDFConnect.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include "Block.h"
#include "SDFConnect.h"
#include "CircularBuffer.h"
#include "Geodesic.h"

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

int InSDFPort :: isItInput () const { return TRUE;}

void InSDFPort :: grabData () { getParticle();}

int OutSDFPort :: isItOutput () const { return TRUE;}

void OutSDFPort :: grabData () { clearParticle();}

void OutSDFPort :: sendData () { putParticle();}

int MultiInSDFPort :: isItInput () const { return TRUE;}
int MultiOutSDFPort :: isItOutput () const { return TRUE;}

PortHole& SDFPortHole :: setPort (
			     const char* s,
                             Block* parent,
                             DataType t,
                             unsigned numTokens,
			     unsigned delay)
{
	// Initialize PortHole
        PortHole::setPort(s,parent,t);

	// Based on the delay, we allocate a Buffer assuming no
	// past Particles are allocated
        numberTokens = numTokens;
	maxBackValue = delay;
	// The number of Particles the buffer has to hold is:
	//		numberTokens current and future Particles
	//		delay past Particles
	if ( numberTokens > int(delay)) bufferSize = numberTokens;
	else bufferSize = delay + 1;

        return *this;
}

// Function to alter only numTokens and delay.
// We re-do porthole initialization if bufferSize changes
PortHole& SDFPortHole :: setSDFParams(unsigned numTokens, unsigned delay) {
	numberTokens = numTokens;
	maxBackValue = delay;
	if ( numberTokens > int(delay)) bufferSize = numberTokens;
	else bufferSize = delay + 1;
	if (myBuffer && myBuffer->size() != bufferSize)
		initialize();
	return *this;
}

MultiPortHole& MultiSDFPort :: setPort (const char* s,
                             Block* parent,
                             DataType t,
                             unsigned numTokens) {
        MultiPortHole::setPort(s,parent,t);
        numberTokens = numTokens;
        return *this;
}

PortHole& MultiInSDFPort :: newPort () {
	LOG_NEW; PortHole& p = *new InSDFPort;
	p.numberTokens = numberTokens;
	return installPort(p);
}
 
 
PortHole& MultiOutSDFPort :: newPort () {
	LOG_NEW; PortHole& p = *new OutSDFPort;
	p.numberTokens = numberTokens;
	return installPort(p);
}

void SDFPortHole :: incCount(int n) { myGeodesic->incCount(n);}
void SDFPortHole :: decCount(int n) { myGeodesic->decCount(n);}

// isA
ISA_FUNC(SDFPortHole,PortHole);
