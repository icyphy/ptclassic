#ifndef _ClusterList_h
#define _ClusterList_h
#ifdef __GNUG__
#pragma interface
#endif

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

Programmer: Soonhoi Ha based on G.C. Sih's code

ClusterList is used by the parallel scheduler to handle
	lists of clusters

*****************************************************************/

#include "Cluster.h"
#include "DoubleLink.h"
#include "StringList.h"

			/////////////////////////
			//  class ClusterLink  //
			/////////////////////////
class ClusterLink : public DoubleLink {
public:
	// Constructor
	ClusterLink(Cluster *clust): DoubleLink(clust) {}

	// Return the pointer to the cluster
	Cluster *getClustp() {return (Cluster*) e;}
};

			/////////////////////////
			//  class ClusterList  //
			/////////////////////////
// For handling lists of clusters
class ClusterList : public DoubleLinkList
{
public:
	// Constructors
	ClusterList() {}
	ClusterList(ClusterList &list);

	// Destructor (need to make it public)
	~ClusterList() {}

	// remove clusters
	void removeClusters();

	ClusterLink* createLink(Cluster* c) {
		INC_LOG_NEW; ClusterLink* tmp = new ClusterLink(c);
		return tmp;
	}

	// reset the clusters in the list.
	void resetList();
	
	void insert(Cluster* cl) { insertLink(createLink(cl)); }
	void append(Cluster* cl) { appendLink(createLink(cl)); }

	// Returns 1 if *clust is a member of the list, else returns 0
	int member(Cluster *clust);

	// For each cluster, set the cluster property of each node inside it.
	void setClusters();
	void resetScore();

	// find clusters and add them into the list from a node list.
	void findClusts(DCNodeList&);

	ClusterLink *firstLink()
		{return (ClusterLink*)DoubleLinkList::head();}

	Cluster *firstClust() {return (Cluster*)(firstLink()->getClustp());}

	int listSize() {return DoubleLinkList::size();}

	// Inserts the cluster smallest ExecTime first
	void insertSorted(Cluster *c);

	// Removes and returns a pointer to the first cluster in the list
	Cluster *popHead() 
		{ return (Cluster*) DoubleLinkList :: takeFromFront(); }

	StringList print();
};

			/////////////////////////////
			//  class ClusterListIter  //
			/////////////////////////////
// For iterating through ClusterLists

class ClusterListIter : private DoubleLinkIter {
public:
	ClusterListIter(const ClusterList& l) : DoubleLinkIter(l) {}

	ClusterLink* nextLink() 
		{ return (ClusterLink*) DoubleLinkIter :: nextLink(); }
	Cluster *next() {return (Cluster*)DoubleLinkIter::next();}
	Cluster *operator++() {return next();}

	void reset() {DoubleLinkIter::reset();}

	void reconnect(const ClusterList& l) {DoubleLinkIter::reconnect(l);}
};
	
#endif

