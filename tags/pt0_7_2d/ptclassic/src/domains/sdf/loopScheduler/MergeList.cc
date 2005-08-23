static const char file_id[] = "MergeList.cc";
/******************************************************************
Version identification:
@(#)MergeList.cc	1.10	3/5/96

Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

 Programmer:  Soonhoi Ha based on Shuvra Bhattacharyya's work.
 Date of creation: 4/92

 MergeList 

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "MergeList.h"
#include "LSCluster.h"
#include "LSNode.h"
#include "LSGraph.h"
#include "ClusterNodeList.h"

///////////////////////
// gcd compute    
///////////////////////

// Compute the greatest common divisor of two positive integers -- 
// the arguments are assumed to be positive.
int euclid(int a, int b) {
	int t;
	while (a > 0) {
		t = b % a;
		b = a;
		a = t;
	}
	return b;
}

MergeLink::MergeLink(LSNode* base, LSNode* relative, int d) : DoubleLink(0) {
	base_node = base;
	adjacent_node = relative;
	direction = d;

	int basereps = base->myMaster()->repetitions;
	int relreps = relative->myMaster()->repetitions;
	gcd = euclid(basereps, relreps);

	adj_ix = relative->invocationNumber();

	preClust = 0;
	mainClust = 0;
	postClust = 0;

	setUp();
}

MergeLink::~MergeLink() {
	delete preClust;
	delete mainClust;
	delete postClust;
}

void MergeList::insertMerge(LSNode* base, LSNode* relative, int d) {
	MergeListIter nextLink(*this);
	MergeLink *m;
	LOG_NEW; MergeLink* newlink = new MergeLink(base, relative, d);
	int n1 = newlink->par_inv;
	while ((m = nextLink++) != 0) {
		int n2 = m->par_inv;
		if (n1 < n2) {
			insertAhead(newlink, m);
			return;
		}
		else if ((n1 == n2) && (newlink->adj_ix < m->adj_ix)) {
			insertAhead(newlink,m);
		}
	}
	appendLink(newlink);
}

// set up members
void MergeLink :: setUp() {
	if (direction) {
		par_node = base_node;
		son_node = adjacent_node;
	}
	else {
		par_node = adjacent_node;
		son_node = base_node;
	}
	parRep = par_node->myMaster()->repetitions;
	sonRep = son_node->myMaster()->repetitions;
	par_inv = parRep / gcd;
	son_inv = sonRep / gcd;

	delete preClust;
	delete mainClust;
	delete postClust;
	preClust = 0;
	mainClust = 0;
	postClust = 0;
}

//////////////////////////
// formRepeatedCluster
//////////////////////////	
// Make  clusters with the current MergeLink.
int MergeLink::formRepeatedCluster(LSGraph &g)
{
	// setup parent-child relationship
	setUp();

	// check for errors
	if (par_node == 0 || son_node == 0) return FALSE;

	// invocation index of base and adjacent node
	int ix = par_node->invocationNumber();
	int iy = son_node->invocationNumber();

	// attempt clustering to check whether deadlock occurs.
	// on error, deallocate all dynamic memory
	if (!preClustering(g, ix, iy) ||
	    !mainClustering(g, ix, iy) ||
	    (son_node && ! postClustering(g))) {
		setUp();
		return FALSE;
	}

	// no deadlock occurs. THEN,
	// 1. initial phase
	if (adj_ix > 1) initialPhase();
	else	g.removeCandidate(adjacent_node);

	// 2. create clusters
	createClusters(g);

	// 3. final phase
	if (par_node) finalPhase(g);

	return TRUE;
}

//////////////////////////
// preClustering
//////////////////////////	
// Make an initial cluster if it is different from the main cluster.

int MergeLink :: preClustering(LSGraph& g, int& ix, int& iy) {
	if (par_node == 0 || son_node == 0) return FALSE;

	//  set up the limit of indices
	int parLimit, sonLimit;
	if (ix + par_inv > parRep) parLimit = parRep;
	else	parLimit = ix + par_inv - 1;
	if (iy + son_inv > sonRep) sonLimit = sonRep;
	else	sonLimit = iy + son_inv - 1;

	// temporal indices
	int parIx = ix; 
	int sonIx = iy;

	int prevPar;
	// detect the candidate nodes for the initial cluster.
	LSNode* nextP = par_node;
	LSNode* nextS = son_node;
	do {
		prevPar = parIx;
		nextS = nextP->nextConnection(nextS,1);
		if (!nextS) return FALSE;
		sonIx = nextS->invocationNumber();
		nextP = nextS->nextConnection(nextP,0);
		if (!nextP) return FALSE;
		parIx = nextP->invocationNumber();
	} while ((parIx != prevPar) && (sonIx <= sonLimit) &&
		 (parIx <= parLimit));
	
	// Check whether the initial cluster needs to be made.
	if ((parIx >= parLimit) && (sonIx >= sonLimit))
			return TRUE;

	// Form the initial cluster.
	delete preClust;
	LOG_NEW; preClust = new ClusterNodeList(0);
	int tx = par_node->invocationNumber();
	while (tx <= parIx) {
		preClust->append(par_node);
		par_node = par_node->getNextInvoc();
		tx++;
	}

	// append isolated par_nodes if any.
	appendIsolatedPar(preClust);

	// insert isolated son_nodes if any.
	insertIsolatedSon(preClust);

	// insert connected son_nodes
	tx = son_node->invocationNumber();
	while (tx <= sonIx) {
		preClust->append(son_node);
		son_node = son_node->getNextInvoc();
		tx++;
	}

	// Check that the formation of this invocation will not
	// introduce a cycle in the graph.
	if (g.introducesCycle(*preClust)) {
		LOG_DEL; delete preClust;
		preClust = 0;
		return FALSE;
	}

	// new indices for the next step
	ix = par_node ? par_node->invocationNumber() : (parRep + 1);
	iy = sonIx + 1;

	return TRUE;
}
	
//////////////////////////
// mainClustering
//////////////////////////	
// The second argument is the invocation index of the par_node, and
// the third  argument is the invocation index of the son_node.
// Return FALSE, if clustering fails.

int MergeLink :: mainClustering(LSGraph& g, int ix, int iy) {

	int startX = ix, startY = iy;
	int stopX = ix + par_inv - 1;
	int stopY = iy + son_inv - 1;

	// return TRUE if no clustering can be made
	if ((stopX > parRep) || (stopY > sonRep)) return TRUE;

	// check for errors
	if (par_node == 0 || son_node == 0) return FALSE;

	// while all invocations are available,
	ClusterNodeList* clist = 0;
	LSNode* x = (LSNode*) par_node->getInvocation(startX);
	LSNode* y = (LSNode*) son_node->getInvocation(startY);
	int numClust = 0;

	while ((stopX <= parRep) && (stopY <= sonRep)) {
		LOG_NEW; clist = new ClusterNodeList(clist);
		numClust++;
		if (!mainClust) mainClust = clist;

		while (startX <= stopX) {
			clist->append(x);
			x = x->getNextInvoc();
			startX++;
		}
		while (startY <= stopY) {
			clist->append(y);
			y = y->getNextInvoc();
			startY++;
		}
	
		// check that the formation of this cluster will not
		// introduce a cycle in the graph.
		if (g.introducesCycle(*clist)) {
			LOG_DEL; delete mainClust;
			mainClust = 0;
			return FALSE;
		}

		stopX = startX + par_inv - 1;
		stopY = startY + son_inv - 1;
	}

	// Now, the clustering is successful.
	if (!mainClust) return TRUE; 	// no cluster is made.

	par_node = x;

	// If there is only one main cluster, we may want to include
	// isolated nodes into the cluster.
	if (numClust == 1) {
		// append isolated par_nodes if any.
		appendIsolatedPar(mainClust);
		
		// insert isolated son_nodes if any.
		insertIsolatedSon(mainClust);
	}

	son_node = y;

	return TRUE;
}
		
//////////////////////////
// postClustering
//////////////////////////	

int MergeLink :: postClustering(LSGraph& g) {
	if (par_node == 0 || son_node == 0) return FALSE;

	LSNode* nextSon = son_node;
	while ((nextSon->getNextInvoc()) != 0)
		nextSon = nextSon->getNextInvoc();

	LSNode* nextPar = nextSon->nextConnection(par_node,0);

	// create postClust
	delete postClust;
	LOG_NEW; postClust = new ClusterNodeList(0);

	while (par_node != nextPar) {
		postClust->append(par_node);
		par_node = par_node->getNextInvoc();
	}
	postClust->append(par_node);
	par_node = par_node->getNextInvoc();

	// append isolated par_nodes if any.
	appendIsolatedPar(postClust);

	while (son_node) {
		postClust->append(son_node);
		son_node = son_node->getNextInvoc();
	}

	// Check that the formation of this invocation will not
	// introduce a cycle in the graph.
	if (g.introducesCycle(*postClust)) {
		LOG_DEL; delete postClust;
		postClust = 0;
		return FALSE;
	}

	return TRUE;
}

//////////////////////////
// Initial Phase
//////////////////////////	
// If the adj_ix is not equal to one, earlier invocations are not
// connected to base_node. Then, update the repetition counter of
// the master of this adjacent_node.
void MergeLink :: initialPhase() {
	if (adjacent_node == 0) return;

	DataFlowStar* org = adjacent_node->myMaster();
	org->repetitions = adj_ix - 1;

	// break the invocation links.
	LSNode* n = (LSNode*) org->myMaster();
	while (n->getNextInvoc() != adjacent_node) {
		n = n->getNextInvoc();
	}
	n->setNextInvoc(0);
}

//////////////////////////
// Create Clusters
//////////////////////////
// With the ClusterNodeLists registered in the clustering attempts,
// create real clusters in the APEG graph
void MergeLink :: createClusters(LSGraph& g) {
	// If the base_node is the first invocation, that means
	// there are isolated parent nodes.
	int isolatedFlag = ((base_node->invocationNumber() > 1) && direction);

	// For preClust....
	if (preClust) {
		if (isolatedFlag) { 
			insertIsolatedPar(preClust);
			isolatedFlag = FALSE;
		}

		LOG_NEW; LSCluster* newCluster = new LSCluster(g, preClust);
		g.addList(newCluster);
		newCluster->repetitions = 1;

		g.newClusterInvocation(newCluster, preClust, 1);

		// update the candidate lists of the expanded graph.
		g.sortedInsert((LSNode*) newCluster->myMaster());

		// delete the ClusterNodeList structure.
		LOG_DEL; delete preClust;
		preClust = 0;
	}

	if (mainClust) {
		// take care of isolated parents
		if (isolatedFlag) {
			if (mainClust->nextList()) {
				LSNode* tmp = 
				   (LSNode*) base_node->myMaster()->myMaster();
				g.formIsolatedCluster(*tmp);
			} else {
				insertIsolatedPar(mainClust);
			}
		}

		// create the cluster master.
		LOG_NEW; LSCluster* newCluster = new LSCluster(g, mainClust);
		g.addList(newCluster);

		int i = 1;			// Cluster invocation number.
		ClusterNodeList* clist = mainClust;
		g.newClusterInvocation(newCluster, clist, 1);
		while ((clist = clist->nextList()) != 0) {
			i++;
			g.newClusterInvocation(newCluster, clist, i) ;
		}

		// set up the repetition counter of the new cluster master.
		newCluster->repetitions = i;

		// update the candidate lists of the expanded graph.
		g.sortedInsert((LSNode*) newCluster->myMaster());

		// delete the ClusterNodeList structure.
		LOG_DEL; delete mainClust;
		mainClust = 0;
	}

	// For postClust....
	if (postClust) {
		LOG_NEW; LSCluster* newCluster = new LSCluster(g, postClust);
		g.addList(newCluster);
		newCluster->repetitions = 1;

		g.newClusterInvocation(newCluster, postClust, 1);

		// update the candidate lists of the expanded graph.
		g.sortedInsert((LSNode*) newCluster->myMaster());

		// delete the ClusterNodeList structure.
		LOG_DEL; delete postClust;
		postClust = 0;
	}
}

//////////////////////////
// Final Phase
//////////////////////////	
// If the par_node is not NULL, some invocations of par_node are
// unconnected to the son_nodes. Then, we update the master setup of
// the par_node
void MergeLink :: finalPhase(LSGraph& g) {
	if (par_node == 0) return;

	DataFlowStar* master = par_node->myMaster();
	master->setMaster(par_node);
	int total = master->repetitions;
	master->repetitions = total - par_node->invocationNumber() + 1;

	// update the candidate lists of the expanded graph.
	g.sortedInsert(par_node);

	// update the invocation numbers
	int i = 1;
	while (par_node) {
		par_node->setInvocationNumber(i);
		i++;
		par_node = par_node->getNextInvoc();
	}
}


///////////////////////////////
// Handling of Isolated nodes
///////////////////////////////	

// append isolated par_nodes if any.
void MergeLink :: appendIsolatedPar(ClusterNodeList* clist) {
	while (par_node && (par_node->connected() == 0)) {
		clist->insert(par_node);
		par_node = (LSNode*) par_node->getNextInvoc();
	}
}

// insert isolated par_nodes if any.
void MergeLink :: insertIsolatedPar(ClusterNodeList* clist) {
	// first invocation.
	LSNode* tmp = (LSNode*) base_node->myMaster()->myMaster();
	while (tmp != base_node) {
		clist->insert(tmp);
		tmp = (LSNode*) tmp->getNextInvoc();
	}
}

// insert isolated son_nodes if any.
void MergeLink :: insertIsolatedSon(ClusterNodeList* clist) {

	int tx = son_node->invocationNumber() - 1;
	LSNode* sonMaster = (LSNode*) son_node->myMaster()->myMaster();
	while (tx >= 1) {
		LSNode* tmp = (LSNode*) sonMaster->getInvocation(tx);
		if (!tmp->connected()) {
			clist->append(tmp);
			if (direction) {
				adj_ix = tx;
				adjacent_node = tmp;
			}
			tx--;
		}
		else return;
	}
}
