#include "type.h"
#include "DDFScheduler.h"
#include "Output.h"
#include "StringList.h"
#include "IntState.h"
#include "FloatState.h"
#include "Geodesic.h"
#include "GalIter.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 8/9/90
 Revisions:

These are the methods for the dynamic dataflow scheduler.

**************************************************************************/

extern const char DDFdomainName[];

void fireSource(Star&, int);

/*******************************************************************
		Main DDF scheduler routines
*******************************************************************/

int isSource(const Star& s) {
	BlockPortIter nextp(s);
	PortHole* p;
	int flag = 0;

	while ((p = nextp++) != 0) {
		if (p->isItInput()) {
			if ((p->myGeodesic->numInitialParticles))
				return TRUE;
			else flag = 1;
		}
	}
	if (flag) return FALSE;
	return TRUE;
}

	////////////////////////////
	// displaySchedule
	////////////////////////////

StringList DDFScheduler :: displaySchedule () {
	return "DDF schedule is computed at run-time\n";
}

extern int warnIfNotConnected (Galaxy&);

	////////////////////////////
	// setup
	////////////////////////////

int DDFScheduler :: setup (Block& b) {
	haltRequestFlag = 0;

	Galaxy& galaxy = b.asGalaxy();

	GalStarIter nextStar(galaxy);

	// initialize sourceBlocks list
	sourceBlocks.initialize();
	numFiring = 0;

	// check connectivity
	if (warnIfNotConnected (galaxy)) {
		return FALSE;
	}

	// star initialize.  We allow any domain ending in "DF"
	// (for example, SDF, DDF).  Also compute the size.
	Star* s;
	galSize = 0;
	while ((s = nextStar++) != 0) {
		const char* dom = s->domain();
		int l = strlen(dom) - 2;
		if (strcmp(dom + l, "DF") != 0) {
			Error::abortRun (*s, " is not a DDF or SDF star");
			return FALSE;
		}
		// member initialization.
		s->prepareForScheduling();

		// put the source block into the sourceBlocks list.
		if (isSource(*s))
			sourceBlocks.put(s);
		galSize++;
	}

	galaxy.initialize();

	// If user gives the option of numOverlapped, set it.
	IntState* ist = (IntState*) galaxy.stateWithName("numOverlapped");
	if (ist) numOverlapped = (int) (*ist);

	// If user gives the option of schedulePeriod, set it.
	FloatState* fst = (FloatState*) galaxy.stateWithName("schedulePeriod");
	if (fst) schedulePeriod = float ((double) *fst);

	currentTime = schedulePeriod;

	return !haltRequestFlag;
}

	////////////////////////////
	// run
	////////////////////////////

// Check through the block list of the galaxy. If meet a runnable star,
// run it. Repeat  until there is no runnable star over one complete scan.
// Later, we will compare the performance of list-scan with that of 
// queue-management scheme which requires lots of memory (de/)allocations.
//
// EXPLICIT STOPPING CONDITION : # of firing of all sources - 
// default stopTime = 0
int
DDFScheduler :: run (Block& block) {

	if (haltRequestFlag) {
		Error::abortRun(block, ": Can't continue after run-time error");
		return FALSE;
	}

	Galaxy& galaxy = block.asGalaxy();

	// initialize the SpaceWalk member
	GalStarIter nextStar(galaxy);

	int scanSize = galSize;		// how many not-runnable stars scanned

	while (numFiring < stopTime && !haltRequestFlag) {

	   int numStop;
	   
	   // turn on the option of "overlapped execution"
	   if (numFiring + int(numOverlapped) >= stopTime) 
		numStop = stopTime - numFiring;
	   else		
		numStop = int(numOverlapped);

	   // fire source blocks...
	   numFiring += numStop;
	   ListIter nextb(sourceBlocks);
	   for (int i = sourceBlocks.size(); i > 0; i--) {
		Star& s = *(Star*)nextb++;
		fireSource(s, numStop);
		if (haltRequestFlag) break;
	   }

	   scanSize = galSize;
   
	   while (scanSize) {

		// look at the next star in the block-list
		Star* s = nextStar++;
		if (!s) {
			nextStar.reset();
			s = nextStar++;
		}
		
		// check the star is runnable
		// run it as many times as we can.
		if (isRunnable(*s)) {
			// run the star
			do {
				s->fire();
				if (haltRequestFlag) exit(1);
			} while (isRunnable(*s));

			// reset scanSize
			scanSize = galSize;
		} else {
			scanSize--;
		}

	   } /* inside while */

	   currentTime += schedulePeriod;

	} /* outside while */
	return TRUE;
}

