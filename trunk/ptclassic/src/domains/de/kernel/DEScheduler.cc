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

	// initialize the global event queue...
	eventQ.initialize();

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
		p->arrivalTime = 0.0;
		p->completionTime = 0.0;

	}

	galaxy.initialize();
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
			eventQ.levelput(terminal, level);	// push back
			currentTime = stopTime;
			return 0;
		}

		// set currentTime
		currentTime = level;

		DEStar* s = (DEStar*) &terminal->parent()->asStar();

		// Grab the Particle from the geodesic to the terminal.
		// Record the arrival time, and flag existence of data.
		// We may require more than one events on an arc such as
		// SDFinDEWormholes. Then, wait...
		if (terminal->myGeodesic->size() < terminal->numberTokens)
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
				delete h;
				if (tl->myGeodesic->size() >= tl->numberTokens)
					tl->grabData();
			} else if (h->level > level)
				goto L1;

		} // end of inside while

 		// fire the star.
 L1 :		s->go();
		if (haltRequestFlag) break;

		// generate events after star execution.
  		for (int k = s->numberPorts(); k > 0; k--) {
			DEPortHole& p = (DEPortHole&) s->nextPort();
			p.sendData();
		}

	} // end of while
	return TRUE;
}
