#ifndef _SDFScheduler_h
#define _SDFScheduler_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "Scheduler.h"
#include "Galaxy.h"
#include "SDFStar.h"
#include "SDFConnect.h"
#include "DataStruct.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 5/29/90
 Revisions: This code was formerly in Scheduler.h

 Methods for the SDF Scheduler

**************************************************************************/


	////////////////////////////
	// SDFSchedule
	////////////////////////////

// This class stores the constructed schedule for access at runtime.
// For now it is simply a list of pointers to SDFStars.
// Later, we will want it to be a list of lists, where each list
// has an entry specifying the number of repetitions of its components.

class SDFSchedule : public SequentialList {
public:
	// Add an element to the end of the list
	void append(SDFStar& s) { SequentialList::put(&s);}

	// Return the number of elements on the list
	int size() const {return SequentialList::size();}

	// Display the schedule
	StringList printVerbose () const;

	// Clear the data structure
	void initialize() {SequentialList::initialize();}
};

// Iterator for SDFSchedule
class SDFSchedIter : private ListIter {
public:
	SDFSchedIter(const SDFSchedule& s) : ListIter(s) {}
	SDFStar* next() { return (SDFStar*)ListIter::next();}
	SDFStar* operator++() { return next();}
	ListIter::reset;
};

class GalStarIter;

	////////////////////////////
	// SDFScheduler
	////////////////////////////

class SDFScheduler : public Scheduler {
protected:
	SDFSchedule mySchedule;
public:
	// The setup function computes an SDF schedule
	// and initializes all the blocks.
	int setup(Galaxy&);

	// The run function resumes the run where it was left off.
	int run(Galaxy&);

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

	StringList displaySchedule();

	// Constructor sets default options
	SDFScheduler ();

	// Destructor
	~SDFScheduler ();

	// timing/interation control
	void setStopTime (float limit);
	void resetStopTime (float v);

	// stopping time is floating, in general, though it is always
	// integral for SDF
	float getStopTime () {return float(numIters);}

	// scheduler Period : used when interfaced with timed domain.
	float schedulePeriod;

        // my domain
        const char* domain() const;

	// Generate code using the Target to produce the right language
	void compileRun();

protected:
	// Flag for errors detected while computing the schedule
	int invalid;

        // The following virtual protected methods are called
        // successively by the setup method. Each sets the invalid
        // flag if it encounters a problem.

        // Verify connectivity for the galaxy.
        virtual int checkConnectivity(Galaxy& galaxy);
      
        // Initialize the galaxy for scheduling.
        virtual int prepareGalaxy(Galaxy&galaxy);

        // Check that all stars belong to the right domain.
        virtual int checkStars(Galaxy &galaxy);

        // Initialize the stars for scheduling.
        virtual int prepareStars(Galaxy &galaxy);

        // set the repetitions property of each atomic block
        // in the galaxy, and recursively call the repetitions
        // function for each non-atomic block in the galaxy.
        virtual int repetitions (Galaxy& galaxy);

        // Schedule the synchronous data flow graph.
        virtual int computeSchedule(Galaxy & galaxy);

	// run one iteration of the SDF schedule
	virtual void runOnce();

private:
	int numIters;
	int numItersSoFar;

	/******************************************************
		Members used in computing the repetitions
	*******************************************************/
	// set the repetitions property of each atomic block is
	// the subgraph (which may be the whole graph) connected
	// to block.  Called by repetitions().
	int reptConnectedSubgraph(Block& block);

	// set the repetitions property of blocks on each side of
	// a connection.  Called by reptConnectedSubgraph().
	int reptArc(PortHole& port1, PortHole& port2);

	// least common multiple of the denominators of the repetitions
	// member of a connected subgraph
	int lcm;


	/******************************************************
		Members used in computing the schedule
	*******************************************************/

	// Check to see whether or not an atomic block (star or
	// wormhole) can be scheduled.

	// Returns 0 if the star is run, 1 if not, but it has not
	// been run the number of times given by repetitions, and 2 if
	// it has been run the number of times given by repetitions.

	int simRunStar(SDFStar& atom,
		       int deferFiring = FALSE);

	// The next function runs simRunStar and adds the star to
	// the schedule if appropriate
	int addIfWeCan(SDFStar& atom, int deferFiring = FALSE);

	// If deferredFiring is TRUE, then we will need a list to store
	// pointers to blocks whose firing is deferred until the end of
	// a pass.  A fixed length vector is used because the only effect
	// the length of the vector has is a mild effect on the speed
	// of the scheduler.
	SDFStar* deferredStar;

	// State of progress in computing the schedule
	int passValue;

        // Function to report on deadlock
        void reportDeadlock(GalStarIter&);
};

#endif
