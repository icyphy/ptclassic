#ifndef _DCClusterList_h
#define _DCClusterList_h
#ifdef __GNUG__
#pragma interface
#endif

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

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
	DCCluster *operator++() {return next();}

	void reset() {DoubleLinkIter::reset();}

	void reconnect(const DCClusterList& l) {DoubleLinkIter::reconnect(l);}
};
	
#endif

