#ifndef _DLGraph_h
#define _DLGraph_h
#ifdef __GNUG__
#pragma interface
#endif

#include "ParGraph.h"
#include "DLNode.h"

/****************************************************************
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

Programmer: Soonhoi Ha
Date of last revision:

*****************************************************************/

                ///////////////////////
                //   class DLGraph   //
                ///////////////////////

// This class stores the precedence graph which is scheduled by
// Dynamic level Scheduler.  It consists of DLNodes (derived from ParNodes) 
// connected through a subset of the arcs of the ExpandedGraph.

class DLGraph : public ParGraph {

public:
	// Display the graph
	StringList display();
	
	// reset the graph for new schedules.
	// initialize runnableNodes.
	void resetGraph();

	// compute the total work of a node's descendents.
	int workAfterMe(ParNode* pd);

	// Fetch the runnable node of highest static level.
	DLNode* fetchNode()
		{ return (DLNode*) runnableNodes.takeFromFront(); }

	// decrease "unschedNodes"
	void decreaseNodes()	{ unschedNodes--; }
	int  numUnschedNodes()	{ return unschedNodes; }

	// methods for "unschedWork"
	void decreaseWork(int val) { unschedWork -= val; }
	int sizeUnschedWork()	{ return unschedWork; }

protected:
	// reset the busy flag of the DLNodes.
	virtual void resetNodes();

	// number of unscheduled nodes.
	int unschedNodes;

	// The sum of unscheduled work
	int unschedWork;

	// redefine these virtual allocator to allocate DL Nodes.
	EGNode *newNode(DataFlowStar*, int);
};

#endif
