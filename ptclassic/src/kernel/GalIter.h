/**************************************************************************
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

 Programmer:  J. T. Buck
 Date of creation: 11/28/90

"Recursive" iterators for Galaxy: return all Blocks, or all Stars,
at every level.

**************************************************************************/
#ifndef _GalIter_h
#define _GalIter_h 1

#ifdef __GNUG__
#pragma interface
#endif
#include "Galaxy.h"
#include "Star.h"

class IterContext;
class CGalTopBlockIter;
class CIterContext;

class GalAllBlockIter {
public:
	inline GalAllBlockIter(Galaxy& g);
	inline ~GalAllBlockIter();
	inline Block* next();
	inline Block* operator++(POSTFIX_OP) { return next();}
	inline void reset();
        // remove the currently pointed to Block and update the
        // internal iterators
        inline void remove();
protected:
	GalTopBlockIter *thisLevelIter;
	IterContext *stack;
	inline void push(Galaxy&);
	inline void pop();
};

class CGalAllBlockIter {
public:
	inline CGalAllBlockIter(const Galaxy& g);
	inline ~CGalAllBlockIter();
	inline const Block* next();
	inline const Block* operator++(POSTFIX_OP) { return next();}
	inline void reset();
protected:
	CGalTopBlockIter *thisLevelIter;
	CIterContext *stack;
	inline void push(const Galaxy&);
	inline void pop();
};

// GalStarIter essentially uses a GalAllBlockIter and skips the
// internal galaxies, returning only stars.

class GalStarIter : private GalAllBlockIter {
public:
	inline GalStarIter(Galaxy& g);
	inline Star* next();
	inline Star* operator++(POSTFIX_OP) { return next();}
	inline void reset() {	GalAllBlockIter::reset();}

        // remove the currently pointed to Block and update the
        // internal iterators
        inline void remove();
	// need a public destructor because of private derivation
	inline ~GalStarIter() {}
};


class CGalStarIter : private CGalAllBlockIter {
public:
	inline CGalStarIter(const Galaxy& g);
	inline const Star* next();
	inline const Star* operator++(POSTFIX_OP) { return next();}
	inline void reset() {	CGalAllBlockIter::reset();}

	// need a public destructor because of private derivation
	inline ~CGalStarIter() {}
};

class GalGalaxyIter : private GalAllBlockIter {
public:
    inline GalGalaxyIter(Galaxy& g):GalAllBlockIter(g) {};
    inline Galaxy* next();
    inline Galaxy* operator++(POSTFIX_OP) { return next();}
    GalAllBlockIter::reset;
    virtual ~GalGalaxyIter() {};
    inline void remove();
};

#endif

