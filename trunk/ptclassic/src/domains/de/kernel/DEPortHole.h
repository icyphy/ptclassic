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
 Date of creation: 5/30/90
 Revisions:

This file contains definitions of DE PortHoles.
Two DE-specific struct/classes are defined.
	Event : entry for the global event queue.
		consists of destination PortHole and Particle.
	EventQueue : derived from PriorityQueue.

******************************************************************/

#ifndef _DEPortHole_h
#define _DEPortHole_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "NamedObj.h"
#include "DataStruct.h"
#include "dataType.h"
#include "type.h"
#include "PortHole.h"

class Particle;

#define MINDEPTH -100000	// guarantee it is far below the minimum depth

        //////////////////////////////////////////
        // class DEPortHole
        //////////////////////////////////////////

// Contains all the special features required for
//   Discrete Event system

class DEPortHole : public PortHole
{
public:
	DEPortHole();
	~DEPortHole();

	// The setPort function is redefined to set DE-specific members.
	PortHole& setPort(const char* portName,
			  Block* parent,
			  DataType type = FLOAT);

	// DEPortHole has a "timeStamp" attribute.
	double timeStamp;

	// Is there a new data?
	int dataNew;

 	// The depth is the longest path from this port to a terminating port.
        // Terminating ports are those of delayType or toEventHorizons.
        // To use depth as a fineLevel for PriorityQueue,
        // we do sign-reversal : depth = - longest path.  Therefore,
        // stars with smallest depth has the highest priority for scheduling
        // at the same global time.
        int depth;
};


	///////////////////////////////////////////
	// class InDEPort
	//////////////////////////////////////////

class InDEPort : public DEPortHole
{
protected:
friend class DEScheduler;
friend class MultiInDEPort;
	// It is said "complete" if this input affects all outputs
	// immediately. Valid definition for functional stars.
	// The default value is "complete" (TRUE).
	// If it is not complete, outputs listed in the "triggerList"
	// structure are examined for determining the depth of the
	// porthole.
	int complete;

	// If there are simultaneous events for a star, this port should
	// have higher priority than "beforeP" porthole. (default = NULL)
	GenericPort* beforeP;

	// If it is not complete, or it does not affect all outputs,
	// it keeps the list of the (possibly-)affected outputs.
	SequentialList *triggerList;

	// Queue for the all simultaneous events.
	Queue* inQue;

        // number of simultaneous events on the same arc - 1.
        int moreData;

public:

	int isItInput () const {return TRUE; }

	// Get particles from global queue and set the timeStamp.
	// At A TIME INSTANCE, the first call of this method returns TRUE,
	// afterwards return FALSE.
	virtual int getFromQueue(Particle* p);

	// operator to return a zero delayed Particle,
	// which is same as operator % (0).
	// reset dataNew = FALSE.
	Particle& get();

	// add an output to "triggerList" list.
	void triggers (GenericPort& op);
	void triggers ()	{ complete = FALSE; }

	// set "beforeP" member. 
	void before(GenericPort& gp) { beforeP = &gp; }

	// Number of the pending simultaneous events in the porthole.
	int numSimulEvents() {  if (inQue) return inQue->length();
			 	else return (moreData - 1); }

	// Fetch a simultaneous event from the global queue.
	void getSimulEvent();

	// create inQue
	void createQue() {
		if (!inQue) { INC_LOG_NEW; inQue = new Queue; }
	}

	// clean itself before a new phase of firing.
	virtual void cleanIt(); 

	// constructor and destructor
	InDEPort() : complete(TRUE), triggerList(0), beforeP(0), inQue(0) {}
	~InDEPort();
};

	////////////////////////////////////////////
	// class OutDEPort
	////////////////////////////////////////////

class OutDEPort : public DEPortHole
{
friend class MultiOutDEPort;
public:
	int isItOutput() const {return TRUE; }

	// Send a data into the global queue and signal an event to
	// the event queue.
	void sendData();

	// operator to return a zero delayed Particle,
	// which is same as operator % (0).
	// set the timestamp.
	Particle& put(double stamp);
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
protected:
	// It is said "complete" if this input affects all outputs
	// immediately. Valid definition for functional stars.
	// The default value is "complete" (TRUE).
	// If it is not complete, outputs listed in the "triggerList"
	// structure are examined for determining the depth of the
	// porthole.
	int complete;

	// If it is not complete, or it does not affect all outputs,
	// it keeps the list of the (possibly-)affected outputs.
	SequentialList *triggerList;

	// If there are simultaneous events for a star, this port should
	// have higher priority than "beforeP" porthole. (default = NULL)
	GenericPort* beforeP;

public:
        int isItInput () const {return TRUE; }
 
        // Add a new physical port to the MultiPortHole list
        PortHole& newPort();


	// add an output to "triggerList" list.
	void triggers (GenericPort& op);
	void triggers ()	{ complete = FALSE; }

	// set "beforeP" member
	void before(GenericPort& gp) { beforeP = &gp; }

	// constructor
	MultiInDEPort() : complete(TRUE), triggerList(0), beforeP(0) {}

	// destructor
	~MultiInDEPort();
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
	InDEMPHIter(MultiInDEPort& mph) : MPHIter(mph) {}
	InDEPort* next() { return (InDEPort*) MPHIter::next();}
	InDEPort* operator++(POSTFIX_OP) { return next();}
};

class OutDEMPHIter : public MPHIter {
public:
	OutDEMPHIter(MultiOutDEPort& mph) : MPHIter(mph) {}
	OutDEPort* next() { return (OutDEPort*) MPHIter::next();}
	OutDEPort* operator++(POSTFIX_OP) { return next();}
};

#endif
