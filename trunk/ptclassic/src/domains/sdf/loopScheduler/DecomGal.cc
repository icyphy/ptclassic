static const char file_id[] = "DecomGal.cc";

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

#ifdef __GNUG__
#pragma implementation
#endif

#include "DecomGal.h"
#include "streamCompat.h"

// loop all clusters with repetition counters
void DecomGal :: loopAll() {
	SDFClusterGalIter nextClust(*this);
	SDFCluster* c;
	while ((c = nextClust++) != 0) {
		int k = c->reps();
		if (k > 1) c->loopBy(k);
	}
}

// remove Arcs with delays if these delays are enough to
// fire the destination nodes as many as repetitions numbers.
int DecomGal :: simplify() {
	if (logstrm)
		*logstrm << "starting simplify()\n";

	int flag = FALSE;
	SDFClusterGalIter nextClust(*this);
	SDFCluster* c;
	SDFClustPort* toBeDeleted = 0;	// necessary in order to not destroy
					// the list under current iteration.

	while ((c = nextClust++) != 0) {
		SDFClustPortIter nextP(*c);
		SDFClustPort* p;
		while ((p = nextP++) != 0) {
			if (p->isItOutput()) continue;
			if (p->numTokens() >= 
				p->numIO() * int(c->repetitions)) {
				// this port has enough delay. Remove it.
				LOG_DEL; delete p->far();
				p->disconnect(0);
				if (toBeDeleted) {
					LOG_DEL; delete toBeDeleted;
				}
				toBeDeleted = p;
				flag = TRUE;
			}
		}
	}

	if (toBeDeleted) {
		LOG_DEL; delete toBeDeleted;
	}

	if (logstrm)
		*logstrm << "finishing simplify()\n";

	return flag;
}
				
// Detect all strongly connected components and cluster them
// to decompose the galaxy.
void DecomGal :: decompose() {
	if (logstrm)
		*logstrm << "starting decompose()\n";

	if (numberClusts() <= 1) return;

	SDFClusterGalIter nextClust(*this);
	SDFCluster* c;

	int change = TRUE;

	while (change) {
		change = FALSE;

		// reset visit flags.
		nextClust.reset();
		while ((c = nextClust++) != 0) {
			c->setVisit(0);
		}
		nextClust.reset();
		
		// find a strongly connected components.
		while ((c = nextClust++) != 0) {
			SequentialList* clist;
			if (!(c->visited()) && (clist = findSCComponent(c))) {
				makeCluster(clist);
				LOG_DEL; delete clist;
				change = TRUE;
				break;
			}
		}
	}

	setUpClusters();

	if (logstrm)
		*logstrm << "finishing decompose()\n";
}

// find a strongly connected component
// depth-first search.
SequentialList* DecomGal :: findSCComponent(SDFCluster* c) {
	c->setVisit(1);
		
	SDFClustPortIter nextP(*c);
	SDFClustPort* p;

	while ((p = nextP++) != 0) {
		// downward direction.
		if (p->isItInput()) continue;

		SDFCluster* peer = p->far()->parentClust();
		if (peer == c) continue;
		if (peer->visited() > 0) {
			return makeList(c, peer);
		} else if (peer->visited() == 0) {
			SequentialList* tempList = findSCComponent(peer);
			if (tempList) return tempList;
		}
	}
	c->setVisit(-1);
	return 0;
}

// The nodes between the stop node and the start node are
// strongly connected. We trace back the nodes from the start node
// to the stop node to identify those nodes.
SequentialList* DecomGal :: makeList(SDFCluster* start, SDFCluster* stop) {

	LOG_NEW; SequentialList* newList = new SequentialList;

	SDFCluster* path = start;
	
	while (path != stop) {
		newList->prepend(path);

		SDFClustPortIter nextP(*path);
		SDFClustPort* p;

		while ((p = nextP++) != 0) {
			//  upward direction.
			if (p->isItOutput()) continue;
			SDFCluster* peer = p->far()->parentClust();
			if (peer == path) continue;
			if (peer->visited()) {
				path = peer;
				break;
			}
		}
	}
	newList->prepend(stop);
	return newList;
}
	
// Make clusters of strongly connected graph.
void DecomGal :: makeCluster(SequentialList* nlist) {

	DecomClusterBag* bag = 0;
	
	// check whether there is a cluster Bag in the list.
	// If yes, choose one.
	ListIter nodes(*nlist);
	SDFCluster* c;
	while ((c = (SDFCluster*) nodes++) != 0) {
		if ((bag = (DecomClusterBag*) c->asSpecialBag())) break;
	}
	if (!bag) {
		LOG_NEW; bag = new DecomClusterBag;  
		addBlock(bag->setBlock(genBagName(), this));
	}

	nodes.reset();
	while ((c = (SDFCluster*) nodes++) != 0) {
		if (c->asSpecialBag()) {
			if (c != bag) bag->merge((SDFClusterBag*)c, this);
		} else {
			bag->absorb(c,this);
		}
	}
}

// setup decomposed clusters
void DecomGal :: setUpClusters() {

	if (logstrm)
		*logstrm << "starting cluster-setup\n";

	SDFClusterGalIter nextC(*this);
	SDFCluster* c;

	// setup them
	while ((c = nextC++) != 0) {
		DecomClusterBag* tBag = (DecomClusterBag*) c->asSpecialBag();
		if (tBag) {
			tBag->createGalaxy(logstrm);
			tBag->setUpGalaxy();
			tBag->resetId();
		}
	}

	if (logstrm)
		*logstrm << "finishing setup\n";
}

// After finding out the new repetition numbers of each block,
// we update the repetition of this bag and the numberTokens of the
// portholes.

void DecomClusterBag :: setUpGalaxy() {

	// move clusters from gal to cgal.
	SDFClusterBagIter nextC(*this);
	SDFCluster* c;

	while ((c = nextC++) != 0) {
		gal->removeBlock(*c);
		nextC.reset();
		cgal->addBlock(*c, c->name());
	}

	// reference to compute the repetition property.
	int refIter = 0;
	SDFClustPortIter nextPort(*this);
	SDFClustPort* p = nextPort++;
	if (p) { 
		SDFCluster* inC = p->inPtr()->parentClust();
		refIter = inC->repetitions;
	}
	
	// calculate new repetitions
	loopSched->setGalaxy(*cgal);
	loopSched->setup();

	// update data members of this bag and portholes.
	nextPort.reset();
	int flag = 0;
	while ((p = nextPort++) != 0) {
		SDFClustPort* inP = p->inPtr();
		SDFCluster* inC = inP->parentClust();
		int fac = inC->repetitions;
		if (!flag) {
			flag = TRUE;
			repetitions = refIter / fac;
		}
		p->numberTokens = inP->numberTokens * fac;

		// detach the "inp" Port.
		inP->parent()->removePort(*inP);
		inP->setNameParent("",0);
	}
}

// run the cluster, taking into account the loop factor
void DecomClusterBag::go() {
	loopSched->setStopTime(loop()+exCount);
	loopSched->run();
	exCount += loop();
}

// destructor
DecomClusterBag :: ~DecomClusterBag() {
	LOG_DEL; delete cgal;
	
	SDFClustPortIter nextPort(*this);
	SDFClustPort* p;
	while ((p = nextPort++) != 0) {
		delete p->inPtr();
	}
}
