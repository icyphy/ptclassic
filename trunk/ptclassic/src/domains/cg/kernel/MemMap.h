#ifndef _MemMap_h
#define _MemMap_h 1
/******************************************************************
Version identification:
%w%	$Date$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: E. A. Lee

 This is support for lists of allocated memory arranged sequentially.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "MReq.h"

class ProcMemory;

// A memory assignment.
// This is normally accessed through the MemMap class.
class MemAssignment {
	friend class MemMap;
	friend class MemMapIter;
	MemAssignment* next;
	unsigned start;
	MReq& req;
public:
	unsigned addr() { return start;}
	int length() { return req.size();}
	int circ() { return req.circ();}

	StringList print() { return req.print();}
	MemAssignment(unsigned addr, MReq& mreq, MemAssignment* link = 0)
		: next(link), start(addr), req(mreq) {}
	~MemAssignment() {}
};

// A list of memory assignments.
class MemMap {
	friend class MemMapIter;
	MemAssignment* first;
public:
	MemMap() : first(0) {}
	// Clear the memory map.
	// Warning! does delete on all the MemAssignment objects!
	void zero();
	~MemMap() {zero();}
	int empty() const { return first == 0;}
	void appendSorted(unsigned start, MReq& mreq);
};

class MemMapIter {
	MemAssignment* ptr;
	MemMap& ref;
public:
	MemMapIter(MemMap& l) : ref(l), ptr(l.first) {}
	void reset() { ptr = ref.first;}
	MemAssignment* next() {
		MemAssignment* res = ptr;
		if (ptr) ptr = ptr->next;
		return res;
	}
	MemAssignment* operator++() { return next();}
};

#endif
