#ifndef _Cluster_h
#define _Cluster_h
#ifdef __GNUG__
#pragma interface
#endif

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

Programmer: Soonhoi Ha based on Gil Sih's code

Cluster is used by the parallel scheduler

*****************************************************************/

#include "ClustArcList.h"
#include "DCNode.h"

			/////////////////////
			//  class Cluster  //
			/////////////////////
// A cluster is a group of nodes.
class Cluster {
public:

	// Constructors
	Cluster(DCNodeList *nds);
	Cluster(Cluster *clus1, Cluster *clust2);

	// Destructor
	// we "own" the node list itself but not its contents,
	// so this will zap only the SingleLink things that make up
	// the list.
	~Cluster(); 

	// Return the sum of execution times of all nodes in the cluster
	int getExecTime() {return ExecTime;}

	// add an clusterArc.
	void addArc(Cluster*, int);

	// Fix the InOutArcs when combining clusters
	void fixArcs(Cluster *c1, Cluster *c2);

	// reset members
	void resetMember() { intact = 1; score = 0; }

	// set name
	void setName(const char* n) { name = n; }

	// get name
	const char* readName() { return name; }

	// Assigns every node in the cluster a pointer to the cluster
	void setCluster(Cluster*);

	// return the cluster between two component clusters to be
	// pulled out. May check the processor constrains later.
	// Right now, just return the cluster of smaller execution time.
	Cluster* pullWhich() { 
		return (component1->getExecTime() < component2->getExecTime())?
			component1: component2;
	}

	// Returns the best cluster to
	//      combine it with, looking at communication cost.
	Cluster* findCombiner();

	// print
	StringList print();

	// Assigns every node in the cluster to the given processor
	void assignP(int procNum);

	// Returns the processor that this cluster is assigned to
	int getProc() {return Proc;}

	// switch clusters
	void switchWith(Cluster* cl) {
		int temp = Proc;
		assignP(cl->getProc());
		cl->assignP(temp);
	}

	// The cluster, itself or any subcluster, was broken into 
	// its components.
	void broken() {intact = 0;}

	// Tells if the cluster is broken or not
	int getIntact() { return intact; }

	// Return pointers to the cluster components, 0 if elementary
	Cluster *getComp1() {return component1;}
	Cluster *getComp2() {return component2;}

	// get and set the score.
	int getScore() {return score;}
	int setScore(int sc) { return score = sc;}

private:
	const char* name;	// my name

	// A score reflecting propensity to communicate off processor
	int score;

	// The nodes in the cluster
	DCNodeList* nodes;

	// The processor that this cluster is assigned to
	int Proc;

	// The sum of execution times of nodes in the cluster
	int ExecTime;

	// intact = 1 if the cluster is still unbroken
	int intact;

	// The component clusters if this cluster is not elementary
	Cluster *component1;
	Cluster *component2;

	// The arcs coming into and out of the cluster
	ClustArcList InOutArcs;
};

#endif
