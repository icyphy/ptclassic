#ifndef _ProcMemory_h
#define _ProcMemory_h 1
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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

 Programmer: J. Buck, E. A. Lee and J. Pino

// a ProcMemory object represents a processor's memory.  In the base
// class, no assumption about the organization of the memory is made.
// The operation supported by a ProcMemory is to allocate space for
// states.  A memory has an attribute which states must match to qualify
// for allocation in this memory.

// To support efficient allocation, we use a two-stage process:
// allocReq requests allocation of memory for the state or porthole,
// but the allocations don't take place until performAllocation is
// called.
******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "MReq.h"
#include "MemMap.h"
#include "IntervalList.h"

inline int consistent(bitWord t,const Attribute& a) {
	return t == a.eval(t);
}

class ProcMemory {
public:
	// Return the name of the memory.
	// This is not a NamedObj because we want readName() to be virtual.
	virtual const char* name() { return myName; }

	const Attribute& reqdStateAttributes() const {
		return reqdStateAttributeBits;
	}

	const Attribute& reqdPortAttributes() const {
		return reqdPortAttributeBits;
	}

	// Constructor.  Specify the name, required state attributes, and
	// required PortHole attributes for any states or ports that
	// will be assigned this memory.
	ProcMemory (const char* n, const Attribute& a, const Attribute& p)
		: myName(n), reqdStateAttributeBits(a), reqdPortAttributeBits(p)
		{}

	// Destructor.  Dummy here, but declare it virtual.
	virtual ~ProcMemory() {}

	// Return true if the State has the required attributes.
	int match(const State& s) {
		return consistent(s.attributes(),reqdStateAttributes());
	}

	// Return true if the PortHole has the required attributes.
	int match(const PortHole& p) {
		return consistent(p.attributes(), reqdPortAttributes());
	}

	// Log a request for memory, to be allocated later.
	// Returns TRUE if the attributes of the state or port match the
	// required attributes of the memory, and hence return TRUE if the
	// request was successfully logged.
	virtual int allocReq(const State& s) = 0;
	virtual int allocReq(AsmPortHole& p) = 0;

	// Perform all registered allocation requests
	virtual int performAllocation() = 0;

	// Reset the memory, clearing all previous allocations
	virtual void reset() = 0;

	// Return a string describing the memory map.
	// The string consists of multiple lines, with each line beginning
	// with the startString and ending with the endString.
	// In this base class, just return a comment.
	virtual StringList printMemMap(const char* startString,
				       const char* endString) {
		StringList l = startString;
		l += " don't know how to print memory map ";
		l += endString;
		return l;
	}
private:
	Attribute reqdStateAttributeBits;
	Attribute reqdPortAttributeBits;
	const char* myName;	// my name
};


class LinProcMemory : public ProcMemory {
public:
	// Constructor with name, required State attributes, required PortHole
	// attributes, starting address, and length specified.
	LinProcMemory(const char* n, const Attribute& a,
		      const Attribute& p,const char* memoryMap);

	// Copy constructor: allocation lists are always made empty,
	// we only copy the total memory list.

	LinProcMemory(const LinProcMemory& arg)
		: ProcMemory(arg), mem(arg.mem), consec(0) {}

	~LinProcMemory() { reset();}

	// Reset the memory, clearing all previous allocations
	void reset();

	// Add memory to the available list.
	void addMem(const IntervalList& toAdd);

	// (re)initialize the mem and memAvail members.
	void initMem(const IntervalList& memory);

	// Log a request for memory, to be allocated later.
	// Returns TRUE if the attributes of the state or port match the
	// required attributes of the memory, and hence return TRUE if the
	// request was successfully logged.
	int allocReq(const State& s);
	int allocReq(AsmPortHole& p);

	// Perform all registered allocation requests
	int performAllocation();

	// Return a string describing the memory map.
	// The string consists of multiple lines, with each line beginning
	// with the startString and ending with the endString.
	virtual StringList printMemMap(const char* startString, const char* endString);
protected:
	MReqList lin;		// linear allocation requests
	MReqList circ;		// circular allocation requests
	IntervalList mem;	// total memory
	IntervalList memAvail;	// available memory
	MConsecStateReq *consec;// consecutive-allocation requests
	MemMap	map;		// map of allocations

	// Assign an address for a request and record it in the map
	void assign(MReq* request, unsigned addr);

	// allocate using first-fit
	int firstFitAlloc(unsigned reqSize,unsigned &resultAddr);

	// allocate requiring circular alignment
	int circBufAlloc(unsigned reqSize,unsigned &resultAddr);
};

// this models a two-address-space chip such as the Motorola 56000
// or the Analog Devices 2100 family.  All porthole buffers are placed
// in x memory.  States may go into either memory (based on attributes)
// or go into both memories if they have the AB_SYMMETRIC attribute.
class DualMemory : public LinProcMemory {
public:
	// Constructor
	DualMemory(const char* n_x,	// name of the first memory space
		   const Attribute& st_x,	// attribute for states
		   const Attribute& p_x,	// attribute for portholes
		   const char* x_map,		// map of x memory
		   const char* n_y,	// name of the second memory space
		   const Attribute& st_y,	// attribute for states
		   const Attribute& p_y,	// attribute for portholes
		   const char* y_map		// map of y memory
	);

	// Copy constructor
	DualMemory(const DualMemory& arg) :
		LinProcMemory(arg), x(arg.x), y(arg.y) {}

	// Reset the memory, clearing all previous allocations
	void reset();

	// Destructor
	~DualMemory();

	// Log a request for memory, to be allocated later.
	// Returns TRUE if the attributes of the state or port match the
	// required attributes of the memory, and hence return TRUE if the
	// request was successfully logged.
	int allocReq(const State&);
	int allocReq(AsmPortHole&);

	// Perform all registered allocation requests
	int performAllocation();

	// Return a string describing the memory map.
	// The string consists of multiple lines, with each line beginning
	// with the startString and ending with the endString.
	virtual StringList printMemMap(const char* startString, const char* endString);
protected:
	LinProcMemory x;
	LinProcMemory y;
};
	
#endif
