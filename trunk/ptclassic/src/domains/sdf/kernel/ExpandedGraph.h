/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha, based on S.  Bhattacharyya's code.
 
This file defines a APEG (expanded-graph) translated from an SDF graph.
It also contains all iterator class associated with the APEG graph.
The ExpandedGraph is used for the loop scheduler and parallel schedulers.

*******************************************************************/

#ifndef _ExpandedGraph_h
#define _ExpandedGraph_h
#ifdef __GNUG__
#pragma interface
#endif

#include "EGNode.h"

class Galaxy;

/////////////////////////
// class ExpandedGraph //
/////////////////////////
//
// Base class for expanded graphs.
//

class ExpandedGraph {

friend class EGMasterIter;
friend class EGSourceIter;

private:

	// Create the precedence links corresponding to an
	// arc in the SDF graph. The arguments : source star,
	// # samples produced by src, dest star, # sampless 
	// consumed by dest, number of delays on the arc.
	// Return value is nonzero iff there were no problems.
	int ExpandArc(DataFlowStar* src, PortHole* src_port, DataFlowStar* dest, 
		      PortHole* dest_port);

	// Check whether a star depends on previous invocations
	// of itself, and if so, insert the necessary precedence
	// links. If enforcedSelfLoop = TRUE, always makes the links.
	int enforcedSelfLoop;
	int SelfLoop(DataFlowStar& s);

	// Set up the invocations which are to form the initial graph.
	void initialize_invocations();
	void createInvocations(DataFlowStar*);

	// create an arc between two nodes
	EGGate* makeArc(EGNode *src, EGNode *dest, int samples, int delay)
		{return src->makeArc(dest,samples,delay);}

	// Create an arc between invocation "i" of star "src", and
	// invocation "j" of "dest". The arc goes from src to dest, and
	// carries "n_sam" samples, with "n_d" delay.
	// return 0 if fails. Return the source_gate.
	EGGate* connect_invocations(DataFlowStar* src, int i, DataFlowStar* dest,
                         int j, int n_sam, int n_d);

	// Identify which port and which sample is assign to each gate.
	// This information is necessary for parallel code generation
	void setGate(EGGate* src_gate, PortHole* src_port, int src_ix,
		     PortHole* dest_port, int dest_ix);

	// the node count
	int nodecount;

protected:
	// pointer to the original galaxy
	Galaxy* myGal;

	// The list of the first instances of the clusters and SDF 
	// atomic blocks whose invocations make up the extended graph
	EGNodeList masters;

	// The source nodes for the expanded graph : 
	// A node is considered a source node if it has no input arcs 
	// (ancestor links) of if all of its input arcs have one or 
	// more delays associated with them.
	EGNodeList sources;

	// Create a new star node with given invocation index
	virtual EGNode *newNode(DataFlowStar*, int);

public:

	// Constructor, Destructor
	ExpandedGraph() : myGal(0), nodecount(0) {}	
	virtual ~ExpandedGraph();

	// return the nodecount
	int numNodes() { return nodecount; }

	// Constructor for creating the expanded graph from a galaxy.
	// The resulting graph will depict the precedence relationships
	// among the stars in the galaxy.
	// Return value : nonzero iff a valid expanded graph could be created.
	// If the selfLoop is TRUE, make an arc between invocations of a
	// star regardless of the dependency.
	virtual int createMe (Galaxy& galaxy, int selfLoopFlag = 0);

	// Initialize the internal data structure.
	virtual void initialize();

	// insert a new source node into the source list for the graph
	void insertSource(EGNode *p) { sources.insert(p); }

	// method for displaying the graph
	virtual StringList display();

	// this method removes all arcs which have delays on
	// them
	virtual void removeArcsWithDelay();

};

///////////////////////////////////
// class EGMasterIter            //
///////////////////////////////////
//
// iterator class for the list of 
// masters in a expanded graph 
//

class EGMasterIter : public EGNodeListIter {
public:
	EGMasterIter(ExpandedGraph& eg) : EGNodeListIter(eg.masters) {}

  	// Return the next cluster in the masters list.
	void reconnect(ExpandedGraph& eg) 
		{EGNodeListIter::reconnect(eg.masters);} 
}; 

/////////////////////////////////////
// class EGSourceIter              //
/////////////////////////////////////

class EGSourceIter : public EGNodeListIter {
public:
	EGSourceIter(ExpandedGraph &g) : EGNodeListIter(g.sources) {}
	void reconnect(ExpandedGraph &g) 
		{EGNodeListIter::reconnect(g.sources);}
};

///////////////////////////////
// class EGIter              //
///////////////////////////////
//
// An iterator to access all the nodes of an expanded graph. 
// The invocations are traversed master by master, and for a given master
// all invocations are traversed.
//

class EGIter {
public:
	EGIter(ExpandedGraph& g) {
		INC_LOG_NEW; next_master = new EGMasterIter(g);
		ref_node = (*next_master)++;
	}
	~EGIter() { INC_LOG_DEL; delete next_master; }

	// start the traversal again
	void reset() {
		next_master->reset();
		ref_node = (*next_master)++;
	}

	// return the next node
	EGNode* next();
	EGNode* operator++() { return next(); }

	// reconnect this iterator to another graph
	void reconnect(ExpandedGraph& g) {
		next_master->reconnect(g);
		ref_node = (*next_master)++;
	}

private:
	EGNode* ref_node;
	EGMasterIter* next_master;
};

#endif
