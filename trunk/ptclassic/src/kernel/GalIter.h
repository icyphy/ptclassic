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
#ifndef _GalIter_h
#define _GalIter_h 1

#ifdef __GNUG__
#pragma once
#pragma interface
#endif
#include "Galaxy.h"
#include "Star.h"

class IterContext;

class GalAllBlockIter {
public:
	GalAllBlockIter(Galaxy& g);
	~GalAllBlockIter();
	Block* next();
	Block* operator++() { return next();}
	void reset();
protected:
	GalTopBlockIter *thisLevelIter;
	IterContext *stack;
	void push(Galaxy&);
	void pop();
};

// GalStarIter essentially uses a GalAllBlockIter and skips the
// internal galaxies, returning only stars.

class GalStarIter : private GalAllBlockIter {
public:
	GalStarIter(Galaxy& g);
	Star* next();
	Star* operator++() { return next();}
	void reset() {	GalAllBlockIter::reset();}

	// need a public destructor because of private derivation
	~GalStarIter() {}
};

#endif

