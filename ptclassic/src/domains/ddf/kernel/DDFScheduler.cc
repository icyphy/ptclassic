static const char file_id[] = "DDFScheduler.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include "type.h"
#include "SDFStar.h"
#include "DDFScheduler.h"
#include "GalIter.h"
#include "defConstructs.h"
#include <std.h>

#include "SDFScheduler.h"
	
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY

 Programmer:  Soonhoi Ha, J. Buck
 Date of creation: 4/24/91
	Keep the main part of the DDF scheduler.
 	Strip off the auto-wormholization routine to DDFAutoWorm.cc.

 Revisions: (1/9/91) add dynamic error detecting routines.
	    (Feb, 91) efficient scheduling for dynamic constructs.

These are the methods for the dynamic dataflow scheduler.

**************************************************************************/
	
extern const char DDFdomainName[];

static int fireSource(Star&, int);

// correct numTokens value for each EventHorizon.
extern void renewNumTokens(Block* b, int flag);

/*******************************************************************
 error messages for inconsistent graphs
*******************************************************************/

typedef const char* MESSAGE;

static MESSAGE err0 = " lies in an inconsistent DDF system:\n";
static MESSAGE err1_1 = "Check for a delay-free loop. ";
static MESSAGE err1_2 = "The auto-wormholization procedure may create an "
"artificial deadlock.\n";
static MESSAGE err1_3 = "You can disable the procedure by setting the DDF\n"
			"target parameter 'restructure' to NO.";

static MESSAGE err2_1 = " maximum buffer size exceeded (numTokens > ";
static MESSAGE err2_2 = ") \nCheck for inconsistent sample rates or ";
static MESSAGE err2_3 = "other semantic errors. \nYou may ";
static MESSAGE err2_4 = "increase the maximum buffer size by changing the ";
static MESSAGE err2_5 = "'maxBufferSize' parameter\nof the DDF target.";

/*******************************************************************
		Main DDF scheduler routines
*******************************************************************/

