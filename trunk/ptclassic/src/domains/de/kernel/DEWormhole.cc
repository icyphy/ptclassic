/******************************************************************
Version identification:
 $Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: Soonhoi Ha 
 Date of creation: 6/15/90
 Revision : set up the flag (delayType) depending on the scheduler in start().

*******************************************************************/
#include "DEWormhole.h"
#include "DEScheduler.h"

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
	scheduler->setCurrentTime(arrivalTime);

	// run the inner scheduler.
	run();
}

// cloner -- clone the inside and make a new wormhole from that.
Block* DEWormhole :: clone() const
{
	return new DEWormhole(gal.clone()->asGalaxy());
}

// sumUp();  If the inner domain is timed and stopBeforeDeadlocked,
// put the wormhole into the process queue.
void DEWormhole :: sumUp() {
	if (scheduler->stopBeforeDeadlocked) {
		DEScheduler* sched = (DEScheduler*) parent()->mySched();
		sched->eventQ.levelput(&(this->selfStar), 
			scheduler->currentTime ,0);
	}
}
		
// getStopTime() ; gives the stopTime to the inner domain.
// If syncMode is set (in DEScheduler), return the currentTime meaning that
//	the global clock of the inner domain marches with that in DE domain.
// Otherwise, execute the inner domain until it is deadlocked by returning
//	the stopTime of the DEScheduler.

float DEWormhole :: getStopTime() {
	DEScheduler* sched = (DEScheduler*) parent()->mySched();
	if (sched->syncMode) {
		return sched->currentTime;
	} else {
		return sched->whenStop();
	}
}
