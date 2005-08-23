#ifndef _MReq_h
#define _MReq_h 1
/******************************************************************
Version identification:
@(#)MReq.h	1.16	3/2/95

Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

 Programmer: J. Buck

 This is support for lists of memory requests that can be arranged
 in decreasing order, for efficient memory allocation.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "AsmStar.h"
#include "AsmPortHole.h"

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
	MReqListIter(MReqList& l) :  ptr(l.first), ref(l) {}
	void reset() { ptr = ref.first;}
	MReq* next() {
		MReq* res = ptr;
		if (ptr) ptr = ptr->next;
		return res;
	}
	MReq* operator++(POSTFIX_OP) { return next();}
};

#endif
