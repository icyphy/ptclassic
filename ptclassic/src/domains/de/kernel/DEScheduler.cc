#include "type.h"
#include "DEScheduler.h"
#include "Output.h"
#include "StringList.h"
#include "FloatState.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 5/30/90
 Revisions:
	6/20/90 -- no longer call initialize() and start() on each
	Star.  Galaxy::initialize does this now.

These are the methods for the discrete event scheduler.

**************************************************************************/

extern Error errorHandler;

extern const char DEdomainName[];

/*******************************************************************
		Main DE scheduler routines
*******************************************************************/


	////////////////////////////
	// displaySchedule
	////////////////////////////

StringList DEScheduler :: displaySchedule () {
	return "DE schedule is computed at run-time\n";
}


extern StringList checkConnect (Galaxy&);

	////////////////////////////
	// setup
	////////////////////////////

int DEScheduler :: setup (Block& b) {
	haltRequestFlag = 0;

	Galaxy& galaxy = b.asGalaxy();
	
	// initialize the SpaceWalk member
	alanShepard.setupSpaceWalk(galaxy);

	// initialize the global event queue and process queue.
	eventQ.initialize();
	processQ.initialize();

	// check connectivity
	StringList msg = checkConnect (galaxy);
	if (msg.size() > 0) {
		errorHandler.error (msg);
		return FALSE;
	}

	// Notify each star of the global event queue, and fire source
	// stars to initialize the global event queue.
	for (int i = alanShepard.totalSize(galaxy); i>0; i--) {
		Star& s = alanShepard.nextStar();
		if (strcmp (s.domain(), DEdomainName) != 0) {
			StringList msg = s.readFullName();
			msg += " is not a DE star: domain = ";
			msg += s.domain();
			errorHandler.error(msg);
			return FALSE;
		}
		// set up the block's event queue.
		DEStar* p = (DEStar*) &s;
		p->eventQ = &eventQ;

		// reset data members
		p->prepareForScheduling();
	}

	galaxy.initialize();
	
	// set the depth of the stars...
	alanShepard.setupSpaceWalk(galaxy);
	for (i = alanShepard.totalSize(galaxy); i>0; i--) {
		Star& s = alanShepard.nextStar();
		DEStar* ds = (DEStar*) &s;
		setDepth(ds);
	}

	// set the relative time scale.
	FloatState* st = (FloatState*) galaxy.stateWithName("timeScale");
	if (st)	relTimeScale = float ((double) (*st));
	else	relTimeScale = 1.0;
	
	if (!relTimeScale) {
		errorHandler.error("zero timeScale is not allowed in DE.");
		return FALSE;
	}

	return !haltRequestFlag;
}

	////////////////////////////
	// run
	////////////////////////////

// pop the top element (earliest event) from the global queue
// and fire the destination star. Check all simultaneous events to the star.
// Run until StopTime
int
DEScheduler :: run (Block& galaxy) {
	if (haltRequestFlag) {
		errorHandler.error ("Can't continue after run-time error");
		return FALSE;
	}
	while (eventQ.length() > 0 && !haltRequestFlag) {

		// fetch the earliest event.
		LevelLink* f   = eventQ.get();
		float level      = f->level;
		DEPortHole* terminal = (DEPortHole*) f->e;

		// if level > stopTime, RETURN...
		if (level > stopTime)	{
			eventQ.pushBack(f);		// push back
			currentTime = stopTime;
			stopBeforeDeadlocked = TRUE;  // there is extra events.
			return 0;
		}

		// set currentTime
		currentTime = level;

		DEStar* s = (DEStar*) &terminal->parent()->asStar();

		// Grab the Particle from the geodesic to the terminal.
		// Record the arrival time, and flag existence of data.
		// We may require more than one events on an arc such as
		// SDFinDEWormholes. Then, wait...
		if (terminal->numTokens() < terminal->numberTokens)
			continue;
		s->arrivalTime = level;
		terminal->grabData();
	
		// Check if there is another event launching onto the
		// same star with the same time stamp (not the same port)...
		int l;
		eventQ.reset();
		l = eventQ.length();
		DEStar* dest;
		while (l > 0) {
			l--;
			LevelLink* h = eventQ.next();
			DEPortHole* tl = (DEPortHole*) h->e;
			dest = (DEStar *) &tl->parent()->asStar();

			// if same destination star with same time stamp..
			if (level == h->level && dest == s && tl != terminal) {
				eventQ.extract(h);
				if (tl->numTokens() >= tl->numberTokens)
					tl->grabData();
			} else if (h->level > level)
				goto L1;

		} // end of inside while

 		// fire the star.
 L1 :		s->fire();

		if (haltRequestFlag) return FALSE;

		// If the nextTime is greater than the currentTime,
		// execute the processes stored in the processQ.
		if (processQ.length() > 0 && nextTime() > currentTime) {
		   int temp = 1;
		   do {
			// fetch the earliest process.
			LevelLink* pf   = processQ.get();
			float stamp      = pf->level;
			DEStar* sr = (DEStar*) f->e;
			// enforce that fire the process star at last at
			// the currentTime.
			if (nextTime() > stamp) {
				currentTime = stamp;
				sr->fire();
			} else {
				processQ.pushBack(pf);	// push back
				temp = 0;
			}
		    } while ( temp && processQ.length() > 0);
		}

	} // end of while

	stopBeforeDeadlocked = FALSE;	// yes, no more events...
	return TRUE;
}


	////////////////////////////
	// Miscellanies
	////////////////////////////

int DEScheduler :: amITimed() {return TRUE ;}

float DEScheduler :: nextTime() {
        float val = stopTime;
        eventQ.reset();
        if (eventQ.length() > 0) {
                val = eventQ.next()->level;
        }
        return (val < stopTime)? val : stopTime ; // return min(val, stopTime)
}

// calculate the depth of a DEStar.
int DEScheduler :: setDepth(DEStar* s) {
	
	// 1. Check if the depth is already set, then return immediately.
	if (s->depth < 0) return s->depth;

	// 2. If it is a delay-type star, set depth = -1;
	if (s->delayType == TRUE)	{
		s->depth = -1;
	} else {

		// 3. indicate that this star is under consideration.
		//    By depth > 0, we can detect the delay-free loop (error)
		s->depth = 1;

		// 4. scan through all output connections. Get the minimum
		//    depth to set its depth.
		int min = 0;
		for (int i = s->numberPorts(); i > 0; i--) {
			PortHole& port = s->nextPort();

			// 5. check whether it is on the wormhole boundary.
			//    and check it is on the feedback arc.
			if (port.isItOutput() && port.far()->parent() != 
						 port.parent()) {
			   int val;
			   if (port.far()->isItOutput()) {
				val = 0;
			   } else {
				DEStar* dp = (DEStar*) port.far()->parent();

				// 6. detect the delay-free loop.
				if (dp->depth > 0) {
					StringList msg = "Warning :: ";
					msg += dp->readFullName();
					msg += " lies on a delay-free loop";
					errorHandler.error(msg);
					val = 0;
				} else {
					// 7. recursive call.
					val = setDepth(dp);
				}
			   }
			   if (min >= val) min = val - 1;
			}
		}
		
		// 5. check whether there is no output.
		if (min >= 0)   s->depth = -1;
		else		s->depth = min;
	}
	return s->depth;
}
			
