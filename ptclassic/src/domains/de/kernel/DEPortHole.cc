static const char file_id[] = "DEPortHole.cc";
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 5/30/90
 Revisions: If the galaxy inside a Wormhole has a output data, stop the
	DE scheduler if "stopAfterOutput" flag is set(10/24/90).

Code for functions declared in DEPortHole.h

**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "DEPortHole.h"
#include "EventHorizon.h"
#include "DEScheduler.h"
#include "Block.h"
#include "Plasma.h"
#include "StringList.h"
#include "DEStar.h"
#include "PriorityQueue.h"
#include "CircularBuffer.h"

/**********************************************************

 Member functions for DEPortHoles..

 **********************************************************/

// constructor
DEPortHole :: DEPortHole() : timeStamp(0.0), dataNew(FALSE), depth(-1) {}

// destructor
// (does nothing extra, but avoids out-of-line versions from cfront)
DEPortHole :: ~DEPortHole() {}

PortHole& DEPortHole :: setPort (
			     const char* s,
                             Block* parent,
                             DataType t)
{
	// Initialize PortHole
        PortHole::setPort(s,parent,t);

	// Initialize the DE-specific members
	timeStamp = 0.0;
	dataNew = FALSE;
	depth = -1;

        return *this;
}

Particle& InDEPort :: get() 
{
// Return an input Particle
	if ((!inQue) || dataNew == FALSE) {
		dataNew = FALSE;
		return *(Particle *) *(myBuffer->here());
	} else {
		Particle** myP = myBuffer->next();
		(*myP)->die();
		Particle* p = (Particle*) inQue->get();
		*myP = p; 
		if (inQue->length() == 0)
			dataNew = FALSE;
		return (**myP);
	}
}

void InDEPort :: getSimulEvent()
{
        if (moreData > 1) {
                DEScheduler* sched = (DEScheduler*) parent()->scheduler();
		int store = moreData;
		moreData = 0;
                if (sched->fetchEvent(this, timeStamp)) {
			// offset the increase from "receiveData()" method.
			moreData = store - 1;
		} else {
			moreData = 0;
			dataNew = FALSE;
		}
        } else if (!inQue) {
                dataNew = FALSE;
        }
}

int InDEPort :: getFromQueue (Particle* p)
{
	// get an event from the global queue into either porthole or
	// inQue depending on whether inQue exists or not.
	// inQue is created when mode of the DEstar is set to "PHASE".
	timeStamp = ((DEStar*) parent())->arrivalTime;
	dataNew = TRUE;
	if (!inQue) {
		if (moreData) {
			moreData++;
			return FALSE;
		}
		Particle** myP = myBuffer->next();
		(*myP)->die();
		*myP = p; 
		moreData++;
	} else {
		inQue->put(p);
	} 
	return TRUE;
}

// clean it for the next firing
void InDEPort :: cleanIt()
{
	if (!inQue) moreData = 0;
	else {
		while (inQue->length() > 0) {
			Particle* p = (Particle*)inQue->get();
			p->die();
		}
	}
}

void InDEPort :: triggers (GenericPort& op)
{
	if (!triggerList) {
		complete = FALSE;
		LOG_NEW; triggerList = new SequentialList;
	}
	if (triggerList) triggerList->put(&op);
}

// destructor
InDEPort :: ~InDEPort () {
	cleanIt();
	LOG_DEL; delete inQue;
	// don't kill the triggerlist if I belong to an MPH!
	if (!getMyMultiPortHole()) {
		LOG_DEL; delete triggerList;
	}
}

Particle& OutDEPort :: put(double stamp)
{
// if there is a new Particle, send it
	if (dataNew) sendData();

// Return a current Particle 
	dataNew = TRUE;
	timeStamp = stamp;
	return **(myBuffer->here());
}	

void OutDEPort :: sendData ()
{
// It sends a data to the global eventQ with timeStamp.
// inside the DE system. At the DEin##_WormHole boundary, no event is signaled.
   if (dataNew) {

	// get the Particle.
	Particle** p = myBuffer->here();
	Particle* pp = myPlasma->get();
	*pp = **p;

	// put to the event queue.
	DEStar* dp = (DEStar*) parent();

	// determine the level of the event.
	int level;
	// If the port lies on the Wormhole boundary, inform timeStamp.
	if (farSidePort->isItOutput()) {
		EventHorizon* q = farSidePort->asEH();
		DEScheduler* sr = (DEScheduler*) parent()->scheduler();
		q->setTimeMark(timeStamp / sr->relTimeScale);
		level = -1;
	} else {
		level = ((InDEPort*) farSidePort)->depth;
	}

	if (dp->delayType)
		dp->eventQ->pushTail(pp, farSidePort, timeStamp, level);
	else
		dp->eventQ->pushHead(pp, farSidePort, timeStamp, level);

	
	dataNew = FALSE;
   }
}	

MultiInDEPort :: ~MultiInDEPort ()
{
	LOG_DEL; delete triggerList;
}

void MultiInDEPort :: triggers (GenericPort& op)
{
	if (!triggerList) {
		complete = FALSE;
		LOG_NEW; triggerList = new SequentialList;
	}
	if (triggerList) triggerList->put(&op);
}

PortHole& MultiInDEPort :: newPort () {
	LOG_NEW; InDEPort& p = *new InDEPort;
	// DE-specific
	p.complete = complete;
	p.triggerList = triggerList;
	p.beforeP = beforeP;
	return installPort(p);
}
 
 
PortHole& MultiOutDEPort :: newPort () {
	LOG_NEW; OutDEPort& p = *new OutDEPort;
	return installPort(p);
}
