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


extern int warnIfNotConnected (Galaxy&);

	////////////////////////////
	// setup
	////////////////////////////

int DEScheduler :: setup (Block& b) {
	haltRequestFlag = 0;
	currentTime = 0;

	Galaxy& galaxy = b.asGalaxy();
	
	// initialize the SpaceWalk member
	alanShepard.setupSpaceWalk(galaxy);

	// initialize the global event queue and process queue.
	eventQ.initialize();

	// check connectivity
	if (warnIfNotConnected (galaxy)) return FALSE;

	// Notify each star of the global event queue, and fire source
	// stars to initialize the global event queue.
	for (int i = alanShepard.totalSize(galaxy); i>0; i--) {
		Star& s = alanShepard.nextStar();
		if (strcmp (s.domain(), DEdomainName) != 0) {
			Error::abortRun (s, " is not a DE star");
			return FALSE;
		}
		// set up the block's event queue.
		DEStar* p = (DEStar*) &s;
		p->eventQ = &eventQ;

		// reset data members
		p->prepareForScheduling();
	}

	// set the depth of the stars...
	alanShepard.setupSpaceWalk(galaxy);
	for (i = alanShepard.totalSize(galaxy); i>0; i--) {
		Star& s = alanShepard.nextStar();
		DEStar* ds = (DEStar*) &s;
		setDepth(ds);
	}

	galaxy.initialize();
	
	// set the relative time scale.
	FloatState* st = (FloatState*) galaxy.stateWithName("timeScale");
	if (st)	relTimeScale = float ((double) (*st));
	else	relTimeScale = 1.0;
	
	// set the synchronization flag
	IntState* ist = (IntState*) galaxy.stateWithName("sycnMode");
	if (ist)	syncMode = int(ist);	
	else		syncMode = TRUE;
	
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
// Run until StopTime.
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

		// if level > stopTime, RETURN...
		if (level > stopTime)	{
			eventQ.pushBack(f);		// push back
			// set currentTime = next event time.
			currentTime = level/relTimeScale;
			stopBeforeDeadlocked = TRUE;  // there is extra events.
			return 0;
		// If the event time is less than the global clock,
		// it is an error...
		} else if (level < currentTime - 0.1) {
			errorHandler.error("global clock is screwed up in DE");
			return FALSE;
		}

		// set currentTime
		currentTime = level;

		// check if the normal event is fetched
		//
		// For normal events, the fineLevel of the LevelLink
		// class is negative (which is the minus of the depth of
		// the destination star)
		// But, we also put the process-star in the event queue
		// with zeroed fineLevel --> which will put the process-stars
		// at the end among simultaneous events as a side-effect.
		DEStar* s;
		DEPortHole* terminal = 0;
		if (f->fineLevel != 0) {
			terminal = (DEPortHole*) f->e;
			s = (DEStar*) &terminal->parent()->asStar();

			// Grab the Particle from the geodesic to the terminal.
			// Record the arrival time, and flag existence of data.
			// We may need more than one events on an arc such as
			// SDFinDEWormholes. Then, wait...
			if (terminal->numTokens() < terminal->numberTokens)
				continue;
			terminal->grabData();
		} else {
		// process star is fetched
			s = (DEStar*) f->e;
		}
		s->arrivalTime = level;
				
		// Check if there is another event launching onto the
		// same star with the same time stamp (not the same port)...
		int l;
		eventQ.reset();
		l = eventQ.length();
		DEStar* dest;
		while (l > 0) {
			l--;
			LevelLink* h = eventQ.next();
			if (h->level > level)	goto L1;
			DEPortHole* tl = 0;
			if (h->fineLevel != 0) {
				tl = (DEPortHole*) h->e;
				dest = (DEStar *) &tl->parent()->asStar();
			} else {
				dest = (DEStar*) h->e;
			}

			// if same destination star with same time stamp..
			if (dest == s) {
				if (tl && tl != terminal) {
				     eventQ.extract(h);
				     if (tl->numTokens() >= tl->numberTokens)
					tl->grabData();
				} else if (!tl) {
				     eventQ.extract(h);
				}
			} 
		} // end of inside while

 		// fire the star.
 L1 :		s->fire();

		if (haltRequestFlag) return FALSE;

	} // end of while

	stopBeforeDeadlocked = FALSE;	// yes, no more events...
	return TRUE;
}


	////////////////////////////
	// Miscellanies
	////////////////////////////

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
