/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  S.  Bhattacharyya

*******************************************************************/

#ifndef _ReachabilityMatrix_h
#define _ReachabilityMatrix_h

class LSNode;
class LSGraph;
class ClusterNodeList;
class EGGateList;
class BooleanMatrix;

////////////////////////////
// Class ReachabilityMatrix //
////////////////////////////
//
// This class implements a matrix for indicating reachability between 
// nodes in an expanded graph. Each node in a graph has an index into the
// reachability matrix for the graph. If node A has index i and node B 
// has index j, then entry i,j is 1 if there is a path from A to B, 
// entry i,j is 0 otherwise.  There is a mechanism for clusterizing nodes
// and modifying the reachability matrix appropriately.
//
// Only a single bit is allocated for each entry in the matrix
// (class BooleanMatrix).

class ReachabilityMatrix {

private : 

	// the number of rows and columns in the matrix
	int dim;

	// the reachability matrix
	BooleanMatrix *matrix; 

	// the last index allocated to a node 
	int current_index;

	// connect two nodes, the connection goes from parent to child
	inline void connect(LSNode *parent, LSNode *child);

	// add the set of nodes reachable by the child to
	// the set of nodes reachable by the parent
	void addPaths(int parentindex, int childindex);

	// Set up the reachability matrix for the graph "g".
	void initialize(LSGraph&);  

	// set up the connections for node "p"
	void initialize(LSNode*);

public :
	// Constructor to create the Reachability Matrix for an expanded graph.
	ReachabilityMatrix(LSGraph&);

	// destructor -- deallocate the reachability matrix
	~ReachabilityMatrix(); 

	// Check whether the formation of cluster "nl" creates
	// a cyclic path in the graph. 
	int introducesCycle(ClusterNodeList&);

	// Modify the matrix for the formation of cluster "nl".
	// Return the RM index corresponding to the new cluster.
	int formCluster(ClusterNodeList&, EGGateList&);

	// return 1 iff there is a path from src to dest
	// return 0 iff there is no path  
	int pathExists(LSNode&, LSNode&);

	// allocate an index into the reachability matrix
	// Return -1 if we're out of indeces.
	int allocateIndex();
};

#endif

