static const char file_id[] = "HuGraph.cc";

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

