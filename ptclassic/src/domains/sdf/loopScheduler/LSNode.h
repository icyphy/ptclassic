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

 Programmer:  Shuvra Bhattacharyya, Soonhoi Ha (4/92)
 Date of creation: 5/9/91

Loop scheduler

*******************************************************************/


#ifndef _LSNode_h
#define _LSNode_h
#ifdef __GNUG__
#pragma interface
#endif

#include "EGNode.h"

class LSCluster;

//////////////////
// class LSNode //
//////////////////
//
// Nodes for expanded graphs which are
// to be used by the loop scheduler.
//

class LSNode : public EGNode {
public:

	// constructor with master and invocation number arguments
	LSNode(DataFlowStar* master, int no) : EGNode(master,no) {
		fired_flag = 0; }

	// set, get, and reset the active flag
	void markAsActive() {active_flag=1;}
	void markAsInactive() {active_flag=0;}
	int inActiveCluster() {return active_flag;}

	// return the LSNode which is connected to this node
	// and has the highest invocation index among the nodes of
	// the same master.
	// The second argument is a flag. If set, search descendants.
	// If reset, search ancestors.
	LSNode* nextConnection(LSNode*, int);

	// This node will be clustered in the argument node.
	// Delete outside connections to implement the associated
	// connection on the argument cluster node.
	void updateOutsideConnections(LSNode*);

	// Set this node's index into the connection matrix
	void setRMIndex(int x) {rm_index=x;}

	// return FALSE if it is an isolated (unconnected) node.
	int connected() { return ancestors.size() + descendants.size(); }

	// get this node's connection-matrix index
	int RMIndex() {return rm_index;}

	// fire it.
	void fireMe() {fired_flag = 1; }

	// Indicate whether or not this node has already been fired. 
	int alreadyFired() {return fired_flag;}

	// Return whether or not this node is presently fireable.
	int fireable();

	// redefine
	LSNode* getNextInvoc() { return (LSNode*) EGNode :: getNextInvoc(); }

private:
	// flag for cluster formation -- this marks whether or not this
	// node is in the current cluster being considered
	unsigned active_flag : 1;

	// Indicates whether or not this node has been fired.
	unsigned fired_flag : 1;

	// index into connection matrix
	int rm_index;
};

class LSNodeLink : public EGNodeLink
{
public:
	LSNode* node()	{ return (LSNode*) e; }

	LSNodeLink(LSNode* e) : EGNodeLink(e) {}
};

class LSNodeList : public DoubleLinkList
{
public:
	LSNodeLink* createLink(LSNode* e)
		{ INC_LOG_NEW; return new LSNodeLink(e); }

	LSNode* takeFromFront()
		{ return (LSNode*) DoubleLinkList :: takeFromFront(); }

        void append(LSNode* e) { appendLink(createLink(e)); }
        void insert(LSNode* e) { insertLink(createLink(e)); }
};

class LSNodeListIter : public DoubleLinkIter
{
public:
	LSNodeListIter(const LSNodeList& l) : DoubleLinkIter(l) {}
	LSNodeLink* nextLink()
		{ return (LSNodeLink*) DoubleLinkIter::nextLink(); }
	LSNode* next() { return (LSNode*) DoubleLinkIter::next(); }
	LSNode* operator++(POSTFIX_OP) { return next(); }
};


#endif
