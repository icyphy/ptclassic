static const char file_id[] = "ReachabilityMatrix.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  S.  Bhattacharyya, Soonhoi Ha

*******************************************************************/
#include "ReachabilityMatrix.h"
#include "BooleanMatrix.h"
#include "EGConnect.h"
#include "Error.h"
#include "LSNode.h"
#include "LSGraph.h"
#include "ClusterNodeList.h"

int ReachabilityMatrix::allocateIndex() {
	if (current_index >= dim) {
		Error::abortRun("Reachability Matrix index allocation is out of bounds");
		return -1;
	}
	current_index++;
	return current_index-1;
}

ReachabilityMatrix::ReachabilityMatrix(LSGraph &g) {
	// determine the dimension of the matrix
	dim = g.numNodes();

	LOG_NEW; matrix = new BooleanMatrix(dim,dim);

	current_index=0;
	initialize(g);
}

ReachabilityMatrix::~ReachabilityMatrix() {
	LOG_DEL; delete matrix;
}

inline void ReachabilityMatrix::connect(LSNode *parent, LSNode *child)
{
	matrix->setElem(parent->myRMIndex(), child->myRMIndex(), 1);
}


inline void ReachabilityMatrix::addPaths(int parentindex, int childindex)
{
	matrix->orRows(parentindex,childindex);
}

// fill RM for a given LSNode.
void ReachabilityMatrix::initialize(LSNode *p) {
	EGGateLinkIter nextKid(p->descendants);
	EGGate *q;  
	LSNode *n;

	if (p->alreadyVisited()) return; 
	p->beingVisited();
	p->setRMIndex(allocateIndex());
	while ((q = nextKid++)!=0) {
		n = (LSNode*) q->farEndNode();
		initialize(n);
	}
	nextKid.reset();
	while ((q = nextKid++)!=0) {
		n = (LSNode*) q->farEndNode();
		connect(p,n);
		addPaths(p->myRMIndex(),n->myRMIndex());
	} 
}

// initialize the reachability matrix.
void ReachabilityMatrix::initialize(LSGraph &g)
{
	LSNode *p;
	g.resetVisits(); 
	LSIter iter(g);
	while ((p=iter++)!=0) {
		initialize(p);
	}
}


// The index of the cluster is taken as the RM index of the first node 
// in "nl". The rows and columns for all other "nl" rows are invalidated.
int ReachabilityMatrix::formCluster(ClusterNodeList &nl, EGGateList &gl)
{
	ClusterNodeListIter nextNode(nl);
	LSNode *firstnode;
	LSNode *p;

	if ((firstnode = nextNode++) == 0) return 0;
	int clusterindex = firstnode->myRMIndex();
	while ((p = nextNode++) != 0) {
		int j = p->myRMIndex();
		addPaths(clusterindex, j); 
	}

	// now add the paths from each node of the cluster
	// to each of the ancestors of the cluster
	EGGateLinkIter nextAncestor(gl);
	EGGate* d;
	while ((d = nextAncestor++) != 0 ) {
		LSNode* temp = (LSNode*) d->farEndNode();
    		addPaths(temp->myRMIndex(), clusterindex); 
		connect(temp, firstnode);
	}

	return clusterindex;
}

int ReachabilityMatrix::pathExists(LSNode &src, LSNode &dest)
{ 
	return matrix->getElem(src.myRMIndex(), dest.myRMIndex());
}

// Check whether any descendant of the cluster, which is outside the 
// cluster, has a path which returns to the cluster.
int ReachabilityMatrix::introducesCycle(ClusterNodeList &nl)
{
	ClusterNodeListIter nextNode(nl);
	LSNode *p;

	nl.markAsActive();
	while ((p = nextNode++) != 0) {
		EGGateLinkIter nextKid(p->descendants);
		EGGate *g;
		while ((g = nextKid++) != 0) {
			LSNode *q = (LSNode*) g->farEndNode();
			if (!q->inActiveCluster()) {
				ClusterNodeListIter nextClusterNode(nl);
				LSNode *r;
				while ((r = nextClusterNode++) != 0) 
					if (pathExists(*q,*r)) {
						nl.markAsInactive();
						return TRUE; 
					}
			}
		}
	}
	nl.markAsInactive();
	return FALSE;
}