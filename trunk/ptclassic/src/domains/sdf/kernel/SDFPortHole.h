/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 5/29/90, J. Buck

This file contains definitions of SDF-specific PortHole classes.

******************************************************************/
#ifndef _SDFConnect_h
#define _SDFConnect_h 1
#include "Connect.h"

/*****************************************************************
SDF: Synchronous Data Flow

This is a common special case that is handled differently
from other cases:

	Each PortHole promises to consume or generate a fixed
	number of Particles each time the Star is invoked.
	This number is stored in the PortHole and can be accessed
	by the SDFScheduler

	The incrementing of time is forced by the SDFScheduler,
	and not by the Star itself. Incrementing time is effected
	by consuming or generating Particles
****************************************************************/

        //////////////////////////////////////////
        // class SDFPortHole
        //////////////////////////////////////////

// Contains all the special features required for
//   synchronous dataflow (SDF)

class SDFPortHole : public PortHole
{
public:
        // The setPort function is redefined to take one more optional
        // argument, the number of Particles consumed/generated
        PortHole& setPort(const char* portName,
                          Block* parent,
                          dataType type = FLOAT,
			  // Number Particles consumed/generated
                          unsigned numTokens = 1,
			  // Maximum delay the Particles are accessed
			  unsigned delay = 0);

	// Function to alter only numTokens and delay.
	// We re-do porthole initialization if bufferSize changes
	PortHole& setSDFParams(unsigned numTokens = 1, unsigned delay=0) {
		numberTokens = numTokens;
		bufferSize = numberTokens + delay;
		if (myBuffer && myBuffer->size() != bufferSize)
			initialize();
		return *this;
	}

	// Services of PortHole that are often used:
	// setPort(dataType d);
	// Particle& operator % (int);
};

	///////////////////////////////////////////
	// class InSDFPort
	//////////////////////////////////////////

class InSDFPort : public SDFPortHole
{
public:
	int isItInput () {return TRUE; }

	// Get Particles from input Geodesic
	void beforeGo() {getParticle() ;}

        // Services of PortHole that are often used: 
        // setPort(dataType d); 
        // Particle& operator % (int);
};

	////////////////////////////////////////////
	// class OutSDFPort
	////////////////////////////////////////////

class OutSDFPort : public SDFPortHole
{
public:
        int isItOutput () {return TRUE; }

	void increment();

	// Move the current Particle in the input buffer -- this
	// method is invoked by the SDFScheduler before go()
	void beforeGo() { clearParticle() ;}

	// Put the Particles that were generated into the
	// output Geodesic -- this method is invoked by the
	// SDFScheduler after go()
	void afterGo() { putParticle() ;}

        // Services of PortHole that are often used: 
        // setPort(dataType d); 
        // Particle& operator % (int);
};

        //////////////////////////////////////////
        // class MultiSDFPort
        //////////////////////////////////////////
 
// Synchronous dataflow MultiPortHole
 
class MultiSDFPort : public MultiPortHole {
public:
        // The number of Particles consumed
        unsigned numberTokens;
 
        // The setPort function is redefined to take one more optional
        // argument, the number of Particles produced
        MultiPortHole& setPort(const char* portName,
                          Block* parent,
                          dataType type = FLOAT,        // defaults to FLOAT
                          unsigned numTokens = 1);      // defaults to 1
};

        //////////////////////////////////////////
        // class MultiInSDFPort
        //////////////////////////////////////////
        
// MultiInSDFPort is an SDF input MultiPortHole
 
class MultiInSDFPort : public MultiSDFPort {
public:
        int isItInput () {return TRUE; }
 
        // Add a new physical port to the MultiPortHole list
        PortHole& newPort();
};
 
 
        //////////////////////////////////////////
        // class MultiOutSDFPort
        //////////////////////////////////////////

// MultiOutSDFPort is an SDF output MultiPortHole  

class MultiOutSDFPort : public MultiSDFPort {     
public:
        int isItOutput () {return TRUE; }

        // Add a new physical port to the MultiPortHole list
        PortHole& newPort();
};

#endif
