#ifndef _Scheduler_h
#define _Scheduler_h 1

#include "type.h"
#include "Galaxy.h"
#include "SpaceWalk.h"
#include "Star.h"
#include "DataStruct.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 1/17/89
 Revisions:

 Scheduler sets up the execution by determining the order in which
 Blocks are executed. This is done in two phases -- setup and
 run

 At present only an SDF scheduler is implemented

**************************************************************************/

	////////////////////////////
	// Scheduler
	////////////////////////////

class Scheduler {
public:
	// The dummy expressions below are just to prevent compiler warnings
	// about galaxy being unused.
	virtual int setup(Block& galaxy) {&galaxy;}
	virtual int run(Block& galaxy) {&galaxy;}
	virtual int wrapup(Block& galaxy) {&galaxy;}

protected:
	// The following member is used to visit all atomic blocks
	// (stars and wormholes) in the galaxy exacly once each
	// It must be initialized in the setup() method.
	SpaceWalk alanShepard;
};

	////////////////////////////
	// SDFSchedule
	////////////////////////////

// This class stores the constructed schedule for access at runtime.
// For now it is simply a list of pointers to blocks.
// Later, we will want it to be a list of lists, where each list
// has an entry specifying the number of repetitions of its components.

class SDFSchedule : SequentialList {
public:
	// Add an element to the end of the list
	void append(Block* b) { SequentialList::put(b);}

	// Return the number of elements on the list
	int size() {return SequentialList::size();}

	// Return the next block on the list
	Block& operator ++ () {return *(Block*) next(); }

	// Same as operator ++, but alternative format
	Block& nextBlock () {return *(Block*) next(); }

	// Reset the last reference pointer so that accesses start
	// at the head of the list
	void reset() {SequentialList::reset();}

	// Display the schedule
	operator char* ();

	// Clear the data structure
	void initialize() {SequentialList::initialize();}
};


	////////////////////////////
	// SDFScheduler
	////////////////////////////

// Constant used by the SDF scheduler
// The value may have a minor effect on the speed of the scheduler,
// but will not affect the correctness.  Changing the value may result
// in different (all correct) schedules.
#define MAX_NUM_DEFERRED_BLOCKS 10

class SDFScheduler : public Scheduler {

	SDFSchedule mySchedule;
public:
	// The setup function computes an SDF schedule
	// and initializes all the blocks.
	int setup(Block& galaxy);

	// The run function resumes the run where it was left off.
	int run(Block& galaxy);

	// The wrapup function runs the termination routines of all the
	// atomic blocks
	int wrapup(Block& galaxy);

	// Options for the scheduler are specified by setting
	// various Booleans.  The repeatedFiring option will schedule
	// a block several times in a row, if possible.  This consumes
	// more memory, but may make the scheduler much faster.
	// if used in combination with the deferredFiring option, the effect is
	// exactly the opposite: memory usage may be decreased, making
	// static buffering more effective.
	int repeatedFiring;

	// The scheduler makes repeated
	// passes through the list of atomic blocks in the Universe or
	// WormHole.  However, the order in which it looks at the blocks
	// to determine whether they are ready to be fired is arbitrary,
	// so the choice of firing order is not always the best.
	// The deferredFiring option tries to put off the firing of
	// any block as long as possible.  The motivation is to minimize
	// the buffer sizes, to maximize the effectiveness of static
	// buffering.  On each pass, a block is
	// deferred until after the pass has been completed if any
	// block that it feeds data to can be fired.
	// It is deferred until some future pass if all of its output
	// Geodesics already have enough data to satisfy all the destination
	// blocks.  This is a rather complicated optimization, arrived at
	// after much trial and error.  It significantly increases the
	// execution time of the scheduler.
	int deferredFiring;

	char* displaySchedule() {return mySchedule.operator char* ();}

	// Constructor sets default options
	SDFScheduler () {
		repeatedFiring = TRUE;
		deferredFiring = TRUE;
	}
private:
	/******************************************************
		Members used in computing the repetitions
	*******************************************************/
	// set the repetitions property of each atomic block
	// in the galaxy, and recursively call the repetitions
	// function for each non-atomic block in the galaxy.
	int repetitions (Galaxy& galaxy);

	// set the repetitions property of each atomic block is
	// the subgraph (which may be the whole graph) connected
	// to block.  Called by repetitions().
	int reptConnectedSubgraph(Block& block);

	// set the repetitions property of blocks on each side of
	// a connection.  Called by reptConnectedSubgraph().
	int reptArc(SDFPortHole& port1, SDFPortHole& port2);

	// least common multiple of the denominators of the repetitions
	// member of a connected subgraph
	int lcm;


	/******************************************************
		Members used in computing the schedule
	*******************************************************/

	// Check to see whether or not an atomic block (star or
	// wormhole) can be scheduled by checking the numInitialParticles
	// member of the input geodesics and comparing it to the
	// numberTokens member of the input PortHoles.  If the
	// block can be scheduled, then the numInitialParticles member of
	// the inputs is updated by subtracting the numberTokens.
	// Note that by the time the scheduler finishes,
	// numInitialParticles will have been restored to its initial value.
	// If the updateOutputs flag is TRUE (the default), then
	// the numInitialParticles member of the output geodesics
	// is also updated.
	// (For parallel schedules, this update will be deferred).
	// Returns 0 if the star is run, 1 if not, but it has not
	// been run the number of times given by repetitions, and 2 if
	// it has been run the number of times given by repetitions.

	int simRunStar(Block& atom,
		       int deferFiring = FALSE,
		       int updateOutputs = TRUE);

	// Determine whether a star can be run (checking the
	// numInitialParticles
	// member of the geodesic and the noTimes member of the block).
	// Returns 0 if the star is runnable, 1 if not, but has not
	// been run the number of times given by repetitions, and 2 if
	// it has been run the number of times given by repetitions.
	int notRunnable(SDFStar& atom);

	// If deferredFiring is TRUE, then we will need a list to store
	// pointers to blocks whose firing is deferred until the end of
	// a pass.  A fixed length vector is used because the only effect
	// the length of the vector has is a mild effect on the speed
	// of the scheduler.
	int numDeferredBlocks;
	Block* deferredBlocks[MAX_NUM_DEFERRED_BLOCKS];
};

#endif
