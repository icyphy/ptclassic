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
#include "SDFScheduler.h"
#include "LoopScheduler.h"
#include "SDFCluster.h"
#include "GalIter.h"
#include "GraphUtils.h"
#include "HierCluster.h"

/*virtual*/double HierScheduler::getStopTime() {
    return TRUE; // topScheduler.getStopTime();
}	

/*virtual*/ void HierScheduler::setStopTime(double /*limit*/) {
    // topScheduler.setStopTime(limit);
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

/*virtual*/ int HierScheduler::run() { return TRUE /*topCluster->run()*/; }

int HierScheduler::dagNodes() const {
    return TRUE; // topScheduler.dagNodes();
}

int flattenGalaxy(Galaxy& g) {
    return g.stateWithName("Scheduler")?FALSE:TRUE;
}

Cluster* newUserSpecifiedCluster(Galaxy& g) {
    // FIXME - shouldn't this also fix the cluster/internal stars
    // repetitions number?
    const char* schedType = g.stateWithName("Scheduler")->initValue();
    Scheduler* scheduler;
    if (strcmp(schedType,"Cluster") == 0)
	scheduler = new SDFClustSched("");
    else if (strcmp(schedType,"Loop") == 0)
	scheduler = new LoopScheduler("");
    else
	scheduler = new SDFScheduler;
    Cluster* cluster = new HierCluster(g);
    cluster->setScheduler(scheduler);
    return cluster;
}

#define VISITED flags[0]
#define BLOCK_NUMBER flags[1]
#define ANCESTORS flags[2]
#define DECENDENTS flags[3]

// The complexity is shown within the O() function.
// Let:
//	V = number of top blocks in the galaxy
//	E = number of arcs in the galaxy

void clusterChains(Galaxy& g) {

    // initialize flags for finding chain clusters - O(V)
    DSGalTopBlockIter nextBlock(g);
    Block *block;
    int blockNum = 0;
    while ((block = nextBlock++) != NULL) {
	block->VISITED = block->BLOCK_NUMBER = blockNum++;
	block->ANCESTORS = block->DECENDENTS = 0;
    }
    
    // compute total number of ancestors and decendents - O(E)
    nextBlock.reset();
    while ((block = nextBlock++) != NULL) {
	BlockOutputIter nextPort(*block);
	PortHole* port;
	while ((port = nextPort++) != NULL) {
	    Block*  farBlock = port->far()->parent();
	    if (farBlock->VISITED == block->BLOCK_NUMBER) continue;
	    farBlock->VISITED = block->BLOCK_NUMBER;
	    farBlock->ANCESTORS++;
	    block->DECENDENTS++;
	}
    }

    nextBlock.reset();
    while ((block = nextBlock++) != NULL) {
	if (block->ANCESTORS==1 && block->DECENDENTS==1) {
	    BlockOutputIter nextPort(*block);
	    Block* decendent = (nextPort.next())->parent();
	    if (decendent->parent()->parent())
		((HierCluster*)decendent->parent())->merge(*block);
	    else
		(new HierCluster(*block))->merge(*decendent);
	}
    }

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

    if (!repetitions()) return;
    initializeForClustering(*galaxy(),flattenGalaxy,newUserSpecifiedCluster);

    TopologyMatrix topology(*galaxy());

    // Well-order clustering - FIXME, we should be ignoring arcs with
    // sufficient delay!
    GraphMatrixIter nextSink(topology);
    GraphMatrixIter nextSource(topology);
    int sink;

    while ((sink = nextSink++) != -1) {
	int possibleSource, source(-1);
	nextSource.reset();
	while ((possibleSource = nextSource++) != -1) {
	    if (possibleSource == sink) continue;
	    if (topology[possibleSource][sink]) {
		if (source != -1) {
		    source = -1;
		    break;
		}
		else 
		    source = possibleSource;
	    }
	}
	if (source != -1) {
	    Block *src = topology.graphBlock[source];
	    Block *snk = topology.graphBlock[sink];
	    HierCluster* cluster=NULL;
	    if (! snk->isItAtomic()) {
		cluster = (HierCluster*)snk;
		cluster->absorb(*src);
	    }
	    else {
		cluster = src->isItAtomic() ?
		    new HierCluster(src->asStar()):(HierCluster*)src;
		cluster->absorb(*snk);
	    }
	    topology.cluster(*cluster,source,sink);
	    nextSink.reset();
	    sink = nextSink++;
	}
    }
	    
    StringList dot, textDescription, topo;

    topo << topology.print();
    dot << printDot(*galaxy());
    textDescription << galaxy()->print(FALSE);

    mtarget->writeFile(dot,".dot");
    mtarget->writeFile(textDescription,".desc");
    mtarget->writeFile(topo,".topo");

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

int HierScheduler::computeSchedule(Galaxy& /*g*/) {
    return FALSE ; //topCluster->generateSchedule();
}

void HierScheduler :: compileRun() {
    // topScheduler.compileRun();
}

void HierScheduler :: prepareCodeGen() {
    // prepare code generation for each processing element:
    // galaxy initialize, copy schedule, and simulate the schedule.
    // ParScheduler::prepareCodeGen();
}

HierScheduler::~HierScheduler() {
}


