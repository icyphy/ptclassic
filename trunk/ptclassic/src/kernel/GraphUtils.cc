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

BlockParentIter::BlockParentIter(Block&b) : first(b) {
    reset();
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
	InfString blockName = block->name();
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
		if (port->isItInput()) {
		    InfString sinkName =
			port->far()->realPort().parent()->name();
		    replaceString(sinkName,"=","_");
		    replaceString(sinkName,".","_");    
		    dot << depth << blockName << " -> " << sinkName << ";\n";
		}
	    }
	}
    }
    return dot;
}

StringList printDot(Galaxy& galaxy) {
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
