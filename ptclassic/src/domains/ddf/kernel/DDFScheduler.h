#ifndef _DDFScheduler_h
#define _DDFScheduler_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "DynDFScheduler.h"
#include "DDFStar.h"
#include "IntState.h"
#include "RecurScheduler.h"
#include "defConstructs.h"

/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990,1991,1992,1993 The Regents of the University of California.
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

 Programmer:  Soonhoi Ha, J. Buck
 Date of creation: 8/9/90
	 revision: 1/21/91  Detect special construct (case, for, dowhile,
			    recursion) for efficient scheduling.

 Methods for the DDF Scheduler

**************************************************************************/


	////////////////////////////
	// DDFScheduler
	////////////////////////////

class DDFScheduler : public DynDFScheduler {
public:
	// set parameters for scheduler
	void setParams(int numOver, double pd, int maxbsize, int restruct) {
		DynDFScheduler::setParams(numOver,pd,maxbsize);
		restructure = restruct;
		// If restructuring is being requested, flag that we do
		// know, at this level, whether or not to use a DDF or an
		// SDF scheduler.
		if (restruct) candidateDomain = Unknown;
	}
	// my domain
	const char* domain() const ;

	// The setup function computes an DDF schedule
	// and initializes all the blocks.
	void setup();

	// Constructor sets default options
	DDFScheduler ();
	~DDFScheduler ();

	// check whether the domain is predefined construct or not.
	// Returns 0 if candiadateDomain is DDF, 2 if SDF, 1 if other
	// (like Unknown).
	int isSDFType()	;

	// reset "restructured" flag for DDFSelf star
	void resetFlag() {
		restructured = FALSE;
		DynDFScheduler::resetFlags();
	}
protected:
	void initStructures();
private:
	// candidate domain
	int candidateDomain;

	// real scheduler for other constructs
	Scheduler* realSched;

	// list of newly created wormholes of SDF domain.
	SequentialList sdfWorms;

	// cruft used for generating names for SDF galaxies.
	static int nW;
	const char* wormName();

	// decide the type of the construct.
	void detectConstruct(Galaxy&);

	// select scheduler
	void selectScheduler(Galaxy&);

	// modify the topology
	void makeSDFWormholes(Galaxy&);

	// Parameter to specify whether or not to perform restructuring.
	// Restructuring is a highly experimental facility that will
	// take a DDF galaxy and identify SDF subsystems.  The SDF scheduler
	// is then used on the subsystems.  The idea is to allow for the
	// efficiency of the SDF scheduler even within a DDF system.
	short restructure;

	// flag to be set when restructuring has already occured on the
	// galaxy to which this scheduler applies.
	short restructured;

};

#endif
