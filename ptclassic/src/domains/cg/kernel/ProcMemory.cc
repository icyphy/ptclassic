/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck

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
		delete q;
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
		MemInterval* newInt = new MemInterval(reqAddr + reqSize,
						      hiSize, p->link);
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
	l = new MemInterval(q->addr,q->len);
	MemInterval* p = l;
	while (q->link) {
		q = q->link;
		p->link = new MemInterval(q->addr,q->len);
		p = p->link;
	}
	p->link = 0;
}

void MemoryList::zero() {
	MemInterval* p;
	while (l->link) {
		p = l; l = l->link; delete p;
	}
}

int MemoryList::addChunk(unsigned addr,unsigned len) {
	if (len == 0) return TRUE;
	if (addr + len <= min) {
		min = addr;
		l = new MemInterval(addr, len, l);
	}
	else if (addr > max) {
		max = addr + len - 1;
		MemInterval* p = l;
		while (p->link) p = p->link;
		p->link = new MemInterval(addr, len);
	}
	else return FALSE;
	return TRUE;
}


// stuff for LinProcMemory
void LinProcMemory::reset() {
	lin.zero();
	circ.zero();
	delete consec;
}

void LinProcMemory::allocReq(AsmPortHole& p) {
	if (!match(p)) return;
	MPortReq* r = new MPortReq(p);
	if (p.circAccess())
		circ.appendSorted(*r);
	else lin.appendSorted(*r);
}

void LinProcMemory::allocReq(const State& s) {
	if (!match(s)) return;

	if (s.attributes() | AB_CONSEC) {
		if (!consec) consec = new MConsecStateReq;
		consec->append(s);
		return;
	}
	if (consec->size() > 0) {
		lin.appendSorted(*consec);
		consec = 0;
	}
	MStateReq* r = new MStateReq(s);
	if (s.attributes() | AB_CIRC)
		circ.appendSorted(*r);
	else lin.appendSorted(*r);
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
	}
	MReqListIter nextLin(lin);
	while ((r = nextLin++) != 0) {
		unsigned addr;
		if (!mem.firstFitAlloc(r->size(),addr)) {
			Error::abortRun("Memory allocation failure!");
			return FALSE;
		}
		r->assign(*this,addr);
	}
	return TRUE;
}

static unsigned share_len(unsigned xa,unsigned ya,unsigned xl,unsigned yl) {
	unsigned s = max(xa,ya);
	unsigned end = min(xa+xl,ya+yl);
	return max(end,s) - s;
}

DualMemory:: DualMemory(const Attribute& st, // attribute for states
			const Attribute& p,  // attribute for portholes
			const Attribute& a_x,// attribute for X, as opposed to
					     // Y memory.
			unsigned x_addr,     // start of x memory
			unsigned x_len,	     // length of x memory
			unsigned y_addr,     // start of y memory
			unsigned y_len	     // length of y memory
			) : 
			sAddr(max(x_addr,y_addr)),
			sLen(share_len(x_addr,y_addr,x_len,y_len)),
	LinProcMemory(st,p,sAddr,sLen), x(st,p,0,0), y(st,p,0,0), 
	xAddr(x_addr),xLen(x_len), yAddr(y_addr),yLen(y_len), xmemAttr(a_x)
{}

void DualMemory::allocReq(AsmPortHole& p) {
	if (!match(p)) return;
	if (p.attributes() | AB_SHARED)
		LinProcMemory::allocReq(p);
	else if (consistent(p.attributes(), xmemAttr))
		x.allocReq(p);
	else
		y.allocReq(p);
}

void DualMemory::allocReq(const State& s) {
	if (s.attributes() | AB_SHARED)
		LinProcMemory::allocReq(s);
	else if (consistent(s.attributes(), xmemAttr))
		x.allocReq(s);
	else
		y.allocReq(s);
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
		   
