static const char file_id[] = "DCClusterList.cc";
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

#include "DCClusterList.h"

// Constructor to initialize one DCClusterList with another
DCClusterList::DCClusterList(DCClusterList &list) {

        initialize();

        DCClusterListIter iter(list);
        DCCluster *cl;
        while ((cl = iter++) != 0) {
                append(cl);
        }
}

// destroy clusters
void DCClusterList :: removeDCClusters() {
	DCClusterListIter iter(*this);
	DCCluster* cl;
	DCCluster* prev = 0;

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

void DCClusterList::insertSorted(DCCluster *c) {

        DCClusterLink *copy = createLink(c);

	int cExec = c->getExecTime(); // The execution time of cluster c

        DCClusterListIter iter(*this);
        DCClusterLink *clink;
        while ((clink = (DCClusterLink*)iter.nextLink()) != 0) {
                if (cExec <= (clink->getDCClustp()->getExecTime()))
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
void DCClusterList :: resetList() {
	DCClusterListIter citer(*this);
	DCCluster* cl;
	
	while ((cl = citer++) != 0) {
		cl->resetMember();
	}
}

			////////////////
			///  member  ///
			////////////////
// Returns 1 if the list contains clust, otherwise it returns 0.
int DCClusterList::member(DCCluster *clust) {
	DCClusterListIter iter(*this);
	DCCluster *cl;
	while ((cl = iter++) != 0) {
		if (cl == clust) return TRUE;
	}
	return FALSE;
}

			/////////////////////
			///  setDCClusters  ///
			/////////////////////
void DCClusterList::setDCClusters() {
	DCClusterListIter iter(*this);
	DCCluster *cl;
	while ((cl = iter++) != 0) {
		cl->setDCCluster(0);
	}
}

void DCClusterList::resetScore() {
	DCClusterListIter iter(*this);
	DCCluster *cl;
	while ((cl = iter++) != 0) {
		cl->setScore(0);
	}
}

// find clusts and insert them into the list from a node list
void DCClusterList :: findDCClusts(DCNodeList& nlist) {

	initialize();
	DCNodeListIter niter(nlist);
	DCNode* n;
	while ((n = niter++) != 0) {
		if (n->getType() == 0) {
			DCCluster* cl = n->cluster;
			if (member(cl) == 0) insert(cl);
		}
	}

	if (listSize() < 2) { // Only one slp clust, split into comps
		DCCluster* cl = popHead();
		DCCluster *comp1 = cl->getComp1();
		DCCluster *comp2 = cl->getComp2();
		if (comp1 == 0) return; // DCCluster cl is elementary
		insert(comp1);
		insert(comp2);
	}
}

// print
StringList DCClusterList :: print() {
	StringList out;
	DCClusterListIter citer(*this);
	DCCluster* cl;

	out += "** cluster lists **\n";
	while ((cl = citer++) != 0) {
		out += cl->print();
	}
	return out;
}
