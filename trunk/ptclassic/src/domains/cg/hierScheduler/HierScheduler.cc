static const char file_id[] = "HierScheduler.cc";

/*****************************************************************
Version identification:
@(#)HierScheduler.cc	1.19 8/8/96

Copyright (c) 1990-1996 The Regents of the University of California.
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
    schedule << "{\n  { scheduler \"Pino's Hierarchical Scheduler\" }\n"
	     << "  { numberOfStars " << sdfStars << " }\n"
	     << "  { numberOfStarOrClusterFirings " << dagNodes() << " }\n"
	     << "  { galaxy " << galaxy()->fullName() << " }\n"
	     << "  { cluster\n"
	     << topScheduler.displaySchedule()
	     << "  }\n";
    GalStarIter nextStar(wormholes);
    DataFlowStar* star;
    while ((star = (DataFlowStar*) nextStar++) != NULL)
	schedule << "  { cluster " << star->fullName() << " {\n"
	    << star->scheduler()->displaySchedule()
	    << "  } }\n";
    schedule << "}\n";
    return schedule;
}   

int HierScheduler::run() { return topScheduler.run(); }

ParProcessors* HierScheduler::setUpProcs(int num) {
     parProcs = topScheduler.setUpProcs(num);
     parProcs->moveStars = TRUE;
     return parProcs;
}

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

// This function is currently only at the file scope level - if others
// depend on it, we may want to promote it to be a cluster method.
// It is used to resolve the procId of a cluster - if there is an
// inconsistancy, Error::abortRun will be called.

inline int resolveProcId(Cluster& cluster) {
    GalStarIter nextStar(cluster);
    CGStar* star;
    int procId = -1;
    while ((star = (CGStar*) nextStar++) != NULL) {
	int starProcId = star->getProcId();
	if (starProcId != -1) {
	    if (procId == -1)	procId = starProcId;
	    else if (procId != starProcId) {
		StringList msg;
		msg << "The procId of the star, " << star->fullName()
		    << ", is " << starProcId << ".  This is "
		    " inconsistant with the resolved procId of the "
		    " cluster, " << cluster.fullName() 
		    << ", which is " << procId;
		Error::abortRun(msg);
		return procId;
	    }
	}
    }
    return procId;
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

    // clusterChains(*galaxy());

    // Now we output the clustered graph (with and w/o hierarchy
    // exposed).  We need to do this before we do the cluster->worm
    // conversion
    StringList dot;
    dot = printClusterDot(*galaxy());
    mtarget->writeFile(dot,".cdot");
    dot = printDot(*galaxy());
    mtarget->writeFile(dot,".dot");
    
    // We may want to promote this code into a Cluster method if
    // others going to need to convert their clusters into wormholes.
    // If so, we'll have to convert the ClusterIter into a
    // GalAllBlockIter - this code only converts the top-cluster blocks.
    ClusterIter clusters(*galaxy());
    Cluster* cluster;
    while ((cluster = clusters++) != NULL) {
	if (cluster->clusterScheduler()) {
	    // First, resolve the procId 
	    int procId = resolveProcId(*cluster);
	    // Make sure there wasn't a inconsistancy in resolving the procId
	    if (SimControl::haltRequested()) return;

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

	    // If the resolved procId has been resolved to something
	    // other than automatic mapping (-1, the default) then we
	    // set the procId of the wormhole
	    if (procId != -1) ((CGStar&)newWorm).setProcId(procId);

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
	    shadow->setScheduler(cluster->clusterScheduler());
	    shadow->initialize();
	}
    }

    // Compute top-level schedule
    topScheduler.setGalaxy(*galaxy());
    topScheduler.setup();

    // Now explode the wormholes in the subgalaxies
    // This code doesn't handle nested CGWormholes - FIXME
    // This code doesn't handle parallel CGWormholes - FIXME
    int i;
    for (i=0; i < parProcs->size() ; i++) {
	Galaxy* subGalaxy = parProcs->getProc(i)->myGalaxy();
	GalStarIter stars(*subGalaxy);
	Star* star;
	while ((star = stars++) != NULL) {
	    Wormhole* worm = ((CGStar*)star)->asWormhole();
	    if (worm) {
		stars.remove();
		// Sets all the internal stars and the scheduler to
		// the correct target
		worm->insideGalaxy().setTarget(star->target());
		subGalaxy->addBlock(*worm->explode(),star->name());
	        wormholes.addBlock(*star,star->name());
	    }
	}
    }

    // Set all looping levels for child targets > 0.  Targets might
    // have to do different style buffering (ie. CGC)
    int childNum = 0;
    CGTarget* child;
    while ((child = mtarget->cgChild(childNum++)))
	child->loopingLevel = "3";
}

void HierScheduler :: compileRun() {
    topScheduler.compileRun();
}

HierScheduler::~HierScheduler() {
    delete &topScheduler;
}


