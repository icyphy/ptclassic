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

                        ////////////////////////
                        ///  sortedInsert  ///
                        ////////////////////////

// Insert a ParNode into the EGNodeList in sorted order.
// Redefine it to sort by timeTBS first and static Level next.

void HuGraph::sortedInsert(EGNodeList& nlist, ParNode *node, int) {

	HuNode*  pd = (HuNode*) node;

	// Attach a link iterator to the runnableNodes
	EGNodeListIter NodeIter(nlist);
	EGNodeLink* nl;
	EGNodeLink* tmp = nlist.createLink(node);

	int nodeT = pd->availTime();
	int nodeSL = node->getLevel();  // The StaticLevel of node

	// Find the correct location for node in the list
	while ((nl = NodeIter.nextLink()) != 0) {
		HuNode* pd = (HuNode*) nl->node();
		// Sort earliest timeTBS first
		if (pd->availTime() > nodeT) break;
		else if (pd->availTime() < nodeT) continue;
		else if (pd->getLevel() <= nodeSL) break;
	}

	if (nl) nlist.insertAhead(tmp, nl);
	else    nlist.appendLink(tmp);
}

// HuNode specific reset.
void HuGraph :: resetNodes() {

	EGIter nxtNod(*this);
	HuNode* node;

	while ((node = (HuNode*) nxtNod++) != 0) {
		node->resetVisit();
		node->resetWaitNum();
		node->setAvailTime(0);
		node->setPreferredProc(0);
	}
}
