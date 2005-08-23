static const char file_id[] = "HierCluster.cc";
/******************************************************************
Version identification:
 @(#)HierCluster.cc	1.6	8/2/96

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
 Date of creation: 11/13/95


*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "HierCluster.h"
#include "Fraction.h"
#include "DataFlowStar.h"
#include "SDFScheduler.h"
#include "LoopScheduler.h"
#include "SDFCluster.h"

int HierCluster::absorb(Cluster& c, int removeFlag) {
    adjustRepetitions((HierCluster&)c);
    execTime += ((HierCluster&)c).execTime;
    return Cluster::absorb(c, removeFlag);
}

int HierCluster::merge(Cluster& c, int removeFlag) {
    adjustRepetitions((HierCluster&)c);
    execTime += ((HierCluster&)c).execTime;
    return Cluster::merge(c, removeFlag);
}

int HierCluster::flatten(Galaxy* into, int removeFlag) {
    adjustRepetitions(1);
    return Cluster::flatten(into,removeFlag);
}

int HierCluster::flattenTest(Galaxy& g) {
    return g.stateWithName("Scheduler")?FALSE:TRUE;
}   

Cluster* HierCluster::convertGalaxy(Galaxy& g) {
    const char* schedType = g.stateWithName("Scheduler")->initValue();
    HierCluster* cluster = (HierCluster*)Cluster::convertGalaxy(g);
    Scheduler* scheduler;
    if (strcmp(schedType,"Cluster") == 0)
	scheduler = new SDFClustSched("");
    else if (strcmp(schedType,"Loop") == 0)
	scheduler = new LoopScheduler("");
    else
	scheduler = new SDFScheduler;
    cluster->setClusterScheduler(scheduler);
    
    ClusterIter next(*cluster);
    HierCluster *c;
    while ((c = (HierCluster*)next++) != NULL)
	cluster->repetitions = gcd(c->repetitions,cluster->repetitions);

    next.reset();
    while ((c = (HierCluster*)next++) != NULL) {
	c->repetitions /= cluster->repetitions;
	cluster->execTime += c->repetitions * c->execTime;
    }
    
    return cluster;
}

Cluster* HierCluster::convertStar(Star& s) {
    HierCluster* cluster = (HierCluster*)Cluster::convertStar(s);
    DataFlowStar &star = (DataFlowStar&)s;
    cluster->repetitions = star.repetitions;
    cluster->execTime = star.myExecTime();
    star.repetitions = 1;
    return cluster;
}

void HierCluster::adjustRepetitions(HierCluster& clusterToMergeOrAbsorb) {
    // Compute the repetitions for the cluster
    int newRepetitions=0;
    newRepetitions = gcd(repetitions,clusterToMergeOrAbsorb.repetitions);
    clusterToMergeOrAbsorb.adjustRepetitions
	(clusterToMergeOrAbsorb.repetitions/newRepetitions);
    adjustRepetitions(newRepetitions);
}
	
int HierCluster::adjustRepetitions(int newRepetitionCount) {
    if (newRepetitionCount == repetitions) return TRUE;
    if (newRepetitionCount > repetitions) {
	repetitions = newRepetitionCount;
	return TRUE;
    }
    
    if (! repetitions % newRepetitionCount) {
	StringList message;
	message << "Can't reduce the repetitions count of "
		<< this->name() << " from " << repetitions
		<< " to " << newRepetitionCount
		<< " because it is a non-integral rate change."; 
	Error::abortRun(*this,message);
	return FALSE;
    }

    int scaleFactor = repetitions / newRepetitionCount;
    execTime *= scaleFactor;
    GalTopBlockIter nextBlock(*this);
    Block* block;
    while ((block = nextBlock++) != NULL) {
	if (block->isItAtomic()) {
	    DataFlowStar& star = *(DataFlowStar*)block;
	    star.repetitions = star.reps() * scaleFactor;
	}
	else {
	    HierCluster& cluster = *(HierCluster*)block;
	    cluster.repetitions = (int) cluster.repetitions * scaleFactor;
	}
    }

    repetitions = newRepetitionCount;
    return TRUE;
}




