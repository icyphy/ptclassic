static const char file_id[] = "HierScheduler.cc";

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

Programmer: Jose Luis Pino

*****************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "HierScheduler.h"
#include "GalIter.h"
#include "GraphUtils.h"
#include "HierCluster.h"
#include <iostream.h>

// These are needed for Cluster->Worm conversion
#include "ShadowTarget.h"
#include "Domain.h"
#include "EventHorizon.h"
#include "CGWormBase.h"

/*virtual*/double HierScheduler::getStopTime() {
    return topScheduler.getStopTime();
}	

/*virtual*/ void HierScheduler::setStopTime(double limit) {
    topScheduler.setStopTime(limit);
}

/*virtual*/ StringList HierScheduler::displaySchedule(){
    StringList schedule;
    schedule << "J.L. Pino's Hierarchical Scheduler\n\t["
	     << sdfStars << " SDF Nodes]\t[" << dagNodes()
	     << " Precedence DAG Nodes]\n\tTop-level "
	     << topScheduler.displaySchedule(); 
    GalStarIter nextStar(*galaxy());
    DataFlowStar* star;
    while ((star = (DataFlowStar*) nextStar++) != NULL)
	if (star->isItWormhole()) {
	    schedule << "Cluster \"" << star->fullName() << "\" using\n\t"
		     << star->scheduler()->displaySchedule()
		     << "_______________________________________"
		     << "_______________________________________\n\n";
	}
    return schedule;
}   

/*virtual*/ int HierScheduler::run() { return topScheduler.run(); }

int HierScheduler::dagNodes() const {
    return topScheduler.dagNodes();
}

#define VISITED flags[0]
#define NUMBER flags[1]
#define PREDECESSORS flags[2]
#define SUCCESSORS flags[3]

inline int amIChainSource(Block& b) {
    if (b.SUCCESSORS != 1) return FALSE;
    if (b.PREDECESSORS != 1) return TRUE;
    PredecessorIter predecessors(b);
    Block* predecessor = predecessors.next();
    if (predecessor && predecessor->SUCCESSORS > 1) return TRUE;
    return FALSE;
}

// The complexity is shown within the O() function.
// Let:
//	V = number of top blocks in the galaxy
//	E = number of arcs in the galaxy

void clusterChains(Galaxy& g) {

    // initialize flags for finding chain clusters - O(V)
    ClusterIter nextCluster(g);
    Cluster *cluster;
    int clusterNum = 0;
    while ((cluster = nextCluster++) != NULL) {
	// It is more efficient to initialize the flag with
	// the largest index first
	cluster->SUCCESSORS = 0;
	cluster->PREDECESSORS = 0;
	cluster->NUMBER = clusterNum++;
	cluster->VISITED = cluster->NUMBER;
    }

    // compute total number of predecessors and successors - O(E)
    nextCluster.reset();
    while ((cluster = nextCluster++) != NULL) {
	SuccessorIter nextSuccessor(*cluster);
	Block* successor;
	while ((successor = nextSuccessor++) != NULL) {
	    if (successor->VISITED == cluster->NUMBER) continue;
	    successor->VISITED = cluster->NUMBER;
	    successor->PREDECESSORS++;
	    cluster->SUCCESSORS++;
	}
    }

    nextCluster.reset();
    while ((cluster = nextCluster++) != NULL) {
	if (!amIChainSource(*cluster)) continue;
	SuccessorIter successors(*cluster);
	Block* successor = successors.next();
	while (successor && successor->PREDECESSORS == 1) {
	    cluster->merge((Cluster&)*successor,FALSE);
	    if (successor->SUCCESSORS != 1) break;
	    SuccessorIter successors(*cluster);
	    successor = successors.next();
	}
    }
    
    cleanupAfterCluster(g);
}

void HierScheduler :: setup () {

    invalid = FALSE;
    
    if (!galaxy()) {
	Error::abortRun("HierScheduler: no galaxy!");
	return;
    }

    if (!checkConnectivity()) return;

    // We count the number of stars in the unclustered system to
    // return the number of stars in the original sdf graph.
    sdfStars = totalNumberOfStars(*galaxy());

    galaxy()->initialize();

    if (SimControl::haltRequested()) return;

    if (!repetitions()) return;
    HierCluster temp;
    temp.initializeForClustering(*galaxy());

    StringList clusterdot, dot, textDescription;

    clusterdot << printClusterDot(*galaxy());
    // mtarget->writeFile(clusterdot,".cdot1");
    clusterdot.initialize();
    
    // clusterChains(*galaxy());
    
    clusterdot << printClusterDot(*galaxy());
    dot << printDot(*galaxy());
    textDescription << galaxy()->print(FALSE);

    mtarget->writeFile(dot,".dot");
    mtarget->writeFile(clusterdot,".cdot");
    mtarget->writeFile(textDescription,".desc");

    // We may want to promote this code into a Cluster method if
    // others going to need to convert their clusters into wormholes.
    // If so, we'll have to convert the ClusterIter into a
    // GalAllBlockIter - this code only converts the top-cluster blocks.
    ClusterIter clusters(*galaxy());
    Cluster* cluster;
    while ((cluster = clusters++) != NULL) {
	if (cluster->scheduler()) {
	    
	    // Replace the cluster with a new wormhole
	    clusters.remove();
	    ShadowTarget* shadow = new ShadowTarget;
	    Star& newWorm =
		Domain::named(cluster->domain())->newWorm(*cluster,shadow);
	    // Configure the new wormhole
	    newWorm.setTarget(mtarget);
	    if (SimControl::haltRequested()) return;
	    galaxy()->addBlock(newWorm,cluster->name());

	    // We could introduce a virtual function here so that
	    // other types of clusters can configure the new worm
	    // star
	    ((CGWormBase*)((CGStar&)newWorm).asWormhole())->execTime =
		((HierCluster*)cluster)->execTime;

	    // Configure the worm ports
	    ClusterPortIter nextPort(*cluster);
	    ClusterPort* clusterPort;
	    while ((clusterPort = nextPort++) != NULL) {
		DFPortHole& insidePort = (DFPortHole&)clusterPort->realPort();
		DFPortHole* outsidePort =
		    (DFPortHole*)insidePort.far()->asEH()->ghostAsPort();
		// We could introduce a virtual function here so that
		// other types of clusters can configure the new worm
		// ports

		// Compute the SDF parameters
		int numXfer = insidePort.numXfer();
		int maxBackValue = insidePort.maxDelay();
		int reps =
		    ((HierCluster*)clusterPort->alias()->parent())->
		    repetitions;
		outsidePort->setSDFParams
		    (numXfer*reps, numXfer*(reps-1)-maxBackValue);
	    }

	    // Set the scheduler/compute the schedule
	    shadow->setGalaxy(*cluster);
	    shadow->setScheduler(cluster->scheduler());
	    shadow->initialize();
	}

	
    }

    // Compute top-level schedule
    topScheduler.setGalaxy(*galaxy());
    topScheduler.setup();
    StringList schedule;
    schedule << displaySchedule();
    mtarget->writeFile(schedule,".sched");

    // Set all looping levels for child targets > 0.  Targets might
    // have to do different style buffering (ie. CGC)
    int childNum = 0;
    CGTarget* child;
    while ((child = mtarget->cgChild(childNum++)))
	child->loopingLevel = 3;
    
    mtarget->requestReset();
    Error::abortRun("Sorry, but the rest of the scheduler is not implemented");
    return;
}

void HierScheduler :: compileRun() {
    topScheduler.compileRun();
}

HierScheduler::~HierScheduler() {
    delete &topScheduler;
}