static int isSource(Star& s) {
	BlockPortIter nextp(s);
	PortHole* p;

	while ((p = nextp++) != 0) {
		if (p->isItInput()) {
			int numP = p->numInitDelays();
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

void DDFScheduler :: setup () {
	if (!galaxy()) {
		Error::abortRun("DDFScheduler: no galaxy!");
		return;
	}
	clearHalt();

	// if canDom is already set SDF, do SDFScheduling
	if (canDom == SDF) {
		realSched->setup();
		return;
	}
		
	DFGalStarIter nextStar(*galaxy());

	// initialize sourceBlocks list
	sourceBlocks.initialize();
	numFiring = 0;

	// check connectivity
	if (warnIfNotConnected (*galaxy())) {
		return;
	}

	// star initialize.  Stars must be derived from DataFlowStar.
	// (for example, SDF, DDF).  Also compute the size.
	DataFlowStar* s;
	while ((s = nextStar++) != 0) {
		if (!s->isA("DataFlowStar")) {
			Error::abortRun (*s, " is not a dataflow star");
			return;
		}
		else if (!s->setDynamicExecution(TRUE)) {
			// this will reject code-generation stars, for now.
			Error::abortRun (*s,
				 " cannot be executed by a dynamic scheduler");
			return;
		}
	}

	galaxy()->initialize();

	currentTime = schedulePeriod;
	overFlow = FALSE;

	if (restructured == FALSE) {
		// fancy stuff...
		// auto-creation of SDF wormholes, decide the most efficient
		// scheduler (Case, For, DoWhile, Recur)
		makeSDFWormholes(*galaxy());
		selectScheduler(*galaxy());
		restructured = TRUE;
	}

	if (canDom == DDF) {
		nextStar.reset();
		Star* sW;

		galSize = 0;
		while ((sW = nextStar++) != 0) {
			// put the source block into the sourceBlocks list.
			if (isSource(*sW))
				sourceBlocks.put(sW);
			galSize++;
		}

		Block* gpar = galaxy()->parent();
		if (gpar && gpar->isItWormhole()) 
			renewNumTokens(gpar, FALSE);
	}
		
	return;
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
DDFScheduler :: run () {

	msg.initialize();

	if (haltRequested()) {
		Error::abortRun(*galaxy(), ": Can't continue after run-time error");
		return FALSE;
	}

	// check whether it is a predefined construct or not.
	switch(canDom) {
		case DDF : break;
		case SDF : 
			realSched->setStopTime((float) stopTime);
		default : return realSched->run(); 

	}
		
	DFGalStarIter nextStar(*galaxy());

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
		DataFlowStar* s = nextStar++;
		if (!s) {
			nextStar.reset();
			s = nextStar++;
		}
		
		// check the star is runnable
		// run it as many times as we can.
		if (isRunnable(*s)) {
			// run the star
			do {
				if (!s->run()) return FALSE;
				deadlock = FALSE;
			} while (isRunnable(*s));

			// reset scanSize
			scanSize = galSize;
		} else {
			scanSize--;
		}
		// some arc is full 
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

static int checkInputOverflow(Star& s, int maxToken, StringList& msg) {
	// check arc overflow on input arcs to s.

	BlockPortIter nextp(s);
	PortHole* p;
	while ((p = nextp++) != 0) {
		if (p->isItOutput()) continue;
		if (p->numTokens() > maxToken) {
			msg << p->fullName();
			return TRUE;
		}
	}
	return FALSE;
}

int DDFScheduler :: isRunnable(DataFlowStar& s) {

	// if source, return FALSE
	if (isSource(s)) return FALSE;

	lazyDepth = 1;		 // initialize lazyDepth

	DFPortHole* wp = s.waitPort();

	if (wp) {
		// check arc overflow
		if (checkInputOverflow(s,maxToken,msg)) {
			overFlow = TRUE;
			return FALSE;
		}

		// regular routine

		int nwait = s.waitTokens();

		// if enough tokens already return TRUE.
		if (wp->numTokens() >= nwait) return TRUE;

		// if # tokens are static, return FALSE.
		// if on wormhole boundary, return FALSE.
		
		if (!wp->isDynamic() || wp->atBoundary())
			return FALSE;

		// OK, try to get some tokens by evaluating the source star.

		DataFlowStar* srcStar = (DataFlowStar*)wp->far()->parent();

		// if dynamic, enable lazy evaluation.
		lazyDepth++;
		int lzStatus;
		while (wp->numTokens() < nwait &&
		       (lzStatus = lazyEval(srcStar) != 0))
			;	// empty loop body, go until enough tokens
				// or lazy eval fails.
		lazyDepth--;
		return lzStatus;
	}
	// this handles the no-waitport case.
	// if all input ports have no data, return FALSE
	int count = 0;
	BlockPortIter nextp(s);
	PortHole *p;
	while ((p = nextp++) != 0) {
		if (p->isItInput() && 
			p->numTokens() > p->numInitDelays())
			count++;
	}
	if (count == 0)		
		return FALSE;

	// if SDFStar, or no-specified waitPort for DDFStar,....

	return (checkLazyEval(&s));
}
	

int DDFScheduler :: lazyEval(DataFlowStar* s) {

	if (lazyDepth > galSize) return FALSE;

	DFPortHole* wp = s->waitPort();
	if (wp) {
		int nwait = s->waitTokens();

		// if on wormhole boundary, return FALSE: we cannot
		// get tokens to make this star executable.
		if (wp->atBoundary()) return FALSE;

		DataFlowStar* srcStar = (DataFlowStar*)wp->far()->parent();
		// if dynamic, enable lazy evaluation.
		lazyDepth++;
		int lzStatus;
		while (wp->numTokens() < nwait &&
		       (lzStatus = lazyEval(srcStar) != 0))
			;	// empty loop body
		lazyDepth--;
		// execute star if lazy evaluation succeeded in
		// producing enough tokens.
		if (lzStatus) s->run();
		return lzStatus;
	}
	else if (checkLazyEval(s)) {
		// fire the star
		s->run();
		return TRUE;
	}
	else
		return FALSE;
}

int DDFScheduler :: checkLazyEval(DataFlowStar* s) {
	BlockPortIter nextp(*s);

	for (int i = s->numberPorts(); i > 0; i--) {
		
		// look at the next input port
		PortHole& p = *nextp++;
		if (p.isItOutput()) continue;

		int req = p.numXfer() - p.numTokens();
		// if I require input but am on a wormhole boundary,
		// quit now.
		if (req > 0 && p.atBoundary())
			return FALSE;

		DataFlowStar* str = (DataFlowStar*) p.far()->parent();
		if (str->scheduler() == this) {
			// check dynamic port for ddf star.
			DFPortHole* po = (DFPortHole*) p.far();
			if (req > 0 && po->isDynamic()) {
				return FALSE;
			}
		}

		while (p.numXfer() > p.numTokens()) {
			lazyDepth++;
			if (!lazyEval(str)) 
				return FALSE;
			lazyDepth--;
		}

		// check Geodesic size if it exceeds the limit
		if (p.numTokens() > maxToken) {
			msg += p.fullName();
			overFlow = TRUE;
			return FALSE;
		}
	}
	return TRUE;
}

static int fireSource(Star& s, int k) {
			
	// check how many unused tokens are on output arcs.
	int min = 1000000;		// large enough number
	int minIn = 1000;
	BlockPortIter nextp(s);
	for (int j = s.numberPorts(); j > 0; j--) {
		PortHole& port = *nextp++;

	  	if (port.numXfer() == 0) {
			Error::abortRun(s,
			      ": output port requires undefined number of tokens");
			return FALSE;
		}
		int r = port.numTokens()/port.numXfer();
		if (port.isItOutput()) {
			if (r < min) min = r;
		} else {
			if (minIn > r) minIn = r;
		}
	}

	if (s.numberPorts() == 0) min = 0;

	// fire sources "k-min" times.
	if (minIn > k - min) minIn = k - min;
	for (int i = 0; i < minIn; i++) {
		if (!s.run()) return FALSE;
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
	case SDF :
		LOG_NEW; realSched = new SDFScheduler;
		realSched->setGalaxy(galaxy);
		realSched->setup();
		if (galaxy.parent()) 
			renewNumTokens(galaxy.parent(), TRUE);
		break;
	case Unknown :
	case DDF : break;
	default :
		realSched = getScheduler(canDom);
		if (!realSched) {
			Error::abortRun("Undefined Scheduler for ",
					nameType(canDom), " construct.");
			return;
		}
		realSched->setGalaxy(galaxy);
		realSched->setup();
		break;
	}
}

	////////////////////////////
	// detectConstruct
	////////////////////////////

// This routine attempts to detect a special construct and run an
// optimized scheduler.  We cannot use this trick if there are wormholes
// for arbitrary domains, or stars that are not DDF, present.  This
// is a cheat and should be cleaned up.

void DDFScheduler :: detectConstruct(Galaxy& gal) {

	DFGalStarIter nextStar(gal);
	DataFlowStar* s;

	if ((canDom == DDF) || (canDom == SDF)) return;

	while ((s = nextStar++) != 0) {
		if (s->isItWormhole()) {
			const char* dom = s->scheduler()->domain();
			if (strcmp(dom , "DF")) {
				canDom = DDF;
				return;
			}
		}
		else if (!s->isA("DDFStar")) {
			canDom = DDF;
			return;
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
	overFlow = FALSE;
	lazyDepth = 0;
}

DDFScheduler::~DDFScheduler() { LOG_DEL; delete realSched; }

// setStopTime, for compatibility with DE scheduler.
// for now, we assume each schedule interation takes 1.0
// time units.  (Argh).  Deal with roundoff problems.
void DDFScheduler::setStopTime (double limit) {
	stopTime = int(limit + 0.001) ;
}

void DDFScheduler::resetStopTime (double) {
	stopTime = 1; numFiring = 0;
}

int DDFScheduler::isSDFType() {
	int flag = 1;
	if (canDom == DDF) flag = 0;
	else if (canDom == SDF) flag = 2;
	return flag;
}

void DDFScheduler::resetFlag() { 
	restructured = FALSE ;
	numOverlapped = 1;
	maxToken = 1024;
	overFlow = FALSE;
}

