#ifndef _ProcMemory_h
#define _ProcMemory_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

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
#include "MReq.h"
#include "MemMap.h"

inline int consistent(bitWord t,const Attribute& a) {
	return t == a.eval(t);
}

class ProcMemory {
private:
	Attribute reqdStateAttributeBits;
	Attribute reqdPortAttributeBits;
	const char* name;	// my name
public:
	// Return the name of the memory.
	// This is not a NamedObj because we want readName() to be virtual.
	virtual const char* readName() { return name; }

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
		: name(n), reqdStateAttributeBits(a), reqdPortAttributeBits(p)
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
};

// An interval of memory
class MemInterval {
	friend class MemoryList;
	unsigned addr, len;
	MemInterval* link;
	MemInterval(unsigned a, unsigned l, MemInterval* nxt = 0) :
		addr(a), len(l), link(nxt) {}
	~MemInterval();
};

// A collection of intervals of memory
class MemoryList {
	MemInterval* l;
	unsigned min, max;

	// body of copy constructor
	void copy(const MemoryList& src);

	// Remove all MemIntervals.
	void zero();

public:
	// Constructor with starting address and length of memory.
	MemoryList(unsigned addr,unsigned len) : min(addr), max(addr+len-1) {
		INC_LOG_NEW; l = new MemInterval(addr,len);
	}

	// allocate using first-fit
	int firstFitAlloc(unsigned reqSize,unsigned &resultAddr);

	// allocate requiring circular alignment
	int circBufAlloc(unsigned reqSize,unsigned &resultAddr);

	// copy constructor
	MemoryList(const MemoryList& src) { copy(src);}

	// assignment operator
	MemoryList& operator=(const MemoryList& src) {
		zero();
		copy(src);
		return *this;
	}

	// destructor
	~MemoryList() { zero();}

	// Add a new piece to the memory list.  It must not lie in the
	// range of the existing list.
	// This is useful when some objects must be allocated in a
	// restricted range -- first use a MemoryList with a small range,
	// allocate the objects, then grow the memory, then allocate
	// more objects.
	int addChunk(unsigned addr,unsigned len);

	// Reset the memory, clearing all previous allocations.
	void reset();

	// print (debug)
	StringList print();
};

class LinProcMemory : public ProcMemory {
protected:
	MReqList lin;
	MReqList circ;
	MemoryList mem;
	MConsecStateReq *consec;
	MemMap	map;
	// Record a memory allocation in the memory map.
	void record(MReq* request, unsigned addr);

public:
	// Constructor with name, required State attributes, required PortHole
	// attributes, starting address, and length specified.
	LinProcMemory(const char* n, const Attribute& a,
		      const Attribute& p,unsigned addr, unsigned len);

	~LinProcMemory() { reset();}

	// Reset the memory, clearing all previous allocations
	void reset();

	// Log a request for memory, to be allocated later.
	// Returns TRUE if the attributes of the state or port match the
	// required attributes of the memory, and hence return TRUE if the
	// request was successfully logged.
	int allocReq(const State& s);
	int allocReq(AsmPortHole& p);

	// Perform all registered allocation requests
	int performAllocation();

	void copyMem(MemoryList& src) { mem = src;}

	// Return a string describing the memory map.
	// The string consists of multiple lines, with each line beginning
	// with the startString and ending with the endString.
	virtual StringList printMemMap(const char* startString, const char* endString);
};

// this models a two-address-space chip such as the Motorola 56000
// or the Analog Devices 2100 family.  All porthole buffers are placed
// in x memory.  States may go into either memory (based on attributes)
// or go into both memories if they have the AB_SYMMETRIC attribute.
class DualMemory : public LinProcMemory {
protected:
	LinProcMemory x;
	LinProcMemory y;
	unsigned sAddr, sLen;
	unsigned xAddr, xLen;
	unsigned yAddr, yLen;
public:
	// Constructor
	DualMemory(const char* n_x,	// name of the first memory space
		   const Attribute& st_x,	// attribute for states
		   const Attribute& p_x,	// attribute for portholes
		   unsigned x_addr,	// start of x memory
		   unsigned x_len,	// length of x memory
		   const char* n_y,	// name of the second memory space
		   const Attribute& st_y,	// attribute for states
		   const Attribute& p_y,	// attribute for portholes
		   unsigned y_addr,	// start of y memory
		   unsigned y_len	// length of y memory
	);

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
};
	
#endif
