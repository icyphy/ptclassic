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
#include <string.h>


DSBlockPortIter::DSBlockPortIter(Block& b,int (*test)(PortHole&))
:DSNamedObjIter(), testPort(test)
{
    BlockPortIter nextPort(b);
    PortHole *port;
    while ((port = nextPort++) != NULL)
	put(*port);
    initInternalIterator();
}

DSGalAllBlockIter::DSGalAllBlockIter(Galaxy&g,int(*test)(Block&))
:DSBlockIter(test)
{
    GalAllBlockIter nextBlock(g);
    Block *block;
    while ((block = nextBlock++) != NULL)
	put(*block);
    initInternalIterator();
}

DSGalTopBlockIter::DSGalTopBlockIter(Galaxy&g,int(*test)(Block&))
:DSBlockIter(test)
{
    GalTopBlockIter nextBlock(g);
    Block *block;
    while ((block = nextBlock++) != NULL)
	put(*block);
    initInternalIterator();
}

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

StringList printTopBlockDot(Galaxy& galaxy, const char* depth) {
    StringList dot;
    GalTopBlockIter nextBlock(galaxy);
    Block* block;
    while ((block = nextBlock++) != NULL) {
	InfString blockName = block->className();
	blockName << "_" << block->flags[0];
	replaceString(blockName,"=","_");
	replaceString(blockName,".","_");
	if (! block->isItAtomic()) {
	    StringList clusterDepth = depth;
	    clusterDepth << "  ";
	    dot << depth << "subgraph cluster_" << blockName << " {\n"
		<< printTopBlockDot(block->asGalaxy(),clusterDepth)
		<< depth << "}\n";
	}
	else {
	    BlockPortIter nextPort(*block);
	    PortHole *port;
	    while ((port = nextPort++) != NULL) {
		if (port->isItOutput()) {
		    InfString sinkName =
			port->far()->realPort().parent()->className();
		    sinkName <<"_"<<port->far()->realPort().parent()->flags[0];
		    replaceString(sinkName,"=","_");
		    replaceString(sinkName,".","_");    
		    dot << depth << blockName << " -> " << sinkName << ";\n";
		}
	    }
	    // Special case for peek-poke ports
// 	    if (!strncmp("Poke",blockName,4)) {
// 		InfString peekName = blockName;
// 		strncpy(peekName,"Peek",4);
// 		dot << depth << blockName << " -> " << peekName
// 		    << "[style = \"dotted\"];\n"; 
// 	    }
	}
    }
    return dot;
}

StringList printDot(Galaxy& galaxy) {
    numberAllBlocks(galaxy);
    StringList dot;
    dot << "digraph " << galaxy.name() << " {\n"
	<< "  node [shape=record,width=.1,height=.1];\n"
	<< printTopBlockDot(galaxy,"  ") << "}\n";
    return dot;
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

GraphMatrix::GraphMatrix(Galaxy& g, int flag):
IntMatrix(g.numberBlocks(),g.numberBlocks()){
    flagLoc = flag;
    graphBlock = NULL;
    *(IntMatrix*)this = 0;

    graphBlock = new (Block*)[g.numberBlocks()];

    numBlocks = 0;
    GalTopBlockIter nextBlock(g);
    Block* block;
    while ((block = nextBlock++) != NULL) {
	graphBlock[numBlocks] = block;
	block->flags[flagLoc] = numBlocks++;
    }
}

GraphMatrix::~GraphMatrix() {
    delete [] graphBlock;
}

int GraphMatrix::cluster(Block& replacement,int a, int b) {
    if ( ! (0 <= a < numBlocks && 0 <= b < numBlocks)) return FALSE;
    graphBlock[a] = &replacement;
    graphBlock[b] = NULL;
    return TRUE;
}

// int GraphMatrix::clusterByBlockRefs(Block& replacement, Block& a, Block& b) {
//     if (&a == &b) return TRUE;
//     if ( ! 0 <= a.flags[flagLoc] < numBlocks ||
// 	 ! 0 <= b.flags[flagLoc] < numBlocks)
// 	return FALSE;
//     return clusterByMatrixIndex(replacement,a.flags[flagLoc],b.flags[flagLoc]);
// }

StringList GraphMatrix::print() {
    StringList out;
    int i;
    out << "Index\tName\n";
    for (i = 0 ; i < numBlocks; i++)
	out << i << '\t'
	    << (graphBlock[i]?graphBlock[i]->name():"Ignore")
	    << '\n';
    out << '\n' << IntMatrix::print();
    return out;
}

TopologyMatrix::TopologyMatrix(Galaxy& g,int flagLocation):
GraphMatrix(g,flagLocation) {
    identity();
    GalTopBlockIter nextBlock(g);
    Block* block;
    while ((block = nextBlock++) != NULL) {
	int blockIndex = block->flags[flagLocation];
	BlockPortIter nextPort(*block);
	PortHole *port;
	while ((port = nextPort++) != NULL) {
	    // Assumes that all ports are connected and all ports
	    // have a parent
	    int sourceIndex, sinkIndex;
	    if (port->isItInput()) {
		sourceIndex = port->far()->parent()->flags[flagLocation];
		sinkIndex = blockIndex;
	    }
	    else {
		sourceIndex = blockIndex;
		sinkIndex = port->far()->parent()->flags[flagLocation];
	    }
	    (*this)[sourceIndex][sinkIndex] = TRUE;
	}
    }
}

int TopologyMatrix::cluster(Block& replacement, int a, int b) {
    int i;
    if (!GraphMatrix::cluster(replacement,a,b)) return FALSE;
    for (i=0 ; i < numBlocks ; i++) {
	(*this)[a][i] = (*this)[a][i] || (*this)[b][i];
	(*this)[i][a] = (*this)[i][a] || (*this)[i][b];
    }
    return TRUE;
}
