static const char file_id[] = "GraphUtils.cc";

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

#include "GraphUtils.h"
#include "InfString.h"
#include "GalIter.h"
#include "Cluster.h"
#include <string.h>

// function to count stars in a galaxy
int totalNumberOfStars(Galaxy& g) {
    GalStarIter nextStar(g);
    int n = 0;
    while (nextStar++ != NULL) n++;
    return n;
}

Block* BlockParentIter::next() {
    Block* current = nextParent;
    nextParent = nextParent? nextParent->parent() : (Block*) NULL;
    return current;
}

inline InfString dotName(Block& b) {
    InfString name;
    name << b.name() << "_" << b.flags[0];
    replaceString(name,"=","_");
    replaceString(name,".","_");
    return name;
}

StringList printTopBlockDot(Galaxy& galaxy, const char* depth) {
    StringList dot;
    GalTopBlockIter nextBlock(galaxy);
    Block* block;
    while ((block = nextBlock++) != NULL) {
	InfString blockName;
	blockName << dotName(*block);
	if (! block->isItAtomic()) {
	    StringList clusterDepth = depth;
	    clusterDepth << "  ";
	    dot << depth << "subgraph cluster_" << blockName << " {\n"
		<< printTopBlockDot(block->asGalaxy(),clusterDepth)
		<< depth << "}\n";
	}
	else
	    dot << depth << blockName << ";\n";
    }
    return dot;
}

StringList printDot(Galaxy& galaxy) {
    numberAllBlocks(galaxy);
    StringList dot;
    dot << "digraph " << galaxy.name() << " {\n"
	<< "  node [shape=record,width=.1,height=.1];\n"
	<< printTopBlockDot(galaxy,"  ");
    GalStarIter nextStar(galaxy);
    Star* source;
    while ((source = nextStar++) != NULL) {
	InfString sourceName;
	sourceName << dotName(*source);
	SuccessorIter nextSuccessor(*source);
	Block* successor;
	while ((successor = nextSuccessor++) != NULL)
	    dot << "  " << sourceName << " -> " << dotName(*successor) << ";\n";
    }
    dot << "}\n";
    return dot;
}

StringList printClusterDot(Galaxy& galaxy) {
    numberAllBlocks(galaxy);
    StringList dot;
    dot << "digraph " << galaxy.name() << " {\n"
	<< "  node [shape=record,width=.1,height=.1];\n";
    ClusterIter nextCluster(galaxy);
    Cluster* cluster;
    while ((cluster = nextCluster++) != NULL) {	
	InfString sourceName;
	sourceName << dotName(*cluster);
	if (!cluster->numberBlocks())
	    dot << "  " << sourceName << ";\n";
 	SuccessorIter nextSuccessor(*cluster);
	Block* successor;
	while ((successor = nextSuccessor++) != NULL)
	    dot << "  " << sourceName << " -> " << dotName(*successor) <<";\n";
    }
    dot << "}\n";
    return dot;
}
	
void cleanupAfterCluster(Galaxy& g) {
    GalTopBlockIter nextBlock(g);
    Block* block;
	
    while ((block = nextBlock++) != NULL) {
	if (! block->isItAtomic() &&
	    ! block->asGalaxy().numberBlocks()) {
	    delete block;
	    nextBlock.remove();
	}
	else if (block->parent() != &g)
	    nextBlock.remove();
    }
}

void replaceString(char* master, const char* match, const char* insert) {
    int length = strlen(match);
    if (!length || strlen(match) != strlen(insert)) return;
    while ((master = strstr(master,match)) != NULL) {
	int i;
	for (i=0; i<length; i++) *master++ = insert[i];
    }
}

void numberAllBlocks(Galaxy& galaxy, int flagLocation) {
    GalAllBlockIter nextBlock(galaxy);
    Block* block;
    int number=0;
    while ((block = nextBlock++) != NULL)
	block->flags[flagLocation] = number++;
}

void numberBlocks(Galaxy& galaxy, int flagLocation) {
    GalTopBlockIter nextBlock(galaxy);
    Block* block;
    int number=0;
    while ((block = nextBlock++) != NULL)
	block->flags[flagLocation] = number++;
}

void resetAllFlags(Galaxy& galaxy,int flagLocation) {
    GalAllBlockIter nextBlock(galaxy);
    Block* block;
    while ((block = nextBlock++) != NULL)
	block->flags[flagLocation] = 0;
}

void resetFlags(Galaxy& galaxy,int flagLocation) {
    GalTopBlockIter nextBlock(galaxy);
    Block* block;
    while ((block = nextBlock++) != NULL)
	block->flags[flagLocation] = 0;
}

void deleteGalaxy(Galaxy&g) {
    BlockPortIter nextPort(g);
    PortHole* port;
    while ((port = nextPort++) != NULL)
	delete port;
    delete &g;
}
