#ifndef _DDFClustSched_h
#define _DDFClustSched_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "DDFScheduler.h"
#include "DDFStar.h"
#include "IntState.h"
#include "SDFCluster.h"

/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

 Programmer:  Soonhoi Ha, E. A. Lee

 Methods for the DDFClustSched: scheduler performing the clustering of
 SDF stars.

**************************************************************************/


	////////////////////////////
	// DDFClustSched
	////////////////////////////

class DDFClustSched : public DDFScheduler {
public:
	// The setup function computes a DDF schedule
	// and initializes all the blocks.
	void setup();

	// run function
	/* virtual */ int run();

	// Constructor sets default options
	DDFClustSched (const char* log = 0);
	~DDFClustSched ();

	// reset "restructured" flag (used by DDFSelf star to
	// to implement recursion.
	void resetFlag() {
		restructured = FALSE;
		DynDFScheduler::resetFlags();
	}

	// Turn on or off the feature where the scheduler runs in one
	// iteration until it deadlocks.
	void deadlockIteration(int flag) {runUntilDeadlock = flag;}

protected:
	// Create SDF clusters.
	void initStructures();

	// A source is a star with no inputs.
	/* virtual */ int isSource(Star&); 

private:
	// logging
	const char* logFile;
	ostream* logstrm;

	// flag to be set when restructuring has already occured on the
	// galaxy to which this scheduler applies.
	short restructured;

	////////////////////////////////////////////////////////
	// members for SDF clustering: hidden from the outside
	////////////////////////////////////////////////////////

	// candidate domain
	// if 0, DDF domain, if 1, SDF domain
	int amISDF;

	// real scheduler for other constructs
	Scheduler* realSched;

	// SDFClusterGals if restructuring is performed
	SDFClusterGal* cgal;

	// select scheduler
	void selectScheduler(SDFClusterGal&);

	// modify the topology by clustering SDFStars.
	//
	int makeSDFClusters(SDFClusterGal&);
	int expandCluster(SDFCluster*, SDFClusterGal*, SDFClusterBag*);

	// Check the input ports and return the state of the actor.
	// If it is enabled and non-deferrable, return 1.
	// If it is a non-source, enabled and deferrable, return 2.
	// Otherwise, return 0.
	// An actor is deferrable if all its output connections already
	// have enough data to enable the downstream actor.
	//
	int enabledState(SDFCluster*);

	// Check whether a star is blocked by missing inputs.
	//
	int blockedOnInput(SDFCluster* );
	
	// check whether output has enough tokens to fire the destination
	// actor. If yes, deferrable.
	int isOutputDeferrable(SDFCluster*);

	// Check to see whether a pragma is registered for the star associated
	// with an atomic cluster.  If so, parse that pragma and store it in
	// the star.  Also, add a pointer to the star to the pragmaStars list.
	int pragmaRegistered(SDFAtomCluster* as);

	// List used to store pointers to the stars that have pragmas
	// registered.  This allows for efficient access at runtime.
	// Note that every element appended to this list must be a
	// DataFlowStar*.
	SequentialList pragmaStars;

	// This flag indicates whether an iteration should consist
	// of running until deadlock.  By default, it is FALSE.
	int runUntilDeadlock;
	
	// Flags associated with blocks.
	enum {
	  visit = 0,
	  firings = 1,
	  iter = 2
	};
};

#endif
