static const char file_id[] = "ParGraph.cc";

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

#include "ParGraph.h"
#include "EGConnect.h"
#include "UserOutput.h"
#include "streamCompat.h"
#include "Error.h"

			//////////////////
			///  CreateMe  ///
			//////////////////
// This is the effective constructor for making an expanded graph 
// Returns TRUE if graph is okay, else returns FALSE

int ParGraph::createMe(Galaxy& galaxy) {

	nodePairs.initialize();
	myGal = &galaxy;

	if (ExpandedGraph::createMe(galaxy) == 0) {
		Error::abortRun("Cannot create expanded graph");
		return FALSE;
	}
	if (logstrm)
		*logstrm << "Created expanded graph successfully\n";

	if (initializeGraph() == 0) {
		Error::abortRun("Cannot initialize precedence graph");
		return FALSE;
	}
	return TRUE;
}

int ParGraph :: initializeGraph() { return TRUE; }

			///////////////////
			///  SetNodeSL  ///
			///////////////////

// This function finds and sets the StaticLevel for the given node,
//    where the StaticLevel is defined as the longest directed path in
//    execution time from the node to an end node (over all endnodes)
// StaticLevel, a data member of class DLNode, is initialized
//    by the constructor to 0 when the node is made.

int ParGraph::SetNodeSL(ParNode* aNodep) {

	int temp, level = 0;
	EGGateLinkIter desciter(aNodep->descendants); // iterator for descs
	EGGate *dflink;
	ParNode* node;

	aNodep->beingVisited();
	if (aNodep->getSL() != 0) {	// StaticLevel already set
		level = aNodep->getSL();
	}
	else {	// Find maximum of descendants levels 
		while ((dflink = desciter++) != 0) {
			node = (ParNode*)dflink->farEndNode();
			temp = node->getSL();
			if (temp == 0) {
				if (node->alreadyVisited()) {
				   Error::abortRun("Graph is deadlocked\n");
				   return -1;
				} 
				temp = SetNodeSL(node);
			}
	     	   	if (temp > level)
				level = temp;
		}
		desciter.reset();
		level += aNodep->myExecTime();	   // Add runtime of node
		aNodep->assignSL(level);  // Assign StaticLevel

		// update global parameter.
		nodeCount++;
		ExecTotal += aNodep->myExecTime();
	}
	aNodep->resetVisit();
	return level;
}

			////////////////////////
			///  sortedInsert  ///
			////////////////////////

// Insert a ParNode into the EGNodeList in sorted order.
// Sorts highest SL first if flag = 1, lowest SL first if flag = 0

void ParGraph::sortedInsert(EGNodeList& nlist, ParNode *node, int flag) {

	// Attach a link iterator to the runnableNodes
	EGNodeListIter NodeIter(nlist);
	ParNode *pd;
	EGNodeLink* nl;
	EGNodeLink* tmp = nlist.createLink(node);

	int nodeSL = node->getLevel();  // The StaticLevel of node

	// Find the correct location for node in the list
	while ((nl = NodeIter.nextLink()) != 0) {
		ParNode* pd = (ParNode*) nl->myNode();
		if (flag == 1) {
			// Sort highest SL first
			if (pd->getLevel() <= nodeSL) break;
		} else {
			// Sort lowest SL first
			if (pd->getLevel() >= nodeSL) break;
		}
	}
	if (nl) nlist.insertAhead(tmp, nl);
	else 	nlist.appendLink(tmp);

}

			/////////////////////////////
			///  removeArcsWithDelay  ///
			/////////////////////////////

// Remove the arcs with delay and record node pairs.
void ParGraph :: removeArcsWithDelay() {

	EGIter nextNode(*this);         // iterator to the DLGraph.
	ParNode* node;

	while ((node = (ParNode*) nextNode++) != 0) {
		EGGateLinkIter preciter(node->ancestors);
		EGGate* q;

		while((q = preciter++)!=0)
			if (q->delay() > 0) {
				q->removeMyArc();
				ParNode* p = (ParNode*) q->farEndNode();
				LOG_NEW; NodePair* pair = new NodePair(p, node);
				nodePairs.put(pair);
			}
	}
}

// compute the maximum schedule distance between node pairs.
int ParGraph :: pairDistance() {

	ListIter nextPair(nodePairs);
	NodePair* p;
	int distance = 0;

	while((p = (NodePair*) nextPair++) != 0) {
		int temp = p->getStart()->getScheduledTime() +
               p->getStart()->myExecTime() - p->getDest()->getScheduledTime();
		if (temp > distance) distance = temp;
	}
	return distance;
}

ParGraph :: ~ParGraph() {}