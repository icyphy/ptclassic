static const char file_id[] = "DLGraph.cc";

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

#include "DLGraph.h"
#include "EGGate.h"

// redefine the virtual methods
EGNode *DLGraph :: newNode(DataFlowStar* s, int i)
	{ LOG_NEW; return new DLNode(s,i); }

                        ////////////////////
                        ///  resetGraph  ///
                        ////////////////////

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

	pd->beingVisited();

	// iterate for descendents.
	while ((dflink = desciter++) != 0) {
		node = (ParNode*)dflink->farEndNode();
		if (node->alreadyVisited()) continue;
	        total += workAfter(node);
	}
	total += pd->getExTime();
	return total;
}

