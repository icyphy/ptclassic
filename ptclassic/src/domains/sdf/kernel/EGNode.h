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

 Programmer:  Soonhoi Ha, based on S.  Bhattacharyya's code.
 
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
	EGNode* operator++(POSTFIX_OP) { return next(); }
};

#endif
