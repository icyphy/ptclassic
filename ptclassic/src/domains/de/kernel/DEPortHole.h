#ifndef _DEconnect_h
#define _DEconnect_h 1

#include "NamedObj.h"
#include "DataStruct.h"
#include "dataType.h"
#include "type.h"
#include "Connect.h"
#include "PriorityQueue.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 5/30/90
 Revisions:

This file contains definitions of DE PortHoles.
Two DE-specific struct/classes are defined.
	Event : entry for the global event queue.
		consists of destination PortHole and Particle.
	EventQueue : derived from PriorityQueue.

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

	// number of simultaneous events on the same arc - 1.
	int moreData;

	// If there are simultaneous events for a star, this port should
	// have higher priority than "beforeP" porthole. (default = NULL)
	GenericPort* beforeP;

	// If it is not complete, or it does not affect all outputs,
	// it keeps the list of the (possibly-)affected outputs.
	SequentialList *triggerList;

public:
	int isItInput () const {return TRUE; }

	// Get particles from global queue and set the timeStamp
	void grabData(Particle* p);

	// operator to return a zero delayed Particle,
	// which is same as operator % (0).
	// reset dataNew = FALSE.
	Particle& get();

	// add an output to "triggerList" list.
	void triggers (GenericPort& op);
	void triggers ()	{ complete = FALSE; }

	// set "beforeP" member. 
	void before(GenericPort& gp) { beforeP = &gp; }

	// check whether there are simultaneous events on the same arc.
	// If yes, fetch the event from the event queue.
	void getSimulEvent();
	int existSimulEvents() {return moreData;}

	// constructor
	InDEPort() : complete(TRUE), triggerList(0), beforeP(0) {}
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

        //////////////////////////////////////////
        // class Event and EventQueue
        //////////////////////////////////////////

class Event
{
public:	
	PortHole* dest;		// destination PortHole
	Particle* p;		// particle or event
	Event*	  next;		// form a event-list

	Event() : next(0) {}
};

class EventQueue : public PriorityQueue
{
	Event*  freeEventHead;
	Event*  getEvent(Particle*, PortHole*);
	void    putEvent(Event* e) {
			e->next = freeEventHead;
			freeEventHead = e;
		}
	void clearFreeEvents();

protected:
	void clearFreeList() {
		clearFreeEvents();
		PriorityQueue :: clearFreeList();
	}
public:
	void pushHead(Particle* p, PortHole* ph, float v, float fv) {
		Event* temp = getEvent(p, ph);
		leveltup(temp, v, fv);
	}

	void pushTail(Particle* p, PortHole* ph, float v, float fv) {
		Event* temp = getEvent(p, ph);
		levelput(temp, v, fv);
	}
	void putFreeLink(LevelLink* p);

	EventQueue() : freeEventHead(0) {}
};

#endif
