/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 10/30/91

Iterators that work on const container objects, returning const
pointers to inside objects.

The more commonly used non-const iterators are defined in files
Block.h, PortHole.h, State.h, or Galaxy.h.

**************************************************************************/
#ifndef _ConstIters_h
#define  _ConstIters_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "Block.h"
#include "PortHole.h"
#include "State.h"
#include "Galaxy.h"

// Step through PortHoles in a PortList
class CPortListIter : private ListIter {
public:
	CPortListIter(const PortList& plist) : ListIter (plist) {}
	const PortHole* next() { return (const PortHole*)ListIter::next();}
	const PortHole* operator++() { return next();}
	ListIter::reset;
};

// Step through States in a StateList
class CStateListIter : private ListIter {
public:
	CStateListIter(const StateList& sl) : ListIter (sl) {}
	const State* next() { return (const State*)ListIter::next();}
	const State* operator++() { return next();}
	ListIter::reset;
};

// Step through PortHoles in a Block
class CBlockPortIter : public CPortListIter {
public:
	CBlockPortIter(const Block& b) : CPortListIter (b.ports) {}
};

// Step through States in a Block
class CBlockStateIter : public CStateListIter {
public:
	CBlockStateIter(const Block& b) : CStateListIter (b.states) {}
};

// Step through MultiPortHoles in a Block
class CBlockMPHIter : private ListIter {
public:
	CBlockMPHIter(const Block& b) : ListIter (b.multiports) {}
	const MultiPortHole* next() {
		return (const MultiPortHole*)ListIter::next();
	}
	const MultiPortHole* operator++() { return next();}
	ListIter::reset;
};

// Step through PortHoles in a MultiPortHole
class CMPHIter : public CPortListIter {
public:
	CMPHIter(const MultiPortHole& mph) : CPortListIter (mph.ports) {}
};

// Step through Blocks in a Galaxy
class CGalTopBlockIter : private ListIter {
public:
	CGalTopBlockIter(const Galaxy& g) : ListIter(g.blocks) {}
	const Block* next() { return (const Block*)ListIter::next();}
	const Block* operator++() { return next();}
	ListIter::reset;
};

#endif
