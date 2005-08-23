static const char file_id[] = "DEWormhole.cc";
/******************************************************************
Version identification:
@(#)DEWormhole.cc	2.29 02/22/99

Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

 Programmer: Soonhoi Ha 
 Date of creation: 6/15/90
 Revision : set up the flag (delayType) depending on the scheduler in start().

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "DETarget.h"
#include "DEWormhole.h"
#include "DEScheduler.h"
#include "PriorityQueue.h"
#include "CircularBuffer.h"
#include "Geodesic.h"
#include "Plasma.h"
 

/*******************************************************************

	class DEWormhole methods

********************************************************************/
 
void DEWormhole :: begin()
{
        //Running default behaviour for begin
        Wormhole::begin();
 
	//Checks if the wormhole requires selfFiring. If it does, then the
        // wormhole is fired for the first time now.
        if (myTarget()->selfFiringRequested()) {
                DEBaseSched* sched = (DEBaseSched*) outerSched();
                DEStar* me = this;
                sched->queue()->levelput(me, sched->now(), 0);
        }
}

void DEWormhole :: setup()
{
	Wormhole :: setup();
	delayType = FALSE;
}
	
void DEWormhole :: go()
{
	// synchronize the two domains
	myTarget()->setCurrentTime(arrivalTime);

	// run the inner scheduler.
	Wormhole::run();

	// Checking if the inner domain requests self-firing. If it does,
        // then it is setup for the next firing.
        if (myTarget()->selfFiringRequested()) {
                DEBaseSched* sched = (DEBaseSched*) outerSched();
                DEStar* me = this;
                sched->queue()->levelput(me, myTarget()->nextFiringTime(), 0);
        }
 }

void DEWormhole :: wrapup()
{
	myTarget()->wrapup();
}

// cloner -- clone the inside and make a new wormhole from that.
Block* DEWormhole :: clone() const
{
LOG_NEW; return new DEWormhole(gal.clone()->asGalaxy(), myTarget()->cloneTarget());
}

// makeNew, analogous to clone.
Block* DEWormhole :: makeNew() const
{
LOG_NEW; return new DEWormhole(gal.makeNew()->asGalaxy(), myTarget()->cloneTarget());
}

// sumUp();  If the inner domain is timed and stopBeforeDeadlocked,
// put the wormhole into the process queue.
void DEWormhole :: sumUp() {
/*
// Instead of here, use selfFiringRequested() and nextFiringTime() to refire
	if (scheduler()->stopBeforeDeadlocked()) {
		DEBaseSched* sched = (DEBaseSched*) outerSched();
		DEStar* me = this;
		sched->queue()->levelput(me, sched->now(), 0);
	}
*/
}
		
// getStopTime() ; gives the stopTime to the inner domain.
// If syncMode is set (in DEBaseSched), return the currentTime meaning that
//	the global clock of the inner domain marches with that in DE domain.
// Otherwise, execute the inner domain until it is deadlocked by returning
//	the stopTime of the DEBaseSched.

double DEWormhole :: getStopTime() {
	DEBaseSched* sched = (DEBaseSched*) outerSched();
	if (sched->syncMode) {
		return sched->now();
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
		// first return existing particle to plasma
		(*myP)->die();
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
        PortHole :: initialize();
        ToEventHorizon :: initialize();
}

int DEtoUniversal :: isItInput() const 
	{ return EventHorizon :: isItInput(); }
int DEtoUniversal :: isItOutput() const 
	{ return EventHorizon :: isItOutput(); }
int DEtoUniversal :: onlyOne() const { return TRUE; }
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

		DEBaseSched* s;
	
		// 2. copy the timeMark from ghostPort if it is an input.
		if (DEfromUniversal :: isItInput()) {
			s = (DEBaseSched*) innerSched();
			timeMark = ghostPort->getTimeMark() * s->relTimeScale;
		} else {
			s = (DEBaseSched*) outerSched();
			timeMark = s->now();
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
		myBuffer->backup(numberTokens);
		for (int k = numberTokens; k > 0; k--) {
			// 3. get the particle
			Particle** p = myBuffer->next();
			Particle* pp = myPlasma->get();
			*pp = **p;

		   	s->queue()->pushHead(pp, far(), timeMark, level);
		}
	
		// 5. repeat as long as new data exists.
		tokenNew = FALSE;
   		transferData();
   	}

}

void DEfromUniversal :: initialize() {
	PortHole :: initialize();
	FromEventHorizon :: initialize();
}

int DEfromUniversal :: isItInput() const 
	{ return EventHorizon :: isItInput(); }
int DEfromUniversal :: isItOutput() const 
	{ return EventHorizon :: isItOutput(); }
int DEfromUniversal :: onlyOne() const { return TRUE; }
EventHorizon* DEfromUniversal :: asEH() { return this; }

