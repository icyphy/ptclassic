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

#include "AsmStar.h"
#include "AsmConnect.h"
#include "dataType.h"

class ProcMemory;

// A memory assignment.
// This is normally accessed through the MemMap class.
class MemAssignment {
	friend class MemMap;
	friend class MemMapIter;
	MemAssignment* next;
public:
	unsigned start;
	unsigned length;

	// A memory allocation is for either a State or a PortHole.
	// Hence, after the allocation is done, one of the following
	// two pointers will be non-null.
	const State* state;
	AsmPortHole* port;

	// The following Boolean indicates whether a block of memory
	// will be accessed as a circular buffer.
	int circular;

	// The type is registered here, for the convenience of symbolic
	// debuggers.
	DataType type;

	MemAssignment() : next(0), start(0), length(0), state(0), port(0),
			  circular(FALSE), type(INT) {}
	~MemAssignment() {}
};

// A list of memory assignments.
class MemMap {
	friend class MemMapIter;
	MemAssignment* first;
	MemAssignment* last;
public:
	MemMap() : first(0), last(0) {}
	// Clear the memory map.
	// Warning! does delete on all the MemAssignment objects!
	void zero();
	~MemMap() {zero();}
	int empty() const { return first == 0;}
	void appendSorted(unsigned start, unsigned length,
			  const State* state, AsmPortHole* port,
			  int circular, DataType type);
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
