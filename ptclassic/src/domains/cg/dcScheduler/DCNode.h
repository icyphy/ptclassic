#ifndef _DCNode_h
#define _DCNode_h
#ifdef __GNUG__
#pragma interface
#endif

#include "ParNode.h"
#include "StringList.h"

class DCGraph;
class Cluster;

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

Programmer: Soonhoi Ha based on G.C. Sih's code

DCNode is used by the parallel scheduler to represent
	a precedence graph node.

*****************************************************************/

                //////////////////////
                //   class DCNode   //
                //////////////////////

class DCNode;

class DCNodeList : public EGNodeList
{
public:
	DCNode* takeFromFront()
		{ return (DCNode*) EGNodeList :: takeFromFront(); }

	DCNode* headNode() { 
		return head? ((DCNode*) ((EGNodeLink*) head)->myNode()) : 0; }
	int member(DCNode* n);

	void removeNodes();
};

// This class represents a node in the precedence graph.  While
// it is derived from the expanded graph node, it contains additional
// data members which are used by SDFParScheduler.

class DCNode : public ParNode {
friend class DCGraph;

private:
	// set of merge nodes reachable through directed path is, which
	// keeps compact information on the transitive closure.
	DCNodeList TClosure;

	// Transitive Closure in reverse direction
	DCNodeList RTClosure;

	// temporary processor assignment
	int tempProcId;

	// The time the node starts and finishes execution
	int bestStart;
	int bestFinish;

public: 
	// Constructor declaration
	DCNode(SDFStar* s, int invoc_no);

	// Constructor used for communication nodes
	DCNode(int type);

	// Get the number of samples passed between this and destnode.
	int getSamples(DCNode *destnode);

	// return TRUE if it is a merge node.
	int amIMerge() { return tempAncs.mySize() - 1; }
	int amIBranch() { return tempDescs.mySize() - 1; }

	// print information
	StringList print();	// Prints star name and invocation

	// The elementary cluster this node belongs to
	Cluster *elemCluster;

        // The highest-level cluster the node currently belongs to
        Cluster *cluster;

	// get and set the temporary processor id.
	void assignProc(int n) { tempProcId = n; }
	int whichProc(){ return tempProcId; }

	// Saves tempProcId into procId.
	void saveInfo() { procId = tempProcId; bestStart = scheduledTime; 
			  bestFinish = finishTime; }

	// get the bestStart and bestFinish
	int getBestStart() { return bestStart; }
	int getBestFinish() { return bestFinish; }

	// return an adjacent node in the given node list.
	// if direction = 1, look at the ancestors, if -1, descendants.
	DCNode* adjacentNode(DCNodeList&, int direction);
};

class DCNodeListIter : public EGNodeListIter
{
public:
        DCNodeListIter(const EGNodeList& l) : EGNodeListIter(l) {}
        DCNode* next() { return (DCNode*) EGNodeListIter::next(); }
        DCNode* operator++() { return next(); }
};

class DCAncestorIter : public ParAncestorIter
{
public:
        DCAncestorIter(DCNode* n) : ParAncestorIter(n) {}
        DCNode* next() { return (DCNode*) ParAncestorIter::next(); }
        DCNode* operator++() { return next(); }
};

class DCDescendantIter : public ParDescendantIter
{
public:
        DCDescendantIter(DCNode* n) : ParDescendantIter(n) {}
        DCNode* next() { return (DCNode*) ParDescendantIter::next(); }
        DCNode* operator++() { return next(); }
};

#endif
