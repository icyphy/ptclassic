static const char file_id[] = "DynDFScheduler.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include "DynDFScheduler.h"
#include "GalIter.h"
#include "SDFScheduler.h"
	
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990,1991,1992,1993 The Regents of the University of California.
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

 Programmer:  Soonhoi Ha, Joseph Buck

These are the methods for the basic dynamic dataflow scheduler.  It
can handle any type of DataFlowStar.  It does not do any kind of
restructuring of the universe (as is done by DDFScheduler).

**************************************************************************/

// debug
#include <iostream.h>

// default max # of tokens on an arc.
const int DEFAULT_MAX_TOKEN = 1024;

static int fireSource(Star&, int);

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

// return the star that is the parent at the other end of a porthole
inline DataFlowStar* peerStar(PortHole& p) {
	return (DataFlowStar*)p.far()->parent();
}

	////////////////////////////
	// displaySchedule
	////////////////////////////

StringList DynDFScheduler :: displaySchedule () {
	return "DDF schedule is computed at run-time\n";
}

extern int warnIfNotConnected (Galaxy&);

	////////////////////////////
	// setup
	////////////////////////////

void DynDFScheduler :: setup () {
	if (!galaxy()) {
		Error::abortRun("DynDFScheduler: no galaxy!");
		return;
	}
	clearHalt();


	numFiring = 0;

	// check connectivity
	if (warnIfNotConnected (*galaxy())) {
		return;
	}

	if (!checkBlocks()) return;

	galaxy()->initialize();

	currentTime = schedulePeriod;
	overFlowArc = 0;
	initStructures();
	return;
}

int DynDFScheduler::checkBlocks() {
	DFGalStarIter nextStar(*galaxy());
	// star initialize.  Stars must be derived from DataFlowStar.
	// (for example, SDF, DDF).  Also compute the size.
	DataFlowStar* s;
	while ((s = nextStar++) != 0) {
		if (!s->isA("DataFlowStar")) {
			Error::abortRun (*s, " is not a dataflow star");
			return FALSE;
		}
		else if (!s->setDynamicExecution(TRUE)) {
			// this will reject code-generation stars, for now.
			Error::abortRun (*s,
				 " cannot be executed by a dynamic scheduler");
			return FALSE;
		}
	}
	return TRUE;
}

void DynDFScheduler::initStructures() {
	GalStarIter nextStar(*galaxy());
	// initialize sourceBlocks list
	sourceBlocks.initialize();
	Star* sW;
	galSize = 0;
	while ((sW = nextStar++) != 0) {
		// put the source block into the sourceBlocks list.
		if (isSource(*sW))
			sourceBlocks.put(sW);
		galSize++;
	}
}
		
static void reportArcOverflow(PortHole* p, int maxToken) {
	StringList msg =
	"Dynamic scheduler error: maximum buffer size exceeded (limit = ";
	msg << maxToken << ")\n";
	msg << "Check for inconsistent sample rates or other semantic errors."
	    << "\nYou may increase the maximum buffer size by changing the "
	    << "\n'maxBufferSize' parameter\nof the target.";
	Error :: abortRun(*p,msg);
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
DynDFScheduler :: run () {

	if (haltRequested()) {
		Error::abortRun(*galaxy(), ": Can't continue after run-time error");
		return FALSE;
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
			if (overFlowArc)  { 
				reportArcOverflow(overFlowArc, maxToken);
				return FALSE;
			}
			// there is a deadlock condition.
			if (lazyDepth > galSize) {
				deadlock = TRUE;
				break;
			} /* inside while */
		}
		if (deadlock == TRUE) {
			Error::abortRun("deadlock detected: check for a ",
					"delay-free loop");
			return FALSE;
		}

		currentTime += schedulePeriod;
		
	} /* outside while */
	return TRUE;
}

// check arc overflow on input arcs to s.  Return a porthole that has
// overflowed if one exists.

static PortHole* checkInputOverflow(Star& s, int maxToken) {

	BlockPortIter nextp(s);
	PortHole* p;
	while ((p = nextp++) != 0) {
		if (p->isItOutput()) continue;
		if (p->numTokens() > maxToken) {
			return p;
		}
	}
	return 0;
}

int DynDFScheduler :: isRunnable(DataFlowStar& s) {

	// if source, return FALSE
	if (isSource(s)) return FALSE;

	lazyDepth = 1;		 // initialize lazyDepth

	DFPortHole* wp = s.waitPort();

	if (wp) {
		// check arc overflow
		if ((overFlowArc = checkInputOverflow(s,maxToken)) != 0) {
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

		DataFlowStar* srcStar = peerStar(*wp);

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
	

int DynDFScheduler :: lazyEval(DataFlowStar* s) {

	if (lazyDepth > galSize) return FALSE;

	DFPortHole* wp = s->waitPort();
	if (wp) {
		int nwait = s->waitTokens();

		// if on wormhole boundary, return FALSE: we cannot
		// get tokens to make this star executable.
		if (wp->atBoundary()) return FALSE;

		DataFlowStar* srcStar = peerStar(*wp);
		// if dynamic, enable lazy evaluation.
		lazyDepth++;
		int lzStatus;
		while (wp->numTokens() < nwait &&
		       (lzStatus = lazyEval(srcStar) != 0))
			;	// empty loop body
		lazyDepth--;
		// execute star if lazy evaluation succeeded in
		// producing enough tokens.
		if (lzStatus) {
			s->run();
		}
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

int DynDFScheduler :: checkLazyEval(DataFlowStar* s) {
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

		DataFlowStar* str = peerStar(p);
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
			overFlowArc = &p;
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

// constructor: set default options

DynDFScheduler::DynDFScheduler () {
	stopTime = 1;
	numOverlapped = 1; 
	schedulePeriod = 10000.0;
	maxToken = DEFAULT_MAX_TOKEN;
	overFlowArc = 0;
	lazyDepth = 0;
}

// setStopTime, for compatibility with DE scheduler.
// for now, we assume each schedule interation takes 1.0
// time units.  (Argh).  Deal with roundoff problems.
void DynDFScheduler::setStopTime (double limit) {
	stopTime = int(limit + 0.001) ;
}

void DynDFScheduler::resetStopTime (double) {
	stopTime = 1; numFiring = 0;
}

void DynDFScheduler::resetFlags() { 
	numOverlapped = 1;
	maxToken = DEFAULT_MAX_TOKEN;
	overFlowArc = 0;
}
