static const char file_id[] = "SDFScheduler.cc";
/**************************************************************************
Version identification:
@(#)SDFScheduler.cc	2.47	07/17/96

Copyright (c) 1990-1996 The Regents of the University of California.
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

 Programmer:  E. A. Lee, D. G. Messerschmitt, J. Buck, S. Bhattacharyya

 SDF Scheduler methods

**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "type.h"
#include "SDFScheduler.h"
#include "SDFStar.h"
#include "Fraction.h"
#include "Error.h"
#include "StringList.h"
#include "FloatState.h"
#include "GalIter.h"
#include "Target.h"
#include "EventHorizon.h"
#include "checkConnect.h"

extern const char SDFdomainName[];

/************************************************************************

	Synchronous dataflow (SDF) Scheduler

*************************************************************************/

	////////////////////////////
	// printVerbose
	////////////////////////////

// Return a description of the schedule

StringList
SDFSchedule :: printVerbose () const {
	StringList out;
	SDFSchedIter next(*this);
	for (int i = size(); i > 0; i--) {
		out << "  { fire " << (next++)->fullName() << " }\n";
	}
	return out;
}

/*******************************************************************
		Main SDF scheduler routines
*******************************************************************/


	////////////////////////////
	// run
	////////////////////////////

// runs the number of times indicated by numIters.
int SDFScheduler :: run () {
	// Call haltRequested to process pending events and check for halt
	// If the user hit the DISMISS button in the run control panel,
	// then the universe referenced by galaxy() will return a null pointer
	int haltFlag = SimControl::haltRequested();
        if (!galaxy() || invalid || haltFlag) {
	    invalid = TRUE;
            Error::abortRun("SDF Scheduler has no galaxy to run");
            return FALSE;
        }

	while (numItersSoFar < numIters && !SimControl::haltRequested()) {
		runOnce();
		currentTime += schedulePeriod;
		numItersSoFar++;
	}
	// FIXME (wtc): I don't think we need the following line.
	// I commented it out and see if things still work.
	//numIters++;
	return TRUE;
}

	////////////////////////////
	// runOnce
	////////////////////////////

// What is now runOnce used to be called run
void SDFScheduler :: runOnce () {

   // assume the schedule has been set by the setup member
   // Adjust the schedule pointer to point to the beginning of the schedule.
	SDFSchedIter nextStar(mySchedule);
	DataFlowStar* star;
	while ((star = nextStar++) != 0 && !invalid) {
		// Fire the next star in the list
		invalid = !star->run();
	}
}

	////////////////////////////
	// setup
	////////////////////////////

void SDFScheduler :: setup () {
	// These initializations should go before the error checking
	numItersSoFar = 0;
	numIters = 1;			// reset the member "numIters"
	clearHalt();
	invalid = FALSE;

	if (! galaxy()) {
		invalid = TRUE;
		Error::abortRun("SDF Scheduler has no galaxy defined");
		return;
	}

	// Check to see if the original user galaxy is connected
        if (!checkConnectivity()) return;

        prepareGalaxy();
	// If halt is requested in the middle of galaxy initialization,
	// the galaxy is left in an invalid state.
	if (SimControl::haltRequested()) {
		invalid = TRUE;
		return;
	}

	// Check connectivity again, blocks could have been deleted
	// with galaxy()->deleteBlockAfterInit.  This can happen with
	// hof stars and the CGC wormhole target

        if (!checkConnectivity()) return;

	currentTime = 0;

	// force recomputation of repetitions and noTimes.  Also make
	// sure all stars are SDF.

        if (!checkStars()) return;

	if (!repetitions()) return;

        // Schedule the graph.

        if (!computeSchedule(*galaxy())) return;

	adjustSampleRates();
	return;
}


int SDFScheduler::prepareGalaxy()
{
	if (! galaxy()) invalid = TRUE;
	if (invalid) return FALSE;

	galaxy()->initialize();
	return TRUE;
}

