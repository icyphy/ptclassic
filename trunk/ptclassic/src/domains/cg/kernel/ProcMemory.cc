static const char file_id[] = "ProcMemory.cc";
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
#include "ProcMemory.h"
#include "AsmStar.h"
#include "miscFuncs.h"
#include "isa.h"
#include "dataType.h"
#include <builtin.h>

int MemoryList::firstFitAlloc(unsigned reqSize, unsigned &reqAddr) {
	MemInterval* p = l;
	while (p && p->len < reqSize) p = p->link;
	if (p == 0) return FALSE;
	reqAddr = p->addr;
	if (p->len < reqSize) {
		p->addr += reqSize;
		p->len -= reqSize;
	}
	else if (p->link) {
		// delete this chunk
		MemInterval* q = p->link;
		*p = *q;
		LOG_DEL; delete q;
	}
	else p->len = 0;
	return TRUE;
}

int MemoryList::circBufAlloc(unsigned reqSize, unsigned& reqAddr) {
	unsigned mask = 1;
	while (mask < reqSize) mask <<= 1;
	mask -= 1;
	MemInterval* p;
	for (p = l; p; p = p->link) {
		if (p->len < reqSize) continue;
		// get the next higher aligned address
		reqAddr = (p->addr + mask) & ~mask;
		// break if it fits
		if (p->addr + p->len - reqAddr >= reqSize) break;
	}
	if (p == 0) return FALSE;
	if (reqAddr == p->addr) {
		p->addr += reqSize;
		p->len -= reqSize;
	}
	else if (reqAddr - p->addr == p->len - reqSize) {
		p->len -= reqSize;
	}
	else {
	// OK, block has split.  Make new piece for 2nd half
		int loSize = reqAddr - p->addr;
		int hiSize = p->len - reqSize - loSize;
		LOG_NEW; MemInterval* newInt =
			new MemInterval(reqAddr + reqSize, hiSize, p->link);
		p->len = loSize;
		p->link = newInt;
	}
	return TRUE;
}

// copy constructor
void MemoryList::copy(const MemoryList& src) {
	min = src.min;
	max = src.max;
	MemInterval* q = src.l;
	LOG_NEW; l = new MemInterval(q->addr,q->len);
	MemInterval* p = l;
	while (q->link) {
		q = q->link;
		LOG_NEW; p->link = new MemInterval(q->addr,q->len);
		p = p->link;
	}
	p->link = 0;
}

void MemoryList::zero() {
	MemInterval* p;
	while (l->link) {
		p = l; l = l->link; LOG_DEL; delete p;
	}
}

int MemoryList::addChunk(unsigned addr,unsigned len) {
	if (len == 0) return TRUE;
	if (addr + len <= min) {
		min = addr;
		LOG_NEW; l = new MemInterval(addr, len, l);
	}
	else if (addr > max) {
		max = addr + len - 1;
		MemInterval* p = l;
		while (p->link) p = p->link;
		LOG_NEW; p->link = new MemInterval(addr, len);
	}
	else return FALSE;
	return TRUE;
}

void MemoryList::reset() {
	zero();
	if(l) { LOG_DEL; delete l; }
	LOG_NEW; l = new MemInterval(min, max-min+1);
}


// stuff for LinProcMemory

void LinProcMemory::record(MReq* request, unsigned addr) {
	AsmPortHole* p = request->port();

	// Determine the type
	DataType type;
	if(p) type = p->myType();
	else type = FLOAT;	// Temporary HACK.  The type of a state
				// seems to need work for code generation.

	// Determine whether the access is circular
	int circular;
	if(p) circular = p->circAccess();
	else circular = FALSE;	// Temporary HACK.  Want to be able to allow
				// circular buffers for state.

	// Append the allocation to the memory map
	map.appendSorted(addr,request->size(),
			 request->state(), p,
			 circular, type);
}

void LinProcMemory::reset() {
	lin.zero();
	circ.zero();
	map.zero();
	mem.reset();
	LOG_DEL; delete consec;
}

int LinProcMemory::allocReq(AsmPortHole& p) {
	if (!match(p)) return FALSE;
	LOG_NEW; MPortReq* r = new MPortReq(p);
	if (p.circAccess())
		circ.appendSorted(*r);
	else lin.appendSorted(*r);
	return TRUE;
}

