/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 8/9/90

This file contains definitions of DDF-specific PortHole classes.

******************************************************************/
#ifndef _DDFConnect_h
#define _DDFConnect_h 1
#ifdef __GNUG__
#pragma interface
#endif
#include "Connect.h"

        //////////////////////////////////////////
        // class DDFPortHole
        //////////////////////////////////////////

// Contains all the special features required for
//   dynamic dataflow (DDF)

class DDFPortHole : public PortHole
{
	friend class MultiDDFPort;

	int varying;	// flag to be set if dynamic

public:
	DDFPortHole() : varying(0) {}

        // The setPort function is redefined to take one more optional
        // argument, the number of Particles consumed/generated
        PortHole& setPort(const char* portName,
                          Block* parent,
                          DataType type = FLOAT,
			  // Number Particles consumed/generated
                          unsigned numTokens = 1,
			  // Maximum delay the Particles are accessed
			  unsigned delay = 0);

	int isDynamic() { return varying ;}

	// function to alter only numTokens.
	void setDDFParams(unsigned numTokens = 1);

	// these methods are for recursion construct.
	// Since DDFStars will not be heavily used in a system, we 
	// include them in this base class.
	void imageConnect();			// connect with imagePort
	virtual void moveData();	// move them to imagePort
	Geodesic* imageGeo;
	PortHole* imagePort;
};

	///////////////////////////////////////////
	// class InDDFPort
	//////////////////////////////////////////

class InDDFPort : public DDFPortHole
{
public:
	int isItInput () const; // {return TRUE; }

	// Get Particles from input Geodesic
	void grabData();

	// for special case
	void moveData();
};

	////////////////////////////////////////////
	// class OutDDFPort
	////////////////////////////////////////////

class OutDDFPort : public DDFPortHole
{
public:
        int isItOutput () const; // {return TRUE; }

	void increment();

	// Move the current Particle in the input buffer -- this
	// method is invoked by the DDFScheduler grabData()
	void grabData();

	// Put the Particles that were generated into the
	// output Geodesic -- this method is invoked by the
	// DDFScheduler sendData()
	void sendData();

	// for special case
	void moveData();
};

        //////////////////////////////////////////
        // class MultiDDFPort
        //////////////////////////////////////////
 
// Dynamic dataflow MultiPortHole
 
class MultiDDFPort : public MultiPortHole {
protected:
	// add the DDFPortHole to my portlist.
	// Used by MultiInDDFPort and MultiOutDDFPort.
	PortHole& finishNewPort(DDFPortHole&);
public:
        // The number of Particles consumed
        unsigned numberTokens;
 
        // The setPort function is redefined to take one more optional
        // argument, the number of Particles produced
        MultiPortHole& setPort(const char* portName,
                          Block* parent,
                          DataType type = FLOAT,        // defaults to FLOAT
                          unsigned numTokens = 1);      // defaults to 1
};

        //////////////////////////////////////////
        // class MultiInDDFPort
        //////////////////////////////////////////
        
// MultiInDDFPort is an DDF input MultiPortHole
 
class MultiInDDFPort : public MultiDDFPort {
public:
        int isItInput () const; // {return TRUE; }
 
        // Add a new physical port to the MultiPortHole list
        PortHole& newPort();
};
 
 
        //////////////////////////////////////////
        // class MultiOutDDFPort
        //////////////////////////////////////////

// MultiOutDDFPort is an DDF output MultiPortHole  

class MultiOutDDFPort : public MultiDDFPort {     
public:
        int isItOutput () const; // {return TRUE; }

        // Add a new physical port to the MultiPortHole list
        PortHole& newPort();
};

#endif
