#ifndef _ParGraph_h
#define _ParGraph_h
#ifdef __GNUG__
#pragma interface
#endif

#include "ExpandedGraph.h"
#include "Galaxy.h"
#include "ParNode.h"

/****************************************************************
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

Programmer: Soonhoi Ha
Date of last revision:

*****************************************************************/

class logstrm;

                ////////////////////////
                //   class ParGraph   //
                ////////////////////////

// This class stores the basic precedence graph which is scheduled by
// parallel scheduler.  It consists of ParNodes (derived from EGNodes) 
// connected through a subset of the arcs of the ExpandedGraph.

class ParGraph : public ExpandedGraph {

protected:
	// The number of Nodes in the graph
	int nodeCount;

	// The sum of node execution times in the graph
	int ExecTotal;

	// my galaxy.
	Galaxy* myGal;

	// SequentialList of Node-Pairs.
	SequentialList nodePairs;

	// initialize the graph 
	virtual int initializeGraph();
	
	// Find and set the static level of the given node
	int SetNodeSL(ParNode*);

	// redefine these virtual allocator to allocate DL Nodes.
	EGNode *newNode(DataFlowStar*, int);

	// for logging information
	ostream* logstrm;

public:
	// Null constructor : See comment for ExpandedGraph constructor
	ParGraph(): logstrm(0), myGal(0) {}
	~ParGraph();

	// A list of initially runnable Nodes
	EGNodeList runnableNodes;

	// fill runnableNodes list.
	void findRunnableNodes();

	// Function which actually constructs the expanded graph
	int createMe(Galaxy& galaxy, int selfLoopFlag = 0);

	// The sum of node execution times in the graph
	int getExecTotal() {return ExecTotal;}

	// get my galaxy.
	Galaxy* myGalaxy() { return myGal; }

	// set log stream
	void setLog(ostream* l) { logstrm = l; }

	// return the maximum schedule distance between node pairs.
	int pairDistance();

        // Put given node in sorted order into the EGNodeList
	// Sorts highest SL first if flag = 1, lowest SL first if flag = 0
	void sortedInsert(EGNodeList&, ParNode *, int flag);

	// Replenishes the tempAncs and tempDescs for each node in the DCGraph.
	void replenish(int flag);

        // redefine
        void removeArcsWithDelay();

	// restore hidden gates
	void restoreHiddenGates();

};

class NodePair {
	ParNode* start;
	ParNode* dest;

public:
	NodePair(ParNode* s, ParNode* d) {
		start = s; dest = d;
	}
	ParNode* getStart() { return start; }
	ParNode* getDest()  { return dest; }
};

#endif