int SDFScheduler::checkConnectivity()
{
	if (! galaxy()) invalid = TRUE;
	if (invalid) return FALSE;

	if (warnIfNotConnected(*galaxy())) {
                invalid = TRUE;
                return FALSE;
	}
	return TRUE;
}

// Check that all stars in "galaxy" have the SDF property,
// and that their portholes produce or consume positive numbers
// of tokens.  We assume they have already been checked (by the
// target, say) and are at least DataFlowStar.
//
// Note that "isSDFinContext" is used.  If actors are inserted
// after the schedule is computed, the SDF property may be lost,
// so in that case checkStars should be overriden and isSDF used
// instead.

int SDFScheduler::checkStars()
{
	if (! galaxy()) invalid = TRUE;
	if (invalid) return FALSE;

	DFGalStarIter nextStar(*galaxy());
	DataFlowStar* s;
	while ((s = nextStar++) != 0) {
		if (!s->isSDFinContext()) {
			Error::abortRun(*s, " is not SDF");
			invalid = TRUE;
			return FALSE;
		}
		// Check if each of the star's portholes produces or
		// consumes positive number of tokens.
		BlockPortIter nextPort(*s);
		PortHole* p;
		while ((p = nextPort++) != 0) {
			if(p->numXfer() <= 0) {
				Error::abortRun(*s, "Porthole \"", p->name(),
				    "\" does not produce or consume positive number of tokens.");
				invalid = TRUE;
				return FALSE;
			}
		}
	}
	return TRUE;
}

// if the schedule is for a wormhole, we need to set the number
// of tokens transferred by the attached event horizons.
// This function does the job.
// Note that eh->setParams has no effect if either end of the
// wormhole boundary is restrained to transfer only one sample at a time.

void SDFScheduler::adjustSampleRates() {
	if (! galaxy()) invalid = TRUE;
	if (invalid) return;

	BlockPortIter nextPort(*galaxy());
	PortHole* p;
	while ((p = nextPort++) != 0) {
		DFPortHole* dp = (DFPortHole*) &p->newConnection();
		int num = dp->numXfer() * dp->parentReps();
		PortHole* dfar = dp->far();
		if (dfar == 0) continue;
		EventHorizon* eh = dfar->asEH();
		if (eh == 0) continue;
		eh->setParams(num);
	}
}

int SDFScheduler::computeSchedule(Galaxy& g)
{
	DFGalStarIter nextStar(g);
	DataFlowStar* s;

	mySchedule.initialize();

        // MAIN LOOP
        // After a scheduler pass, the passValue tells us whether the
        // scheduler is finished.  After the pass, the meaning of the
        // possible values is:
        //      2 - scheduler is finished
        //      1 - scheduler is deadlocked
        //      0 - more passes are required

	do {
                passValue = 2;
		deferredStar = 0;
		int numAddedThisPass = 0;
                nextStar.reset();
                while ((s = nextStar++) != 0) {
                        int runResult;

                        do {
                                runResult = addIfWeCan(*s,deferredFiring);
				if (runResult == 0) numAddedThisPass++;
                        } while (runResult == 0);
                }

                // If the deferred firing option is set, we must now schedule
                // the deferred star (if any).  We permit it to be
		// "re-deferred", unless no stars at all were added in the
		// most recent pass.

		// use of "deferAgain" is intended to produce the minimum
		// buffer sizes between stars.

		if (deferredStar) {
			int deferAgain = deferredFiring;
			if (numAddedThisPass == 0) deferAgain = 0;

			addIfWeCan(*deferredStar,deferAgain);
		}
	} while (passValue == 0);
	// END OF MAIN LOOP

	if (passValue == 1) reportDeadlock(nextStar);

	return !invalid;
}

