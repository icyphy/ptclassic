/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 11/28/90

"Recursive" iterators for Galaxy: return all Blocks, or all Stars,
at every level.

**************************************************************************/
#include "GalIter.h"

// This private class is a stack of GalTopBlockIter iterators.

class IterContext {
	friend GalAllBlockIter;

	GalTopBlockIter* iter;
	IterContext* link;
	IterContext(GalTopBlockIter* ii,IterContext* l)
		: iter(ii), link(l) {}
};

// Constructor.  Clear stack, create an iterator for this level.
GalAllBlockIter::GalAllBlockIter(const Galaxy& g) {
	stack = 0;
	thisLevelIter = new GalTopBlockIter(g);
}

GalStarIter::GalStarIter(const Galaxy& g) : GalAllBlockIter(g) {}

// The reset method pops up to the top level, then resets the top iterator
void GalAllBlockIter::reset() {
	while (stack) pop();
	thisLevelIter->reset();
}

// Destructor.
GalAllBlockIter::~GalAllBlockIter() {
	reset();
	delete thisLevelIter;
}

// push current iterator onto stack, enter a new galaxy g.
inline void GalAllBlockIter::push(const Galaxy& g) {
	stack = new IterContext(thisLevelIter, stack);
	thisLevelIter = new GalTopBlockIter(g);
}

// pop an iterator off of the stack.
void GalAllBlockIter::pop() {
	if (stack == 0) return;
	IterContext* t = stack;
	delete thisLevelIter;
	thisLevelIter = t->iter;
	stack = t->link;
	delete t;
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

// This method returns the next star.
Star* GalStarIter::next() {
	while (1) {
		Block* b = GalAllBlockIter::next();
		if (!b) return 0;
		if (b->isItAtomic()) return &(b->asStar());
	}
}
