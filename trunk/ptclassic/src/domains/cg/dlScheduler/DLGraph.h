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

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

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
	void resetNodes();

	// number of unscheduled nodes.
	int unschedNodes;

	// The sum of unscheduled work
	int unschedWork;

	// redefine these virtual allocator to allocate DL Nodes.
	EGNode *newNode(DataFlowStar*, int);
};

#endif
