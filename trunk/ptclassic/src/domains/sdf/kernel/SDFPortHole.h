/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 5/29/90, J. Buck

This file contains definitions of SDF-specific PortHole classes.

******************************************************************/
#ifndef _SDFPortHole_h
#define _SDFPortHole_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "PortHole.h"

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
        // class DFPortHole
        //////////////////////////////////////////

// Common ancestor for BDFPortHole and SDFPortHole.

class DFPortHole : public PortHole
{
protected:
	int maxBackValue;	// maximum % argument allowed
public:
	DFPortHole();

	// Tell whether port uses old values
	int usesOldValues() const { return maxBackValue >= numberTokens;}

	// access the maximum delay
	int maxDelay() const { return maxBackValue;}

	// Modify simulated counts on geodesics, for scheduling
	void decCount(int n);
	void incCount(int n);

	// class identification
	int isA(const char*) const;

	// associated control port (default: none)
	virtual PortHole* assocPort();

	// relation to associated port (default: none)
	virtual int assocRelation() const;
};

        //////////////////////////////////////////
        // class SDFPortHole
        //////////////////////////////////////////

// Contains all the special features required for
//   synchronous dataflow (SDF)

class SDFPortHole : public DFPortHole
{
public:


        // The setPort function is redefined to take one more optional
        // argument, the number of Particles consumed/generated
        PortHole& setPort(const char* portName,
                          Block* parent,
                          DataType type = FLOAT,
			  // Number Particles consumed/generated
                          unsigned numTokens = 1,
			  // Maximum delay the Particles are accessed
			  unsigned maxPctValue = 0);

	// Function to alter only numTokens and delay.
	// We re-do porthole initialization if bufferSize changes
	PortHole& setSDFParams(unsigned numTokens = 1, unsigned maxPctValue=0);


	// class identification
	int isA(const char*) const;

	// Services of PortHole that are often used:
	// setPort(DataType d);
	// Particle& operator % (int);
};

	///////////////////////////////////////////
	// class InSDFPort
	//////////////////////////////////////////

class InSDFPort : public SDFPortHole
{
public:
	int isItInput () const ; // {return TRUE; }

	// Get Particles from input Geodesic
	void receiveData();

        // Services of PortHole that are often used: 
        // setPort(DataType d); 
        // Particle& operator % (int);
};

	////////////////////////////////////////////
	// class OutSDFPort
	////////////////////////////////////////////

class OutSDFPort : public SDFPortHole
{
public:
        int isItOutput () const; // {return TRUE; }

	void increment();

	// Move the current Particle in the input buffer -- this
	// method is invoked by the SDFScheduler before go()
	void receiveData();

	// Put the Particles that were generated into the
	// output Geodesic -- this method is invoked by the
	// SDFScheduler after go()
	void sendData();

        // Services of PortHole that are often used: 
        // setPort(DataType d); 
        // Particle& operator % (int);
};

        //////////////////////////////////////////
        // class MultiSDFPort
        //////////////////////////////////////////
 
// Synchronous dataflow MultiPortHole
 
class MultiSDFPort : public MultiPortHole {
protected:
        // The number of Particles consumed
        unsigned numberTokens;
public:
	MultiSDFPort();
	~MultiSDFPort();

	// Function to alter only numTokens and delay.
	// We re-do porthole initialization if bufferSize changes
	MultiPortHole& setSDFParams(unsigned numTokens = 1,
				    unsigned maxPstValue=0);

        // The setPort function is redefined to take one more optional
        // argument, the number of Particles produced
        MultiPortHole& setPort(const char* portName,
                          Block* parent,
                          DataType type = FLOAT,        // defaults to FLOAT
                          unsigned numTokens = 1);      // defaults to 1
};

        //////////////////////////////////////////
        // class MultiInSDFPort
        //////////////////////////////////////////
        
// MultiInSDFPort is an SDF input MultiPortHole
 
class MultiInSDFPort : public MultiSDFPort {
public:
        int isItInput () const; // {return TRUE; }
 
        // Add a new physical port to the MultiPortHole list
        PortHole& newPort();
};
 
 
        //////////////////////////////////////////
        // class MultiOutSDFPort
        //////////////////////////////////////////

// MultiOutSDFPort is an SDF output MultiPortHole  

class MultiOutSDFPort : public MultiSDFPort {     
public:
        int isItOutput () const; // {return TRUE; }

        // Add a new physical port to the MultiPortHole list
        PortHole& newPort();
};

#endif
