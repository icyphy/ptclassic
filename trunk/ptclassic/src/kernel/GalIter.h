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
	GalAllBlockIter(const Galaxy& g);
	~GalAllBlockIter();
	Block* next();
	Block* operator++() { return next();}
	void reset();
protected:
	GalTopBlockIter *thisLevelIter;
	IterContext *stack;
	void push(const Galaxy&);
	void pop();
};

// g++ 1.39.1 has a bug that prevents me from saying
// class GalStarIter : private GalAllBlockIter
// (it worked with 1.37.1 and is legal according to the ARM)

class GalStarIter : public GalAllBlockIter {
public:
	GalStarIter(const Galaxy& g);
	Star* next();
	Star* operator++() { return next();}
	GalAllBlockIter::reset();
};

#endif

