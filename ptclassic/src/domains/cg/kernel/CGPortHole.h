/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee

This file contains definitions of CG-specific PortHole classes.

******************************************************************/
#ifndef _CGConnect_h
#define _CGConnect_h 1
#include "Connect.h"
#include "SDFConnect.h"

/*****************************************************************
CG: Code generation

These PortHoles are much like SDF PortHoles, from which they are derived.
****************************************************************/

        //////////////////////////////////////////
        // class CGPortHole
        //////////////////////////////////////////

// Contains all the special features required for
//   synchronous dataflow (CG)

class CGPortHole : public SDFPortHole
{
	char* regClass;	// identifies the register class of the portHole
public:
	void setRegClass(char* className) {regClass = className; }

	// Services of SDFPortHole that are used often:
	// setPort(char* portName, Block* parent, dataType type,
	//	unsigned numTokens, unsigned delay)
	// setSDFParams(unsigned numTokens, unsigned delay);
};

	///////////////////////////////////////////
	// class InCGPort
	//////////////////////////////////////////

class InCGPort : public CGPortHole
{
public:
	int isItInput () const ; // {return TRUE; }

	// Get Particles from input Geodesic
	// *****  CG has no data on the portholes
	// void grabData();
};

	////////////////////////////////////////////
	// class OutCGPort
	////////////////////////////////////////////

class OutCGPort : public CGPortHole
{
public:
        int isItOutput () const; // {return TRUE; }

	void increment();

	// Move the current Particle in the input buffer -- this
	// method is invoked by the CGScheduler before go()
	// void grabData();

	// Put the Particles that were generated into the
	// output Geodesic -- this method is invoked by the
	// CGScheduler after go()
	// void sendData();
};

        //////////////////////////////////////////
        // class MultiCGPort
        //////////////////////////////////////////
 
// Synchronous dataflow MultiPortHole for code generation
 
class MultiCGPort : public MultiSDFPort {
	int memLoc;
public:
	void setMemLoc(int i) { memLoc = 1;}

	// Services of MultiSDFPort that are used often:
	// setPort(const char* portName, Block* parent, dataType type,
	//	unsigned numTokens)
};

        //////////////////////////////////////////
        // class MultiInCGPort
        //////////////////////////////////////////
        
// MultiInCGPort is an CG input MultiPortHole
 
class MultiInCGPort : public MultiCGPort {
public:
        int isItInput () const; // {return TRUE; }
 
        // Add a new physical port to the MultiPortHole list
        PortHole& newPort();
};
 
 
        //////////////////////////////////////////
        // class MultiOutCGPort
        //////////////////////////////////////////

// MultiOutCGPort is an CG output MultiPortHole  

class MultiOutCGPort : public MultiCGPort {     
public:
        int isItOutput () const; // {return TRUE; }

        // Add a new physical port to the MultiPortHole list
        PortHole& newPort();
};

#endif
