#ifndef _GraphUtils_h
#define _GraphUtils_h
#ifdef __GNUG__
#pragma interface
#endif


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

Programmer: Jose Luis Pino, Praveen Murthy

This file contains generic routines useful for traversing graphs.
These routines include several iterators like

SuccessorIter - to iterate over Blocks connected via output ports of a Block
PredecessorIter - """"" 				input """""
BlockOutputIter - to iterate over the output ports of a Block
BlockInputIter - """"		 input """"

Also, there are routines to number blocks (as 0,1,2,...), reset flags
in blocks at specified locations, to count the number of blocks in a galaxy,
to determine sources, sinks, whether the graph is acyclic,...
In addition, the function printDot converts a Galaxy to AT&T's "dotty"
format; more on this is given below.


Some terminology: by "marking" or "tagging" a Block (or PortHole), we
mean the assignment of an integer (usually non-zero) to flags[flag_loc]
in the corresponding Block or PortHole.
Note that flags (a FlagArray object) is a public member of NamedObj, and
Block and PortHole are both derived from NamedObj.  flag_loc is usually
specified as an argument to the function.
So most of the numbering and reseting functions operate on the flags member.

*****************************************************************/

#include "StringList.h"
#include "Galaxy.h"
#include "GalIter.h"


// Returns the total number of stars in a galaxy (recursively
// all galaxies which are contained in the galaxy specified by
// the user
int totalNumberOfStars(Galaxy& g);

// Determines whether the Galaxy is acyclic or not.  The ignoreTaggedArcs
// parameter works the following way: if ignoreTaggedArcs is true, then
// if certain arcs have been "marked"
// by setting the flag in the arc at flags[tagLoc] > 0, then
// those arcs are ignored in the isAcyclic computation. For example,
// one use might be to ignore arcs that have delays on them in SDF graphs.
int isAcyclic(Galaxy* g, int ignoreTaggedArcs=0, int tagLoc=0);

// Find the source nodes in the galaxy.  See the function definition
// for more comments.
void findSources(Galaxy* g, int flagLoc, SequentialList& sources, Block* deletedNode=0);

// Find the sink nodes in the galaxy.  See the function definition
// for more comments.
void findSinks(Galaxy* g, int flagLoc, SequentialList& sinks, Block* deletedNode=0);

// An iterator over the parents of a block
// This iterates "vertically"; ie, the parent of a Block, the parent's parent
// and so on.
class BlockParentIter {
public:
    BlockParentIter(Block& b):first(b) { reset(); }
    Block* next();
    inline Block* operator++(POSTFIX_OP) { return next();}
    inline void reset() { nextParent = first.parent();}
private:
    Block& first;
    Block* nextParent;
};

// This method returns a StringList that describes a Galaxy in
// dotty format.  To find out more about this format refer to the
// URL http://www.research.att.com/orgs/ssr/book/reuse.
StringList printDot(Galaxy&);

// This is a useful funtion to replace all instances of a string
// with another string within a string.  Note, the string to match
// and the string to insert must be the same size
void replaceString(char* /*master string*/,
		   const char* /*string to match*/,
		   const char* /*string to insert*/);

// Number all of the blocks (i.e, 0,1,2,...) inside the galaxy at the 
// specified flag
// location.  That is each Block in the galaxy is numbered at
// flags[flagLocation] starting from 0.
void numberAllBlocks(Galaxy&,int flagLocation=0);

// Number the top blocks inside of the galaxy at the specified flag
// location. This one only numbers the "top" blocks: it does not
// descend the hierarchy if the top blocks happen to be galaxies.
void numberBlocks(Galaxy&,int =0);

// reset the flag specified for all the blocks inside of the galaxy
void resetAllFlags(Galaxy& galaxy,int flagLocation=0, int resetValue=0);

// reset the flag specified only the top blocks inside of the galaxy
void resetFlags(Galaxy& galaxy,int flagLocation=0, int resetValue=0);

// The following iterates over the output ports of a Block.
// Hopefully this implementation will be improved by having input and
// output ports of a block stored in separate lists.  This could make
// 2x speedup in some of the clustering code.
class BlockOutputIter: private BlockPortIter {
public:
    inline BlockOutputIter(Block& b):BlockPortIter(b) {};
    inline PortHole* next() {
	PortHole *port;
	while ((port = PortListIter::next()) != NULL && !port->isItOutput());
	return port;
    }

