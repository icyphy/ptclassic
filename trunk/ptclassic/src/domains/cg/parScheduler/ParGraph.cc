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
#include "streamCompat.h"
#include "Error.h"

// redefine the virtual methods
EGNode *ParGraph :: newNode(SDFStar* s, int i)
	{ LOG_NEW; return new ParNode(s,i); }

			//////////////////
			///  CreateMe  ///
			//////////////////
// This is the effective constructor for making an expanded graph 
// Returns TRUE if graph is okay, else returns FALSE

int ParGraph::createMe(Galaxy& galaxy, int selfLoopFlag) {

	nodePairs.initialize();
	myGal = &galaxy;

	if (ExpandedGraph::createMe(galaxy, selfLoopFlag) == 0) {
		Error::abortRun("Cannot create expanded graph");
		return FALSE;
	}
	if (logstrm)
		*logstrm << "Created expanded graph successfully\n";

	// reset the busy flags
	EGIter niter(*this);
	EGNode* n;
	while ((n = niter++) != 0)
		n->resetVisit();

	if (initializeGraph() == 0) {
		Error::abortRun("Cannot initialize precedence graph");
		return FALSE;
	}
	return TRUE;
}

			/////////////////////////
			///  initializeGraph  ///
			/////////////////////////

// This function sets the following properties of the graph:
// 1) nodeCount : The number of nodes (star invocations) in the graph
// 2) runnableNodes : A list of the initially runnable DLNodes
//
// and the following property for each node in the graph:
// StaticLevel : The longest path in execution time from the node
//              to the end of the graph (over all the endnodes).

int ParGraph :: initializeGraph() { 
	EGSourceIter nxtSrc(*this);
	ParNode *src;

	// Remove the arcs with delay from the ancestors and descendants
	removeArcsWithDelay();

	// initialize members
	nodeCount = 0;
	ExecTotal = 0;

	// Set the levels for each node
	while ((src = (ParNode*)nxtSrc++) != 0) {
		if (SetNodeSL(src) < 0) return FALSE;
	}
	return TRUE;
}

			//////////////////////
			///  findRunnable  ///
			//////////////////////

void ParGraph :: findRunnableNodes() {
	// reset the runnable node list.
	runnableNodes.initialize();

	EGSourceIter nxtSrc(*this);
	ParNode* src;
	while ((src = (ParNode*) nxtSrc++) != 0)
		sortedInsert(runnableNodes,src,1); // sort the runnable nodes.
}

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
				q->hideMe();
				ParNode* p = (ParNode*) q->farEndNode();
				LOG_NEW; NodePair* pair = new NodePair(p, node);
				nodePairs.put(pair);
			}
	}
}

			////////////////////////////
			///  restoreHiddenGates  ///
			////////////////////////////

// restore the removed arcs with delays.
void ParGraph :: restoreHiddenGates() {
	EGIter Nodeiter(*this);
	EGNode* node;
	while((node = Nodeiter++) != 0) {
		EGGateLinkIter hidden(node->hiddenGates);
		EGGate* g;
		while ((g = hidden++) != 0) {
			g->getLink()->removeMeFromList();
			if (g->isItInput()) {
				node->ancestors.insertGate(g,0);
			} else {
				node->descendants.insertGate(g,0);
			}
		}
	}
}

			///////////////////
			///  replenish  ///
			///////////////////
// Replenishes the tempAncs and tempDescs for each node in the DCGraph.

void ParGraph::replenish(int flag) {
	EGIter Noditer(*this);  // Attach iterator to the ParGraph
	ParNode *nodep;
	while ((nodep = (ParNode*) Noditer++) != 0) {
		nodep->copyAncDesc(this, flag);
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

ParGraph :: ~ParGraph() {
	ListIter nextPair(nodePairs);
	NodePair* p;

	while((p = (NodePair*) nextPair++) != 0) {
		LOG_DEL; delete p;
	}
}
