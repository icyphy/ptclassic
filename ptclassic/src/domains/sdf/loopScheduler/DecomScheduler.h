/******************************************************************
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
 date: 5/92

Loop scheduler inside a decomposited galaxy.

*******************************************************************/

#ifndef _DecomScheduler_h
#define _DecomScheduler_h
#ifdef __GNUG__
#pragma interface
#endif

#include "SDFCluster.h"
#include "LSGraph.h"
#include "StringList.h"

class DecomGal;
class ostream;

/////////////////////////
// class DecomScheduler //
/////////////////////////

class DecomScheduler : public SDFBagScheduler {
public:
	DecomScheduler(ostream* log = 0): logstrm(log) {}

	// generate schedule
	int genSched(DecomGal*);

	// Display the schedule
	StringList displaySchedule(int depth);

	// default (for SDFScheduler::displaySchedule override)
	StringList displaySchedule() { return displaySchedule(0);}

	// Generate code using the Target to produce the right language.
	void genCode(Target&, int depth);

private:
	// Attempt to form a cluster with base node "p" in graph "g".
	// This involves examining each node adjacent to "p" and
	// choosing the one which commits the fewest invocations of
	// "p" to each of the new cluster's invocations. 
	// We guarantee that a cluster candidate does not introduce deadlock.
	void attemptMerge(LSNode &p, LSGraph &g);

	// The expanded graph of the universe whose
	// schedule we are creating. The loop scheduler
	// creates this graph and modifies it as
	// clusters are formed. 
	LSGraph myGraph;

	// Build the hierarchy of clusters. 
	void buildClusters(LSGraph &g);

	// Schedule the top-level graph in which all masters (SDF atomic
	// star or a LSCluster) have only one invocation.
	int topLevelSchedule(LSGraph &g);

	// for logging errors
	ostream* logstrm;

	// do nothing in the following method
	int computeSchedule(Galaxy&) { return TRUE; }
};

#endif
