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

// This class represents a node in the precedence graph.  While
// it is derived from the expanded graph node, it contains additional
// data members for parallel scheduling

class ParNode : public EGNode {

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

public: 
	// Constructor declaration
	ParNode(SDFStar* Mas, int invoc_no);

	// Constructor used for idle nodes or communication nodes
	ParNode(int type);

	// Is it correponds to a atomic CG star?
	CGStar* myStar() { return (CGStar*) myMaster(); }

	// Functions for assigning and obtaining the StaticLevel and processor
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
	void resetWaitNum() { waitNum = ancestors.mySize(); }

	// increase waitNum by 1
	void incWaitNum() { waitNum++; }

	// Return whether or not this node is presently fireable.
	int fireable() { waitNum--;
			 return (waitNum > 0)? FALSE : TRUE ; }

	// print outs.
	StringList print();	// Prints star name and invocation
};

#endif
