#ifndef _MReq_h
#define _MReq_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck

 This is support for lists of memory requests that can be arranged
 in decreasing order, for efficient memory allocation.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "AsmStar.h"
#include "AsmConnect.h"

class ProcMemory;

// a generic memory allocation request.
class MReq {
	friend class MReqList;
	friend class MReqListIter;
	MReq* next;
public:
	virtual void assign(ProcMemory&,unsigned) = 0;
	virtual int size() = 0;
	MReq() : next(0) {}
	virtual ~MReq() {}

	// Return a pointer to the AsmPortHole using the requested memory.
	// If it is not an AsmPortHole, return NULL.
	virtual AsmPortHole* port() { return NULL; }

	// Return a pointer to the State using the requested memory.
	// If it is not a State, return NULL.
	virtual const State* state() { return NULL; }

	// return TRUE if the request is for a circular buffer.
	virtual int circ() { return 0;}

	// print name of object requesting storage
	virtual StringList print() = 0;
};

// a list of requests.
class MReqList {
	friend class MReqListIter;
	MReq* first;
	MReq* last;
public:
	MReqList() : first(0), last(0) {}
	// warning! does delete on all the MReq objects!
	void zero();
	~MReqList() {zero();}
	int empty() const { return first == 0;}
	void append(MReq& m);
	void appendSorted(MReq& m);
};

class MPortReq : public MReq {
	AsmPortHole& myport;
public:
	MPortReq(AsmPortHole& p) : myport(p) {}
	void assign(ProcMemory& proc, unsigned addr) {
		myport.assignAddr(proc,addr);
	}
	StringList print();
	int size() { return myport.localBufSize();}

	// Return a pointer to the AsmPortHole using the requested memory.
	AsmPortHole* port() { return &myport; }

	int circ();
};

class MStateReq : public MReq {
	const State& mystate;
public:
	MStateReq(const State& s) : mystate(s) {}
	void assign(ProcMemory& proc, unsigned addr);
	int size() { return mystate.size();}
	StringList print();

	// Return a pointer to the State using the requested memory.
	const State* state() { return &mystate; }

	int circ();
};

class MConsecStateReq : public MReq {
	MReqList lis;
	int sz;
public:
	MConsecStateReq() : sz(0) {}
	void append(const State& s) {
		INC_LOG_NEW; lis.append(*new MStateReq(s));
		sz += s.size();
	}
	StringList print();
	int size() { return sz;}
	void zero();
	~MConsecStateReq() { zero();}
	void assign(ProcMemory&,unsigned);
	// Here, state returns a pointer to the FIRST state in
	// the list
	const State* state();
};

class MReqListIter {
	MReq* ptr;
	MReqList& ref;
public:
	MReqListIter(MReqList& l) : ref(l), ptr(l.first) {}
	void reset() { ptr = ref.first;}
	MReq* next() {
		MReq* res = ptr;
		if (ptr) ptr = ptr->next;
		return res;
	}
	MReq* operator++() { return next();}
};

#endif
