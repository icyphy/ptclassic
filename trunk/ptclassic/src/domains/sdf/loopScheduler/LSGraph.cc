static const char file_id[] = "LSGraph.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Shuvra Bhattacharyya, Soonhoi Ha (4/92)
 Date of creation: 5/9/91

Loop scheduler

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "LSGraph.h"
#include "LSCluster.h"
#include "ClusterNodeList.h"

/////////////////////
// LSGraph methods //
/////////////////////

void LSGraph::resetVisits()
{
	EGIter nextNode(*this);
	EGNode *n;

	while ((n=nextNode++)!=0)
		n->resetVisit();
}

int LSGraph::createMe(Galaxy& galaxy, int) {
	clusters.initialize();
	int flag = ExpandedGraph :: createMe(galaxy, 0);
	removeArcsWithDelay();
	if (flag) flag = makeRM();
	return flag;
}

// insert a node into "candidates" list sorted by repetition counter.
void LSGraph::sortedInsert(LSNode *p) {
	LSNodeListIter nextNode(candidates);
	LSNodeLink* nl;
	LSNodeLink* tmp = candidates.createLink(p);

	int freq = p->myMaster()->repetitions; 

	while ((nl = nextNode.nextLink()) != 0) {
		LSNode* n = nl->myNode();
		if (freq >= n->myMaster()->repetitions) {
			candidates.insertAhead(tmp,nl);
			return;
		}
	}
	candidates.appendLink(tmp);
}

// "candidates" now keeps the first invocations of all nodes in the
// APEG graph, sorting them with the repetition counter.
// A node of highest repetition counter will be considered first
// for clustering.
void LSGraph::initializeCandidates()
{
	candidates.initialize();

	EGMasterIter nextMaster(*this);
	EGNode *m;
	while ((m = nextMaster++) != 0)
		sortedInsert((LSNode*) m);
}

EGNode *LSGraph::newNode(SDFStar *s, int i) 
	{ LOG_NEW; return new LSNode(s,i); }


// create invocations of the clusters.
// Invocation indices should start from 1 and go on sequentially.
// Take off all nodes in the clusters from the APEG graph and insert
// the created nodes.
// Include the first invocation to the candidates list.
// return FALSE if error occurs.

int LSGraph :: newClusterInvocation(LSCluster* newCluster,
		ClusterNodeList* clist, int invoc_no) {

	// new node
	LOG_NEW; LSNode* newNode = new LSNode(newCluster, invoc_no);
	if (invoc_no == 1)  { newCluster->setMaster(newNode);
			      prevNode = 0; }
	if(prevNode) prevNode->setNextInvoc(newNode);
	prevNode = newNode;

	// Disconnect and destroy the nodes contained in this cluster.
	// For each connection destroyed, the corresponding connection
	// to the new cluster invocation is established.
	clist->markAsActive();
	ClusterNodeListIter nextNode(*clist);
	LSNode* n;
	while ((n = nextNode++) != 0) {
		n->updateOutsideConnections(newNode);
	}
	clist->markAsInactive();
	newNode->setRMIndex(RM->formCluster(*clist, newNode->ancestors));

	return TRUE;
}

// make the reachability matrix
int LSGraph::makeRM() {
	LOG_DEL; delete RM;
	LOG_NEW; RM = new ReachabilityMatrix(*this);

	LSIter nextNode(*this);
	LSNode *p;
	while ((p = nextNode++) != 0) {
		if (RM->pathExists(*p,*p)) {
			StringList msg = "Deadlocked graph -- insufficient delay in loop containing ";
			msg +=  p->myMaster()->readFullName();
			Error::abortRun(msg);
			return FALSE;
		}
	}
	return TRUE;
}

// form a cluster of isolated nodes
void LSGraph :: formIsolatedCluster(LSNode& p) {

	// make a cluster
	LOG_NEW; ClusterNodeList* clist = new ClusterNodeList(0);
	LSNode* n = &p;
	while (n && (n->connected() == 0)) {
		clist->append(n);
		n = n->getNextInvoc();
	}

	LOG_NEW; LSCluster* newCluster = new LSCluster(*this, clist);
	addList(newCluster);
	newCluster->repetitions = 1;

	// create a LS node associated with this cluster.
	newClusterInvocation(newCluster, clist, 1);
	sortedInsert((LSNode*) newCluster->myMaster());

	LOG_DEL; delete clist;
}

// destructor
LSGraph :: ~LSGraph() {

	LOG_DEL; delete RM;

	ListIter nextCluster(clusters);
	LSCluster* n;

	while ((n = (LSCluster*) nextCluster++) != 0) {
		n->myMaster()->deleteInvocChain();
		LOG_DEL; delete n;
	}
}
