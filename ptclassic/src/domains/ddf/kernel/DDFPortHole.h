/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.
							COPYRIGHTENDKEY

 Programmer:  Soonhoi Ha
 Date of creation: 8/9/90

This file contains definitions of DDF-specific PortHole classes.

******************************************************************/
#ifndef _DDFPortHole_h
#define _DDFPortHole_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "DFPortHole.h"

	//////////////////////////////////////////
	// class DDFPortHole
	//////////////////////////////////////////

// Contains all the special features required for
//   dynamic dataflow (DDF)

class DDFPortHole : public DFPortHole
{
	friend class MultiDDFPort;

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

	int isDynamic() const { return varying ;}

	// Redefine to set the "varying" member.
	PortHole& setSDFParams(unsigned numTokens, unsigned maxPctValue);

	// Function to alter the numberTokens member, which indicates the
	// number of tokens produced or consumed, or 0 if not known.
	// This is really just another name for setSDFParams.
	void setDDFParams(unsigned numTokens = 1);

	// FIXME: I find this exceedingly ugly -- JTB
	// these methods are for recursion construct.
	// Since DDFStars will not be heavily used in a system, we 
	// include them in this base class.
	void imageConnect();			// connect with imagePort

	// Move particles to the imagePort.  Returns the number of particles
	// actually transferred.  Used for recursion only.
	virtual int moveData();
	Geodesic* imageGeo;
	PortHole* imagePort;

protected:
	int varying;	// flag to be set if dynamic
};

	///////////////////////////////////////////
	// class InDDFPort
	//////////////////////////////////////////

class InDDFPort : public DDFPortHole
{
public:
	int isItInput () const; // {return TRUE; }

	// Get Particles from input Geodesic
	void receiveData();

	// Move particles to the imagePort.  Returns the number of particles
	// actually transferred.  Used for recursion only.
	int moveData();
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
	// method is invoked by the DDFScheduler receiveData()
	void receiveData();

	// Put the Particles that were generated into the
	// output Geodesic -- this method is invoked by the
	// DDFScheduler sendData()
	void sendData();

	// Move particles to the imagePort.  Returns the number of particles
	// actually transferred.  Used for recursion only.
	int moveData();
};

	//////////////////////////////////////////
	// class MultiDDFPort
	//////////////////////////////////////////
 
// Dynamic dataflow MultiPortHole
 
class MultiDDFPort : public MultiDFPort {
protected:
	// add the DDFPortHole to my portlist.
	// Used by MultiInDDFPort and MultiOutDDFPort, but defined
	// here because MultiDDFPort is a friend of DDFPortHole.
	PortHole& finishNewPort(DDFPortHole&);
public:
	// Function to alter the numberTokens member, which indicates the
	// number of tokens produced or consumed, or 0 if not known.
	// This is really just another name for setSDFParams.
	void setDDFParams(unsigned numTokens = 1);
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

// Iterators for MultiDDFPorts -- they aren't required but make coding
// stars a bit cleaner.  They "know" that MultiInDDFPorts have InDDFPorts
// and MultiOutDDFPorts have OutDDFPorts.

class InDDFMPHIter : public MPHIter {
public:
	InDDFMPHIter(MultiInDDFPort& mph) : MPHIter(mph) {}
	InDDFPort* next() { return (InDDFPort*) MPHIter::next();}
	InDDFPort* operator++(POSTFIX_OP) { return next();}
};

class OutDDFMPHIter : public MPHIter {
public:
	OutDDFMPHIter(MultiOutDDFPort& mph) : MPHIter(mph) {}
	OutDDFPort* next() { return (OutDDFPort*) MPHIter::next();}
	OutDDFPort* operator++(POSTFIX_OP) { return next();}
};

#endif