    // Sometimes, we would like to obtain only "marked" ports. So the following
    // skips over ports that are not tagged at flag_loc by flag_value.
    // I.e, this next(.,.) function returns only the output ports that
    // have flags[flag_loc] == flag_value.
    inline PortHole* next(int flag_loc, int flag_value) {
	PortHole* port;
	while ((port = PortListIter::next())!= NULL &&
		(port->flags[flag_loc] != flag_value || port->isItInput()));
	return port;
    }

    inline PortHole* operator++(POSTFIX_OP) { return BlockOutputIter::next();}
    BlockPortIter::reset;
    BlockPortIter::remove;
};

// Iterates over the input ports of a Block.
class BlockInputIter: private BlockPortIter {
public:
    inline BlockInputIter(Block& b):BlockPortIter(b) {};
    inline PortHole* next() {
	PortHole *port;
	while ((port = PortListIter::next()) != NULL && !port->isItInput());
	return port;
    }

    // Skip over ports that are not tagged at flag_loc by flag_value.
    // I.e, this next(.,.) function returns only the input ports that
    // have flags[flag_loc] == flag_value.
    inline PortHole* next(int flag_loc, int flag_value) {
	PortHole* port;
	while ((port = PortListIter::next())!= NULL &&
		(port->flags[flag_loc] != flag_value || port->isItOutput()));
	return port;
    }

    inline PortHole* operator++(POSTFIX_OP) { return BlockInputIter::next();}
    BlockPortIter::reset;
    BlockPortIter::remove;
};

// Find the original port from a alias chain
// This could be a method of the class generic port
inline GenericPort& aliasedPort(GenericPort& port) {
    GenericPort* p = &port;
    while (p->aliasFrom()) p = p->aliasFrom();
    return *p;
}

// This iterates over the successor Blocks of a Block.  A successor
// Block is a Block that is connected via an output port.
class SuccessorIter {
public:
    inline SuccessorIter(Block& b) {
	successorPortIter = new BlockOutputIter(b);
    }
    inline ~SuccessorIter() { delete successorPortIter; }
    inline void reset() { successorPortIter->reset(); }

    inline Block* next() {
	PortHole* port;
	Block* parent = 0;
	while ((port=(*successorPortIter)++)!= NULL && (!port->far()
	       || (parent=port->far()->parent())==NULL));
	return port? parent: (Block*)NULL;	
    }

    // Sometimes. we want to only get the successors that have been "marked".
    // This next(.,.) function returns only the successor Blocks that have
    // flags[flag_loc] == flag_val
    inline Block* next(int flag_loc, int flag_val) {
	PortHole* port;
	Block* parent = 0;
	while ((port=(*successorPortIter)++)!= NULL && 
		( !port->far() || ((parent=port->far()->parent())==NULL)
		|| parent->flags[flag_loc] != flag_val));
	return port? parent: (Block*)NULL;	
    }

    inline Block* operator++(POSTFIX_OP) { return next(); }
private:
    BlockOutputIter *successorPortIter;
};

// This iterates over the predecessor Blocks of a Block.  A predecessor
// Block is a Block that is connected via an input port.
class PredecessorIter {
public:
    inline PredecessorIter(Block& b){
	predecessorPortIter = new BlockInputIter(b);
    }
    inline ~PredecessorIter() { delete predecessorPortIter; }
    inline void reset() { predecessorPortIter->reset(); }
    // Skips over incorrectly connected ports, should we?
    inline Block* next() {
	PortHole* port;
	Block* parent = 0;
	while ((port=(*predecessorPortIter)++)!= NULL && (!port->far()
	       || (parent=port->far()->parent())==NULL));
	return port? parent: (Block*)NULL;
    }

    // Sometimes. we want to only get the predecessors that have been "marked".
    // This next(.,.) function returns only the predecessor Blocks that have
    // flags[flag_loc] == flag_val
    inline Block* next(int flag_loc, int flag_val) {
	PortHole* port;
	Block* parent = 0;
	while ((port=(*predecessorPortIter)++)!= NULL && 
		(!port->far() || ((parent=port->far()->parent())==NULL)
		|| parent->flags[flag_loc] != flag_val));
	return port? parent: (Block*)NULL;
    }

    inline Block* operator++(POSTFIX_OP) { return next(); }
private:
    BlockInputIter *predecessorPortIter;
};

// Delete a galaxy and its ports - useful for clustering, the Galaxy
// destructor does not delete its ports
void deleteGalaxy(Galaxy&);

// Return the name for a block
StringList dotName(Block& b);
#endif

