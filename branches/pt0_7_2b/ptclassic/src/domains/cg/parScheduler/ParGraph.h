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

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

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

	// for logging information
	ostream* logstrm;

public:
	// Null constructor : See comment for ExpandedGraph constructor
	ParGraph(): logstrm(0), myGal(0) {};
	~ParGraph();

	// Function which actually constructs the expanded graph
	int createMe(Galaxy& galaxy);

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

        // redefine
        void removeArcsWithDelay();
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
