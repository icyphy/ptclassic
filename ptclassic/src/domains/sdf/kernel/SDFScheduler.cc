// Programmer: Edward A. Lee
// EECS Dept., UC Berkeley
// Jan. 5, 1990

// SCCS version identification
// @(#)Scheduler.cc	1.8	1/15/90


#include "type.h"
#include "Scheduler.h"
#include "Fraction.h"
#include "Output.h"
#include "StringList.h"

extern Error errorHandler;

/*******************************************************************
		Members of SDFSchedule
*******************************************************************/

	////////////////////////////
	// operator char*
	////////////////////////////

// Display a schedule

SDFSchedule :: operator char* () {
	StringList out;
	out = "SDF SCHEDULE:\n";
	reset();
	for (int i = size(); i>0; i--) {
		out += nextBlock().readFullName();
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

int SDFScheduler :: run (Block& galaxy, int numIterations = 1) {

   int i,j;
   SDFStar* currentStar;

   // assume the schedule has been set by the setup member
   // Adjust the schedule pointer to point to the beginning of the schedule.
   mySchedule.reset();

   // Dummy statement to prevent compiler warning
   galaxy;

   for (j = numIterations; j>0; j--)
	for (i = mySchedule.size(); i>0; i--) {

	    // next star in the list
	    currentStar = &(SDFStar&)mySchedule.nextBlock();

	    // First get input Particles and empty output Particles
	    currentStar->getParticles();

	    // Then run the star
	    currentStar->go();
	}
}


	////////////////////////////
	// wrapup
	////////////////////////////

int SDFScheduler :: wrapup (Block& galaxy) {

   // Run the termination routines of all the atomic stars.
   for (int i = alanShepard.totalSize((Galaxy&)galaxy); i>0; i--)
        ((SDFStar&)alanShepard.nextBlock()).wrapup();
}



	////////////////////////////
	// setup
	////////////////////////////

int SDFScheduler :: setup (Block& galaxy) {

   // initialize the SpaceWalk member
   alanShepard.setupSpaceWalk((Galaxy&)galaxy);

   int i;

   SDFStar* star;
   // Begin by setting the repetitions member of each component star
   // to zero, to force it to be recomputed.
   for (i = alanShepard.totalSize((Galaxy&)galaxy); i>0; i--) {
	// Get the next atomic block and set its repetitions property
	star = &(SDFStar&)alanShepard.nextBlock();

	star->repetitions = 0;
   }
   alanShepard.setupSpaceWalk((Galaxy&)galaxy);

   // This computes the number of times each component star must be
   // run in one cycle of a periodic schedule.
   // The cast to Galaxy here assumes the Block being operated on
   // is a galaxy.
   repetitions((Galaxy&)galaxy);

   Block* atom;
   Block* dead;
   int passValue;
   int runResult;

   // Disable default scheduler optimizations
   // deferredFiring = FALSE;
   // repeatedFiring = FALSE;

   // MAIN LOOP
   do {
	// After a scheduler pass, the passValue tells us whether the
	// scheduler is finished.  After the pass, the meaning of the
	// possible values is:
	// 	2 - scheduler is finished
	// 	1 - scheduler is deadlocked
	// 	0 - more passes are required
	passValue = 2;
	// In case the deferred firing option is enabled:
	numDeferredBlocks = 0;

	// step through the Galaxy
	for (i = alanShepard.totalSize((Galaxy&)galaxy); i>0; i--) {
	    atom = &alanShepard.nextBlock();

	    // Inner loop:  if the repeatedFiring option is set,
	    // the following is repeated as long as the star can be run
	    do {
		runResult = simRunStar(*atom,deferredFiring);
		if(runResult == 0) {
		   mySchedule.append(atom);	// put block on the schedule
		   passValue = 0;	// Any zero is a successful run
		} else
		   if ((runResult == 1) && (passValue != 0)) {
			passValue = 1;	// Potential deadlock
			dead = atom;	// culprit?
		   }
	    } while (repeatedFiring && (runResult == 0));
	}

	// If the deferred firing option is set, we must now schedule
	// the deferred blocks
	for (i=0; i<numDeferredBlocks; i++) {
		runResult = simRunStar(*deferredBlocks[i], FALSE);
		if(runResult == 0) {
		   mySchedule.append(deferredBlocks[i]);
					// put block on the schedule
		   passValue = 0;	// Any zero is a successful run
		} else
		   if ((runResult == 1) && (passValue != 0)) {
			passValue = 1;	// Potential deadlock
			dead = deferredBlocks[i];	// culprit?
		   }
	}

   } while (passValue == 0);
   // END OF MAIN LOOP

   if (passValue == 1)
	errorHandler.error("DEADLOCK! Not enough delay in a loop containing:",
                                   dead->readFullName());

   // Run the initialize routines of all the atomic stars.
   for (i = alanShepard.totalSize((Galaxy&)galaxy); i>0; i--)
	((SDFStar&)alanShepard.nextBlock()).initialize();

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
	Block* currentBlock;

	// Initialize the least common multiple, which after all the
	// repetitions properties have been set, will equal the lcm
	// of the all the denominators.
	lcm = 1;

	// The following iteration occurs exactly as many times as
	// there are blocks in the universe.
	// Note that for a connected universe, this iteration would
	// entirely unnecessary.  Is there some way to get rid of it?
	// Should we allow disconnected universes?
	for(int i = alanShepard.totalSize(galaxy); i>0; i--) {

	   // Get the next atomic block:
	   currentBlock = &alanShepard.nextBlock();

	   // First check to see whether a repetitions property has
	   // been set.  If so, do nothing.  Otherwise, compute the
	   // repetitions property.
	   // The block must be an SDFStar for this to work.
	   // Should there be some error checking here?
	   if(((SDFStar*)currentBlock)->repetitions == 0) {

		// Set repetitions to 1 and set repetitions for
		// all connected block.
		((SDFStar*)currentBlock)->repetitions = 1;

		// set the repetitions property of all atomic blocks
		// connected to the currentBlock
		reptConnectedSubgraph(*currentBlock);
	   }
	}

	// Once all the repetitions properties are set, the lcm member
	// contains the least common multiple of all the denominators
	// of all the fractional repetitions.  To convert them to
	// integers, we multiply through by lcm.
	for(i = alanShepard.totalSize(galaxy); i>0; i--) {
	   // Get the next atomic block:
	   currentBlock = &alanShepard.nextBlock();

	   ((SDFStar*)currentBlock)->repetitions =
		((SDFStar*)currentBlock)->repetitions * lcm;

	   // simplify the result
	   ((SDFStar*)currentBlock)->repetitions.simplify();
	}
}


	////////////////////////////
	// reptConnectedSubgraph
	////////////////////////////

// The following function sets the repetitions property of
// all atomic blocks connected to block, directly or indirectly.
// It is assumed that block has its repetitions property already set.

int SDFScheduler :: reptConnectedSubgraph (Block& block) {
	PortHole* currentPort;

	// Step through each portHole
	for(int i = block.numberPorts(); i>0; i--) {
	   currentPort = &block.nextPort();
	   if(reptArc(*(SDFPortHole*)currentPort,
			*(SDFPortHole*)currentPort->farSidePort))
	      // recursively call this function on the farSideBlock,
	      // having determined that it has not previously been done.
	      reptConnectedSubgraph (*(*currentPort->farSidePort).blockIamIn);
	}
}


	////////////////////////////
	// reptArc
	////////////////////////////

// This function sets the repetitions property of two blocks
// on opposite sides of a connection.  It is assumed that port1
// belongs to a block that has already had its repetitions property set.
// If in addition port2 has had its repetitions property set,
// then consistency is checked, and FALSE is returned.  Otherwise,
// TRUE is returned.

int SDFScheduler :: reptArc (SDFPortHole& port1, SDFPortHole& port2){
	Fraction& nearStarRepetitions =
			((SDFStar*)(port1.blockIamIn))->repetitions;
	Fraction& farStarRepetitions =
			((SDFStar*)(port2.blockIamIn))->repetitions;
	Fraction farStarShouldBe;
	Fraction temp;

	// compute what the far star repetitions property should be.
	// Casts to Fraction ensure the result is type Fraction.
	farStarShouldBe =
	   nearStarRepetitions * (Fraction)port1.numberTokens /
	   (Fraction)port2.numberTokens;

	// Simplify the fraction
	farStarShouldBe.simplify();

	if (farStarRepetitions == (Fraction)0) {
	   // farStarRepetitions has not been set, so set it
	   farStarRepetitions = farStarShouldBe;

	   // update the least common multiple of the denominators
	   temp = (Fraction)(lcm,farStarShouldBe.denominator);
	   lcm = temp.computeGcdLcm().lcm;
	   return TRUE;
	} else {
	   // farStarRepetitions has been set, so test for equality
	   if (!(farStarRepetitions == farStarShouldBe)) {
		errorHandler.error("Inconsistent sample rate at:",
				   (port1.blockIamIn)->readFullName());
		// MUST GIVE MORE INFORMATION HERE
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

int SDFScheduler :: simRunStar (Block& atom,
				int deferFiring = FALSE,
				int updateOutputs = TRUE){

	// Consider checking here to be sure the block is atomic

	int test = notRunnable((SDFStar&)atom);
	if(test) return test;	// return if the star cannot be run

	// If we get to this point without returning, then the star can be run.

	int i;
	SDFPortHole* port;

	// An important optimization for code generation:
	// Postpone any execution of a star feeding data to another
	// star that is runnable.  Also postpone if each output Geodesic
	// has enough data on it to satisfy destination stars.
	// This is optional because it slows down the scheduling.

	if(deferFiring) {

	   // Check to see whether any destination blocks are runnable
	   for(i = atom.numberPorts(); i>0; i--) {
		// Look at the next port in the list
		port = (SDFPortHole*)&atom.nextPort();

		// Is the port an output port?
		if(port->isItOutput()) {

		   // Is the destination star runnable?
		   if(notRunnable(*(SDFStar*)port->farSidePort->blockIamIn)
			== 0) {

			// It is runnable.

			// If possible, store a pointer to the block
			// (If the list of deferredBlocks is full, then
			// the block effectively gets deferred until a
			// future pass.  This is harmless).  But at least
			// one deferred block must go on the list, or the
			// scheduler can deadlock.
			if(numDeferredBlocks < MAX_NUM_DEFERRED_BLOCKS) {
			   deferredBlocks[numDeferredBlocks] =
				port->blockIamIn;
			   numDeferredBlocks += 1;
			}

			// Give up on this star -- it's been deferred
			return 1;
		   }
		}
	   }
	   // Alternatively, the block might be deferred if its output
	   // Geodesics all have enough data on them to satisfy destination
	   // stars, even if the destination stars are not runnable.	

	   // We must detect the case that there are no output ports
	   int outputPorts = FALSE;
	   for(i = atom.numberPorts(); i>0; i--) {

		// Look at the farSidePort of the next port in the list
		port = (SDFPortHole*)(atom.nextPort().farSidePort);

		if(port->isItInput()) {

		   // The farside port is an input.  Check Particle supply
		   if(port->myGeodesic->numInitialParticles
				< port->numberTokens)

			// not enough data.  Block cannot be deferred.
			goto noDefer;

		   // Since the farside port is an input, the nearside is
		   // an output
		   outputPorts = TRUE;
		}
	   }
	   // If we get here, all destinations have enough data,
	   // and the block can be deferred, assuming there are output ports.
	   if (outputPorts) return 1;
	}

noDefer:

	// Iterate over the ports again to adjust
	// the numInitialParticles member of the geodesic.
	// Note that this should work equally well if there are no inputs.

	for(i = atom.numberPorts(); i>0; i--) {

	   // Look at the next port in the list
	   port = (SDFPortHole*)&atom.nextPort();

	   if( port->isItInput() )
		// OK to update numInitialParticles for input PortHole
		port->myGeodesic->numInitialParticles -= port->numberTokens;

	   else if (updateOutputs)
		// OK to update numInitialParticles for output PortHole
		port->myGeodesic->numInitialParticles += port->numberTokens;
	}

	// Increment noTimes
	((SDFStar&)atom).noTimes += 1;

	// Indicate that the block was successfully run
	return 0;
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

	int i;
	SDFPortHole* port;

	// Check to see whether the requisite repetitions have been met.
	if (atom.repetitions.numerator <= atom.noTimes)
	   return 2;

	// Step through all the input ports, checking to see whether
	// there is enough data on the inputs
	for(i = atom.numberPorts(); i>0; i--) {

	   // Look at the next port in the list
	   port = (SDFPortHole*)&atom.nextPort();

	   if(port->isItInput())
		// The port is an input.  Check Particle supply
		if(port->myGeodesic->numInitialParticles < port->numberTokens)
		   // not enough data
		   return 1;
	}

	// If we get to this point without returning, then the star
	// can be run.
	return 0;
}
