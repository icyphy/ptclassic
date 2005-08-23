#ifndef _ParNode_h
#define _ParNode_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "EGNode.h"
#include "ExpandedGraph.h"
#include "StringList.h"
#include "CGStar.h"
#include "Profile.h"

/*****************************************************************
Version identification:
@(#)ParNode.h	1.18	6/4/96	

Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

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
public: 
	// Constructor declaration
	ParNode(DataFlowStar* Mas, int invoc_no);

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

	CGStar* myStar() { return (CGStar*) myMaster(); }
	virtual const char* readRealName();

	// Is it correponds to a atomic CG star?
	int amIBig() { return myStar()->isParallel(); }

	// Functions for assigning and obtaining the StaticLevel.
	void assignSL( int SL ) { StaticLevel = SL; }
	int getSL() { return StaticLevel; }
	virtual int getLevel() { return StaticLevel; }

	// set and get the index of preferred processor
	void setProcId(int i) { procId = i; }
	int getProcId()	{ return procId; }

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
	void resetWaitNum() { waitNum = ancestors.size(); }

	// Return whether or not this node is presently fireable.
	int fireable() { return ((--waitNum) <= 0); }

	// print outs.
	StringList print();	// Prints star name and invocation

// indicate whether all invocations are assigned to the same processor
// or not.
	void setOSOPflag(int i) { myOSOPflag = i; }
	int isOSOP() { return myOSOPflag; }

// set informations for sub-universe generation
// should be called in the increasing order of the invocation number.
	virtual DataFlowStar* copyStar(CGTarget* t, int pid, int flag);

	void setCopyStar(DataFlowStar* s, ParNode* prevN);

        void replaceCopyStar (DataFlowStar& /*newStar*/);
    
	DataFlowStar* getCopyStar() { return clonedStar; }
	ParNode* getNextNode() { return nextNode; }
	ParNode* getFirstNode() { return firstNode; }
	int numAssigned() { return numCopied; }

// Send or Receive star only
	void setOrigin(EGGate* g) { origin = g; }
	EGGate* getOrigin() { return origin; }
	void setPartner(ParNode* n) { partner = n; n->partner = this; }
	ParNode* getPartner() { return partner; }
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

	DataFlowStar* clonedStar;

private:
	// necessary information for sub universe generation
	ParNode* nextNode;	// next invoc. assigned to the same processor
	ParNode* firstNode;	// the earliest invoc. assigned.
	int numCopied;		// number of invoc. assigned
	EGGate* origin;		// Send and Receive star only.
	ParNode* partner;	// comm. node partner
	int myOSOPflag;		// to be set when all invocations are assigned
				// to the same processor.
};

class ParAncestorIter : public EGNodeListIter
{
public:
	ParAncestorIter(ParNode* n) : EGNodeListIter(n->tempAncs) {}
	ParNode* next() { return (ParNode*) EGNodeListIter::next(); }
	ParNode* operator++(POSTFIX_OP) { return next(); }
};

class ParDescendantIter : public EGNodeListIter
{
public:
	ParDescendantIter(ParNode* n) : EGNodeListIter(n->tempDescs) {}
	ParNode* next() { return (ParNode*) EGNodeListIter::next(); }
	ParNode* operator++(POSTFIX_OP) { return next(); }
};

#endif
