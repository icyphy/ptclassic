static const char file_id[] = "QSGraph.cc";

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

#include "QSGraph.h"
#include "QSNode.h"

// redefine the virtual methods
EGNode *QSGraph :: newNode(SDFStar* s, int i)
	{ LOG_NEW; return new QSNode(s,i); }

void QSGraph :: resetGraph() {

	// reset the runnable node list.
	findRunnableNodes();

	// update the minimum execution time if necessary
	minWork = 0;
	EGSourceIter nxtSrc(*this);
	QSNode* src;
	while ((src = (QSNode*) nxtSrc++) != 0) {
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

QSNode* QSGraph :: findTinyBlock(int limit)
{
	// If the geven limit is smaller than the minWork, return 0;
	if (limit < minWork) return (QSNode*) 0;

	// Attach a link iterator to the runnable nodes
	EGNodeListIter NodeIter(runnableNodes);
	QSNode* pd;
	QSNode *obj = 0;
	int closest = limit;

	// go through the runnable node list
	while ((pd = (QSNode*) NodeIter++) != 0) {
                // The QSNode associated with dlnk
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
int QSGraph :: smallestExTime()
{
	// Attach a link iterator to the DLNodeList
	EGNodeListIter NodeIter(runnableNodes);
	QSNode *pd;
	int small = unschedWork;

	// Iterate for all runnable nodes.
	while ((pd = (QSNode*) NodeIter++) != 0) 
		if (pd->myExecTime() < small) small = pd->myExecTime();
	
	minWork = small;
	return small;
}

