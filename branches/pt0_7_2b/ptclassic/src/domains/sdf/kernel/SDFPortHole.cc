#include "Block.h"
#include "SDFConnect.h"

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

void InSDFPort :: beforeGo()
{
// It is assumed this routine is called before a Star executes...
// It gets numberTokens Particles from the Geodesic and stores them
// in the buffer, then setting current time to the last Particle input

	for(int i = numberTokens; i>0; i--)
		{
		// Move the current time pointer in the buffer
		Pointer* p = myBuffer->next();

                // Put current Particle back into Plasma  to be
		// recycled back to some OutSDFPort
		myPlasma->put((Particle*)*p);
 
		// Get another Particle from the Geodesic
        	*p = myGeodesic->get();
		}
}

void OutSDFPort :: beforeGo()
{
	Pointer* p;

// It is assumed this routine is called before a Star executes....
// It moves current by numberTokens in the buffer, initializing
//  each Particle; after the Star executes these Particles will
//  contain data destined for another Star, and will be launched
//  into the Geodesic

// For this case, the buffer always stores the same set of
// Particles -- copies of these Particles are launched into the
// Geodesic so that there is no possibility of modifying a Particle
// that resides in the Geodesic or InSDFPort on the other side

// After this routine executes, the buffer current will point to
// the last Particle, so the operator% references Particles relative
// to this time

	for(int i = numberTokens; i>0; i--) {
	
		// Get and initialize next Particle
        	p = myBuffer->next();
		((Particle*)*p)->initialize();
		}
}

void OutSDFPort :: afterGo()
{
	Pointer* p;

// This method is called after go(); the buffer now contains numberTokens
// Particles that are to be send to the output Geodesic
// We send copies to prevent the Star from modifying Particles in the
// Geodesic or InPortHole on the other side

	// Back up in the buffer by numberTokens
	for(int i = numberTokens; i>0; i--)
        	p = myBuffer->last();

	// Now move forward numberTokens times, sending copy of
	// Particle to Geodesic
	for(i = numberTokens; i>0; i--) {
		p = myBuffer->next();
		
		// Get Particle from Plasma
		Particle* pp = myPlasma->get();

		// Copy from the buffer to this Particle
		*pp = *(Particle*)*p;

		// Launch this Particle into the Geodesic
		myGeodesic->put(pp);
		}

	// Note that the Particle is left in the buffer to be
	// accessed as a past Particle
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
        InSDFPort* newport = new InSDFPort;
        ports.put(*newport);
        parent()->
            addPort(newport->
                        setPort(newName(), parent(), type, numberTokens));
	newport->typePort = typePort;
        return *newport;
}
 
 
PortHole& MultiOutSDFPort :: newPort () {
        OutSDFPort* newport = new OutSDFPort;
        ports.put(*newport);
        parent()->
            addPort(newport->
                        setPort(newName(), parent(), type, numberTokens));
	newport->typePort = typePort;
        return *newport;
}

