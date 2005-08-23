#ifndef _DCCluster_h
#define _DCCluster_h
#ifdef __GNUG__
#pragma interface
#endif

/*****************************************************************
Version identification:
@(#)DCCluster.h	1.5	3/2/95

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

Programmer: Soonhoi Ha based on Gil Sih's code

DCCluster is used by the parallel scheduler

*****************************************************************/

#include "DCClustArcList.h"
#include "DCNode.h"

			///////////////////////
			//  class DCCluster  //
			///////////////////////
// A cluster is a group of nodes.
class DCCluster {
public:

	// Constructors
	DCCluster(DCNodeList *nds);
	DCCluster(DCCluster *clus1, DCCluster *clust2);

	// Destructor
	// we "own" the node list itself but not its contents,
	// so this will zap only the SingleLink things that make up
	// the list.
	~DCCluster(); 

	// Return the sum of execution times of all nodes in the cluster
	int getExecTime() {return ExecTime;}

	// add an clusterArc.
	void addArc(DCCluster*, int);

	// Fix the InOutArcs when combining clusters
	void fixArcs(DCCluster *c1, DCCluster *c2);

	// reset members
	void resetMember() { intact = 1; score = 0; }

	// set name
	void setName(const char* n) { name = n; }

	// get name
	const char* readName() { return name; }

	// Assigns every node in the cluster a pointer to the cluster
	void setDCCluster(DCCluster*);

	// return the cluster between two component clusters to be
	// pulled out. May check the processor constrains later.
	// Right now, just return the cluster of smaller execution time.
	DCCluster* pullWhich() { 
		return (component1->getExecTime() < component2->getExecTime())?
			component1: component2;
	}

	// Returns the best cluster to
	//      combine it with, looking at communication cost.
	DCCluster* findCombiner();

	// print
	StringList print();

	// Assigns every node in the cluster to the given processor
	void assignP(int procNum);

	// Returns the processor that this cluster is assigned to
	int getProc() {return Proc;}

	// switch clusters
	void switchWith(DCCluster* cl) {
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
	DCCluster *getComp1() {return component1;}
	DCCluster *getComp2() {return component2;}

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
	DCCluster *component1;
	DCCluster *component2;

	// The arcs coming into and out of the cluster
	DCClustArcList InOutArcs;
};

#endif
