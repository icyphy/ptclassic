#ifndef _ProcMemory_h
#define _ProcMemory_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck and E. A. Lee

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

	// Return true if the State has the required attributes.
	int match(const State& s) {
		return consistent(s.attributes(),reqdStateAttributes());
	}

	// Return true if the PortHole has the required attributes.
	int match(const PortHole& p) {
		return consistent(p.attributes(), reqdPortAttributes());
	}

	// Returns TRUE if the attributes of the state match the required
	// attributes for states of the memory, and hence the request was
	// registered.
	virtual int allocReq(const State& s) = 0;

	// Returns TRUE if the attributes of the PortHole match the required
	// attributes for PortHoles of the memory, and hence the request was
	// registered.
	virtual int allocReq(AsmPortHole& p) = 0;

	// Perform all registered allocation requests
	virtual int performAllocation() = 0;

	// Reset the memory, clearing all previous allocations
	virtual void reset() = 0;
};

// An interval of memory
class MemInterval {
	friend class MemoryList;
	unsigned addr, len;
	MemInterval* link;
	MemInterval(unsigned a, unsigned l, MemInterval* nxt = 0) :
		addr(a), len(l), link(nxt) {}
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
		l = new MemInterval(addr,len);
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
};

class LinProcMemory : public ProcMemory {
protected:
	MReqList lin;
	MReqList circ;
	MemoryList mem;
	MConsecStateReq *consec;

public:
	// Constructor with name, required State attributes, required PortHole
	// attributes, starting address, and length specified.
	LinProcMemory(const char* n, const Attribute& a,
		      const Attribute& p,unsigned addr, unsigned len)
		: ProcMemory(n,a,p), mem(addr,len), consec(0) {}

	~LinProcMemory() { reset();}

	// Reset the memory, clearing all previous allocations
	void reset();

	// Returns TRUE if the attributes of the state match the required
	// attributes for states of the memory, and hence the request was
	// registered.
	int allocReq(const State& s);

	// Returns TRUE if the attributes of the PortHole match the required
	// attributes for PortHoles of the memory, and hence the request was
	// registered.
	int allocReq(AsmPortHole& p);

	// Perform all registered allocation requests
	int performAllocation();

	void copyMem(MemoryList& src) { mem = src;}
};

// this models a two-address-space chip such as the Motorola 56000
// or the Analog Devices 2100 family.  All porthole buffers are placed
// in x memory.  States may go into either memory (based on attributes)
// or go into both memories if they have the AB_SHARED attribute.
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
	~DualMemory() { reset();}

	// Returns TRUE if the attributes of the state match the required
	// attributes for states of the memory, and hence the request was
	// registered.
	int allocReq(const State&);

	// Returns TRUE if the attributes of the PortHole match the required
	// attributes for PortHoles of the memory, and hence the request was
	// registered.
	int allocReq(AsmPortHole&);

	// Perform all registered allocation requests
	int performAllocation();
};
	
#endif
