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
	AsmPortHole& port;
public:
	MPortReq(AsmPortHole& p) : port(p) {}
	void assign(ProcMemory& proc, unsigned addr) {
		port.assignAddr(proc,addr);
	}
	int size() { return port.bufSize();}
};

class MStateReq : public MReq {
	const State& state;
public:
	MStateReq(const State& s) : state(s) {}
	void assign(ProcMemory& proc, unsigned addr) {
		((AsmStar*)(state.parent()))->addEntry(state,proc,addr);
	}
	int size() { return state.size();}
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
	int size() { return sz;}
	void zero();
	~MConsecStateReq() { zero();}
	void assign(ProcMemory&,unsigned);
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
