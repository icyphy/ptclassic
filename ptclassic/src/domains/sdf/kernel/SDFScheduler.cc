#include "type.h"
#include "Scheduler.h"
#include "SDFStar.h"
#include "Fraction.h"
#include "Error.h"
#include "SpaceWalk.h"

// TEMPORARILY:
// #include <stream.h>

extern Error errorHandler;

int SDFScheduler :: setup (Block& galaxy) {
	// Begin by setting the repetitions member of each component star
	// This computes the number of times each component star must be
	// run in one cycle of a periodic schedule.
	// The cast to Galaxy here assumes the Block being operated on
	// is a galaxy.
	repetitions((Galaxy&)galaxy);
}

// set the repetitions property of each atomic block in the galaxy.

// WARNING: For disconnected systems, the repetitions properties
// are all multiplied by the least common multiple of the denominators
// of *all* the repetitions properties, not just those is a connected
// subsystem.  Hence for disconnected systems, the relative number
// of times the subsystems are invoked may not be obvious.

int SDFScheduler :: repetitions (Galaxy& galaxy) {
	Block* currentBlock;

	// The following class is used to visit all atomic blocks
	// (stars and wormholes) in the galaxy exacly once each
	SpaceWalk alanShepard;

	// It must be initialized
	alanShepard.setupSpaceWalk(galaxy);

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
