#include "type.h"
#include "DEScheduler.h"
#include "Output.h"
#include "StringList.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 5/30/90
 Revisions:

 Modification : implement DE scheduler

**************************************************************************/

extern Error errorHandler;


/*******************************************************************
		Main DE scheduler routines
*******************************************************************/


	////////////////////////////
	// wrapup
	////////////////////////////

int DEScheduler :: wrapup (Block& galaxy) {

	// Run the termination routines of all the atomic stars.
	for (int i = alanShepard.totalSize((Galaxy&)galaxy); i>0; i--)
		((DEStar&)alanShepard.nextBlock()).wrapup();
}

	////////////////////////////
	// displaySchedule
	////////////////////////////

StringList DEScheduler :: displaySchedule (Block& galaxy) {
	return "not yet implemented";
}



	////////////////////////////
	// setup
	////////////////////////////

int DEScheduler :: setup (Block& galaxy) {

	// initialize the SpaceWalk member
	alanShepard.setupSpaceWalk((Galaxy&)galaxy);

	// initialize the global event queue...
	eventQ.initialize();

	// Notify each star of the global event queue, and fire source
	// stars to initialize the global event queue.
	for (int i = alanShepard.totalSize((Galaxy&)galaxy); i>0; i--) {
		// Get the next atomic block
		// and set its global event queue name.
		DEStar& star = (DEStar&)alanShepard.nextBlock();

		star.eventQ = &eventQ;
		star.initialize();
		star.start();
	}
}

	////////////////////////////
	// run
	////////////////////////////

// pop the top element (earliest event) from the global queue
// and fire the destination star. Check all simultaneous events to the star.
// Run until StopTime
int
DEScheduler :: run (Block& galaxy) {

	while (eventQ.length() > 0) {

		// fetch the earliest event.
		LevelLink* f   = eventQ.get();
		float level      = f->level;
		DEPortHole* terminal = (DEPortHole*) f->e;

		// if level > stopTime, RETURN...
		if (level > stopTime)	{
			eventQ.levelput(terminal, level);	// push back
			return ;
		}

		DEStar* s = (DEStar*) terminal->parent();

		// Grab the Particle from the geodesic to the terminal.
		// Record the arrival time, and flag existence of data.
		terminal->grabData();
		terminal->timeStamp = level;
		
		s->arrivalTime = level;
	
		// Check if there is another event launching onto the
		// same star with the same time stamp...
		int l;
		eventQ.reset();
		l = eventQ.length();
		DEStar* dest;
		while (l > 0) {
			l--;
			LevelLink* h = eventQ.next();
			terminal = (DEPortHole*) h->e;
			dest = (DEStar *) terminal->parent();

			// if same destination star with same time stamp..
			if (level == h->level && dest == s) {
				terminal->grabData();
				eventQ.extract(h);
				delete h;
			} else if (h->level > level)
				goto L1;

		} // end of inside while

 		// fire the star.
 L1 :		s->go();

		// generate events after star execution.
  		for (int k = s->numberPorts(); k > 0; k--) {
			DEPortHole& p = (DEPortHole&) s->nextPort();
			p.sendData();
		}

	} // end of while
}
