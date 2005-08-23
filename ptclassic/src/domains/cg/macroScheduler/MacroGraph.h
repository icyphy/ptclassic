#ifndef _MacroGraph_h
#define _MacroGraph_h
#ifdef __GNUG__
#pragma interface
#endif

#include "DLGraph.h"
#include "DLNode.h"

/****************************************************************
Version identification:
@(#)MacroGraph.h	1.3	12/08/97

Copyright (c) 1995 Seoul National University
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
                // class MacroNode  //
                //////////////////////

// This class represents a node in the precedence graph.  While
// it is derived from the expanded graph node, it contains additional
// data members which are used for Quasi-scheduling.

class MacroNode : public DLNode {

public: 
        // Constructor declaration
        MacroNode(DataFlowStar* Mas, int invoc_no):
		 DLNode(Mas, invoc_no), descWork(-1) {}
 
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