int LinProcMemory::allocReq(const State& s) {
	if (!match(s)) return FALSE;

	if (s.attributes() & AB_CONSEC) {
		if (!consec) { LOG_NEW; consec = new MConsecStateReq; }
		consec->append(s);
		return TRUE;
	}
	if (consec && consec->size() > 0) {
		lin.appendSorted(*consec);
		consec = 0;
	}
	LOG_NEW; MStateReq* r = new MStateReq(s);
	if (s.attributes() | AB_CIRC)
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
		if (!mem.circBufAlloc(r->size(),addr)) {
			Error::abortRun("Memory allocation failure!");
			return FALSE;
		}
		r->assign(*this,addr);
		record(r,addr);
	}
	MReqListIter nextLin(lin);
	while ((r = nextLin++) != 0) {
		unsigned addr;
		if (!mem.firstFitAlloc(r->size(),addr)) {
			Error::abortRun("Memory allocation failure!");
			return FALSE;
		}
		r->assign(*this,addr);
		record(r,addr);
	}
	return TRUE;
}

static unsigned share_len(unsigned xa,unsigned ya,unsigned xl,unsigned yl) {
	unsigned s = max(xa,ya);
	unsigned end = min(xa+xl,ya+yl);
	return max(end,s) - s;
}

StringList LinProcMemory::printMemMap(char* startString, char* endString) {
	StringList l = "";
	MemMapIter nextItem(map);
	MemAssignment *mem;
	while((mem = nextItem++) != 0) {
	    l += startString;
	    l += " Loc ";
	    l += mem->start;
	    l += ", length ";
	    l += mem->length;
	    if(mem->port) {
		l += ", ";
		l += mem->port->parent()->readFullName();
		l += "(";
		l += mem->port->readName();
		l += ")";
	    }
	    if(mem->state) {
		l += ", ";
		l += mem->state->parent()->readFullName();
		l += "(";
		l += mem->state->readName();
		l += ")";
	    }
	    // Should print out the type here, but I don't know how.
	    if(mem->circular)
		l += " (circular)";
	    l += endString;
	    l += "\n";
	}
	return l;
}

StringList DualMemory::printMemMap(char* startString, char* endString) {
	StringList l = startString;
	l += " --------------------- Shared memory map: ";
	l += endString;
	l += "\n";
	l += LinProcMemory::printMemMap(startString, endString);

	l += startString;
	l += " --------------------- ";
	l += x.readName();
	l += " memory map: ";
	l += endString;
	l += "\n";
	l += x.printMemMap(startString, endString);

	l += startString;
	l += " --------------------- ";
	l += y.readName();
	l += " memory map: ";
	l += endString;
	l += "\n";
	l += y.printMemMap(startString, endString);

	return l;
}

DualMemory:: DualMemory(const char* n_x,     // name of the first memory space
			const Attribute& st_x, // attribute for states
			const Attribute& p_x,  // attribute for portholes
			unsigned x_addr,     // start of x memory
			unsigned x_len,	     // length of x memory
			const char* n_y,     // name of the second memory space
			const Attribute& st_y, // attribute for states
			const Attribute& p_y,  // attribute for portholes
			unsigned y_addr,     // start of y memory
			unsigned y_len	     // length of y memory
	) : 
	sAddr(max(x_addr,y_addr)),
	sLen(share_len(x_addr,y_addr,x_len,y_len)),
	LinProcMemory("shared",(st_x&st_y)|A_SHARED,(p_x&p_y)|A_SHARED,
			sAddr,sLen),
	x(n_x,st_x,p_x,0,0),
	y(n_y,st_y,p_y,0,0), 
	xAddr(x_addr),xLen(x_len),
	yAddr(y_addr),yLen(y_len)
{}

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
// first do all the shared memory.
	if (!LinProcMemory::performAllocation()) return FALSE;
// now make the remaining-memory-lists for the others.
	MemoryList newMem = mem;
	newMem.addChunk(xAddr,sAddr-xAddr);
	newMem.addChunk(xAddr+xLen,sAddr+sLen-xAddr-xLen);
	x.copyMem(newMem);
	if (!x.performAllocation()) return FALSE;
	newMem = mem;
	newMem.addChunk(yAddr,sAddr-yAddr);
	newMem.addChunk(yAddr+yLen,sAddr+sLen-yAddr-yLen);
	y.copyMem(newMem);
	return y.performAllocation();
}

void DualMemory::reset() {
	x.reset();
	y.reset();
	LinProcMemory::reset();
}
