/******************************************************************
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
class DataFlowStar;

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
public:
	ClusterNodeList(ClusterNodeList* n = NULL) : 
		next(0),first(0),second(0) { if (n) n->next = this; }	
	~ClusterNodeList();
	ClusterNodeList* nextList() { return next; }

	// insert a node into the cluster
	void insert(LSNode *n) { prepend(n);
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
	DataFlowStar* first;
	DataFlowStar* second;

private:
	// can make a linked list of the ClusterNodeLists.
	ClusterNodeList* next;

	// set the component stars: first and second
	void setComponents(LSNode*);
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
	LSNode *operator++ (POSTFIX_OP) {return next();}

	// reset to the beginning of the list
	void reset() {ListIter::reset();}

	// connect this iterator to another nodelist
	void reconnect(ClusterNodeList& l) 
		{ListIter::reconnect(l);} 
};

#endif

