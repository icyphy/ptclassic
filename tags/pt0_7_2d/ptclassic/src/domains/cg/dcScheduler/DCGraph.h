#ifndef _DCGraph_h
#define _DCGraph_h
#ifdef __GNUG__
#pragma interface
#endif

#include "ParGraph.h"
#include "DCNode.h"
#include "DCArcList.h"
#include "DCClusterList.h"

/****************************************************************
Version identification:
@(#)DCGraph.h	1.8	3/2/95

Copyright (c) 1990-1995 The Regents of the University of California.
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

Programmer: Soonhoi Ha based on G.C. Sih's code
Date: 5/92

DCGraph is used by the parallel scheduler to represent
	an acyclic precedence graph.

*****************************************************************/

class ostream;

                //////////////////////
                //   class DCGraph   //
                //////////////////////

// This class stores the precedence graph which is scheduled by
// DeclustScheduler.  It consists of DCNodes (derived from EGNodes) 
// connected through a subset of the arcs of the ExpandedGraph.
// It is important to make this a separate class because there are
// many precedence graphs which can be derived from the same expanded
// graph after performing operations such as retiming or pipelining.  See
//	Gilbert C. Sih, "Multiprocessor Scheduling to Account for
//	Interprocessor Communication", UCB/ERL Memo M91/29 (April, 1991)
// for more information concerning these equivalence transformations.

class DCGraph : public ParGraph {
public:
	// Nodes with more than one descendant
	DCNodeList BranchNodes;

	// Nodes with more than one ancestor
	DCNodeList MergeNodes;

	~DCGraph() {}

	// generate the name of the clusters
	// If type = 0, elementary cluster, otherwise, macro cluster.
	const char* genDCClustName(int type);

	// Display the graph
	StringList display();
	
	// find out the first merge node in intersection of TClosures
	DCNode* intersectNode(DCNode*, DCNode*, int);

	// trace arcs between two nodes.
	DCArcList* traceArcPath(DCNode* branch, DCNode* S, DCNode* D, int dir);

	// add a cutArc
	void addCutArc(DCArc* arc)  
		{ if (cutArcs.member(arc) == 0) cutArcs.append(arc); }

	// form an elementary clusters
	void formElemDCClusters(DCClusterList&);

	// compute the score for clusters.
	// score of a cluster = (#samples passed offproc) = (#samples passed
	// onproc).
	void computeScore();

	// find the processors that clust communicates with
	void commProcs(DCCluster* clust, int* procs);

	// save scheduling information of the nodes
	void copyInfo();

protected:
	// redefine these virtual allocators to allocate DCNodes
	EGNode* newNode(DataFlowStar*, int);

	// Initialize the graph.  Returns TRUE if okay, else returns FALSE.
	int initializeGraph();

private:
	// A list of all the DCNodes sorted largest StaticLevel first
	DCNodeList sortedNodes;

	// curArc list
	DCArcList cutArcs;

	// Removes the cutArcs from the graph
	void removeCutArcs(DCNodeList&);

	// Assign the transitive closure for each node
	void assignTC();

	// Assign the transitive closure for a single node
	// if direction = 1, reverse transitive closure.
	void SingleNodeTC(DCNode*, int direction);

	// merge the TClosure of the second node into that of the first node.
	void mergeClosure(DCNodeList&, DCNodeList&);

	// Trace a path of DCNodes between start and end nodes.
	DCNodeList* tracePath(DCNode *start, DCNode *end, int flag);

	// Called by tracePath to extend the path by one node.
	DCNode* extendPath(DCNode *node, DCNode *dest, int flg);

	// Sort the nodes by StaticLevel
	void sortDCNodes();

	// add a node into a given cluster
	void addToDCCluster(DCNode*, DCNodeList*);

	// set inter cluster arcs
	void setInterclusterArcs();

	int clustNumber;
};

class DCIter : public EGIter {
public:
	DCIter(DCGraph& g) : EGIter(g) {}
	DCNode* next() { return (DCNode*) EGIter :: next(); }
	DCNode* operator++(POSTFIX_OP) { return next(); }
};

#endif
