#ifndef _ParNode_h
#define _ParNode_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "EGNode.h"
#include "ExpandedGraph.h"
#include "StringList.h"
#include "CGStar.h"

/*****************************************************************
Version identification:
$Id$	

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

Programmer: Soonhoi Ha
Date of last revision: 

*****************************************************************/

                //////////////////////
                //   class ParNode   //
                //////////////////////

class ParAncestorIter;
class ParDescendantIter;
class ParGraph;

// This class represents a node in the precedence graph.  While
// it is derived from the expanded graph node, it contains additional
// data members for parallel scheduling

class ParNode : public EGNode {

friend class ParAncestorIter;
friend class ParDescendantIter;

private:
	// necessary information for sub universe generation
	SDFStar* clonedStar;
	ParNode* nextNode;	// next invoc. assigned to the same processor
	ParNode* firstNode;	// the earliest invoc. assigned.
	int numCopied;		// number of invoc. assigned
	EGGate* origin;		// Send and Receive star only.
				// kind of hack.

protected:
	// Length of the longest execution time path to an endnode 
	int StaticLevel;

	// The type of this node.
	int type;

	// processor ID which it is scheduled on.
	int procId;

	// The scheduled time on the processor.
	int scheduledTime;
	int finishTime;

	// execution time
	int exTime;

	// # ancestors to be assigned before assign this node.
	int waitNum;

	// temporary copy of the list of ancestors and descendants
	EGNodeList tempAncs;
	EGNodeList tempDescs;

public: 
	// Constructor declaration
	ParNode(SDFStar* Mas, int invoc_no);

	// Constructor used for idle nodes or communication nodes
	ParNode(int type);

	// Replenishes temporary lists tempAncs and tempDescs from the
	//      permanent ancestors and descendants lists
	void copyAncDesc(ParGraph*, int flag);

	// increase waitNum by 1
	void incWaitNum() { waitNum++; }

        // make connection.
	void connectedTo(ParNode* to) {
		tempDescs.insert(to);
		to->tempAncs.insert(this);
		to->incWaitNum();
	}

	// remove a node from tempDescs.
	void removeDescs(ParNode* n) { tempDescs.remove(n); }
	void removeAncs(ParNode* n) { tempAncs.remove(n); waitNum--; }

	// Is it correponds to a atomic CG star?
	CGStar* myStar() { return (CGStar*) myMaster(); }

	// Functions for assigning and obtaining the StaticLevel and processor
	void assignSL( int SL ) { StaticLevel = SL; }
	int getSL() { return StaticLevel; }
	virtual int getLevel() { return StaticLevel; }

	// set and get the index of preferred processor
	void setProcId(int i) { procId = i; }
	int getProcId()	{ return procId; }
	virtual int whichProc() { return procId; }
	virtual void assignProc(int i) { procId = i; }

	// get the type field
	int getType()	{ return type; }

	// set and get the scheduledTime.
	void setScheduledTime(int i) { scheduledTime = i; }
	int getScheduledTime()	    { return scheduledTime; }
	void setFinishTime(int i) { finishTime = i; }
	int getFinishTime()	    { return finishTime; }

	// get the execution time of the node
	int getExTime() {return exTime; }
	void setExTime(int i) { exTime = i; }

	// indicate how many tokens it is waiting.
	// note that it works for homogeneous graph.
	void resetWaitNum() { waitNum = ancestors.mySize(); }

	// Return whether or not this node is presently fireable.
	int fireable() { waitNum--;
			 return (waitNum > 0)? FALSE : TRUE ; }

	// set informations for sub-universe generation
	// should be called in the increasing order of the invocation number.
	void setCopyStar(SDFStar* s, ParNode* prevN);

	SDFStar* getCopyStar() { return clonedStar; }
	ParNode* getNextNode() { return nextNode; }
	ParNode* getFirstNode() { return firstNode; }
	int numAssigned() { return numCopied; }

	// Send or Receive star only
	void setOrigin(EGGate* g) { origin = g; }
	EGGate* getOrigin() { return origin; }

	// print outs.
	StringList print();	// Prints star name and invocation
};

class ParAncestorIter : public EGNodeListIter
{
public:
	ParAncestorIter(ParNode* n) : EGNodeListIter(n->tempAncs) {}
	ParNode* next() { return (ParNode*) EGNodeListIter::next(); }
	ParNode* operator++() { return next(); }
};

class ParDescendantIter : public EGNodeListIter
{
public:
	ParDescendantIter(ParNode* n) : EGNodeListIter(n->tempDescs) {}
	ParNode* next() { return (ParNode*) EGNodeListIter::next(); }
	ParNode* operator++() { return next(); }
};

#endif
