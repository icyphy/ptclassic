static const char file_id[] = "DCClusterList.cc";
/*****************************************************************
Version identification:
@(#)DCClusterList.cc	1.5	3/2/95

Copyright (c) 1990-1995 The Regents of the University of California.
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
