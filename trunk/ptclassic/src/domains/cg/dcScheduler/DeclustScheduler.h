#ifndef _DeclustScheduler_h
#define _DeclustScheduler_h
#ifdef __GNUG__
#pragma interface
#endif

/*****************************************************************
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

Programmer: Soonhoi Ha based on G.C. Sih's code

The multiprocessor scheduling routines.

*****************************************************************/

#include "Galaxy.h"
#include "DCArcList.h"
#include "DCGraph.h"
#include "DCNode.h"
#include "DCParProcs.h"
#include "DCCluster.h"
#include "DCClusterList.h"
#include "ParScheduler.h"
#include "StringList.h"
#include "MultiTarget.h"


		//////////////////////////////
		//  class DeclustScheduler  //
		//////////////////////////////

// This class is the multiprocessor scheduler for the SDF domain, which 
// implements the declustering scheduling algorithm.  See
// "Gilbert C. Sih, Edward A. Lee, "A Multiprocessor Scheduling Strategy"
//	UCB/ERL Memo M90/119 (December, 1990)   

class DeclustScheduler : public ParScheduler {
public:
	DeclustScheduler(MultiTarget *t, const char* logFile = 0) :
		ParScheduler(t,logFile), schedA(0), schedB(0) {
		INC_LOG_NEW; myGraph = new DCGraph; exGraph = myGraph; }
	~DeclustScheduler();

	// redefine
	void setUpProcs(int);

	// display schedule
	StringList displaySchedule();

	// main body of the schedule.
	int scheduleIt();

protected:
	// redefine preSchedule(): clustering and hierarchical cluster
	// grouping.
	int preSchedule();

private:
	DCGraph* myGraph;

	DCParProcs* schedA;	// working parallel schedule A
	DCParProcs* schedB;	// working parallel schedule B
	DCParProcs* bestSchedule;	// point the best schedule btw A & B.
	DCParProcs* workSchedule;	// point the work schedule btw A & B.

	// The elementary clusters
	DCClusterList EClusts;

	// cluster hierarchy
	DCClusterList combinations;

	// Returns the communication cost from the topology dependent section
	int CommCost(int sP, int dP, int smps, int ty);

// **************  ELEMENTARY CLUSTER FORMATION FUNCTIONS  ***************

	// Finds cutArcs and makes elementary clusters 
	void FindElemDCClusters();

	// parallelism detection
	int ParallelismDetection(DCNode *bnode, int direction);

	// Finds a cutArc for this NonIntersecting Branch case
	void NBranch(DCNode *BNode, DCNode *N1, DCNode *N2, int dir);

	// Finds two cutArcs for this Intersecting Branch case
	void IBranch(DCNode *Bn, DCNode *N1, DCNode *N2, DCNode *Mn, int dir);

	// Returns the finish time for the NoMerge case
	int NoMerge(DCArc *Cut, DCArc *Oth);

	// Returns the finish time for the Merge case
	int Merge(DCArc *Cut1, DCArc *Cut2, DCArc *Oth);

	// Investigates cuts for the IBranch case
	void FindBestCuts(DCArcList*,DCArcList*,DCArc*,DCArc*,DCArc*,int*);

// **************  HIERARCHICAL CLUSTER GROUPING FUNCTIONS  **************

	// Does hierarchical clustering to establish hierarchy
	int DCClusterUp(DCClusterList&);

// **************  CLUSTER HIERARCHY DECOMPOSITION FUNCTIONS  **************

	// Start breaking cluster hierarchy
	int pulldown(DCClusterList *combDCClusts, DCClusterList *Cleft);

// ******************  SCHEDULE ANALYSIS FUNCTIONS  ******************

	// Does schedule analysis on BestSchedule
	void scheduleAnalysis(DCClusterList &remDCClusts);

	// Communication reduction cluster shifting routines
	int commReduction(DCClusterList &remC, DCClusterList *slpC);

	// Tries shifting clusters from heavily loaded to lightly loaded procs
	int loadShift(DCClusterList &remC, DCClusterList *slpC);

// ******************  CLUSTER BREAKDOWN FUNCTIONS  ******************

	// Can break cluster granularity below elementary cluster level
	int clusterBreakdown();

	// Handles SLP section on a single processor
	int findBreakpaths(DCNodeList &slpsect, int &Proc);
};

#endif
