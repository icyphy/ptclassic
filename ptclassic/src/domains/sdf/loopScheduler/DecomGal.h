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

 Programmer:  Soonhoi Ha 
 Date of creation: 4/92

 Galaxy is decomposed into sets of strongly connected components.

*******************************************************************/


#ifndef _DecomGal_h
#define _DecomGal_h
#ifdef __GNUG__
#pragma interface
#endif

#include "SDFCluster.h"
#include "DecomScheduler.h"
class ostream;

                        //////////////////////////
                        // class DecomGal       //
                        //////////////////////////

class DecomGal : public SDFClusterGal
{
public:
	DecomGal(Galaxy& g, ostream* l) : SDFClusterGal(g,l) {}
	DecomGal(ostream* l) : SDFClusterGal(l) {}

	// remove Arcs with delays if these delays are enough to
	// fire the destination nodes as many as repetitions numbers.
	int simplify();

	// Detect all strongly connected components and cluster them
	// to decompose the galaxy.
	void decompose();

	// loop all clusters for the final clustering phase.
	void loopAll();

private:
	// Make clusters of strongly connected graph
	void makeCluster(SequentialList*);

	// The nodes between the stop node and the start node are
	// strongly connected. We trace back the nodes from the start node
	// to the stop node to identify those nodes.
	SequentialList* makeList(SDFCluster* start, SDFCluster* stop);

	// find a strongly connected component
	// depth-first search.
	SequentialList* findSCComponent(SDFCluster* c);

	// setup the decomposed clusters.
	void setUpClusters();
};

class DecomClusterBag : public SDFClusterBag {

public:
	// create the SDFClusterGal
	void createGalaxy(ostream* log) { 
		INC_LOG_NEW; cgal = new DecomGal(log);
		INC_LOG_NEW; loopSched = new DecomScheduler(log);
	}

	// setup Galaxy
	void setUpGalaxy();

	DecomClusterBag() : cgal(0), loopSched(0), idFlag(1) {}
	~DecomClusterBag();

	SDFClusterGal* clusterGal() { return cgal; }

	// regard it as an atomic cluster for Joe's clustering
	SDFClusterBag* asBag() { return idFlag? this: 0; }
	void resetId()	{ idFlag = 0; }

	// return me as a special bag
	SDFClusterBag* asSpecialBag() { return this; }

	// run the cluster
	void go();

protected:
	// redefine, generate my Schedule
	int genSched() { 
		sched = loopSched;
		return loopSched->genSched(cgal); 
	} 

private:
	// private scheduler
	DecomScheduler* loopSched;

	// The outermost bag will create an SDFClusterGal member.
	DecomGal* cgal;

	// id: Bag in the decomposition, Atomic in the Joe's clustering.
	int idFlag;
};

#endif
