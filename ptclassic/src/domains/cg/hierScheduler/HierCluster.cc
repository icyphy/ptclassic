static const char file_id[] = "HierCluster.cc";
/******************************************************************
Version identification:
 $Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

// For the constructors, the HierCluster merge/absorb virtual methods
// are not user - so we must set the repetitions count
HierCluster::HierCluster(Star& star):Cluster(s) {
    DataFlowStar& star = (DataFlowStar&)s;
    repetitions = star.reps();
    star.repetitions = 1;
}

HierCluster::HierCluster(Cluster& cluster):Cluster(cluster) {
    repetitions = ((HierCluster&)cluster).repetitions;
}

HierCluster::HierCluster(Galaxy& galaxy):Cluster(galaxy) {
    repetitions = 0;
    GalStarIter nextStar(galaxy);
    DataFlowStar *star;
    while ((star = (DataFlowStar*)nextStar++) != NULL)
	repetitions = gcd(repetitions,star->reps());
    nextStar.reset();
    while ((star = (DataFlowStar*)nextStar++) != NULL)
	star->repetitions /= repetitions;
}

int HierCluster::absorb(Block& block) {
    adjustRepetitions(block);
    return Cluster::absorb(block);
}

int HierCluster::merge(Cluster& c) {
    adjustRepetitions(c);
    return Cluster::merge(c);
}

int HierCluster::flatten() {
    adjustRepetitions(1);
    return Cluster::flatten();
}

void HierCluster::adjustRepetitions(Block& blockToMergeOrAbsorb) {
    // Compute the repetitions for the cluster
    int newRepetitions=0;
    if (blockToMergeOrAbsorb.isItAtomic()) {
	DataFlowStar& star = (DataFlowStar&) blockToMergeOrAbsorb;
	newRepetitions = gcd(repetitions,star.reps());
	if (newRepetitions > 1)
	    star.repetitions /= newRepetitions;
    }
    else {
	HierCluster& clusterToMergeOrAbsorb =
	    (HierCluster&)blockToMergeOrAbsorb;
	newRepetitions = gcd(repetitions,clusterToMergeOrAbsorb.repetitions);
	clusterToMergeOrAbsorb.adjustRepetitions
	    (clusterToMergeOrAbsorb.repetitions/newRepetitions);
    }
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
    GalTopBlockIter nextBlock(*this);
    Block* block;
    while ((block = nextBlock++) != NULL) {
	if (block->isItAtomic) {
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

inline int reps(Block&b) {
    if (b.isItAtomic())
	return ((DataFlowStar&)b).reps();
    else
	return ((HierCluster&)b).repetitions;
}

enum { VISITED=0, DECENDENTS = 1};

HierCluster* findWellOrderCluster(Galaxy& g) {
    GalTopBlockIter nextBlock(g);
    Block* block;
    while ((block = nextBlock++) != NULL) {
	// check if the 'block' is in the well-ordered chain:
	// first -> block -> farBlock -> last and if so
	// we return cluster the 'block' and 'anotherFarBlock

	// If we have already tested this block, continue
	if (block->flags[VISITED]) continue;
	block->flags[VISITED] = TRUE;

	BlockPortIter* myNextPort(*block);
	PortHole* port;
	Block *first(NULL), *farBlock(NULL), *last(NULL);
	int continueFlag(FALSE);

	// FIXME - We should take into account delays and ignore
	// ports which are connected to geodesics with enough delay
	while ((port = myNextPort++) != NULL) {
	    Block* anotherFarBlock = port->far()->parent();
	    if (port->isItInput()) {
		if (anotherFarBlock == block) continue;
		if (first && anotherFarBlock!=first) break;
		first = anotherFarBlock;
	    }
	    else {
		if (anotherFarBlock == block) continue;
		if (farBlock && anotherFarBlockfarBlock!=farBlock) break;
		if (reps(farBlock) != reps(anotherFarBlockfarBlock)) break;
		farBlock = anotherFarBlock;
	    }
	}

	// If port is not null we have broken out of the while loop
	// prematurely which means this block is not on a well-ordered
	// chain
	if (port) continue;

	if (farBlock) {
	    // Now check the far block decendents
	    BlockPortIter nextFarBlockPort(*farBlock);
	    while ((port = nextFarBlockPort++) != NULL) {
	    }
	}
    }
    return (HierCluster*)NULL;
}

int wellOrderCluster(Galaxy& g) {
    // We do this so that we only visit each node once
    resetFlags(g);
}
