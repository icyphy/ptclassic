static const char file_id[] = "DDFScheduler.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include "type.h"
#include "SDFStar.h"
#include "DDFScheduler.h"
#include "Geodesic.h"
#include "GalIter.h"
#include "ConstIters.h"
#include "defConstructs.h"
#include <std.h>

#include "SDFScheduler.h"
	
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 4/24/91
	Keep the main part of the DDF scheduler.
 	Strip off the auto-wormholization routine to DDFAutoWorm.cc.

 Revisions: (1/9/91) add dynamic error detecting routines.
	    (Feb, 91) efficient scheduling for dynamic constructs.

These are the methods for the dynamic dataflow scheduler.

**************************************************************************/
	
extern const char DDFdomainName[];

int fireSource(Star&, int);

// correct numTokens value for each EventHorizon.
extern void renewNumTokens(Block* b, int flag);

/*******************************************************************
 error messages for inconsistent graphs
*******************************************************************/

typedef const char* MESSAGE;

static MESSAGE err0 = " lies in an inconsistent DDF system : \n";
static MESSAGE err1_1 = "First check DELAY-FREE LOOP or ";
static MESSAGE err1_2 = "The auto-wormholization procedure may create an "
"artificial deadlock.\n";
static MESSAGE err1_3 = "You can disable the procedure by defining a IntState "
"(restructure) and \nsetting 0 in the DDF target.";
static MESSAGE err2_1 = " needs too large input buffer size (>";
static MESSAGE err2_2 = ") \nFirst check INCONSISTENT SAMPLE RATES or ";
static MESSAGE err2_3 = "other semantic errors \n... (sorry for poor hints) \n";
static MESSAGE err2_4 = "You may increase the max buffer size by defining a ";
static MESSAGE err2_5 = "IntState (maxBufferSize) \nin the DDF target.";
	
static int lazyDepth;
static int overFlow;
static StringList msg;

/*******************************************************************
		Main DDF scheduler routines
*******************************************************************/

int isSource(const Star& s) {
	CBlockPortIter nextp(s);
	const PortHole* p;

	while ((p = nextp++) != 0) {
		if (p->isItInput()) {
			int numP = p->myGeodesic->numInit();
			if (!numP || p->numTokens() > numP)
				return FALSE;
		}
	}
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

int DDFScheduler :: setup (Galaxy& galaxy) {
	clearHalt();

	// if canDom is already set SDF, do SDFScheduling
	if (canDom == SDF) 
		return (realSched->setup(galaxy));
		
	GalStarIter nextStar(galaxy);

	// initialize sourceBlocks list
	sourceBlocks.initialize();
	numFiring = 0;

	// check connectivity
	if (warnIfNotConnected (galaxy)) {
		return FALSE;
	}

	// star initialize.  Stars must be derived from DataFlowStar.
	// (for example, SDF, DDF).  Also compute the size.
	Star* s;
	while ((s = nextStar++) != 0) {
		if (!s->isA("DataFlowStar")) {
			Error::abortRun (*s, " is not a dataflow star");
			return FALSE;
		}
		// member initialization.
		s->prepareForScheduling();
	}

	galaxy.initialize();

	currentTime = schedulePeriod;
	overFlow = FALSE;

	if (restructured == FALSE) {
		// fancy stuff...
		// auto-creation of SDF wormholes, decide the most efficient
		// scheduler (Case, For, DoWhile, Recur)
		makeSDFWormholes(galaxy);
		selectScheduler(galaxy);
		restructured = TRUE;
	}

	if (canDom == DDF) {
		GalStarIter nextS(galaxy);
		Star* sW;

		galSize = 0;
		while ((sW = nextS++) != 0) {
			// put the source block into the sourceBlocks list.
			if (isSource(*sW))
				sourceBlocks.put(sW);
			galSize++;
		}

		if (galaxy.parent() && galaxy.parent()->isItWormhole()) 
			renewNumTokens(galaxy.parent(), FALSE);
	}
			
		
	return !haltRequested();
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
DDFScheduler :: run (Galaxy& galaxy) {

	msg.initialize();

	if (haltRequested()) {
		Error::abortRun(galaxy, ": Can't continue after run-time error");
		return FALSE;
	}

	// check whether it is a predefined construct or not.
	switch(canDom) {
		case DDF : break;
		case SDF : 
			realSched->setStopTime((float) stopTime);
		default : return realSched->run(galaxy); 

	}
		
	// initialize the SpaceWalk member
	GalStarIter nextStar(galaxy);

	int scanSize = galSize;		// how many not-runnable stars scanned
	int deadlock = TRUE;		// deadlock detection.

	while (numFiring < stopTime && !haltRequested()) {

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
		if (!fireSource(s, numStop)) return FALSE;
		deadlock = FALSE;
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
				if (!s->fire()) return FALSE;
				deadlock = FALSE;
			} while (isRunnable(*s));

			// reset scanSize
			scanSize = galSize;
		} else {
			scanSize--;
		}
		// some geodesic is full 
		if (overFlow)  { 
			msg += err0;
			msg += err2_1;
			msg += maxToken;
			msg += err2_2;
			msg += err2_3;
			msg += err2_4;
			msg += err2_5;
			Error :: abortRun(*s,msg);
			return FALSE;
		}
		// there is a deadlock condition.
		if (lazyDepth > galSize) {
			msg += err0;
			msg += err1_1;
			msg += err2_3;
			msg += err1_2;
			msg += err1_3;
			Error :: abortRun(*s,msg);
			return FALSE;
		}

	   } /* inside while */

	   if (deadlock == TRUE) {
		msg += err1_1;
		msg += err2_3;
		msg += err1_2;
		msg += err1_3;
		Error :: abortRun(msg);
		return FALSE;
	   }

	   currentTime += schedulePeriod;

	} /* outside while */
	return TRUE;
}

