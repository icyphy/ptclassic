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

Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

 Programmers:	Joseph Buck, Soonhoi Ha, E. A. Lee,
		Thomas Parks, R. S. Stevens

These are the methods for the basic dynamic dataflow scheduler.  It
can handle any type of DataFlowStar.  When given an SDF system,
this scheduler attempts to match the notion of an iteration that
the SDF scheduler uses.  However, when delays are present, it often
fails to do so.  Thus, it may be hard to determine, except experimentally,
how many firings of each star constitute one iteration.

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
			if (p->numXfer() > numP)
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

	// If an error occured while initializing the galaxy,
	// then it is not safe to continue.
	if (SimControl::haltRequested()) return;

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
		if (!s->isA("DataFlowStar") && !s->isA("HOFStar")) {
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
	// initialize lists of sourceBlocks and nonSourceBlocks
	sourceBlocks.initialize();
	nonSourceBlocks.initialize();
	Star* sW;
	galSize = 0;
	while ((sW = nextStar++) != 0) {
		// put the source block into the sourceBlocks list.
		if (isSource(*sW))
			sourceBlocks.put(sW);
		else nonSourceBlocks.put(sW);
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

	if (!galaxy()) {
	    Error::abortRun("No galaxy to run");
	    return FALSE;
        }

	if (haltRequested()) {
	    Error::abortRun(*galaxy(), ": Can't continue after run-time error");
	    return FALSE;
	}

	ListIter nextStar (nonSourceBlocks);

	int deadlock = TRUE;		// deadlock detection.

	// Each pass through this while loop contitutes one iteration.
	// Each pass has two phases:
	//	Fire all "source" blocks once
	//	Fire all non-source blocks as many times as possible.
	// In firing the non-source blocks, source blocks may also
	// be invoked using the lazy evaluation mechanism.  For
	// SDF stars, the scheduler attempts to return the buffers
	// to their original state (numTokens == numInitialDelays).
	// A block with enough initial delays at its inputs to be fired
	// initially is considered a source block, as are, of course,
	// blocks with no input ports.

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

		// how many not-runnable stars scanned
		int scanSize = nonSourceBlocks.size();
   
		// execute stars as much as possible
		while (scanSize) {

			// look at the next star in the block-list
		        DataFlowStar* s = (DataFlowStar*)nextStar++;
			if (!s) {
				nextStar.reset();
				s = (DataFlowStar*)nextStar++;
			}
		
			// check the star is runnable
			// run it as many times as we can.
			if (isRunnable(*s)) {
				// run the star
				do {
					if (!s->run()) return FALSE;
					deadlock = FALSE;
				} while (isRunnable(*s), FALSE);

				// reset scanSize
				scanSize = nonSourceBlocks.size();
			} else {
				scanSize--;
			}
			// some arc is full 
			if (overFlowArc)  { 
				reportArcOverflow(overFlowArc, maxToken);
				return FALSE;
			}
			// there is a deadlock condition.
			if (lazyDepth > nonSourceBlocks.size()) {
				deadlock = TRUE;
				break;
			}
		} /* inside while */
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

int DynDFScheduler :: isRunnable(DataFlowStar& s, 
				 int enableLazyEvalForDynPorts) {

	// Before, this routine would check to see if s is a
	// source star, in which case it would return FALSE.
	// Now, we don't call this for source stars.

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
		// if lazy evaluation turned off for dynamic ports,
		//	return FALSE
		if (!wp->isDynamic() || wp->atBoundary()
		                     || !enableLazyEvalForDynPorts)
			return FALSE;

		// OK, try to get some tokens by evaluating the source star.

		DataFlowStar* srcStar = peerStar(*wp);

		// if dynamic, enable lazy evaluation.
		lazyDepth++;
		int lzStatus = 0;
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
		int lzStatus = TRUE;
		while (wp->numTokens() < nwait &&
		       (lzStatus = lazyEval(srcStar) != 0))
			;	// empty loop body
		lazyDepth--;
		// execute star if lazy evaluation succeeded in
		// producing enough tokens.
		if (lzStatus) {
			if(!s->run()) return FALSE;
		}
		return lzStatus;
	}
	else if (checkLazyEval(s)) {
		// fire the star
		if(!s->run()) return FALSE;
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
    int min = -1;
    int minIn = -1;
    BlockPortIter nextp(s);
    for (int j = s.numberPorts(); j > 0; j--) {
	PortHole& port = *nextp++;

	if (port.numXfer() == 0) {
	    Error::abortRun(port, " requires undefined number of tokens");
	    return FALSE;
	}

	// For inputs, r = number of firings enabled.
	// For outputs, r = effective number of firings already complete.
	// Find the minimum over all inputs (minIn) and outputs (min).
	int r = port.numTokens()/port.numXfer();
	if (port.isItOutput()) {
	    if (min < 0 || r < min) min = r;
	}
	else {
	    if (minIn < 0 || r < minIn) minIn = r;
	}
    }

    // fire sources "k-min" times.
    if (min < 0) min = 0;
    if (minIn < 0 || minIn > k - min) minIn = k - min;
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