// function to report deadlock.
void SDFScheduler::reportDeadlock (DFGalStarIter& next) {
	next.reset();
	DataFlowStar *s;
	StringList starList;

	// Generate a list of stars that are deadlocked
	while ((s = next++) != 0)
	  if (s->notRunnable() == 1)
	    starList << s->fullName() << " is deadlocked\n";
	next.reset();

        if (Error::canMark()) {
                while ((s = next++) != 0)
                        if (s->notRunnable() == 1) Error::mark(*s);
                Error::abortRun (
			 "DEADLOCK: the indicated stars cannot be run:\n",
				 starList);
        }
        else {
                Error::abortRun (
			 "DEADLOCK: the following stars cannot be run:\n",
				 starList);
	}
        invalid = TRUE;
}

// This routine simulates running the star, adding it to the
// schedule where it can be run

int SDFScheduler::addIfWeCan (DataFlowStar& star, int defer) {
	int runRes = simRunStar(star,defer);
	if (runRes == 0) {
		mySchedule.append(star);
		passValue = 0;
	}
	else if (runRes == 1 && passValue != 0 && deferredStar == 0) {
		passValue = 1;
	}
	return runRes;
}

/*******************************************************************
		Routines for computing the repetitions
*******************************************************************/

// For disconnected systems, the connected subgraph is normalized
// independently.  Thus one iteration for the entire system 
// is equivalent to one iteration of each subgraph.

	////////////////////////////
	// repetitions
	////////////////////////////

int SDFScheduler :: repetitions () {
	if (! galaxy()) invalid = TRUE;
	if (invalid) return FALSE;

	// The following iteration occurs exactly as many times as
	// there are blocks in the universe.  This way we can step
	// through all the connected subgraphs.
	DFGalStarIter next(*galaxy());
	DataFlowStar* s;
	while ((s = next++) != 0) {
		// Get the next atomic block:
		
		// First check to see whether a repetitions property has
		// been set.  If so, do nothing.  Otherwise, compute the
		// repetitions property.
		
		if(s->repetitions.num() == 0) {
			// Initialize the least common multiple, which
			// after all the repetitions properties of each node
			// in a connected subgraph have been set, will equal
			// the lcm of the all the denominators.
			lcmOfDenoms = 1;

			subgraph.initialize();
			subgraph.put(*s);

			// Set repetitions to 1 and set repetitions for
			// all blocks connected to the current block.
			// build the 'subgraph' as each connected node
			// is found.
			s->repetitions = 1;
			reptConnectedSubgraph(*s);
 			if (invalid) return FALSE;

			// Normalize the subgraph so that all repetitions
			// of the nodes are integers 
			BlockListIter nextSub(subgraph);
			nextSub.reset();
			DataFlowStar* ds;
			while (( ds = (DataFlowStar*)nextSub++) !=0){
				ds->repetitions *= Fraction(lcmOfDenoms);
				ds->repetitions.simplify();
			}
			
		}
	}
	return TRUE;
}

	////////////////////////////
	// reptConnectedSubgraph
	////////////////////////////

// The following function sets the repetitions property of
// all atomic blocks connected to block, directly or indirectly.
// It is assumed that block has its repetitions property already set.
// The function also builds the a BlockList 'subgraph' which after
// the recursion will contain an entire connected subgraph.

int SDFScheduler :: reptConnectedSubgraph (Block& block) {

	// Step through each portHole
	BlockPortIter nextp(block);
	for(int i = block.numberPorts(); i>0; i--) {
		PortHole& nearPort = *nextp++;
		if (nearPort.atBoundary())
			// if the port is at the boundary of the wormhole.
			continue;
		PortHole& farPort = *(nearPort.far());
		// recursively call this function on the farSideBlock,
		// having determined that it has not previously been done.

		if(reptArc(nearPort,farPort)) {
			reptConnectedSubgraph(*(farPort.parent()));
			subgraph.put(farPort.parent()->asStar());
		}
	}
	return TRUE;
}


	////////////////////////////
	// reptArc
	////////////////////////////

