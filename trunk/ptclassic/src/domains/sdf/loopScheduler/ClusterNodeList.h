/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  S.  Bhattacharyya, Soonhoi Ha (4/92)

*******************************************************************/
#ifndef _ClusterNodeList_h
#define _ClusterNodeList_h
#ifdef __GNUG__
#pragma interface
#endif

#include "DataStruct.h"
#include "StringList.h"
#include "LSNode.h"
class LSGraph;
class SDFStar;

// This class implements a list of expanded graph nodes for a cluster
// invocation. 

////////////////////////////
// class ClusterNodeList  //
////////////////////////////
//
// This class implements a list of pointers to the nodes of a cluster. 
// It is useful for operations that need to be done accross all nodes 
// of a cluster.
//
class ClusterNodeList : public SequentialList
{
private:
	// can make a linked list of the ClusterNodeLists.
	ClusterNodeList* next;

	// set the component stars: first and second
	void setComponents(LSNode*);

public:
	// constructor
	ClusterNodeList(ClusterNodeList* n = NULL) : 
		next(0),first(0),second(0) { if (n) n->next = this; }	
	~ClusterNodeList();
	ClusterNodeList* nextList() { return next; }

	// insert a node into the cluster
	void insert(LSNode *n) { tup(n);
				 setComponents(n); }

	// append a node into the cluster
	void append(LSNode *n) { put(n);
				 setComponents(n); }

	// Set the active flag of each node in the nodelist.
	void markAsActive();

	// Reset the active flag of each node in the nodelist.
	void markAsInactive();

	// print the names of the nodes in the list
	StringList print();

	// first and second component star
	SDFStar* first;
	SDFStar* second;
};

///////////////////////////////
// class ClusterNodeListIter //
///////////////////////////////
//
// Iterator class for cluster node lists.
//

class ClusterNodeListIter : public ListIter {
public:
	// constructor -- attatch the iterator to a list
	ClusterNodeListIter(ClusterNodeList& l) : ListIter(l) {}

	// get the next node referenced in the list
	LSNode *next() {return (LSNode*)ListIter::next();}
	LSNode *operator++ () {return next();}

	// reset to the beginning of the list
	void reset() {ListIter::reset();}

	// connect this iterator to another nodelist
	void reconnect(ClusterNodeList& l) 
		{ListIter::reconnect(l);} 
};

#endif

