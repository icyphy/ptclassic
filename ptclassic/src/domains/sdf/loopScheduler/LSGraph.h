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


#ifndef _LSGraph_h
#define _LSGraph_h
#ifdef __GNUG__
#pragma interface
#endif

#include "ExpandedGraph.h"
#include "ReachabilityMatrix.h"
#include "Galaxy.h"
#include "LSNode.h"
#include "LSCluster.h"

class ClusterNodeList;


                        /////////////////////////
                        // class LSGraph       //
                        /////////////////////////

//////////////////////////////////////////////////////////////
//                                                          //
// Expanded Graph for the Loop Scheduler                    //
//                                                          //
//////////////////////////////////////////////////////////////

class LSGraph : public ExpandedGraph 
{
friend class DecomScheduler;

public:
	LSGraph() : RM(0) {}
	~LSGraph();

	// reset visit flags of nodes.
	void resetVisits();

	// Constructor for creating the expanded graph from a galaxy.
	// The resulting graph will depict the dataflow relationships
	// among the different invocations of the stars in the galaxy.
	int createMe(Galaxy& galaxy, int selfLoopFlag = 0);

	// Set up an invocation of a cluster whose master is given as 
	// the first argument. The second argument contains the nodes
	// in the APEG graph to be clustered. The third argument 
	// indicates the invocation index of the node.
	int newClusterInvocation(LSCluster*, ClusterNodeList*, int);

	// If there is an isolated node in its candidate list,
	// we try to make a cluster of those kinds of nodes.
	void formIsolatedCluster(LSNode&);

	// Pop the first element of the scheduler source list.
	LSNode *candidateFromFront() { return candidates.takeFromFront(); }
	void candidatePushBack(LSNode* p) { candidates.insert(p); }

	// Insert the new LSNode into the candidate list in a manner 
	// which preserves the decreasing-order-of-freqency.
	void sortedInsert(LSNode*);

	// Remove the current from the candidate list.
	void removeCandidate(LSNode* p) { candidates.remove(p); }

	// check whether the formation of cluster "nl" creates
	// a cyclic path in the graph.
	int introducesCycle(ClusterNodeList &nl)
		{return RM->introducesCycle(nl);}

	// Initialize the candidate list by storing the first invocation 
	// of each master in decreasing order of master frequency.
	void initializeCandidates();

	// check whether there is path between nodes.
	int pathExists(LSNode& src, LSNode& dest) {
		return RM->pathExists(src,dest);
	}

	// add a cluster into the list
	void addList(LSCluster* p) { clusters.put(p); }

protected:
	// redefine these virtual allocators to allocate LS  nodes.
	EGNode *newNode(DataFlowStar*,int);

private:
	// make the reachability matrix
	int makeRM();

	// Pointer to the node of the previous invocation.
	// Used in newClusterInvocation() method.
	LSNode* prevNode;

	// The list of nodes maintained for scheduler sequencing --
	// these nodes maintain the order in which the scheduling
	// algorithm visits nodes. 
	LSNodeList candidates;

	// The reachability matrix for the graph
	ReachabilityMatrix* RM;

	// LSCluster list for memory management
	SequentialList clusters;
};


class LSIter : public EGIter {
public:
	LSIter(LSGraph& g) : EGIter(g) {}
	LSNode* next() { return (LSNode*) EGIter :: next(); }
	LSNode* operator++() { return next(); }
};

#endif
