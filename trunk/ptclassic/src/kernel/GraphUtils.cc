static const char file_id[] = "GraphUtils.cc";

/*****************************************************************
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

Programmer: Jose Luis Pino, Praveen K. Murthy

*****************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "GraphUtils.h"
#include "InfString.h"
#include "GalIter.h"
#include <string.h>

int findBackEdge(Block* node, int flagLoc, int ignoreTaggedArcs, int tagLoc);

// function to count stars in a galaxy
int totalNumberOfStars(Galaxy& g) {
    GalStarIter nextStar(g);
    int n = 0;
    while (nextStar++ != NULL) {n++;}
    return n;
}

// Returns TRUE if Galaxy is acyclic; FALSE else.
//
// Done by computing depth first search (dfs) on g.
//
// If ignoreTaggedArcs is 0, then every arc is taken into account.
// if ignoreTaggedArcs is > 0, then arcs that have been tagged (at
// location flags[tagLoc]) non-zero are ignored as 
// precedence arcs.
// 
// It uses flags[0] for all Blocks in the Galaxy for keeping
// track of nodes that have been visited and haven't been visited; any previous
// value in flags[0] will be lost.
//
// Return TRUE or FALSE
//
int isAcyclic(Galaxy* g, int ignoreTaggedArcs, int tagLoc)
{
    // This is done by DFS.
    // flagLoc is hard-wired to be 0 here; perhaps it can become
    // an argument to the function.
    int flagLoc = 0;
    resetFlags(*g,flagLoc,0);
    Block *node;
    GalStarIter nextBlock(*g);
    while ((node = nextBlock++) != NULL) {
	if (node->flags[flagLoc]) continue;
	if (findBackEdge(node,flagLoc,ignoreTaggedArcs,tagLoc)) return FALSE;
    }
    return TRUE;
}

// This method is used by isAcyclic above.  This method does the actual DFS.
int findBackEdge(Block* node, int flagLoc, int ignoreTaggedArcs, int tagLoc)
{
    Block *succ;
    int fl, result;
    PortHole *p;
    node->flags[flagLoc] = 1;
    SuccessorIter nextSucc(*node);
    BlockOutputIter nextO(*node);
    if (ignoreTaggedArcs) {

   	// now we only want to explore those edges
	// that are tagged zero (i.e, ignore arcs tagged non-zero)
	// at flags[tagLoc]

	while ( (p = nextO.next(tagLoc,0)) != NULL) {
	    if (!p->far() || !p->far()->parent()) continue;
	    succ = p->far()->parent();
	    fl = succ->flags[flagLoc];
	    if (fl == 2) continue;
	    if (fl == 1) {
	    	result = TRUE;
	    } else {
	    	result = findBackEdge(succ,flagLoc,ignoreTaggedArcs,tagLoc);
	    }
	    if (result) return TRUE;
	}
    } else {
	while ((succ = nextSucc++) != NULL) {
	    fl = succ->flags[flagLoc];
	    if (fl == 2) continue;
	    if (fl == 1) {
		result = TRUE;
	    } else {
		result = findBackEdge(succ,flagLoc,ignoreTaggedArcs,tagLoc);
	    }
	    if (result) return TRUE;
	}
    }
    node->flags[flagLoc] = 2;
    return FALSE;
}

// Find the source nodes in the Galaxy
// 
// A source node here is any node that has all of its input
// nodes marked.  This method is used in order to simulate
// the removal of nodes from the graph; a node is "removed"
// by marking it at flags[flagLoc] (by the function that calls
// this function).  If no node is marked, then this function will
// return the sources in the graph.  If nodes are marked, then
// the list returned by the function represents the sources
// in the graph that would result from removing the marked nodes
// and all their edges from the graph.
//
// If deletedNode is specified, then the algorithm will assume that
// the only node deleted from the graph (from the last time this function
// was called ) was deletedNode and explore
// only the edges going out of deletedNode.  This will make
// this computation much more efficient when it is used in
// conjunction with functions like isWellOrdered
// because isWellOrdered deletes one node at a time
// and calls findSources
// after each deletion.  IN THIS CASE NOTE THAT THE FUNCTION WILL ONLY
// RETURN THOSE SOURCES FORMED BY THE DELETION OF 
// deletedNode AND NOT ALL SOURCES IN THE GRAPH.
//
// To generalize this function, we could give a
// list of deleted nodes (instead of just one) and only explore the edges
// out of those from the list. 
//
// Worst case running time: O(E) (when deletedNode is specified)
// 
// SideEffects:
// The input list sources is filled with the source nodes
// 
void findSources(Galaxy* g, int flagLoc, SequentialList& sources, Block* deletedNode)
{
    Block *pred, *node;
    int notSource;
    if (deletedNode) {
	SuccessorIter nextNeigh(*deletedNode);
	while ((node = nextNeigh++) != NULL) {
	    notSource = 0;
	    PredecessorIter nextPred(*node);
	    while ((pred=nextPred++) != NULL) {
		if (!pred->flags[flagLoc]) {notSource = 1; break;}
	    }
	    if (!notSource) sources.append(node);
	}
    } else {
	GalTopBlockIter nextBlock(*g);
	while ((node = nextBlock++) != NULL) {
	    if (node->flags[flagLoc]) continue;
	    notSource = 0;
	    PredecessorIter nextPred(*node);
	    while ((pred=nextPred++) != NULL) {
		if (!pred->flags[flagLoc]) {notSource = 1; break; }
	    }
	    if (!notSource) sources.append(node);
	}
    }
}

// Find the sink nodes in the Galaxy; symmetric to findSources
void findSinks(Galaxy* g, int flagLoc, SequentialList& sinks,
	       Block* deletedNode)
{
    // symmetric to findSources; see comments therein
    Block *succ, *node;
    int notSink;
    if (deletedNode) {
	PredecessorIter nextNeigh(*deletedNode);
	while ((node = nextNeigh++) != NULL) {
	    notSink = 0;
	    SuccessorIter nextSucc(*node);
	    while ((succ=nextSucc++) != NULL) {
		if (!succ->flags[flagLoc]) {notSink = 1; break;}
	    }
	    if (!notSink) sinks.append(node);
	}
    } else {
	GalTopBlockIter nextBlock(*g);
	while ((node=nextBlock++) != NULL) {
	    if(node->flags[flagLoc]) continue;
	    notSink = 0;
	    SuccessorIter nextSucc(*node);
	    while ((succ=nextSucc++) != NULL) {
		if (!succ->flags[flagLoc]) {notSink = 1; break;}
	    }
	    if (!notSink) sinks.append(node);
	}
    }
}

Block* BlockParentIter::next() {
    Block* current = nextParent;
    nextParent = nextParent? nextParent->parent() : (Block*) NULL;
    return current;
}

// Used by printDot(Galaxy&)
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

// This method returns a StringList that describes a Galaxy in
// dotty format.  To find out more about this format refer to the
// URL http://www.research.att.com/orgs/ssr/book/reuse.
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

StringList dotName(Block& b){
    InfString name;
    name << b.name() << "_" << b.flags[0];
    replaceString(name,"=","_");
    replaceString(name,".","_");
    replaceString(name,"-","_");
    StringList out = name;
    return out;
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
    while ((block = nextBlock++) != NULL) {
	block->flags[flagLocation] = number++;
    }
}

void numberBlocks(Galaxy& galaxy, int flagLocation) {
    GalTopBlockIter nextBlock(galaxy);
    Block* block;
    int number=0;
    while ((block = nextBlock++) != NULL) {
	block->flags[flagLocation] = number++;
    }
}

void resetAllFlags(Galaxy& galaxy,int flagLocation, int resetValue) {
    GalAllBlockIter nextBlock(galaxy);
    Block* block;
    while ((block = nextBlock++) != NULL)
	block->flags[flagLocation] = resetValue;
}

void resetFlags(Galaxy& galaxy,int flagLocation, int resetValue) {
    GalTopBlockIter nextBlock(galaxy);
    Block* block;
    while ((block = nextBlock++) != NULL)
	block->flags[flagLocation] = resetValue;
}

void deleteGalaxy(Galaxy&g) {
    BlockPortIter nextPort(g);
    PortHole* port;
    while ((port = nextPort++) != NULL)
	delete port;
    delete &g;
}



