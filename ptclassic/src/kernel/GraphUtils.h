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

Programmer: Jose Luis Pino

*****************************************************************/

#include "StringList.h"
#include "Galaxy.h"
#include "GalIter.h"

// Returns the total number of stars in a galaxy (recursively
// all galaxies which are contained in the galaxy specified by
// the user
int totalNumberOfStars(Galaxy& g);

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

// Number all of the blocks inside of the galaxy at the specified flag
// location
void numberAllBlocks(Galaxy&,int =0);

// Number the top blocks inside of the galaxy at the specified flag
// location
void numberBlocks(Galaxy&,int =0);

// reset the flag specified for all the blocks inside of the galaxy
void resetAllFlags(Galaxy&,int =0);

// reset the flag specified only the top blocks inside of the galaxy
void resetFlags(Galaxy&,int =0);

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
    inline PortHole* operator++(POSTFIX_OP) { return BlockOutputIter::next();}
    inline BlockPortIter::reset;
    BlockPortIter::remove;
};

class BlockInputIter: private BlockPortIter {
public:
    inline BlockInputIter(Block& b):BlockPortIter(b) {};
    inline PortHole* next() {
	PortHole *port;
	while ((port = PortListIter::next()) != NULL && !port->isItInput());
	return port;
    }
    inline PortHole* operator++(POSTFIX_OP) { return BlockInputIter::next();}
    inline BlockPortIter::reset;
    BlockPortIter::remove;
};

// This could be a method of the class generic port
inline GenericPort& aliasedPort(GenericPort& port) {
    GenericPort* p = &port;
    while (p->aliasFrom()) p = p->aliasFrom();
    return *p;
}

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
    inline Block* operator++(POSTFIX_OP) { return next(); }
private:
    BlockOutputIter *successorPortIter;
};

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
    inline Block* operator++(POSTFIX_OP) { return next(); }
private:
    BlockInputIter *predecessorPortIter;
};

// Delete a galaxy and its ports - useful for clustering, the Galaxy
// destructor does not delete its ports
void deleteGalaxy(Galaxy&);

StringList dotName(Block& b);
#endif

