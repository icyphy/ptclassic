static const char file_id[] = "GalIter.cc";
/**************************************************************************
Version identification:
@(#)GalIter.cc	2.9	12/1/95

Copyright (c) 1990-1996 The Regents of the University of California.
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
#ifdef __GNUG__
#pragma implementation
#endif

#include "GalIter.h"
#include "ConstIters.h"

// This private class is a stack of GalTopBlockIter iterators.

class IterContext {
	friend GalAllBlockIter;

	GalTopBlockIter* iter;
	IterContext* link;
	IterContext(GalTopBlockIter* ii,IterContext* l)
		: iter(ii), link(l) {}
};

// Constructor.  Clear stack, create an iterator for this level.
GalAllBlockIter::GalAllBlockIter(Galaxy& g) {
	stack = 0;
	LOG_NEW; thisLevelIter = new GalTopBlockIter(g);
}

// The reset method pops up to the top level, then resets the top iterator
void GalAllBlockIter::reset() {
	while (stack) pop();
	thisLevelIter->reset();
}

// Destructor.
GalAllBlockIter::~GalAllBlockIter() {
	reset();
	LOG_DEL; delete thisLevelIter;
}

// push current iterator onto stack, enter a new galaxy g.
inline void GalAllBlockIter::push(Galaxy& g) {
	LOG_NEW; stack = new IterContext(thisLevelIter, stack);
	LOG_NEW; thisLevelIter = new GalTopBlockIter(g);
}

// pop an iterator off of the stack.
void GalAllBlockIter::pop() {
	if (stack == 0) return;
	IterContext* t = stack;
	LOG_DEL; delete thisLevelIter;
	thisLevelIter = t->iter;
	stack = t->link;
	LOG_DEL; delete t;
}

// This method returns the next block.
Block* GalAllBlockIter::next() {
	Block* b = thisLevelIter->next();
	while (!b) {
		// current level exhausted.  Try popping to proceed.
		// if stack is empty we are done.
		if (stack == 0) return 0;
		pop();
		b = thisLevelIter->next();
	}
	// have a block.  If it's a galaxy, we will need to process
	// it; this is pushed so we'll do it next time.
	if (!b->isItAtomic()) push (b->asGalaxy());
	return b;
}

void GalAllBlockIter::remove() {
    thisLevelIter->remove();
}

class CIterContext {
	friend CGalAllBlockIter;

	CGalTopBlockIter* iter;
	CIterContext* link;
	CIterContext(CGalTopBlockIter* ii,CIterContext* l)
		: iter(ii), link(l) {}
};

// Constructor.  Clear stack, create an iterator for this level.
CGalAllBlockIter::CGalAllBlockIter(const Galaxy& g) {
	stack = 0;
	LOG_NEW; thisLevelIter = new CGalTopBlockIter(g);
}

// The reset method pops up to the top level, then resets the top iterator
void CGalAllBlockIter::reset() {
	while (stack) pop();
	thisLevelIter->reset();
}

// Destructor.
CGalAllBlockIter::~CGalAllBlockIter() {
	reset();
	LOG_DEL; delete thisLevelIter;
}

// push current iterator onto stack, enter a new galaxy g.
inline void CGalAllBlockIter::push(const Galaxy& g) {
	LOG_NEW; stack = new CIterContext(thisLevelIter, stack);
	LOG_NEW; thisLevelIter = new CGalTopBlockIter(g);
}

// pop an iterator off of the stack.
void CGalAllBlockIter::pop() {
	if (stack == 0) return;
	CIterContext* t = stack;
	LOG_DEL; delete thisLevelIter;
	thisLevelIter = t->iter;
	stack = t->link;
	LOG_DEL; delete t;
}

// This method returns the next block.
const Block* CGalAllBlockIter::next() {
	const Block* b = thisLevelIter->next();
	while (!b) {
		// current level exhausted.  Try popping to proceed.
		// if stack is empty we are done.
		if (stack == 0) return 0;
		pop();
		b = thisLevelIter->next();
	}
	// have a block.  If it's a galaxy, we will need to process
	// it; this is pushed so we'll do it next time.
	if (!b->isItAtomic()) push (b->asGalaxy());
	return b;
}

GalStarIter::GalStarIter(Galaxy& g) : GalAllBlockIter(g) {}

// This method returns the next star.
Star* GalStarIter::next() {
	while (1) {
		Block* b = GalAllBlockIter::next();
		if (!b) return 0;
		if (b->isItAtomic()) return &(b->asStar());
	}
}

void GalStarIter::remove() {
    GalAllBlockIter::remove();
}

CGalStarIter::CGalStarIter(const Galaxy& g) : CGalAllBlockIter(g) {}

// This method returns the next star.
const Star* CGalStarIter::next() {
	while (1) {
		const Block* b = CGalAllBlockIter::next();
		if (!b) return 0;
		if (b->isItAtomic()) return &(b->asStar());
	}
}

Galaxy* GalGalaxyIter::next() {
    Block *block;
    while ((block = GalAllBlockIter::next()) != NULL && block->isItAtomic());
    return (Galaxy*)block;
}

void GalGalaxyIter::remove() {
    GalAllBlockIter::remove();
}
