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
class CPortListIter : private CNamedObjListIter {
public:
	CPortListIter(const PortList& plist) : CNamedObjListIter (plist) {}
	const PortHole* next() { return (const PortHole*)CNamedObjListIter::next();}
	const PortHole* operator++() { return next();}
	CNamedObjListIter::reset;
};

// Step through States in a StateList
class CStateListIter : private CNamedObjListIter {
public:
	CStateListIter(const StateList& sl) : CNamedObjListIter (sl) {}
	const State* next() { return (const State*)CNamedObjListIter::next();}
	const State* operator++() { return next();}
	CNamedObjListIter::reset;
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

// An iterator for MPHLists, const version
class CMPHListIter : private CNamedObjListIter {
public:
	CMPHListIter(const MPHList& plist) : CNamedObjListIter (plist) {}
	const MultiPortHole* next() {
		return (const MultiPortHole*)CNamedObjListIter::next();}
	const MultiPortHole* operator++() { return next();}
	CNamedObjListIter::reset;
};

// Step through MultiPortHoles in a Block
class CBlockMPHIter : public CMPHListIter {
public:
	CBlockMPHIter(const Block& b) : CMPHListIter (b.multiports) {}
};

// Step through PortHoles in a MultiPortHole
class CMPHIter : public CPortListIter {
public:
	CMPHIter(const MultiPortHole& mph) : CPortListIter (mph.ports) {}
};

// an iterator for BlockList
class CBlockListIter : private CNamedObjListIter {
public:
	CBlockListIter(const BlockList& sl) : CNamedObjListIter (sl) {}
	const Block* next() { return (const Block*)CNamedObjListIter::next();}
	const Block* operator++() { return next();}
	CNamedObjListIter::reset;
};

// Iterator class associated with const Galaxy
class CGalTopBlockIter : public CBlockListIter {
public:
	CGalTopBlockIter(const Galaxy& g) : CBlockListIter(g.blocks) {}
};

#endif
