static const char file_id[] = "HuGraph.cc";

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

#include "HuGraph.h"
#include "HuNode.h"

// redefine the virtual methods
EGNode *HuGraph :: newNode(DataFlowStar* s, int i)
	{ LOG_NEW; return new HuNode(s,i); }

void HuGraph :: resetGraph() {

	// reset the runnable node list.
	findRunnableNodes();

	// update the minimum execution time if necessary
	minWork = 0;
	EGSourceIter nxtSrc(*this);
	HuNode* src;
	while ((src = (HuNode*) nxtSrc++) != 0) {
		if (src->myExecTime() < minWork) minWork = src->myExecTime();
		src->resetAssignedFlag(0);
	}

	resetNodes();

	// reset the appropriate members for schedule.
	unschedNodes = nodeCount;
	unschedWork  = ExecTotal;
}

			///////////////////////
			///  findTinyBlock  ///
			///////////////////////

// find a runnable block whose execution time is closest to the
// given limit.

HuNode* HuGraph :: findTinyBlock(int limit)
{
	// If the geven limit is smaller than the minWork, return 0;
	if (limit < minWork) return (HuNode*) 0;

	// Attach a link iterator to the runnable nodes
	EGNodeListIter NodeIter(runnableNodes);
	HuNode* pd;
	HuNode *obj = 0;
	int closest = limit;

	// go through the runnable node list
	while ((pd = (HuNode*) NodeIter++) != 0) {
                // The HuNode associated with dlnk
		int temp = limit - pd->myExecTime();
		if (temp >= 0 && temp < closest) {
			temp = closest;
			obj = pd;
		}
	}

	// fetch the "obj" node from the runnable list.
	if (obj) runnableNodes.remove(obj);

	return obj;
}

// Return the smallest execution time among the runnable nodes.
int HuGraph :: smallestExTime()
{
	// Attach a link iterator to the DLNodeList
	EGNodeListIter NodeIter(runnableNodes);
	HuNode *pd;
	int small = unschedWork;

	// Iterate for all runnable nodes.
	while ((pd = (HuNode*) NodeIter++) != 0) 
		if (pd->myExecTime() < small) small = pd->myExecTime();
	
	minWork = small;
	return small;
}

