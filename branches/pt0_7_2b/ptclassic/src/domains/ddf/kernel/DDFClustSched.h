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

Copyright (c) 1990-1994 The Regents of the University of California.
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

 Programmer:  Soonhoi Ha

 Methods for the DDFClustSched: scheduler performing the clustering of
 SDF stars

**************************************************************************/


	////////////////////////////
	// DDFClustSched
	////////////////////////////

class DDFClustSched : public DDFScheduler {
public:
	// The setup function computes an DDF schedule
	// and initializes all the blocks.
	void setup();

	// run function
	/* virtual */ int run();

	// Constructor sets default options
	DDFClustSched (const char* log = 0);
	~DDFClustSched ();

	// reset "restructured" flag for DDFSelf star
	void resetFlag() {
		restructured = FALSE;
		DynDFScheduler::resetFlags();
	}

protected:
	void initStructures();

	// do not regard an actor with initial token on the feedback arc
	// as a source. Back to the classical definition of the source.
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
	int makeSDFClusters(SDFClusterGal&);
	int expandCluster(SDFCluster*, SDFClusterGal*, SDFClusterBag*);

	// type of actor
	// if enabled and non-deferrable. return 1
	// if non-source enabled and deferrable, return 2
	// otherwise, return 0;
	int typeOfActor(SDFCluster*);

	// check the blocking condition at input
	int blockAtInput(SDFCluster* );
	
	// check whether output has enough tokens to fire the destination
	// actor. If yes, deferrable.
	int isOutputDeferrable(SDFCluster*);
};

#endif
