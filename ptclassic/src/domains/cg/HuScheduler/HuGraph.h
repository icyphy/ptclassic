#ifndef _QSGraph_h
#define _QSGraph_h
#ifdef __GNUG__
#pragma interface
#endif

#include "DLGraph.h"
#include "QSNode.h"

/****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

Programmer: Soonhoi Ha
Date of last revision:

*****************************************************************/

                //////////////////////
                //   class QSGraph   //
                //////////////////////

// This class stores the precedence graph which is scheduled by
// Hu's level scheduler.  It consists of QSNodes (derived from ParNodes) 
// connected through a subset of the arcs of the ExpandedGraph.

class QSGraph : public DLGraph {

public:
	// reset the graph for new schedules.
	// initialize runnableNodes.
	void resetGraph();

	// find a runnable block whose execution time is closest to
	// the given limit.
	QSNode* findTinyBlock(int limit);

	// return the smallest execution time among the runnable nodes.
	int smallestExTime();
	int getMinWork() { return minWork; }

	// redefine these virtual allocator to allocate DL Nodes.
	EGNode *newNode(DataFlowStar*, int);

private:
	// Minimum execution time among all runnable nodes.
	int minWork;
};

#endif
