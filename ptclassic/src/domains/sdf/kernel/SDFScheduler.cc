/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Revision by Shuvra.
 Date of creation: 1/17/90
 Date of revision: 5/7/91
 Revisions: 5/29/90 -- renamed to SDFScheduler.cc from Scheduler.cc
		Other schedulers will go in separate files.

	    6/20/90 -- setup() now calls Galaxy::initialize rather
	    than calling initialize() and start() for all stars.

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
#include <std.h>

extern const char SDFdomainName[];

/************************************************************************

	Synchronous dataflow (SDF) Scheduler

*************************************************************************/

	////////////////////////////
	// printVerbose
	////////////////////////////

// Display a schedule

StringList
SDFSchedule :: printVerbose () const {
	StringList out;
	out = "SDF SCHEDULE:\n";
	SDFSchedIter next(*this);
	for (int i = size(); i>0; i--) {
		out += (next++)->readFullName();
		out += "\n";
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
int SDFScheduler :: run (Block& galaxy) {
	if (invalid) {
		Error::abortRun(galaxy, ": SDF schedule is invalid; can't run");
		return FALSE;
	}
	if (haltRequested()) {
		Error::abortRun(galaxy,
				": Can't continue after run-time error");
		return FALSE;
	}
	while (numItersSoFar < numIters && !haltRequested()) {
		runOnce();
		currentTime += schedulePeriod;
		numItersSoFar++;
	}
	numIters++;
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
	SDFStar* star;
	while ((star = nextStar++) != 0) {
		// Fire the next star in the list
		star->fire();
		if (haltRequested()) { invalid = TRUE; return;}
	}
}

extern int warnIfNotConnected (Galaxy&);

	////////////////////////////
	// setup
	////////////////////////////

int SDFScheduler :: setup (Block& block) {

	Galaxy& galaxy = block.asGalaxy();
	numItersSoFar = 0;
	numIters = 1;			// reset the member "numIters"
	clearHalt();
	invalid = FALSE;

        checkConnectivity(galaxy);
        if (invalid) return FALSE;
       
        prepareGalaxy(galaxy); 

	// set schedulePeriod if user gives it.
	FloatState* st = (FloatState*) galaxy.stateWithName("schedulePeriod");
	if (st) schedulePeriod = float ((double) (*st));
	currentTime = 0;

	if (haltRequested()) {
		invalid = TRUE;
		return FALSE;
	}


	// force recomputation of repetitions and noTimes.  Also make
	// sure all stars are SDF.
        
        checkStars(galaxy);	
	if (invalid) return FALSE;
  
        prepareStars(galaxy);
	if (invalid) return FALSE;
       
	repetitions(galaxy);
	if (invalid) return FALSE;

        // Schedule the graph.

        computeSchedule(galaxy);
	return !invalid;
}


int SDFScheduler::prepareGalaxy(Galaxy& galaxy)
{
	galaxy.initialize();
	return TRUE;
}



int SDFScheduler::checkConnectivity(Galaxy& galaxy)
{
	if (warnIfNotConnected (galaxy)) {
                invalid = TRUE;
                return FALSE;
	}
	return TRUE; 
}

// Check that all stars in "galaxy" are SDF stars or are derived from SDF.
int SDFScheduler::checkStars(Galaxy& galaxy)
{
	GalStarIter nextStar(galaxy);
	Star* s;
	while ((s = nextStar++) != 0) {
		if (!s->isA("SDFStar")) {
			Error::abortRun(*s, " is not an SDF star");
			invalid = TRUE;
			return FALSE;
		}
	}
	return TRUE;
}



int SDFScheduler::prepareStars(Galaxy& galaxy)
{
	GalStarIter nextStar(galaxy);
	Star* s;
	while ((s = nextStar++) != 0) 
		s->prepareForScheduling();
	return TRUE;
}


int SDFScheduler::computeSchedule(Galaxy& galaxy)
{
	GalStarIter nextStar(galaxy);
	Star* s;

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
                numDeferredBlocks = 0;

                nextStar.reset();
                while ((s = nextStar++) != 0) {
                        SDFStar& atom = *(SDFStar*)s;
                        int runResult;

                        do {
                                runResult = addIfWeCan(atom,deferredFiring);
                        } while (repeatedFiring && (runResult == 0));
                }

                // If the deferred firing option is set, we must now schedule
                // the deferred blocks

                for (int i=0; i<numDeferredBlocks; i++) {
                        SDFStar& atom = (SDFStar&)deferredBlocks[i]->asStar();
                        addIfWeCan (atom);
                }

	} while (passValue == 0);
	// END OF MAIN LOOP

	if (passValue == 1) reportDeadlock(nextStar);
	return !invalid;
}

// function to report deadlock.
void SDFScheduler::reportDeadlock (GalStarIter& next) {
        if (Error::canMark()) {
                next.reset();
                SDFStar *s;
                while ((s = (SDFStar*)next++) != 0)
                        if (notRunnable(*s) == 1) Error::mark(*s);
                Error::abortRun ("DEADLOCK: the indicated stars cannot be run");
        }
        else
                Error::abortRun (*dead,
                                 ": DEADLOCK in a loop containing this block");
        invalid = TRUE;
}

// This routine simulates running the star, adding it to the
// schedule where it can be run

int SDFScheduler::addIfWeCan (SDFStar& star, int defer) {
	int runRes = simRunStar(star,defer);
	if (runRes == 0) {
		mySchedule.append(star);
		passValue = 0;
	}
	else if (runRes == 1 && passValue != 0 && numDeferredBlocks == 0) {
		passValue = 1;
		dead = &star;
	}
	return runRes;
}

/*******************************************************************
		Routines for computing the repetitions
*******************************************************************/

// WARNING: For disconnected systems, the repetitions properties
// are all multiplied by the least common multiple of the denominators
// of *all* the repetitions properties, not just those is a connected
// subsystem.  Hence for disconnected systems, the relative number
// of times the subsystems are invoked may not be obvious.

	////////////////////////////
	// repetitions
	////////////////////////////

int SDFScheduler :: repetitions (Galaxy& galaxy) {

	// Initialize the least common multiple, which after all the
	// repetitions properties have been set, will equal the lcm
	// of the all the denominators.
	lcm = 1;

	// The following iteration occurs exactly as many times as
	// there are blocks in the universe.
	// Note that for a connected universe, this iteration would
	// entirely unnecessary.  Is there some way to get rid of it?
	// Should we allow disconnected universes?
	GalStarIter next(galaxy);
	Star* s;
	while ((s = next++) != 0) {
		// Get the next atomic block:
		SDFStar& star = *(SDFStar*) s;

		// First check to see whether a repetitions property has
		// been set.  If so, do nothing.  Otherwise, compute the
		// repetitions property.
		// The block must be an SDFStar for this to work.
		// Should there be some error checking here?
		if(star.repetitions.numerator == 0) {

			// Set repetitions to 1 and set repetitions for
			// all blocks connected to the current block.
			star.repetitions = 1;
			reptConnectedSubgraph(star);
			if (invalid) return FALSE;
		}
	}

	// Once all the repetitions properties are set, the lcm member
	// contains the least common multiple of all the denominators
	// of all the fractional repetitions.  To convert them to
	// integers, we multiply through by lcm.

	next.reset();
	while ((s = next++) != 0) {
		// Get the next atomic block:
		SDFStar& star = *(SDFStar*) s;
		star.repetitions *= Fraction(lcm);
		star.repetitions.simplify();
	}
	return TRUE;
}


	////////////////////////////
	// reptConnectedSubgraph
	////////////////////////////

// The following function sets the repetitions property of
// all atomic blocks connected to block, directly or indirectly.
// It is assumed that block has its repetitions property already set.

int SDFScheduler :: reptConnectedSubgraph (Block& block) {

	// Step through each portHole
	BlockPortIter nextp(block);
	for(int i = block.numberPorts(); i>0; i--) {
		PortHole& nearPort = *nextp++;
		if (nearPort.far() == NULL) {
			Error::abortRun(nearPort, " is not connected!");
			invalid = TRUE;
			return FALSE;
		} else if (nearPort.isItInput() == nearPort.far()->isItInput())
			// if the port is at the boundary of the wormhole.
			continue;

		PortHole& farPort = *(nearPort.far());
		// recursively call this function on the farSideBlock,
		// having determined that it has not previously been done.

		if(reptArc(nearPort,farPort))
			reptConnectedSubgraph (*(farPort.parent()));
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
	SDFStar& nearStar = (SDFStar&) nearPort.parent()->asStar();
	SDFStar& farStar =  (SDFStar&) farPort.parent()->asStar();
	Fraction& nearStarRepetitions = nearStar.repetitions;
	Fraction& farStarRepetitions = farStar.repetitions;
	Fraction farStarShouldBe;
	Fraction temp;

	// compute what the far star repetitions property should be.

	farStarShouldBe = nearStarRepetitions *
		Fraction(nearPort.numberTokens,farPort.numberTokens);

	// Simplify the fraction
	farStarShouldBe.simplify();
	
	if (farStarRepetitions == Fraction(0)) {
		// farStarRepetitions has not been set, so set it
		farStarRepetitions = farStarShouldBe;

		// update the least common multiple of the denominators
		temp = Fraction(lcm,farStarShouldBe.denominator);
		lcm = temp.computeGcdLcm().lcm;
		return TRUE;
	}
	else {
		// farStarRepetitions has been set, so test for equality
		if (!(farStarRepetitions == farStarShouldBe)) {
			StringList msg = "Sample rate problem between ";
			msg += nearStar.readFullName();
			msg += " and ";
			msg += farStar.readFullName();
                        Error::mark(nearStar);
                        Error::mark(farStar);
                        Error::abortRun(msg);
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

int SDFScheduler :: simRunStar (SDFStar& atom,
				int deferFiring,
				int updateOutputs){

	int test = notRunnable(atom);
	if(test) return test;	// return if the star cannot be run

	// If we get to this point without returning, then the star can be run.

	int i;
	SDFPortHole* port;

	// An important optimization for code generation:
	// Postpone any execution of a star feeding data to another
	// star that is runnable.  Also postpone if each output port
	// has enough data on it to satisfy destination stars.
	// This is optional because it slows down the scheduling.

	if(deferFiring && deferIfWeCan(atom))
		return 1;

	// Iterate over the ports again to adjust
	// the number of tokens on each arc.
	// Note that this should work equally well if there are no inputs.

	SDFStarPortIter nextp(atom);
	for(i = atom.numberPorts(); i>0; i--) {

		// Look at the next port in the list
		port = nextp++;

		// On the wormhole boundary, skip.
		if (port->isItInput() == port->far()->isItInput())
			continue;

		if( port->isItInput() )
			// OK to update size for input PortHole
			port->decCount(port->numberTokens);

		else if (updateOutputs)
			// OK to update size for output PortHole
			port->incCount(port->numberTokens);
	}

	// Increment noTimes
	atom.noTimes += 1;

	// Indicate that the block was successfully run
	return 0;
}

void SDFScheduler :: defer (Block* b) {
	// If possible, store a pointer to the block
	// (If the list of deferredBlocks is full, then
	// the block effectively gets deferred until a
	// future pass.  This is harmless).  But at least
	// one deferred block must go on the list, or the
	// scheduler can deadlock.

	if(numDeferredBlocks < MAX_NUM_DEFERRED_BLOCKS) {
		deferredBlocks[numDeferredBlocks] = b;
		numDeferredBlocks += 1;
	}
}

	///////////////////////////
	// deferIfWeCan
	///////////////////////////

// return TRUE if we defer atom, FALSE if we don't.
// Postpone any execution of a star feeding data to another
// star that is runnable.  Also postpone if each output port
// has enough data on it to satisfy destination stars.

int SDFScheduler :: deferIfWeCan (SDFStar& atom) {

	int i;
	SDFPortHole* port;
	SDFStarPortIter nextp(atom);
	// Check to see whether any destination blocks are runnable
	for(i = atom.numberPorts(); i>0; i--) {
		// Look at the next port in the list
		port = nextp++;
		if (port->far() == NULL) {
			invalid = TRUE;
			return 0;
		}

		// cannot be deferred if on the boundary of wormhole.
		if ((port->isItOutput()) && (port->isItOutput() == port->far()->isItOutput()))
			return FALSE;

		SDFStar& dest = (SDFStar&) port->far()->parent()->asStar();

		// If not an output, or destination is not
		// runnable, skip to the next porthole

		if(!port->isItOutput() || notRunnable(dest) != 0)
			continue;
		
		// It is runnable, but can be postponed.
		defer(port->parent());

		// Give up on this star -- it has been deferred
		return TRUE;
	}

	// Alternatively, the block might be deferred if its output
	// ports all have enough data to satisfy destination
	// stars, even if the destination stars are not runnable.

	// We must detect the case that there are no output ports
	int outputPorts = FALSE;
	nextp.reset();	// restart iterator
	for(i = atom.numberPorts(); i>0; i--) {

		// Look at the farSidePort of the next port in the list
		port = nextp++;
		// Skip if it is not output or not connected
		if (!port->isItOutput() || port->far() == NULL)
			continue;
		
		port = (SDFPortHole*)(port->far());


		// The farside port is an input.  Check Particle supply
		// if not enough, atom cannot be deferred.
		if(port->numTokens() < port->numberTokens)
			return FALSE;
			
		// Since the farside port is an input, the nearside is
		// an output
		outputPorts = TRUE;
	}

	// If we get here, all destinations have enough data,
	// and the block can be deferred, assuming there are output ports.
	if(outputPorts) defer(&atom);
	return outputPorts;
}

	////////////////////////////
	// notRunnable
	////////////////////////////

// Return 0 if the star is can be run now,
// 1 if it can't be run now, but should be runnable later (it has not
// reached its designated number of iterations), and 2 if it
// has been run as often as it needs to be.
// It is assumed that the denominator of the
// repetitions member is unity (as it should be).

int SDFScheduler :: notRunnable (SDFStar& atom) {

	int i, v = 0;
	SDFStarPortIter nextp(atom);
	// Check to see whether the requisite repetitions have been met.
	if (atom.repetitions.numerator <= atom.noTimes)
		v = 2;

	// Step through all the input ports, checking to see whether
	// there is enough data on the inputs
	else for(i = atom.numberPorts(); i>0; i--) {

		// Look at the next port in the list
		SDFPortHole& port = *nextp++;

		if(port.isItInput() && port.far()->isItOutput()) {
		   // If not in the interface of Wormhole
		   if(port.numTokens() < port.numberTokens){ 
			// not enough data
			v = 1;
			break;
		   }
		}
	}
	return v;
}

const char* SDFScheduler::domain() const { return SDFdomainName;}

// display the schedule

StringList SDFScheduler::displaySchedule() {
	return mySchedule.printVerbose();
}

// constructor -- initialize members

SDFScheduler::SDFScheduler () {
	repeatedFiring = TRUE;
	deferredFiring = TRUE;
	numItersSoFar = 0;
	numIters = 1;
	invalid = 1;
	schedulePeriod = 10000.0;
}

// setStopTime, for compatibility with DE scheduler.
// for now, we assume each schedule interation takes 1.0
// time units.  (Argh).  Deal with roundoff problems.
void SDFScheduler::setStopTime (float limit) {
	numIters = int(limit + 0.001);
}

// timing control for wormholes

void SDFScheduler::resetStopTime (float) {
	numIters = 1; numItersSoFar = 0;
}
