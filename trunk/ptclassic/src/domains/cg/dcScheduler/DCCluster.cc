static const char file_id[] = "DCCluster.cc";
/*****************************************************************
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

Programmer: Soonhoi Ha based on G.C. Sih's code.
Date of last revision: 5/92 

*****************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "DCCluster.h"

// Constructor for elementary cluster
DCCluster::DCCluster(DCNodeList* nds) : name("") {

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

		// Set the elemDCCluster data member of each node
  		n->elemDCCluster = this;
	}
	nodes = nds;
	ExecTime = total;
}


// Constructor for higher level cluster which is union of two sub-clusters
DCCluster::DCCluster(DCCluster *clus1, DCCluster *clus2) : name("") {
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
DCCluster :: ~DCCluster() {
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
void DCCluster :: addArc(DCCluster* adj, int numSample) {

	DCClustArc* carc;

	if ((carc = InOutArcs.contain(adj))) {
		carc->addSamples(numSample);
	} else {
		LOG_NEW; carc = new DCClustArc(adj, numSample);
		InOutArcs.prepend(carc);
	}
}

				////////////////////
				///  setDCCluster  ///
				////////////////////
// Sets the node cluster property to point to the cluster
void DCCluster::setDCCluster(DCCluster* par) {
	if (par == 0) par = this;
	if (nodes) {
        	DCNodeListIter iter(*nodes);
        	DCNode *n;
        	while ((n = iter++) != 0) {
                	n->cluster = par;
        	}
	} else {
		component1->setDCCluster(par);
		component2->setDCCluster(par);
	}
}

				//////////////////////
				///  findCombiner  ///
				//////////////////////
// Returns the best cluster to combine it with, looking at communication cost.
// It checks InOutArcs to find which cluster passes the most samples and 
// breaks ties by returning the smallest ExecTime cluster.
DCCluster *DCCluster::findCombiner() {

	DCClustArc *bestArc = 0;
	int mostSamps = 0, smallestExec = 999999;
	DCClustArcListIter iter(InOutArcs);
	DCClustArc *carc;

	while ((carc = iter++) != 0) {
		DCCluster* DCClust = carc->getNeighbor();   // The cluster

		if (carc->getSamples() > mostSamps) {
			mostSamps = carc->getSamples();
			bestArc = carc;
			smallestExec = DCClust->getExecTime();
		} else if ((carc->getSamples() == mostSamps) &&
			(DCClust->getExecTime() < smallestExec)) {
			bestArc = carc;
			smallestExec = DCClust->getExecTime();
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
void DCCluster::fixArcs(DCCluster *c1, DCCluster *c2) {

	DCClustArcListIter iter(c1->InOutArcs);
	DCClustArc *arc;
	DCCluster *cl;

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
void DCCluster::assignP(int procNum) {
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
StringList DCCluster::print() {
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

