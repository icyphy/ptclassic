#ifndef _SDFScheduler_h
#define _SDFScheduler_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "Scheduler.h"
#include "Galaxy.h"
#include "GalIter.h"
#include "SDFStar.h"
#include "SDFPortHole.h"
#include "DataStruct.h"

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

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 5/29/90
 Revisions: This code was formerly in Scheduler.h

 Methods for the SDF Scheduler

**************************************************************************/


	////////////////////////////
	// SDFSchedule
	////////////////////////////

// This class stores the constructed schedule for access at runtime.
// For now it is simply a list of pointers to DataFlowStars.
// Later, we will want it to be a list of lists, where each list
// has an entry specifying the number of repetitions of its components.

class SDFSchedule : private SequentialList {
    friend class SDFSchedIter;
public:
	// Add an element to the end of the list
	void append(DataFlowStar& s) { SequentialList::put(&s);}

	// Return the number of elements on the list
	SequentialList::size;
	SequentialList::initialize;

	// Display the schedule
	StringList printVerbose () const;
};

	////////////////////////////
	// DFGalStarIter
	////////////////////////////

// DFGalStarIter -- legal to use after galaxy has been checked
// and known to have only DataFlowStars and derived classes.

class DFGalStarIter : private GalStarIter {
public:
	DFGalStarIter(Galaxy& g) 
	: GalStarIter(g) {}
	DataFlowStar* next() { return (DataFlowStar*)GalStarIter::next();}
	DataFlowStar* operator++() { return next();}
	GalStarIter::reset;
	// need a public destructor because of private derivation
	~DFGalStarIter() {}
};

	////////////////////////////
	// SDFScheduler
	////////////////////////////

class SDFScheduler : public Scheduler {
	friend class SDFSchedIter;
protected:
	SDFSchedule mySchedule;
public:
	// The setup function computes an SDF schedule
	// and initializes all the blocks.
	void setup();

	// The run function resumes the run where it was left off.
	int run();

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

	// It is deferred until some future pass if one of its output
	// Geodesics already have enough data to satisfy the destination
	// block.  This is a rather complicated optimization, arrived at
	// after much trial and error.  It significantly increases the
	// execution time of the scheduler.

	int deferredFiring;

	StringList displaySchedule();

	// Constructor sets default options
	SDFScheduler ();

	// Destructor
	~SDFScheduler ();

	// timing/interation control
	void setStopTime (double limit);
	void resetStopTime (double v);

	// stopping time is floating, in general, though it is always
	// integral for SDF
	double getStopTime () {return double(numIters);}

	// scheduler Period : used when interfaced with timed domain.
	double schedulePeriod;

        // my domain
        const char* domain() const;

	// copySchedule, not performing the SDF scheduling
	void copySchedule(SDFSchedule& s);

	// Generate code using the Target to produce the right language
	void compileRun();

protected:
	// Flag for errors detected while computing the schedule
	int invalid;

        // The following virtual protected methods are called
        // successively by the setup method. Each sets the invalid
        // flag if it encounters a problem.

        // Verify connectivity for the galaxy.
        virtual int checkConnectivity();
      
        // Initialize the galaxy for scheduling.
        virtual int prepareGalaxy();

        // Check that all stars belong to the right domain.
        virtual int checkStars();

        // set the repetitions property of each atomic block
        // in the galaxy, and recursively call the repetitions
        // function for each non-atomic block in the galaxy.
        virtual int repetitions ();

        // Schedule the synchronous data flow graph.
        virtual int computeSchedule(Galaxy& g);

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
	int lcmOfDenoms;


	/******************************************************
		Members used in computing the schedule
	*******************************************************/

	// Check to see whether or not an atomic block (star or
	// wormhole) can be scheduled.

	// Returns 0 if the star is run, 1 if not, but it has not
	// been run the number of times given by repetitions, and 2 if
	// it has been run the number of times given by repetitions.

	int simRunStar(DataFlowStar& atom,
		       int deferFiring = FALSE);

	// The next function runs simRunStar and adds the star to
	// the schedule if appropriate
	int addIfWeCan(DataFlowStar& atom, int deferFiring = FALSE);

	// If deferredFiring is TRUE, then we will need a list to store
	// pointers to blocks whose firing is deferred until the end of
	// a pass.  A fixed length vector is used because the only effect
	// the length of the vector has is a mild effect on the speed
	// of the scheduler.
	DataFlowStar* deferredStar;

	// State of progress in computing the schedule
	int passValue;

        // Function to report on deadlock
        void reportDeadlock(DFGalStarIter&);
};

// Iterator for SDFSchedule
// also works with SDFScheduler
class SDFSchedIter : private ListIter {
public:
	SDFSchedIter(const SDFSchedule& s) : ListIter(s) {}
	SDFSchedIter(SDFScheduler& sch) : ListIter(sch.mySchedule) {}
	DataFlowStar* next() { return (DataFlowStar*)ListIter::next();}
	DataFlowStar* operator++() { return next();}
	ListIter::reset;
};

#endif