int DDFScheduler :: isRunnable(Star& s) {

	// if source, return FALSE
	if (isSource(s)) return FALSE;

	lazyDepth = 1;		 // initialize lazyDepth

	// check the domain of the star
	// if DDFStar...

	if (strcmp (s.domain(), DDFdomainName) == 0) {

		// if user specified waitPort exists...
		DDFStar* p = (DDFStar*) &s;
		if (p->hasWaitPort()) {
		
			// check Geodesic overflow

			BlockPortIter nextp(s);
			for (int i = s.numberPorts(); i > 0; i--) {
				// look at the next port
				PortHole& tp = *nextp++;
				if (tp.isItInput()) {
					// check Geodesic size 
					int geoSize = tp.numTokens();
					if (geoSize > maxToken) {
						overFlow = TRUE;
						msg += tp.readFullName();
						return FALSE;
					}
				}
			}

			// regular routine

			int req = p->reqTokens();
			while (req > 0) {
			   // if # tokens are static, return FALSE.
			   // if on wormhole boundary, return FALSE.
			   DDFPortHole* po = (DDFPortHole*) p->waitPort;
			   if (po->isDynamic() && po->far()->isItOutput()) {
				// if dynamic, enable lazy evaluation.
				lazyDepth++;
				if (!lazyEval((Star*) po->far()->parent()))
					return FALSE;
				lazyDepth--;
			   } else
					return FALSE;
			   req = p->reqTokens();
			}
			return TRUE;
		}
	}

	// if all input ports have no data, return FALSE
	int count = 0;
	BlockPortIter nextp(s);
	PortHole *p;
	while ((p = nextp++) != 0) {
		if (p->isItInput() && 
			p->numTokens() > p->myGeodesic->numInitialParticles)
			count++;
	}
	if (count == 0)		
		return FALSE;

	// if SDFStar, or no-specified waitPort for DDFStar,....

	return (checkLazyEval(&s));
}
	

