static const char file_id[] = "ClusterList.cc";
/*****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

Programmer: Soonhoi Ha based on G.C. Sih's code
Date of last revision: 5/92 

*****************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "ClusterList.h"

// Constructor to initialize one ClusterList with another
ClusterList::ClusterList(ClusterList &list) {

        initialize();

        ClusterListIter iter(list);
        Cluster *cl;
        while ((cl = iter++) != 0) {
                append(cl);
        }
}

// destroy clusters
void ClusterList :: removeClusters() {
	ClusterListIter iter(*this);
	Cluster* cl;
	Cluster* prev = 0;

	while ((cl = iter++) != 0) {
		LOG_DEL; delete prev;
		prev = cl;
	}
	LOG_DEL; delete prev;
}

				//////////////////////
				///  insertSorted  ///
				//////////////////////
// Insert a cluster into the list sorted smallest ExecTime first.

void ClusterList::insertSorted(Cluster *c) {

        ClusterLink *copy = createLink(c);

	int cExec = c->getExecTime(); // The execution time of cluster c

        ClusterListIter iter(*this);
        ClusterLink *clink;
        while ((clink = (ClusterLink*)iter.nextLink()) != 0) {
                if (cExec <= (clink->getClustp()->getExecTime()))
                        break;
        }

        // Check if we reached the end of the list
        if (clink != 0) {
                insertAhead(copy, clink);
        }
        else {
                appendLink(copy);
        }
}

			///////////////////
			///  resetList  ///
			///////////////////
// reset the members of clusters in the list.
void ClusterList :: resetList() {
	ClusterListIter citer(*this);
	Cluster* cl;
	
	while ((cl = citer++) != 0) {
		cl->resetMember();
	}
}

			////////////////
			///  member  ///
			////////////////
// Returns 1 if the list contains clust, otherwise it returns 0.
int ClusterList::member(Cluster *clust) {
	ClusterListIter iter(*this);
	Cluster *cl;
	while ((cl = iter++) != 0) {
		if (cl == clust) return TRUE;
	}
	return FALSE;
}

			/////////////////////
			///  setClusters  ///
			/////////////////////
void ClusterList::setClusters() {
	ClusterListIter iter(*this);
	Cluster *cl;
	while ((cl = iter++) != 0) {
		cl->setCluster(0);
	}
}

void ClusterList::resetScore() {
	ClusterListIter iter(*this);
	Cluster *cl;
	while ((cl = iter++) != 0) {
		cl->setScore(0);
	}
}

// find clusts and insert them into the list from a node list
void ClusterList :: findClusts(DCNodeList& nlist) {

	initialize();
	DCNodeListIter niter(nlist);
	DCNode* n;
	while ((n = niter++) != 0) {
		if (n->getType() == 0) {
			Cluster* cl = n->cluster;
			if (member(cl) == 0) insert(cl);
		}
	}

	if (listSize() < 2) { // Only one slp clust, split into comps
		Cluster* cl = popHead();
		Cluster *comp1 = cl->getComp1();
		Cluster *comp2 = cl->getComp2();
		if (comp1 == 0) return; // Cluster cl is elementary
		insert(comp1);
		insert(comp2);
	}
}

// print
StringList ClusterList :: print() {
	StringList out;
	ClusterListIter citer(*this);
	Cluster* cl;

	out += "** cluster lists **\n";
	while ((cl = citer++) != 0) {
		out += cl->print();
	}
	return out;
}
