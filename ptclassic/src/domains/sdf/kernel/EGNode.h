/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha, based on S.  Bhattacharyya's code.
 
Now EGNode class is derived from the SDFAtomCluster, which is 
derived from SDFStar class. 

*******************************************************************/
#ifndef _EGNode_h
#define _EGNode_h
#ifdef __GNUG__
#pragma interface
#endif

#include "SDFStar.h"
#include "DoubleLink.h"
#include "EGGate.h"
#include "StringList.h"

class EGNodeLink;

//////////////////
// class EGNode //
//////////////////
//
// A node in the expanded graph.
//

class EGNode
{
public:
  	// constructor with origin and invocation number arguments
  	EGNode(DataFlowStar*, int n = 1);
	virtual ~EGNode();

	void	deleteInvocChain();

	// print me
	StringList printMe();
	StringList printShort();

	// set and get the next invocation
	void setNextInvoc(EGNode* n) { next = n; }
	EGNode* getNextInvoc() { return next; }

	// return the invocation number 
	int invocationNumber() { return invocation; }
	void setInvocationNumber(int i) { invocation = i; }

	// return the i-th invocation where i should be >= invocation
	EGNode* getInvocation(int);

	// the set of immediate antecedent & successor nodes
	EGGateList ancestors;
	EGGateList descendants;

	// hidden Gates
	EGGateList hiddenGates;

	// return a pointer to the corresponding master
	DataFlowStar *myMaster() {return pStar;}

	// Member function to determine whether or not
	// this node is a root(source node) in the expanded graph.
	int root();

	// Create an arc from this node to another. This node is assumed 
	// to be the source, or parent, in the connection and the first 
	// argument is taken to be the destination. Note, both links -- 
	// from child to parent & from parent to child -- are
	// created/updated in this method. 
	EGGate* makeArc(EGNode *dest, int samples, int delay);

	// reset the visited flag, for traversal algorithms
	void resetVisit() { visited = 0; }

	// set the visited flag
	void beingVisited() { visited=1; }

	// get the visited flag
	int alreadyVisited() { return visited; }

	// set the sticky-ness: interdependency between invocations.
	void claimSticky() { stickyFlag = 1; }
	int sticky() { return stickyFlag; }

	// get the execution time of the invocation
	int myExecTime() { return myMaster()->myExecTime(); }

private:

  	// invocation number is tabulated starting from "1" 
  	int invocation; 

	// pointer to the original star
	DataFlowStar* pStar;

	// link to the next invocation.
	EGNode* next;

	// Flag to set if there is dependency between invocations.
	unsigned stickyFlag : 1;

	// flag for graph-traversal algorithms -- this marks whether or
	// not this node has been visited
	unsigned visited : 1;
};

////////////////////////////
// class EGNodeList class //
////////////////////////////

class EGNodeLink : public DoubleLink
{
public:
	EGNode* node()	{ return (EGNode*) e; }

	// constructor
	EGNodeLink(EGNode* e):DoubleLink(e) {}
};

class EGNodeList : public DoubleLinkList
{
public:
	EGNodeLink* createLink(EGNode* e) 
		{ INC_LOG_NEW; return new EGNodeLink(e); }

	void append(EGNode* e) { appendLink(createLink(e)); }
	void insert(EGNode* e) { insertLink(createLink(e)); }

        EGNode* takeFromFront()
                { return (EGNode*) DoubleLinkList :: takeFromFront(); }
	
	EGNode* headNode() { return ((EGNodeLink*) head())->node(); }

	StringList print();
};

class EGNodeListIter : public DoubleLinkIter
{
public:
	EGNodeListIter(const EGNodeList& l) : DoubleLinkIter(l) {}
	EGNodeLink* nextLink() 
		{ return (EGNodeLink*) DoubleLinkIter::nextLink(); }
	EGNode* next() { return (EGNode*) DoubleLinkIter::next(); }
	EGNode* operator++() { return next(); }
};

#endif
