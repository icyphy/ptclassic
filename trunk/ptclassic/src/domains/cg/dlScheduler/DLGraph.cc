static const char file_id[] = "DLGraph.cc";

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

Programmer: Soonhoi Ha
Date of last revision: 

*****************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "DLGraph.h"
#include "EGConnect.h"
#include "UserOutput.h"

// redefine the virtual methods
EGNode *DLGraph :: newNode(SDFStar* s, int i)
	{ LOG_NEW; return new DLNode(s,i); }

                        /////////////////////////
                        ///  initializeGraph  ///
                        /////////////////////////

// This function sets the following properties of the graph:
// 1) nodeCount : The number of nodes (star invocations) in the graph
// 2) runnableNodes : A list of the initially runnable DLNodes
//
// and the following property for each node in the graph:
// StaticLevel : The longest path in execution time from the node
//		to the end of the graph (over all the endnodes). 

int DLGraph::initializeGraph() {

        EGSourceIter nxtSrc(*this);
        DLNode *src;

	// Remove the arcs with delay from the ancestors and descendants
	removeArcsWithDelay();

	// initialize members
	nodeCount = 0;
	ExecTotal = 0;

        // Set the levels for each node
        while ((src = (DLNode*)nxtSrc++) != 0) {
                if (SetNodeSL(src) < 0) return FALSE;
        }
	return TRUE;
}

void DLGraph :: resetGraph() {

	findRunnableNodes();
	resetNodes();

	// reset the appropriate members for schedule.
	unschedNodes = nodeCount;
	unschedWork  = ExecTotal;
}

void DLGraph :: resetNodes() {

        // reset the busy flag of the DLNodes.
        EGIter nxtNod(*this);   // Attach an iterator to the DLGraph
        DLNode *node;

        // Visit each node in the expanded graph
        while ((node = (DLNode*)nxtNod++) != 0) {
                node->resetVisit();
		node->resetWaitNum();
        }
}

			/////////////////
			///  display  ///
			/////////////////

// Displays information about the precedence graph

StringList DLGraph::display() {

	EGIter nextNode(*this);	// Attach the iterator to the DLGraph
	DLNode *p;
	StringList out;

	// Print information about each node
	while ((p = (DLNode*)nextNode++) != 0) {
      		out += p->print();
        }

	out += "\n--------------------------";
	out += "\nThe runnable nodes are:\n";

	EGNodeListIter nextSource(runnableNodes);
	DLNode *source;

	while ((source = (DLNode*)nextSource++) != 0) {
    		out += source->print();
  	}

	return out;
}

			/////////////////////
			///  workAfterMe  ///
			/////////////////////

// compute the sum of all execution time of the descendents of the node(pd).
int workAfter(ParNode* pd);

int DLGraph :: workAfterMe(ParNode* pd)
{
        // reset the busy flag of the ParNode.
        EGIter nxtNod(*this);   // Attach an iterator to the DLGraph
        ParNode *node;

        // Visit each node in the expanded graph
        while ((node = (ParNode*)nxtNod++) != 0) {
		node->resetVisit();
	}
	
	return workAfter(pd);
}

int workAfter(ParNode* pd) {

	EGGateLinkIter desciter(pd->descendants); // iterator for descs
	EGGate *dflink;
	ParNode* node;
	int total = 0;

	if (pd->alreadyVisited())	return 0;

	// iterate for descendents.
	while ((dflink = desciter++) != 0) {
		node = (ParNode*)dflink->farEndNode();
		node->beingVisited();
	        total += workAfter(node);
	}
	total += pd->myExecTime();
	return total;
}

