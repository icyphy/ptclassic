#ifndef _MacroGraph_h
#define _MacroGraph_h
#ifdef __GNUG__
#pragma interface
#endif

#include "DLGraph.h"
#include "DLNode.h"

/****************************************************************
Version identification:
$Id$

Copyright (c) 1995 Seoul National University
All rights reserved.

Programmer: Soonhoi Ha
Date of last revision:

*****************************************************************/

                //////////////////////
                // class MacroNode  //
                //////////////////////

// This class represents a node in the precedence graph.  While
// it is derived from the expanded graph node, it contains additional
// data members which are used for Quasi-scheduling.

class MacroNode : public DLNode {

public: 
        // Constructor declaration
        MacroNode(DataFlowStar* Mas, int invoc_no): descWork(-1),
		 DLNode(Mas, invoc_no) {}
 
        // Constructor used for idle nodes or communication nodes
        MacroNode(int type): DLNode(type) {}
 
        // redefine. Return the urgency of this node.
        /* virtual */ int getLevel()
                { if (StaticLevel > descWork) return StaticLevel;
		  else return descWork; }
 
	// redefine: copy star
	/* virtual */ DataFlowStar* copyStar(CGTarget* t, int pid, int flag);

        // set the urgency of the node
        int setDescWork(int i) { descWork = i; return i; }
	int workAfterMe() { return descWork; }
 
	// profile information
	Profile* profile() { return pf; }
	void withProfile(Profile* p) { pf = p; }

	// name of the real star or galaxy
	/* virtual */ const char* readRealName();

private:
	// If it is a parallel node, it should be associated with
	// a "Profile" and a processor ids to indicate assignment
	// assignedId[i] = j means that (i+1)th profile is assigned to the
	// (j+1)th processor.
	Profile* pf;

        int descWork;
};


                ///////////////////////
                // class MacroGraph  //
                ///////////////////////

// This class stores the precedence graph which is scheduled by
// Dynamic level Scheduler.  It consists of MacroNodes (derived from ParNodes) 
// connected through a subset of the arcs of the ExpandedGraph.

class MacroGraph : public DLGraph {

public:
	// reset the graph for new schedules.
	// initialize runnableNodes.
	/* virtual */ void resetGraph();

	// compute the total work of a node's descendents.
	int workAfter(MacroNode* pd);

	// methods for "unschedWork"
	void decreaseWork(int val) { unschedWork -= val; }
	int sizeUnschedWork()	{ return unschedWork; }

protected:
	// redefine initializeGraph to compute urgency of nodes
	/* virtual */ int initializeGraph();

	// The sum of unscheduled work
	int unschedWork;

	// redefine these virtual allocator to allocate Macro Nodes.
	EGNode *newNode(DataFlowStar*, int);
};

#endif
