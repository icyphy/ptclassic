static const char file_id[] = "ProcMemory.cc";
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
#include "ProcMemory.h"
#include "AsmStar.h"
#include "miscFuncs.h"
#include "isa.h"
#include "dataType.h"
#include <builtin.h>
#include <minmax.h>

int LinProcMemory::firstFitAlloc(unsigned reqSize, unsigned &reqAddr) {
	IntervalListIter nextI(memAvail);
	Interval *p;
	// skip through the list of blocks until we find one that is
	// big enough.  Fail if none are big enough.
	while ((p = nextI++) != 0 && p->length() < reqSize)
		;
	if (p == 0) return FALSE;
	reqAddr = p->origin();
	memAvail.subtract(Interval(reqAddr,reqSize));
	return TRUE;
}

int LinProcMemory::circBufAlloc(unsigned reqSize, unsigned& reqAddr) {
	IntervalListIter nextI(memAvail);
	unsigned mask = 1;
	while (mask < reqSize) mask <<= 1;
	mask -= 1;
	Interval* p;
	while ((p = nextI++) != 0) {
		if (p->length() < reqSize) continue;
		// get the next higher aligned address
		reqAddr = (p->origin() + mask) & ~mask;
		// break if it fits
		if (p->origin() + p->length() - reqAddr >= reqSize) break;
	}
	if (p == 0) return FALSE;
	memAvail.subtract(Interval(reqAddr,reqSize));
	return TRUE;
}

// stuff for LinProcMemory

LinProcMemory::LinProcMemory(const char* n, const Attribute& a,
			     const Attribute& p,const char* memoryMap)
: ProcMemory(n,a,p), mem(memoryMap), consec(0) {
	memAvail = mem;
}

void LinProcMemory::addMem(const IntervalList& toAdd) {
	memAvail |= toAdd;
}

void LinProcMemory::initMem(const IntervalList & memory) {
	mem = memAvail = memory;
}

void LinProcMemory::assign(MReq* request, unsigned addr) {
	// set up the address info for the request.
	request->assign(*this,addr);
	// Append the allocation to the memory map
	map.appendSorted(addr,*request);
}

void LinProcMemory::reset() {
	lin.zero();
	circ.zero();
	map.zero();
	memAvail = mem;
	LOG_DEL; delete consec;
	consec = 0;
}

int LinProcMemory::allocReq(AsmPortHole& p) {
	if (!match(p)) return FALSE;
	if (p.localBufSize() == 0) return TRUE;
	LOG_NEW; MPortReq* r = new MPortReq(p);
	if (r->circ())	circ.appendSorted(*r);
	else		lin.appendSorted(*r);
	return TRUE;
}

int LinProcMemory::allocReq(const State& s) {
	if (!match(s)) return FALSE;

	// request for consecutive allocation: add this state to a
	// MConsecStateReq and do not request memory yet.
	if (s.attributes() & AB_CONSEC) {
		if (!consec) { LOG_NEW; consec = new MConsecStateReq; }
		consec->append(s);
		return TRUE;
	}

	// Test for the last state in a consecutive block (consec is
	// set but this state has no AB_CONSEC).  If it matches, we
	// then complete a request.
	if (consec && consec->size() > 0) {
		consec->append(s);
		if (s.attributes() & AB_CIRC)
			circ.appendSorted(*consec);
		else lin.appendSorted(*consec);
		consec = 0;
		return TRUE;
	}

	// The following is the normal case.
	LOG_NEW; MStateReq* r = new MStateReq(s);
	if (s.attributes() & AB_CIRC)
		circ.appendSorted(*r);
	else lin.appendSorted(*r);
	return TRUE;
};

int LinProcMemory::performAllocation() {
// first allocate the circular buffers
	MReqListIter nextCirc(circ);
	MReq* r;
	while ((r = nextCirc++) != 0) {
		unsigned addr;
		if (!circBufAlloc(r->size(),addr)) {
			StringList m = r->print();
			m << "\nInsufficient memory, needed " << int(r->size())
			  << " words";
			Error::abortRun("Memory allocation failure (circ): ", m);
			return FALSE;
		}
		assign(r,addr);
	}
	MReqListIter nextLin(lin);
	while ((r = nextLin++) != 0) {
		unsigned addr;
		if (!firstFitAlloc(r->size(),addr)) {
			StringList m = r->print();
			m << "\nInsufficient memory, needed " << int(r->size())
			  << " words";
			Error::abortRun("Memory allocation failure: ", m);
			return FALSE;
		}
		assign(r,addr);
	}
	return TRUE;
}

StringList LinProcMemory::printMemMap(const char* startString, const char* endString) {
	StringList l = "";
	MemMapIter nextItem(map);
	MemAssignment *mem;
	while((mem = nextItem++) != 0) {
	    l += startString;
	    l += " Loc ";
	    l += int(mem->addr());
	    l += ", length ";
	    l += mem->length();
	    l += ", ";
	    l += mem->print();
	    if(mem->circ())
		l += " (circular)";
	    l += endString;
	    l += "\n";
	}
	return l;
}

StringList DualMemory::printMemMap(const char* startString, const char* endString) {
	StringList l = startString;
	l += " --------------------- Symmetric memory map: ";
	l += endString;
	l += "\n";
	l += LinProcMemory::printMemMap(startString, endString);

	l += startString;
	l += " --------------------- ";
	l += x.name();
	l += " memory map: ";
	l += endString;
	l += "\n";
	l += x.printMemMap(startString, endString);

	l += startString;
	l += " --------------------- ";
	l += y.name();
	l += " memory map: ";
	l += endString;
	l += "\n";
	l += y.printMemMap(startString, endString);

	return l;
}

DualMemory:: DualMemory(const char* n_x,       // name of first memory space
			const Attribute& st_x, // attribute for states
			const Attribute& p_x,  // attribute for portholes
			const char* x_map,     // X memory map
			const char* n_y,       // name of second memory space
			const Attribute& st_y, // attribute for states
			const Attribute& p_y,  // attribute for portholes
			const char* y_map      // Y memory map
	) : 
	// set up the symmetric memory part
	LinProcMemory("symmetric",
		      (st_x&st_y)|A_SYMMETRIC, (p_x&p_y)|A_SYMMETRIC, "<>"
		      ),
		      x(n_x,st_x,p_x,"<>"), y(n_y,st_y,p_y,"<>")
{
	IntervalList tmpx(x_map);
	IntervalList tmpy(y_map);
	initMem(tmpx&tmpy);
	x.initMem(tmpx-tmpy);
	y.initMem(tmpy-tmpx);
}

DualMemory::~DualMemory() { reset(); }

int DualMemory::allocReq(AsmPortHole& p) {
	if (!LinProcMemory::allocReq(p))
	    if (!x.allocReq(p))
		return y.allocReq(p);
	return TRUE;
}

int DualMemory::allocReq(const State& s) {
	if (!LinProcMemory::allocReq(s))
	    if (!x.allocReq(s))
		return y.allocReq(s);
	return TRUE;
}

int DualMemory::performAllocation() {
// first do all the symmetric memory.
	if (!LinProcMemory::performAllocation()) return FALSE;
// now make the remaining-memory-lists for the others, by adding
// in remaining shared memory.
	x.addMem(memAvail);
	y.addMem(memAvail);
// allocate x and y memory.
	if (!x.performAllocation()) return FALSE;
	return y.performAllocation();
}

void DualMemory::reset() {
	x.reset();
	y.reset();
	LinProcMemory::reset();
}
