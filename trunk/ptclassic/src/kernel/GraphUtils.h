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

#include "StringList.h"
#include "Galaxy.h"
#include "GalIter.h"

inline int defaultBlockTest(Block&) { return TRUE; }

// Delete-safe iterators
class DSGalAllBlockIter {
public:
    DSGalAllBlockIter(Galaxy&, int (*)(Block&) = defaultBlockTest);
    virtual ~DSGalAllBlockIter() { delete nextBlockThatMeetsTest; }
    inline Block* next() { return nextBlockThatMeetsTest->next(); }
    inline Block* operator++(POSTFIX_OP) { return next();}
    inline void reset() { nextBlockThatMeetsTest->reset(); }
private:
    int (*testBlock)(Block&);
    BlockList blocksThatMeetTest;
    BlockListIter *nextBlockThatMeetsTest;
};

class DSGalTopBlockIter {
    DSGalTopBlockIter(Galaxy&, int (*)(Block&) = defaultBlockTest);
    virtual ~DSGalTopBlockIter() {};
		      
inline int defaultGalaxyTest (Block& b) {return ! b.isItAtomic();}
class GalGalaxyIter : private DSGalAllBlockIter {
public:
    GalGalaxyIter(Galaxy& g, int (*test)(Block&) = defaultGalaxyTest):
    DSGalAllBlockIter(g,test) {};
    inline Galaxy* next() { return (Galaxy*)DSGalAllBlockIter::next(); }
    inline Galaxy* operator++(POSTFIX_OP) { return next();}
    DSGalAllBlockIter::reset;

    // need a public destructor because of private derivation
    virtual ~GalGalaxyIter() {}
};

// Returns the total number of stars in a galaxy (recursively
// all galaxies which are contained in the galaxy specified by
// the user
int totalNumberOfStars(Galaxy& g);

class BlockParentIter {
public:
    BlockParentIter(Block&);
    Block* next();
    inline Block* operator++(POSTFIX_OP) { return next();}
    inline void reset() { nextParent = first.parent();}
private:
    Block& first;
    Block* nextParent;
};

// These methods returns a StringList that describes a Galaxy in
// dotty format.  To find out more about this format refer to the
// URL http://www.research.att.com/orgs/ssr/book/reuse.
StringList printDot(Galaxy&);
StringList printTopBlockDot(Galaxy&, const char*);

// This is a useful funtion to replace all instances of a string
// with another string within a string.  Note, the string to match
// and the string to insert must be the same size
void replaceString(char* /*master string*/,
		   const char* /*string to match*/,
		   const char* /*string to insert*/);
#endif
