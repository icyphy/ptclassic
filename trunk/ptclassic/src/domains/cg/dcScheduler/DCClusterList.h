#ifndef _DCClusterList_h
#define _DCClusterList_h
#ifdef __GNUG__
#pragma interface
#endif

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

Programmer: Soonhoi Ha based on G.C. Sih's code

DCClusterList is used by the parallel scheduler to handle
	lists of clusters

*****************************************************************/

#include "DCCluster.h"
#include "DoubleLink.h"
#include "StringList.h"

			/////////////////////////
			//  class DCClusterLink  //
			/////////////////////////
class DCClusterLink : public DoubleLink {
public:
	DCClusterLink(DCCluster *clust): DoubleLink(clust) {}

	// Return the pointer to the cluster
	DCCluster *getDCClustp() {return (DCCluster*) e;}
};

			/////////////////////////
			//  class DCClusterList  //
			/////////////////////////
// For handling lists of clusters
class DCClusterList : public DoubleLinkList
{
public:
	DCClusterList() {}
	DCClusterList(DCClusterList &list);
	~DCClusterList() {}

	// remove clusters
	void removeDCClusters();

	DCClusterLink* createLink(DCCluster* c) {
		INC_LOG_NEW; DCClusterLink* tmp = new DCClusterLink(c);
		return tmp;
	}

	// reset the clusters in the list.
	void resetList();
	
	void insert(DCCluster* cl) { insertLink(createLink(cl)); }
	void append(DCCluster* cl) { appendLink(createLink(cl)); }

	// Returns 1 if *clust is a member of the list, else returns 0
	int member(DCCluster *clust);

	// For each cluster, set the cluster property of each node inside it.
	void setDCClusters();
	void resetScore();

	// find clusters and add them into the list from a node list.
	void findDCClusts(DCNodeList&);

	DCClusterLink *firstLink()
		{return (DCClusterLink*)DoubleLinkList::head();}

	DCCluster *firstDCClust() {return (DCCluster*)(firstLink()->getDCClustp());}

	int listSize() {return DoubleLinkList::size();}

	// Inserts the cluster smallest ExecTime first
	void insertSorted(DCCluster *c);

	// Removes and returns a pointer to the first cluster in the list
	DCCluster *popHead() 
		{ return (DCCluster*) DoubleLinkList :: takeFromFront(); }

	StringList print();
};

			/////////////////////////////
			//  class DCClusterListIter  //
			/////////////////////////////
// For iterating through DCClusterLists

class DCClusterListIter : private DoubleLinkIter {
public:
	DCClusterListIter(const DCClusterList& l) : DoubleLinkIter(l) {}

	DCClusterLink* nextLink() 
		{ return (DCClusterLink*) DoubleLinkIter :: nextLink(); }
	DCCluster *next() {return (DCCluster*)DoubleLinkIter::next();}
	DCCluster *operator++(POSTFIX_OP) {return next();}

	void reset() {DoubleLinkIter::reset();}

	void reconnect(const DCClusterList& l) {DoubleLinkIter::reconnect(l);}
};
	
#endif

