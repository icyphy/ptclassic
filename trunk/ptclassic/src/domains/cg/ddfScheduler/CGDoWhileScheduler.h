#ifndef _CGDoWhileScheduler_h
#define _CGDoWhileScheduler_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "CGConScheduler.h"
#include "DoWhile.h"

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

 Scheduler for "DoWhile" construct.

**************************************************************************/

class CGMacroClusterBag;

class CGDoWhileScheduler : public CGConScheduler, public DoWhile {

public:
	// read the statistics on the number of iteration steps.
	int getStatistics();

	// void download the code
	int downLoadCode(CGStar*, int, int, int);

	// constructor
	CGDoWhileScheduler(MultiTarget* t, ostream* l): 
		CGConScheduler(t,l), body(0), cbag(0) { createSchedule(); }

protected:
	// redefine
	int examine(CGMacroClusterGal* galaxy);
	int closerExamine();
	// redefine
	int setProfile(int, int, Profile*);

	// return the execution time
	int assumeExecTime();

private:
	// add synchronization code at the end of each iteration
	// return the cost of these codes.
	int addSyncCodes(int, Profile*);

	// virtual methods to be filled for a specific target.
	int mySyncTime();

	double calcCost(int, int);	  // compute the optimal total cost.
	double costInGeometric(int, int); // in Geometric dist.
	double costInUniform(int, int);   // in Uniform dist.
	double costInGeneral(int, int);   // in General dist.
	int adjustX(int,int,int,int);	  // adjust "x" value.

	Profile* body;		 // profile of the iteration body.
	CGMacroClusterBag* cbag;
	
	// important variables for the final profile
	// "DoWhile" is same as "For" except bestK = 1 always.
	// There might be cases where bestK > 0, but not considered here.
	int tau;
	int bestX;
};

#endif
