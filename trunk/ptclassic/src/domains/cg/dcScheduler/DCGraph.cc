static const char file_id[] = "DCGraph.cc";
/****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

Programmer: Soonhoi Ha based on G.C. Sih's work.
Date of last revision: 5/92 

*****************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "GalIter.h"
#include "DCGraph.h"
#include "type.h"
#include "streamCompat.h"
#include "Error.h"

EGNode *DCGraph::newNode(DataFlowStar *s, int i)
	{ LOG_NEW; return new DCNode(s,i); }

                        /////////////////////////
                        ///  initializeGraph  ///
                        /////////////////////////
// This function sets the following properties of the graph:
// 1) NodeCount : The number of nodes (star invocations) in the graph
// 2) DCNodeList : The list of DCNodes sorted largest static level first 
// 3) BranchNodes : A list of DCNodes with more than one descendant
// 4) MergeNodes : A list of DCNodes with more than one ancestor
// 5) runnableNodes : A list of the initially runnable DCNodes
//
// and the following properties for each node in the graph:
// 1) StaticLevel : The longest path in execution time from the node
//		to the end of the graph (over all the endnodes). 
// 2) TClosure : list of Merge nodes reachable from the node
// 3) RTClosure : list of Branch node reachable to the node
// 4) tempAncs : A copy of the set of ancestors 
// 5) tempDescs : A copy of the set of descendants

int DCGraph::initializeGraph() {

	if (ParGraph::initializeGraph() == 0) return FALSE;

	// initialize members
	clustNumber = 0;

	cutArcs.initialize();
	findRunnableNodes();

	if (logstrm)
           *logstrm << "Finished assigning levels, starting sortDCNodes\n";

        // Sort node lists, sortedNodes, BranchNodes, MergeNodes.
        sortDCNodes();

	if (logstrm)
		*logstrm << "Finished sorting, starting assignTC\n";

        // Set the transitive closure for each node. 
        assignTC();

	if (logstrm)
		*logstrm << display(); 	// Display the graph

	return TRUE;
}


			//////////////////
			///  assignTC  ///
			//////////////////
// Sets the transitive closure for each node in the DCGraph, which is
//	the set of nodes reachable through a directed path from the node.

void DCGraph::assignTC() {

	DCNodeList RevList;
	DCNodeListIter NxtNod(sortedNodes);   // Attach to the sorted nodelist
	DCNode *pg;

        // Make tempAncs and tempDescs for reverse connection.
        replenish(1);

	// Assign each node in the sorted DCNodeList a SLRank. (1 is highest) 
	while ((pg = NxtNod++) != 0) {

		RevList.insert(pg);

		// Assign reverse transitive closure
  		SingleNodeTC(pg, 1);
	}

	if (logstrm)
		*logstrm << "assigned reverse transitive closure\n";

        // Make tempAncs and tempDescs for normal connection.
	replenish(0);
	NxtNod.reconnect(RevList);

	// Go through DCNodeList backwards, assigning TC's to each node 
  	while ((pg = NxtNod++) != 0) {
  		SingleNodeTC(pg, 0);
  	}

	if (logstrm)
		*logstrm << "assigned forward transitive closure\n";
}

			//////////////////////
			///  SingleNodeTC  ///
			//////////////////////
// Sets the TClosure (transitive closure) for the node as a list of integers.
// If direction = 1, reverse transitive closure.

void DCGraph::SingleNodeTC(DCNode *node, int direction) {
        DCDescendantIter diter(node); // iterator for descs
        DCNode* pnode;

        // Go through descendants and take union of TClosure's
        while ((pnode = diter++) != 0) {

		if (direction == 1) {
			if (pnode->RTClosure.size())
			    mergeClosure(node->RTClosure, pnode->RTClosure);
		} else {
			if (pnode->TClosure.size())
			    mergeClosure(node->TClosure, pnode->TClosure);
		}

		if (pnode->amIMerge()) {
			if (direction == 1) {
				sortedInsert(node->RTClosure, pnode, 1);
			} else {
				sortedInsert(node->TClosure, pnode, 1);
			}
		}
        }
}

			//////////////////////
			///  mergeClosure  ///
			//////////////////////
// Add the TClosure of the second node into the TClosure of the first node
void DCGraph :: mergeClosure(DCNodeList& first, DCNodeList& second) {
	DCNodeListIter niter(second);
	DCNode* pnode;

	while ((pnode = niter++) != 0) 
		sortedInsert(first, pnode, 1);
}

			//////////////////
			///  copyInfo  ///
			//////////////////
// save scheduling information of the nodes
void DCGraph::copyInfo() {
	DCIter Noditer(*this);	// Attach iterator to the DCGraph
	DCNode *nodep;
	while ((nodep = Noditer++) != 0) {
		nodep->saveInfo();
	}
}

			/////////////////////
			///  sortDCNodes  ///
			/////////////////////
// Sort the nodes by StaticLevel into data member DCNodeList.

void DCGraph::sortDCNodes() {
	DCIter nxtNod(*this);	// Attach an iterator to the DCGraph
	DCNode *pg;

	// First make sure the list is clear.
	sortedNodes.initialize();

	// Do an insertion sort for each of the nodes
        while ((pg = nxtNod++) != 0) {
		sortedInsert(sortedNodes, pg, 1);

		// Branchnodes are sorted smallest StaticLevel first
		if (((pg->descendants).size()) > 1)
			sortedInsert(BranchNodes, pg, 0); // Branch node

		// MergeNodes are sorted largest StaticLevel first
		if (((pg->ancestors).size()) > 1) 
			sortedInsert(MergeNodes, pg, 1); // Merge node
		 
	}
}

			/////////////////
			///  display  ///
			/////////////////
// Displays information about the precedence graph

StringList DCGraph::display() {
	StringList out;

	out += ExpandedGraph :: display();

	out += "\nThe DCGraph has initNodes\n";
	EGNodeListIter nextSource(this->runnableNodes);
	DCNode *source;

	while ((source = (DCNode*) nextSource++) != 0) {
    		out += source->print();
  	}

	out += "\nThe DCGraph has BranchNodes\n";
	DCNodeListIter nxtN(BranchNodes);
	DCNode *bn;
	while ((bn = nxtN++) != 0) {
    		out += bn->print();
	}

	nxtN.reconnect(MergeNodes);
	out += "\nThe DCGraph has MergeNodes\n";
	while ((bn = nxtN++) != 0) {
    		out += bn->print();
	}

	return out;
}

			///////////////////////
			///  intersectNode  ///
			///////////////////////

// return the intersecting node of two transitive closures.
DCNode* DCGraph :: intersectNode(DCNode* D1, DCNode* D2, int direction) {
	DCNodeList& first = direction? D1->RTClosure: D1->TClosure;
	DCNodeList& second = direction? D2->RTClosure: D2->TClosure;
	DCNodeListIter fiter(first);
	DCNodeListIter siter(second);
	DCNode* nf, *ns;
	DCNode* last = 0;

	while ((nf = fiter++) != 0) {
		siter.reset();
		while ((ns = siter++) != 0) {
			if (nf == ns) {
				last = nf;
				if (!direction) return last;
			}
		}
	}
	return last;
}
		
int isMember(DCNodeList&, EGNode*);

			//////////////////////
			///  traceArcPath  ///
			//////////////////////
// S is the source node, which is a descendant of the branch node
// If NBranch case, D = 0.
// If IBranch case, D is merge node.
// If dir = 0, doing BranchAnalysis
// If dir = 1, doing MergeAnalysis

DCArcList* DCGraph::traceArcPath(DCNode *branch, DCNode *S, DCNode *D, int dir)
{
	int samples, behind = 0, ahead = 0;
	LOG_NEW; DCArcList* alist = new DCArcList;
	DCArc *narc;

	// A list of DCNodes from source node to destination node
	//      excluding the destination node.
	DCNodeList *dlist = tracePath(S, D, dir);

	DCNodeListIter iter(*dlist);
	DCNode* head;
	DCNode *tail = branch;

	// Add up the execution times of the nodes (don't count branch)
	while ((head = iter++) != 0) {
		ahead += head->getExTime();
	}

	iter.reset();

	// Make an arc for each pair of nodes and initialize triplet
	// The arc is from tail to head
	while ((head = iter++) != 0) {

		if (dir== 0)
			samples = tail->getSamples(head);
		else
			samples = head->getSamples(tail);

		LOG_NEW; narc = new DCArc(tail, head, behind, samples, ahead);

		alist->append(narc);

		behind += head->getExTime();
		ahead -= head->getExTime();
		tail = head;
	}
        
	if (logstrm) {
		*logstrm << "arc trace between " << S->print();
		*logstrm << " and ";
		if (D) *logstrm << D->print();
		else   *logstrm << "end";
		*logstrm << " is;\n";
		*logstrm << alist->print();
	}

        LOG_DEL; delete dlist;  // Finished with node path trace
        return alist;
}

			///////////////////
			///  tracePath  ///
			///////////////////
// This function traces a path of DCNodes between the two given nodes 
//	and returns a pointer to this path (a list of DCNodes).
// Remember to deallocate this list in the calling function!
// If end = 0, it means trace a path to the end of the graph.
// Otherwise it traces a path to end, but not including end.
// If flag = 0, doing Branch Analysis, if flag = 1, doing Merge Analysis
   
DCNodeList *DCGraph::tracePath(DCNode *start, DCNode *end, int flag)
{
	// Create a new DCNodeList to hold the path
	LOG_NEW; DCNodeList *pathtrace = new DCNodeList;
	DCNode *pgn = start;

	pathtrace->append(pgn); // Stick the start node in

	while ((pgn = extendPath(pgn, end, flag)) != end) {
		if (pgn == 0) break;	
		pathtrace->append(pgn);
	}
	if (flag == 1) pathtrace->append(end);	// IBranch case.
	return pathtrace;
}

			////////////////////
			///  extendPath  ///
			////////////////////
// Extends a path by one node along the longest path from the given
//	node to the given destination node.
// If dest = 0, we extend the longest path from node to the end
//	of the graph by one step.
// If flg = 0, look in TClosure.  If flg = 1, look in RTClosure.

DCNode *DCGraph::extendPath(DCNode *node, DCNode *dest, int flg)
{
	DCDescendantIter nxt(node);
	DCNode *pg;

	// First check if this is the Nonintersecting Branch case
	// where we want to return the descendant with highest static level.
	// Note that tempDescs is already sorted with the level.
	if (dest == 0) {
		if ((pg = nxt++) != 0)
			return pg;
		else
			return 0;
	}
				
	int numb;

	// Go through the descendants of node, and return the node with
	// highest level which has dest in its transitive closure.
	// Note that (R)TClosure is already sorted with the level.
	while ((pg = nxt++) != 0) {

		if (flg == 0)
			numb = isMember(pg->TClosure,dest);
		else
			numb = isMember(pg->RTClosure,dest);

		// Check if destnum is in the TC of the descendant
		if (numb == 1) {
			return pg;
		}
	}
	return 0;	// Reached the end of the graph
}

// return TRUE if the node list contains the given node.
int isMember(DCNodeList& nlist, EGNode* n) {

	DCNodeListIter niter(nlist);
	DCNode* nxt;

	while ((nxt = niter++) != 0) {
		if (n == nxt) return TRUE;
	}
	return FALSE;
}

			//////////////////////
			///  genDCClustName  ///
			//////////////////////
const char* DCGraph :: genDCClustName(int type) {
	char buf[20];
	if (type == 0) {
		sprintf (buf, "ElemDCClust%d", clustNumber++);
	} else { 
		sprintf (buf, "MacroDCClust%d", clustNumber++);
	}
	return hashstring(buf);
}

			//////////////////////////
			///  formElemDCClusters  ///
			//////////////////////////

// from the cutArc information, form elementary clusters
void DCGraph :: formElemDCClusters(DCClusterList& EClusts) {

	// removeCutArcs will fill this list with initially runnable nodes.
	DCNodeList starters;
	starters.initialize();

	removeCutArcs(starters);

	// make clusters
	DCNodeList* cnodes;
	DCNodeListIter startList(starters);
	DCNode* startNode;

	while ((startNode = startList++) != 0) {
		if (startNode->alreadyVisited() == 0) {
			LOG_NEW; cnodes = new DCNodeList;
			addToDCCluster(startNode, cnodes);
		
			// make clusters
			LOG_NEW; DCCluster* newC = new DCCluster(cnodes);
			newC->setName(genDCClustName(0));
			EClusts.insertSorted(newC);
		}
	}

	// set inter cluster arcs
	setInterclusterArcs();
}

                        //////////////////////
                        ///  addToDCCluster  ///
                        //////////////////////
void DCGraph :: addToDCCluster(DCNode* n, DCNodeList* nlist) {
	if (n->alreadyVisited()) return;

	// mark visit flag.
	n->beingVisited();

	// put the node into the cluster
	nlist->insert(n);

	// go through tempDescs and tempAncs
	DCDescendantIter diter(n);
	DCNode* en;
	while ((en = diter++) != 0)
		addToDCCluster(en, nlist);

	DCAncestorIter aiter(n);
	while ((en = aiter++) != 0)
		addToDCCluster(en, nlist);
}

                        ///////////////////////
                        ///  removeCutArcs  ///
                        ///////////////////////
// Removes the cutArcs from the graph by changing tempAncs and tempDescs
// Fills the given list with the initially runnable nodes.
// It also clears the visit flag of all nodes.
void DCGraph::removeCutArcs(DCNodeList& elist) {

	DCArcIter iter(cutArcs);
	DCArc *arc;
	DCNode *src, *sink;

	while ((arc = iter++) != 0) {
		src = arc->getSrc();    // The source node of the arc
		sink = arc->getSink();  // The sink node of the arc
		src->tempDescs.remove(sink);
		sink->tempAncs.remove(src);
	}

	DCNodeListIter niter(sortedNodes);
	DCNode *node;
	while ((node = niter++) != 0) {
		node->resetVisit();		// clear the visit flag.
		if (node->tempAncs.size() == 0)
			elist.append(node);
	}
}

			/////////////////////////////
			///  setInterclusterArcs  ///
			/////////////////////////////
void DCGraph::setInterclusterArcs() {

	DCArcIter iter(cutArcs);
	DCArc *arc;

	while ((arc = iter++) != 0) {
		DCNode* src = arc->getSrc();
		DCCluster* srcC = src->elemDCCluster;
		DCNode* sink = arc->getSink();
		DCCluster* sinkC = sink->elemDCCluster;
		if (srcC != sinkC) {    // Intercluster arcs
			int num = src->getSamples(sink);
			sinkC->addArc(srcC,num);
			srcC->addArc(sinkC,num);
		}
	}
}

			//////////////////////
			///  computeScore  ///
			//////////////////////
// Returns a score which indicates how good a candidate the given
//      cluster is for switching onto another processor.
// Returns integer (#samples passed offproc) - (#samples passed onproc)
// The cluster property of each node was set to the proper level in the
//      hierarchy by routine setDCClusters, which was called in scheduleAnalysis.

void DCGraph::computeScore() {
	// set the score.
        DCArcIter iter(cutArcs);
        DCArc *arc;

        while ((arc = iter++) != 0) {
                DCNode* n1 = arc->getSrc();
                DCNode* n2 = arc->getSink();
                DCCluster* sclust = n1->cluster;
                DCCluster* dclust = n2->cluster;
		if (sclust == dclust) continue;	// in the same cluster? ignore.

		int num = n1->getSamples(n2);
		if (sclust->getProc() == dclust->getProc()) {
			sclust->setScore(sclust->getScore() - num);
			dclust->setScore(dclust->getScore() - num);
		} else {
			sclust->setScore(sclust->getScore() + num);
			dclust->setScore(dclust->getScore() + num);
		} 
        }
}

			///////////////////
			///  commProcs  ///
			///////////////////
// find the procs that clust communicates with

void DCGraph::commProcs(DCCluster *clust, int* procs) {

	DCArcIter iter(cutArcs);
	DCArc *arc;

	// Go through the cut-arcs
	while ((arc = iter++) != 0) {
		DCNode* n1 = arc->getSrc();
		DCNode* n2 = arc->getSink();
		DCCluster* sclust = n1->cluster;
		DCCluster* dclust = n2->cluster;
		if (sclust == dclust) continue;

		if (sclust == clust) procs[dclust->getProc()] = 1;
		else if (dclust == clust) procs[sclust->getProc()] = 1;
	}
}

