static const char file_id[] = "Cluster.cc";
/*****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

Programmer: Soonhoi Ha based on G.C. Sih's code.
Date of last revision: 5/92 

*****************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "Cluster.h"

// Constructor for elementary cluster
Cluster::Cluster(DCNodeList* nds) : name("") {

	// InOutArcs are set in DeclustScheduler
	InOutArcs.initialize();

	component1 = component2 = 0;
	score = 0;
	intact = 1;

	DCNodeListIter iter(*nds);
	DCNode *n;
	int total = 0;

	// Find the total execution time of all the nodes
	while ((n = iter++) != 0) {
		total += n->getExTime();

		// Set the elemCluster data member of each node
  		n->elemCluster = this;
	}
	nodes = nds;
	ExecTime = total;
}


// Constructor for higher level cluster which is union of two sub-clusters
Cluster::Cluster(Cluster *clus1, Cluster *clus2) : name("") {
	intact = 1;	// Initialize to unbroken cluster
	score = 0;
	nodes = 0;
	component1 = clus1;
	component2 = clus2;
	ExecTime = clus1->getExecTime() + clus2->getExecTime();

	// Set the InOutArcs
	InOutArcs.initialize();
	fixArcs(clus1, clus2);
	fixArcs(clus2, clus1);
}

// destructor
Cluster :: ~Cluster() {
	if (nodes) {
		// delete nodes
		LOG_DEL; delete nodes;
		// delete cluster arcs
		InOutArcs.removeArcs();
	}
}

				////////////////
				///  addArc  ///
				////////////////
void Cluster :: addArc(Cluster* adj, int numSample) {

	ClustArc* carc;

	if ((carc = InOutArcs.contain(adj))) {
		carc->addSamples(numSample);
	} else {
		LOG_NEW; carc = new ClustArc(adj, numSample);
		InOutArcs.tup(carc);
	}
}

				////////////////////
				///  setCluster  ///
				////////////////////
// Sets the node cluster property to point to the cluster
void Cluster::setCluster(Cluster* par) {
	if (par == 0) par = this;
	if (nodes) {
        	DCNodeListIter iter(*nodes);
        	DCNode *n;
        	while ((n = iter++) != 0) {
                	n->cluster = par;
        	}
	} else {
		component1->setCluster(par);
		component2->setCluster(par);
	}
}

				//////////////////////
				///  findCombiner  ///
				//////////////////////
// Returns the best cluster to combine it with, looking at communication cost.
// It checks InOutArcs to find which cluster passes the most samples and 
// breaks ties by returning the smallest ExecTime cluster.
Cluster *Cluster::findCombiner() {

	ClustArc *bestArc = 0;
	int mostSamps = 0, smallestExec = 999999;
	ClustArcListIter iter(InOutArcs);
	ClustArc *carc;

	while ((carc = iter++) != 0) {
		Cluster* Clust = carc->getNeighbor();   // The cluster

		if (carc->getSamples() > mostSamps) {
			mostSamps = carc->getSamples();
			bestArc = carc;
			smallestExec = Clust->getExecTime();
		} else if ((carc->getSamples() == mostSamps) &&
			(Clust->getExecTime() < smallestExec)) {
			bestArc = carc;
			smallestExec = Clust->getExecTime();
		}
	}

	if (mostSamps == 0)
		return 0;
	else
		return bestArc->getNeighbor();
}

				/////////////////
				///  fixArcs  ///
				/////////////////
void Cluster::fixArcs(Cluster *c1, Cluster *c2) {

	ClustArcListIter iter(c1->InOutArcs);
	ClustArc *arc;
	Cluster *cl;

	while ((arc = iter++) != 0) {
		if ((cl = arc->getNeighbor()) != c2) {
			if (!InOutArcs.contain(c1))
				InOutArcs.put(arc);

			// Change other guys to show this cluster
			cl->InOutArcs.changeArc(c1, this);
		}
	}
}

				/////////////////
				///  assignP  ///
				/////////////////
// Assigns every node in the cluster to the given processor
void Cluster::assignP(int procNum) {
	// Assign the cluster to this processor
	Proc = procNum;

	if (nodes) {
		DCNodeListIter iter(*nodes);
		DCNode *n;
		while ((n = iter++) != 0) {
			n->assignProc(procNum);
		}
	} else {
		component1->assignP(procNum);
		component2->assignP(procNum);
	}
}

				///////////////
				///  print  ///
				///////////////
StringList Cluster::print() {
	StringList out;
	out += "name:  ";
	out += name;
	if (component1) {
		out += "  (";
		out += component1->readName();
		out += ",  ";
		out += component2->readName();
		out += ")\n";
	} else out += "\n";
	return out;
}