int DDFScheduler :: lazyEval(Star* s) {

	if (lazyDepth > galSize) return FALSE;

	if (strcmp (s->domain(), DDFdomainName) == 0) {

		// if user specified waitPort exists...
		DDFStar* p = (DDFStar*) s;
		if (p->hasWaitPort()) {

			int req = p->reqTokens();
			DDFPortHole* po = (DDFPortHole*) p->waitPort;
			while (req > 0) {
			   // if on wormhole boundary, return FALSE.
			   if (po->far()->isItOutput()) {
				lazyDepth++;
				if (!lazyEval((Star*) po->far()->parent())) {
					return FALSE;
				}
				lazyDepth--;
			   } else {
					return FALSE;
			   }
			   req = p->reqTokens();
			}
			s->fire();
			return TRUE;
		}
	}

	int result = checkLazyEval(s);
	if (result) {
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

			int geoSize = p.numTokens();

			int req = p.numberTokens - geoSize;
			if ((req > 0) && 
				// check wormhole, recursive star
				(p.far()->isItInput())) {
				 return FALSE;
			}

			Star* str = (Star*) p.far()->parent();
			if (strcmp (str->domain(), DDFdomainName) == 0) {
			   // check if wormhole...
			   if (str->mySched() == (Scheduler*) this) {
				// check dynamic port for ddf star.
			   	DDFPortHole* po = (DDFPortHole*) p.far();
			   	if (req > 0 && po->isDynamic()) {
					return FALSE;
				}
			   }
			}

			while (req > 0) {
				lazyDepth++;
				if (!lazyEval(str)) 
					return FALSE;
				lazyDepth--;
				req = p.numberTokens - p.numTokens();
			}

			// check Geodesic size if it exceeds the limit
			if (geoSize > maxToken) {
				msg += p.readFullName();
				overFlow = TRUE;
				return FALSE;
			}
		}
	}

	return TRUE;
}

int fireSource(Star& s, int k) {
			
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

	if (s.numberPorts() == 0) min = 0;

	// fire sources "k-min" times.
	if (minIn > k - min) minIn = k - min;
	for (int i = 0; i < minIn; i++) {
		if (!s.fire()) return FALSE;
	}
	return TRUE;
}

// my domain
const char* DDFScheduler :: domain () const { return DDFdomainName ;}

	////////////////////////////
	// selectScheduler
	////////////////////////////

void DDFScheduler :: selectScheduler(Galaxy& galaxy) {

	// select a candidate domain
	detectConstruct(galaxy);

	// set up the right scheduler
	switch(canDom) {
		// special care for SDF domain.
		case SDF : LOG_NEW; realSched = new SDFScheduler;
			realSched->setup(galaxy);
		        if (galaxy.parent()) 
				renewNumTokens(galaxy.parent(), TRUE);
			break;
		case Unknown :
		case DDF : break;
		default : realSched = getScheduler(canDom);
			  if (!realSched) {
				Error::abortRun("Undefined Scheduler for ",
				nameType(canDom), " construct.");
				return;
			  }
			  realSched->setup(galaxy);
			  break;
	}
}

	////////////////////////////
	// detectConstruct
	////////////////////////////

void DDFScheduler :: detectConstruct(Galaxy& gal) {

	GalStarIter nextStar(gal);
	Star* s;

	if ((canDom == DDF) || (canDom == SDF)) return;

	while ((s = nextStar++) != 0) {
		if (s->isItWormhole()) {
			const char* dom = s->mySched()->domain();
			if (strcmp(dom + 1, "DF"))
				canDom = DDF;
		} else if (strcmp(s->domain(), DDFdomainName)) {
			Error::abortRun(*s, " is not a Wormhole.");
			canDom = Unknown;
		}
	}

	canDom = getType(gal);
}

// constructor: set default options

DDFScheduler::DDFScheduler () {
	stopTime = 1;
	canDom = Unknown;
	numOverlapped = 1; 
	restructured = FALSE;
	realSched = 0;
	schedulePeriod = 10000.0;
}

DDFScheduler::~DDFScheduler() { LOG_DEL; delete realSched; }

// setStopTime, for compatibility with DE scheduler.
// for now, we assume each schedule interation takes 1.0
// time units.  (Argh).  Deal with roundoff problems.
void DDFScheduler::setStopTime (float limit) {
	stopTime = int(limit + 0.001) ;
}

void DDFScheduler::resetStopTime (float) {
	stopTime = 1; numFiring = 0;
}

int DDFScheduler::isSDFType() {
	int flag = 1;
	if (canDom == DDF) flag = 0;
	else if (canDom == SDF) flag = 2;
	return flag;
}

void DDFScheduler::resetFlag() { restructured = FALSE ;}