int DDFScheduler :: isRunnable(Star& s) {

	// if source, return FALSE
	if (isSource(s)) return FALSE;

	// check the domain of the star
	// if DDFStar...

	if (strcmp (s.domain(), DDFdomainName) == 0) {

		// if user specified waitPort exists...
		DDFStar* p = (DDFStar*) &s;
		if (p->waitPort != NULL) {
			int req = p->waitNum - p->waitPort->numTokens();
			while (req > 0) {
			   // if # tokens are static, return FASE.
			   // if on wormhole boundary, return FASE.
			   DDFPortHole* po = (DDFPortHole*) p->waitPort;
			   if (po->isDynamic() && po->far()->isItOutput()) {
				// if dynamic, enable lazy evaluation.
				if (!lazyEval((Star*) po->far()->parent()))
					return FALSE;
			   } else
					return FALSE;
			   req = p->waitNum - p->waitPort->numTokens();
			}
			return TRUE;
		}
	}

	// if all input ports have no data, return FALSE
	int count = 0;
	BlockPortIter nextp(s);
	PortHole *p;
	while ((p = nextp++) != 0) {
		if (p->isItInput() && p->numTokens() > 0)
			count++;
	}
	if (count == 0)		
		return FALSE;

	// if SDFStar, or no-specified waitPort for DDFStar,....

	return (checkLazyEval(&s));
}
	

int DDFScheduler :: lazyEval(Star* s) {

	if (strcmp (s->domain(), DDFdomainName) == 0) {

		// if user specified waitPort exists...
		DDFStar* p = (DDFStar*) s;
		if (p->waitPort != NULL) {
			int req = p->waitNum - p->waitPort->numTokens();
			DDFPortHole* po = (DDFPortHole*) p->waitPort;
			while (req > 0) {
			   // if on wormhole boundary, return FASE.
			   if (po->far()->isItOutput()) {
				if (!lazyEval((Star*) po->far()->parent()))
					return FALSE;
			   } else
					return FALSE;
			   req = p->waitNum - p->waitPort->numTokens();
			}
			s->fire();
			return TRUE;
		}
	}

	if (checkLazyEval(s)) {
		// fire the star
		s->fire();
		return TRUE;
	} else
		return FALSE;
}
	
int DDFScheduler :: checkLazyEval(Star* s) {
	BlockPortIter nextp(*s);
	for (int i = s->numberPorts(); i > 0; i--) {
		
		// look at the next port
		PortHole& p = *nextp++;
		if (p.isItInput()) {
			int req = p.numberTokens - p.numTokens();
			if ((req > 0) && 
				// check wormhole, recursive star
				(p.far()->isItInput()))
				 return FALSE;

			Star* str = (Star*) p.far()->parent();
			if (strcmp (str->domain(), DDFdomainName) == 0) {
				// check dynamic port for ddf star.
			   	DDFPortHole* po = (DDFPortHole*) p.far();
			   	if (req > 0 && po->isDynamic())
					return FALSE;
			}

			while (req > 0) {
				if (!lazyEval(str))
					return FALSE;
				req = p.numberTokens - p.numTokens();
			}
		}
	}

	return TRUE;
}

void fireSource(Star& s, int k) {
			
	// check how many unused tokens are on output arcs.
	int min = 10000;		// large enough number
	int minIn = 1000;
	BlockPortIter nextp(s);
	for (int j = s.numberPorts(); j > 0; j--) {
		PortHole& port = *nextp++;
		Geodesic* g = port.myGeodesic;

	  	if (port.numberTokens == 0) {
			Error::abortRun(s,
			      ": output port has undefined number of tokens");
			return;
		}
		if (port.isItOutput()) {
			int r = (g->size())/port.numberTokens;
			if (r < min) min = r;
		} else {
			int k = port.numTokens() / port.numberTokens;
			if (minIn > k) minIn = k;
		}
			
	}

	// fire sources "k-min" times.
	if (minIn < min) min = minIn;
	for (int i = min; i < k; i++) 
		s.fire();
}

// my domain
extern const char DDFdomainName[];

const char* DDFScheduler :: domain () const { return DDFdomainName ;}
