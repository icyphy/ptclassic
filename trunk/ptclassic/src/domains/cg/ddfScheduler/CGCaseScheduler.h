#ifndef _CGCaseScheduler_h
#define _CGCaseScheduler_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "CGConScheduler.h"
#include "Case.h"

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

 Programmer:  Soonhoi Ha

 Scheduler for "Case" construct.

**************************************************************************/

// "Taus" stores the temporary record of the schedule lengths on the 
// processors during the quasi-static scheduling.

class Taus {
public:
	int val;	// its value.
	int flag;	// Set if it will be changed.
	Taus() { val = 0; flag = 0; }
};

class CGCaseScheduler : public CGConScheduler, public Case {

public:
	// read the statistics on the number of iteration steps.
	int getStatistics();

	// download the code
	int downLoadCode(CGStar*, int, int, int);

	// constructor
	CGCaseScheduler(MultiTarget* t, ostream* l): CGConScheduler(t,l),
	     saveNum(0), prevNum(0), arcProfile(0), taus(0),  
	     pis(0), touched(0), deltas(0) {}
	~CGCaseScheduler();

protected:
	// redefinea the virtual functions for schedule preparation.
	int examine(CGMacroClusterGal* galaxy);
	int closerExamine();
	void initMembers();

	// return the assumed execution length
	int assumeExecTime();

	// redefine
	int setProfile(int, int, Profile*);

private:
	// Add control and synchronization codes.
	int addControlCodes(int, Profile*);

	// time for synchronization
	int myControlTime();
	int mySyncTime();
	
	// save of the number of arcs
	int saveNum;
	// previous numProcs
	int prevNum;

	// profiles of the branches
	Profile** arcProfile;

	// Schedule lengthes.
	Taus*	  taus;
	void initTaus(int);		// initialize "taus"
	void initDeltas(int);		// initialize "deltas"
	int decreaseTaus(int, int);	// adjust "taus" and "deltaMatrix" as
					// the scheduling routine goes by.

	// Read the probabilities from the distributions.
	double*   pis;			// branch probabilities.
	void read2Pis();		// for if-then-else only.
					// use GEOMETRIC.
	void readNPis();			// use GENERAL.

	// Flag to be set when this branch is taken into account when
	// summing up the pis.
	int* touched;

	// keep the maximum of t(i,j) - to(j), where i the index of the arc.
	// j is the index of the processor, t(i,j) is the finishTime of the
	// i-th arc on the j-th processor, and to(j) is the scheduled length
	// on the j-th processor
	int*     deltas;

	// dynamically member initialization
	void adjustMembers();

	// calculate the start times
	void calcStartTimes(int, Profile*);
};

#endif
