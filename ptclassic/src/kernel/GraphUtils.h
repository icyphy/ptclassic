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
#include "Matrix.h"


// Delete-safe iterators

// DSNamedObjIter is not meant to be used except as a base class
class DSNamedObjIter {
public:
    virtual ~DSNamedObjIter() { delete nextObjectThatMeetsTest; }
    inline NamedObj* next() { return nextObjectThatMeetsTest->next(); }
    inline NamedObj* operator++(POSTFIX_OP) { return next();}
    inline void reset() { nextObjectThatMeetsTest->reset(); }
    inline void put(NamedObj& o) { objectsThatMeetTest.put(o); }
protected:
    // This method must be called at the end of all derived class constructors
    inline void initInternalIterator() {
	nextObjectThatMeetsTest = new NamedObjListIter(objectsThatMeetTest);
    }
private:
    NamedObjList objectsThatMeetTest;
    NamedObjListIter *nextObjectThatMeetsTest;
};   

inline int defaultPortTest(PortHole&) { return TRUE; }
class DSBlockPortIter:private DSNamedObjIter {
public:
    DSBlockPortIter(Block&,int (*)(PortHole&) = defaultPortTest);
    virtual ~DSBlockPortIter() {};
    inline PortHole* next() { return (PortHole*)DSNamedObjIter::next(); }
    inline PortHole* operator++(POSTFIX_OP) { return next();}
    inline void put(PortHole& p) { if (testPort(p)) DSNamedObjIter::put(p); }
    DSNamedObjIter::reset;
protected:
    // This method must be called at the end of all derived class constructors
    DSNamedObjIter::initInternalIterator;
private:
    int (*testPort)(PortHole&);    
};   

inline int defaultBlockTest(Block&) { return TRUE; }
class DSBlockIter:private DSNamedObjIter {
public:
    DSBlockIter(int (*test)(Block&) = defaultBlockTest)
    :DSNamedObjIter(), testBlock(test) {};
    virtual ~DSBlockIter() {};
    inline Block* next() { return (Block*)DSNamedObjIter::next(); }
    inline Block* operator++(POSTFIX_OP) { return next();}
    inline void put(Block& b) { if (testBlock(b)) DSNamedObjIter::put(b); }
    DSNamedObjIter::reset;
protected:
    // This method must be called at the end of all derived class constructors
    DSNamedObjIter::initInternalIterator;
private:
    int (*testBlock)(Block&);    
};   

class DSGalAllBlockIter:public DSBlockIter {
public:
    DSGalAllBlockIter(Galaxy&, int (*)(Block&) = defaultBlockTest);
};

class DSGalTopBlockIter :public DSBlockIter {
public:
    DSGalTopBlockIter(Galaxy&, int (*)(Block&) = defaultBlockTest);
};
		      
inline int defaultGalaxyTest (Block& b) {return ! b.isItAtomic();}
class GalGalaxyIter : private DSGalAllBlockIter {
public:
    GalGalaxyIter(Galaxy& g, int (*test)(Block&) = defaultGalaxyTest):
    DSGalAllBlockIter(g,test) {};
    inline Galaxy* next() { return (Galaxy*)DSGalAllBlockIter::next(); }
    inline Galaxy* operator++(POSTFIX_OP) { return next();}
    DSGalAllBlockIter::reset;
    virtual ~GalGalaxyIter() {}
};

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

// This class respects galaxy boundaries
class GraphMatrix: public IntMatrix {
friend class GraphMatrixIter;
public:
    GraphMatrix(Galaxy& g, int flagLocation=0);
    ~GraphMatrix();

    virtual int cluster(Block& replacement,int,int);

    Block** graphBlock;

    /*virtual*/ StringList print();
protected:
    int flagLoc;
    int numBlocks;
};

class GraphMatrixIter {
public:
    inline GraphMatrixIter(GraphMatrix& m):myMatrix(m),current(0) {};
    inline int next() {
	while(current < myMatrix.numBlocks &&
	      myMatrix.graphBlock[current] == NULL)
	    current++;
    	if (current >= myMatrix.numBlocks) current = -1;
	return current++;
    }
    inline int operator++(POSTFIX_OP) { return next();}
    inline void reset() { current = 0;}
private:
    GraphMatrix& myMatrix;
    int current;
};

class TopologyMatrix: public GraphMatrix {
public:
    TopologyMatrix(Galaxy& g, int flagLocation=0);
    /*virtual*/ int cluster(Block& replacement,int,int);
};

#endif
