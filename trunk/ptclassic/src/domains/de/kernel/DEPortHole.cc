/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 5/30/90
 Revisions: If the galaxy inside a Wormhole has a output data, stop the
	DE scheduler if "stopAfterOutput" flag is set(10/24/90).

Code for functions declared in DEConnect.h

**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "DEConnect.h"
#include "DEScheduler.h"
#include "Block.h"
#include "Plasma.h"
#include "StringList.h"
#include "Output.h"
#include "DEStar.h"
#include "PriorityQueue.h"
#include "DEWormConnect.h"
#include "CircularBuffer.h"
 
/**********************************************************

 Member functions for DEPortHoles..

 **********************************************************/

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
		myPlasma->put(*myP);
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
                DEScheduler* sched = (DEScheduler*) parent()->mySched();
		int store = moreData;
		moreData = 0;
                if (sched->fetchEvent(this, timeStamp)) {
			// offset the increase from "grabData()" method.
			moreData = store - 1;
		} else {
			moreData = 0;
			dataNew = FALSE;
		}
        } else if (!inQue) {
                dataNew = FALSE;
        }
}

int InDEPort :: grabData (Particle* p)
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
		myPlasma->put(*myP);
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
			Particle* p = inQue->get();
			myPlasma->put(p);
		}
	}
}

void InDEPort :: triggers (GenericPort& op)
{
	if (!triggerList) {
		complete = FALSE;
		triggerList = new SequentialList;
	}
	if (triggerList) triggerList->put(&op);
}

Particle& OutDEPort :: put(float stamp)
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
		EventHorizon* q = (EventHorizon *) farSidePort;
		DEScheduler* sr = (DEScheduler*) parent()->mySched();
		q->timeStamp = timeStamp / sr->relTimeScale;
		level = -1;
	} else {
		if (farSidePort->parent()->isItWormhole()) 
			level = ((DEtoUniversal*) farSidePort)->depth;
		else
			level = ((InDEPort*) farSidePort)->depth;
	}

	if (dp->delayType)
		dp->eventQ->pushTail(pp, farSidePort, timeStamp, level);
	else
		dp->eventQ->pushHead(pp, farSidePort, timeStamp, level);

	
	dataNew = FALSE;
   }
}	

void MultiInDEPort :: triggers (GenericPort& op)
{
	if (!triggerList) {
		complete = FALSE;
		triggerList = new SequentialList;
	}
	if (triggerList) triggerList->put(&op);
}

PortHole& MultiInDEPort :: newPort () {
	InDEPort& p = *new InDEPort;
	// DE-specific
	p.dataNew = FALSE;
	p.timeStamp = 0.0;
	p.depth = -1;
	p.complete = complete;
	p.triggerList = triggerList;
	p.beforeP = beforeP;
	return installPort(p);
}
 
 
PortHole& MultiOutDEPort :: newPort () {
	OutDEPort& p = *new OutDEPort;
	// DE-specific
	p.dataNew = FALSE;
	p.timeStamp = 0.0;
	p.depth = -1;
	return installPort(p);
}

/**********************************************************

 Member functions for EventQueue..

 **********************************************************/

Event* EventQueue:: getEvent(Particle* p, PortHole* ph) {
	Event* temp;
	// a free event instance
	if (freeEventHead) {
		temp = freeEventHead;
		freeEventHead  = temp->next;
	} else {
		temp = new Event;
	}
	// set the event members.
	temp->dest = ph;
	temp->p = p;
	return temp;
}

void EventQueue:: clearFreeEvents() {
	if (!freeEventHead) return;
	Event* temp;
	while (freeEventHead->next) {
		temp = freeEventHead;
		freeEventHead = freeEventHead->next;
		delete temp;
	}
	delete freeEventHead;
}

void EventQueue:: putFreeLink(LevelLink* p) {
	if (p->fineLevel) {
		Event* temp = (Event*) p->e;
		putEvent(temp);
	}
	PriorityQueue:: putFreeLink(p);
}
