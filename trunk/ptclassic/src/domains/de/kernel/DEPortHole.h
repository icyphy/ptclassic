#ifndef _DEconnect_h
#define _DEconnect_h 1

#include "NamedObj.h"
#include "DataStruct.h"
#include "dataType.h"
#include "type.h"
#include "Connect.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 5/30/90
 Revisions:

This file contains definitions of DE PortHoles.

******************************************************************/

class Particle;

        //////////////////////////////////////////
        // class DEPortHole
        //////////////////////////////////////////

// Contains all the special features required for
//   Discrete Event system

class DEPortHole : public PortHole
{
public:
	// The setPort function is redefined to set DE-specific members.
	PortHole& setPort(const char* portName,
			  Block* parent,
			  dataType type = FLOAT);

	// DEPortHole has a "timeStamp" attribute.
	float timeStamp;

	// Is there a new data?
	int dataNew;

};


	///////////////////////////////////////////
	// class InDEPort
	//////////////////////////////////////////

class InDEPort : public DEPortHole
{
public:
	int isItInput () const {return TRUE; }

	// Get particles from input Geodesic and set the timeStamp
	void grabData();

	// operator to return a zero delayed Particle,
	// which is same as operator % (0).
	// reset dataNew = FALSE.
	Particle& get();
};

	////////////////////////////////////////////
	// class OutDEPort
	////////////////////////////////////////////

class OutDEPort : public DEPortHole
{
public:
	int isItOutput() const {return TRUE; }

	// Send a data into the output Geodesic and signal an event to
	// the event queue.
	void sendData();

	// operator to return a zero delayed Particle,
	// which is same as operator % (0).
	// set the timestamp.
	Particle& put(float stamp);
};

        //////////////////////////////////////////
        // class MultiDEPort
        //////////////////////////////////////////
 
// Discrete event MultiPortHole
 
typedef class MultiPortHole MultiDEPort;

        //////////////////////////////////////////
        // class MultiInDEPort
        //////////////////////////////////////////
        
// MultiInDEPort is an DE input MultiPortHole
 
class MultiInDEPort : public MultiDEPort {
public:
        int isItInput () const {return TRUE; }
 
        // Add a new physical port to the MultiPortHole list
        PortHole& newPort();
};
 
 
        //////////////////////////////////////////
        // class MultiOutDEPort
        //////////////////////////////////////////

// MultiOutDEPort is an DE output MultiPortHole  

class MultiOutDEPort : public MultiDEPort {     
public:
        int isItOutput () const {return TRUE; }

        // Add a new physical port to the MultiPortHole list
        PortHole& newPort();
};

// Iterators for MultiDEPorts -- they aren't required but make coding
// stars a bit cleaner.  They "know" that MultiInDEPorts have InDEPorts
// and MultiOutDEPorts have OutDEPorts.

class InDEMPHIter : public MPHIter {
public:
	InDEMPHIter(const MultiInDEPort& mph) : MPHIter(mph) {}
	InDEPort* next() { return (InDEPort*) MPHIter::next();}
	InDEPort* operator++() { return next();}
};

class OutDEMPHIter : public MPHIter {
public:
	OutDEMPHIter(const MultiOutDEPort& mph) : MPHIter(mph) {}
	OutDEPort* next() { return (OutDEPort*) MPHIter::next();}
	OutDEPort* operator++() { return next();}
};

#endif