// This function sets the repetitions property of two blocks
// on opposite sides of a connection.  It is assumed that nearPort
// belongs to a block that has already had its repetitions property set.
// If in addition farPort has had its repetitions property set,
// then consistency is checked, and FALSE is returned.  Otherwise,
// TRUE is returned.

int SDFScheduler :: reptArc (PortHole& nearPort, PortHole& farPort){
	DataFlowStar& nearStar = (DataFlowStar&) nearPort.parent()->asStar();
	DataFlowStar& farStar =  (DataFlowStar&) farPort.parent()->asStar();
	Fraction& nearStarRepetitions = nearStar.repetitions;
	Fraction& farStarRepetitions = farStar.repetitions;
	Fraction farStarShouldBe;

	// compute what the far star repetitions property should be.
	Fraction factor(nearPort.numXfer(),farPort.numXfer());
	farStarShouldBe = nearStarRepetitions * factor;

	// Simplify the fraction
	farStarShouldBe.simplify();

	if (farStarRepetitions.num() == 0) {
		// farStarRepetitions has not been set, so set it
		farStarRepetitions = farStarShouldBe;
		// update the least common multiple of the denominators
		lcmOfDenoms = lcm(lcmOfDenoms,farStarShouldBe.den());
		return TRUE;
	}
	else {
		// farStarRepetitions has been set, so test for equality
		if (!(farStarRepetitions == farStarShouldBe)) {
			Error::abortRun(farStar,"sample rate inconsistency detected");
			invalid = TRUE;
		}
		return FALSE;
	}
}


/*******************************************************************
		Routines for computing the schedule
*******************************************************************/


	////////////////////////////
	// simRunStar
	////////////////////////////

int SDFScheduler :: simRunStar (DataFlowStar& atom, int deferFiring) {
	int status = atom.simRunStar(deferFiring);
	if (status == 3 && !deferredStar) {
		// if no star has been deferred yet, defer this one.
		// if there already is a deferred star, this star effectively
		// is deferred until a future pass.  This is harmless.

		deferredStar = &atom;
		status = 1;
	}
	return status;
}

const char* SDFScheduler::domain() const { return SDFdomainName;}

// display the schedule

StringList SDFScheduler::displaySchedule() {
    StringList out;
    out << "{\n  { scheduler \"Simple SDF Scheduler\" }\n"
	<< mySchedule.printVerbose()
	<< "}\n";
    return out;
}

// constructor -- initialize members

SDFScheduler::SDFScheduler () {
	deferredFiring = TRUE;
	numItersSoFar = 0;
	numIters = 1;
	invalid = TRUE;
	schedulePeriod = 10000.0;
}

// destructor
SDFScheduler::~SDFScheduler () {
	invalid = TRUE;		// dummy for breakpoint.
}

// setStopTime, for compatibility with DE scheduler.
// for now, we assume each schedule interation takes 1.0
// time units.  (Argh).  Deal with roundoff problems.
void SDFScheduler::setStopTime (double limit) {
	numIters = int(floor(limit + 0.001));
}

// timing control for wormholes

void SDFScheduler::resetStopTime (double) {
	numIters = 1; numItersSoFar = 0;
}

void SDFScheduler::compileRun () {
    // assume the schedule has been set by the setup member
    Target& tar = target();

    SDFSchedIter nextStar(mySchedule);
    DataFlowStar* star;
    while ((star = nextStar++) != 0 && !SimControl::haltRequested()) {
	// Fire the next star in the list
	tar.writeFiring(*star, 1);
    }
}

// Instead of executing SDF scheduling algorithm, we copy the schedule
// from the argument list (necessary for copying from the parallel scheduling
// result). Here, we calculate repetitions.
void SDFScheduler :: copySchedule(SDFSchedule& s) {

	// calculate repetition properties of the galaxy
	invalid = FALSE;
	repetitions();

	// main copying routine.
	mySchedule.initialize();
	SDFSchedIter nextStar(s);
	DataFlowStar* star;
	while ((star = nextStar++) != 0) {
		mySchedule.append(*star);
	}
}
