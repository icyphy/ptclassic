/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha, based on Shuvra Bhattacharyya's work
 Date of creation: 4/92

 MergeList

*******************************************************************/


#ifndef _MergeList_h
#define _MergeList_h
#ifdef __GNUG__
#pragma interface
#endif

#include "DoubleLink.h"
#include "LSNode.h"

class LSCluster;
class ClusterNodeList;
class LSGraph;

// These classes implement the list of candidate adjacent nodes for a
// given base node. Each element of a merge list specifies
// a candidate cluster, and merge lists are maintained in
// a sorted order according to the priorities for each candidate
// cluster. The priority of a candidate varies inversely to the
// number of invocations of the base_node which appear in each
// cluster invocation. 


/////////////////////////
// class MergeLink     //
/////////////////////////


// This class implements a candidate cluster

class MergeLink : public DoubleLink {
friend class MergeList;

public:
	// Constructor with base node and adjacent node arguments.
	MergeLink(LSNode*, LSNode*, int);

	// Try to form an iterated cluster based on this merge link.
	int formRepeatedCluster(LSGraph &g);

private:
	// The base  and adjacent node 
	LSNode *base_node;
	LSNode *adjacent_node;

	// invocation index of the adjacent node. - tie break criterion.
	// Among links with same base_inv, the smaller adj_ix takes priority.
	int adj_ix;

	// another representation of a node pair: parent-child relation
	LSNode* par_node;
	LSNode* son_node;

	// The number of node invocations per candidate-cluster invocation.
	// If it is smaller, the priority of this link is higher.
	int par_inv;
	int son_inv;

	// number of repetitions of the masters of both nodes.
	int parRep;
	int sonRep;

	// gcd value of repetition counts of the base_node and adjacent
	// node. This value will determine the number of cluster
	// invocations with these two nodes. Note that there may be 
	// two differenet masters.
	int gcd;

	// Flag. If set, the base_node is the ancestor.
	// If reset, the adjacent node is the ancestor.
	int direction;

	// ClusterNodeLists.
	ClusterNodeList* preClust;
	ClusterNodeList* mainClust;
	ClusterNodeList* postClust;
	
	// set up parent-child relation
	void setUp();

	// Clustering routines.
	// integer arguments are invocation indices of nodes
	// return FALSE if deadlock results.
	int preClustering(LSGraph&, int&, int&);
	int mainClustering(LSGraph&, int, int);
	int postClustering(LSGraph&);

	// Actual clustering phase
	void initialPhase();
	void createClusters(LSGraph&);
	void finalPhase(LSGraph&);

	// Handling of isolated nodes
	void appendIsolatedPar(ClusterNodeList*);
	void insertIsolatedPar(ClusterNodeList*);
	void insertIsolatedSon(ClusterNodeList*);
};

/////////////////////////
// class MergeList     //
/////////////////////////

// This class implements a list of candidate clusters.

class MergeList : public DoubleLinkList {
public: 
	// Insert a new link
	void insertMerge(LSNode*, LSNode*, int);
};

/////////////////////////////
// class MergeListIter     //
/////////////////////////////
// Iterator class for a MergeList.

class MergeListIter : public DoubleLinkIter
{
public:
	MergeListIter(const MergeList& l) : DoubleLinkIter(l) {}
	MergeLink *next() {return (MergeLink*)DoubleLinkIter::nextLink();}
	MergeLink* operator++ () { return next();}
};

#endif
