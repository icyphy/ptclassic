#include "type.h"
#include "DDFScheduler.h"
#include "Output.h"
#include "StringList.h"
#include "IntState.h"
#include "FloatState.h"
#include "Geodesic.h"

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

extern Error errorHandler;

extern const char DDFdomainName[];

void fireSource(Star&, int);

/*******************************************************************
		Main DDF scheduler routines
*******************************************************************/

int isSource(Star& s) {
	for (int i = s.numberPorts(); i > 0; i--) {
		PortHole& p = s.nextPort();
		if (p.isItInput()) return FALSE;
	}
	return TRUE;
}

	////////////////////////////
	// displaySchedule
	////////////////////////////

StringList DDFScheduler :: displaySchedule () {
	return "DE schedule is computed at run-time\n";
}


extern StringList checkConnect (Galaxy&);

	////////////////////////////
	// setup
	////////////////////////////

int DDFScheduler :: setup (Block& b) {
	haltRequestFlag = 0;

	Galaxy& galaxy = b.asGalaxy();

	// initialize the SpaceWalk member
	alanShepard.setupSpaceWalk(galaxy);

	// initialize sourceBlocks list
	sourceBlocks.initialize();
	numFiring = 0;

	// check connectivity
	StringList msg = checkConnect (galaxy);
	if (msg.size() > 0) {
		errorHandler.error (msg);
		return FALSE;
	}

	// star initialize.  We allow any domain ending in "DF"
	// (for example, SDF, DDF)
	for (int i = alanShepard.totalSize(galaxy); i>0; i--) {
		Star& s = alanShepard.nextStar();
		const char* dom = s.domain();
		int l = strlen(dom) - 2;
		if (strcmp(dom + l, "DF") != 0) {
			StringList msg = s.readFullName();
			msg += " is not a DDF star: domain = ";
			msg += dom;
		 	errorHandler.error(msg);
			return FALSE;
		}
		// member initialization.
		s.prepareForScheduling();

		// put the source block into the sourceBlocks list.
		if (isSource(s))
			sourceBlocks.put(&s);
		
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
		errorHandler.error("Can't continue after run-time error");
		return FALSE;
	}

	Galaxy& galaxy = block.asGalaxy();

	// initialize the SpaceWalk member
	alanShepard.setupSpaceWalk(galaxy);

	int galSize = alanShepard.totalSize(galaxy);
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
	   sourceBlocks.reset();
	   for (int i = sourceBlocks.size(); i > 0; i--) {
		Star& s = *(Star*)sourceBlocks.next();
		fireSource(s, numStop);
		if (haltRequestFlag) break;
	   }

	   scanSize = galSize;
	   alanShepard.setupSpaceWalk(galaxy);
   
	   while (scanSize) {

		// look at the next star in the block-list
		Star& s = alanShepard.nextStar();
		
		// check the star is runnable
		// run it as many times as we can.
		if (isRunnable(s)) {
			// run the star
			do {
				s.fire();
				if (haltRequestFlag) exit(1);
			} while (isRunnable(s));

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
	for (int i = s.numberPorts(); i > 0; i--) {
		PortHole& p = s.nextPort();
		if (p.isItInput() && p.numTokens() != 
				     p.myGeodesic->numInitialParticles)
			count++;
	}
	if (count == 0)		return FALSE;

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

	for (int i = s->numberPorts(); i > 0; i--) {
		
		// look at the next port
		PortHole& p = s->nextPort();
		if (p.isItInput()) {
			int req = p.numberTokens - p.numTokens() +
				  p.myGeodesic->numInitialParticles;
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

	for (int j = s.numberPorts(); j > 0; j--) {
		PortHole& port = s.nextPort();
		Geodesic* g = port.myGeodesic;

	  	if (port.numberTokens == 0) {
			errorHandler.error (s.readFullName(), 
			 "output port has undefined number of tokens");
			exit(1);
		}
		int r = (g->size() - g->numInitialParticles)/port.numberTokens;
		if (r < min) min = r;
	}

	// fire sources "k-min" times.

	for (int i = min; i < k; i++) 
		s.fire();
}

// my domain
extern const char DDFdomainName[];

const char* DDFScheduler :: domain () const { return DDFdomainName ;}
