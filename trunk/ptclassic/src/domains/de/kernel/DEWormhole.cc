static const char file_id[] = "DEWormhole.cc";
/******************************************************************
Version identification:
 $Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: Soonhoi Ha 
 Date of creation: 6/15/90
 Revision : set up the flag (delayType) depending on the scheduler in start().

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "DEWormhole.h"
#include "DEScheduler.h"
#include "PriorityQueue.h"
#include "CircularBuffer.h"
#include "Geodesic.h"
#include "Plasma.h"
 

/*******************************************************************

	class DEWormhole methods

********************************************************************/

void DEWormhole :: start()
{
	Wormhole :: setup();
	delayType = FALSE;
}
	
void DEWormhole :: go()
{
	// synchronize the two domains
	target->setCurrentTime(arrivalTime);

	// run the inner scheduler.
	Wormhole::run();
}

void DEWormhole :: wrapup()
{
	target->wrapup();
}

// new phase
void DEWormhole :: startNewPhase() 
{
        BlockPortIter next(*this);
        PortHole* p;
        while ((p = next++) != 0) {
                if (p->isItInput()) {
                        InDEPort* inp = (InDEPort*) p;
                        inp->cleanIt();
                }
        }
}

// cloner -- clone the inside and make a new wormhole from that.
Block* DEWormhole :: clone() const
{
LOG_NEW; return new DEWormhole(gal.clone()->asGalaxy(), target->cloneTarget());
}

// sumUp();  If the inner domain is timed and stopBeforeDeadlocked,
// put the wormhole into the process queue.
void DEWormhole :: sumUp() {
	if (scheduler()->stopBeforeDeadlocked) {
		DEScheduler* sched = (DEScheduler*) parent()->scheduler();
		sched->eventQ.levelput(&(this->selfStar), 
			scheduler()->currentTime ,0);
	}
}
		
// getStopTime() ; gives the stopTime to the inner domain.
// If syncMode is set (in DEScheduler), return the currentTime meaning that
//	the global clock of the inner domain marches with that in DE domain.
// Otherwise, execute the inner domain until it is deadlocked by returning
//	the stopTime of the DEScheduler.

double DEWormhole :: getStopTime() {
	DEScheduler* sched = (DEScheduler*) parent()->scheduler();
	if (sched->syncMode) {
		return sched->currentTime;
	} else {
		return sched->whenStop();
	}
}

/**************************************************************************

	methods for DEtoUniversal

**************************************************************************/

void DEtoUniversal :: receiveData()
{
	// 1. get data.
	getData();

	// 2. time stamp is set from the inside.

	// 3. transfer data
	transferData();
}

int DEtoUniversal :: getFromQueue(Particle* p) {
	if (DEtoUniversal :: isItInput()) {
		if (moreData) return FALSE;

		// get Particle
		Particle** myP = myBuffer->next();
		myPlasma->put(*myP);
		*myP = p;

		// set the data members
		moreData++;
		tokenNew = TRUE;
		timeMark = ((DEStar*) parent())->arrivalTime;
	
		// transfer data
		transferData();
	} else {
		myGeodesic->put(p);
	}
	return TRUE;
}

void DEtoUniversal :: cleanIt() { moreData = 0; }

void DEtoUniversal :: initialize() {
        InDEPort:: initialize();
        ToEventHorizon :: initialize();
}

int DEtoUniversal :: isItInput() const 
	{ return EventHorizon :: isItInput(); }
int DEtoUniversal :: isItOutput() const 
	{ return EventHorizon :: isItOutput(); }

EventHorizon* DEtoUniversal :: asEH() { return this; }

/**************************************************************************

	methods for DEfromUniversal

**************************************************************************/

void DEfromUniversal :: sendData ()
{
// It generates an event into the event queue in the DE world.

	// 1. transfer the data from the ghost Port.
	transferData();

	// check if there is new event arrived.
   	while (tokenNew) {

		DEScheduler* s;
	
		// 2. copy the timeMark from ghostPort if it is an input.
		if (DEfromUniversal :: isItInput()) {
			s = (DEScheduler*) parent()->scheduler();
			timeMark = ghostPort->getTimeMark() * s->relTimeScale;
		} else {
			s = (DEScheduler*) parent()->parent()->scheduler();
			timeMark = s->currentTime;
		}

		// 3. Find out the fineLevel of the event
		int level;
        	// If the port lies on the Wormhole boundary, inform timeMark.
        	if (farSidePort->isItOutput()) {
                	EventHorizon* q = farSidePort->asEH();
                	q->setTimeMark(timeMark / s->relTimeScale);
                	level = -1;
        	} else {
                        level = ((InDEPort*) farSidePort)->depth;
        	}

		// send the event
		for (int k = numberTokens; k > 0; k--) {
			// 3. get the particle
			Particle** p = myBuffer->here();
			Particle* pp = myPlasma->get();
			*pp = **p;

		   	s->eventQ.pushHead(pp, far(), timeMark, level);
		}
	
		// 5. repeat as long as new data exists.
		tokenNew = FALSE;
   		transferData();
   	}

}

void DEfromUniversal :: initialize() {
	OutDEPort :: initialize();
	FromEventHorizon :: initialize();
}

int DEfromUniversal :: isItInput() const 
	{ return EventHorizon :: isItInput(); }
int DEfromUniversal :: isItOutput() const 
	{ return EventHorizon :: isItOutput(); }

EventHorizon* DEfromUniversal :: asEH() { return this; }

