static const char file_id[] = "MReq.cc";
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

 Programmer: J. Buck

 This is support for lists of memory requests that can be arranged
 in decreasing order, for efficient memory allocation.

*******************************************************************/
#include "MReq.h"

// append a request to the end of the list.
void MReqList::append(MReq& m) {
	if (first == 0) first = &m;
	else last->next = &m;
	last = &m;
	m.next = 0;
}

// append a request with an insertion sort, so requests are in
// decreasing order of size.
void MReqList::appendSorted(MReq& m) {
	if (first == 0) {
		append(m);
		return;
	}
	int sz = m.size();
	if (first->size() < sz) {
		// new guy is biggest, put it first
		m.next = first;
		first = &m;
		return;
	}
	MReq* q = first; MReq* p = first->next;
	while (p && p->size() >= sz) {
		q = p;
		p = p->next;
	}
	// insert new item between p and q
	m.next = p;
	q->next = &m;
	return;
}

void MReqList::zero() {
	while (first) {
		MReq* p = first;
		first = p->next;
		LOG_DEL; delete p;
	}
}

int MPortReq::circ() {
	if ( myport.circAccess() )
		return TRUE;
	if ( ((AsmPortHole*)(myport.far()))->circAccess() )
		return TRUE;
	return FALSE;
}

StringList MPortReq::print() {
	StringList out = "port ";
	out += myport.parent()->fullName();
	out += "(";
	out += myport.name();
	out += "), type ";
	out += myport.type();
	return out;
}

int MStateReq::circ() {
	return (mystate.attributes() & AB_CIRC) != 0;
}

StringList MStateReq::print() {
	StringList out = "state ";
	out += mystate.parent()->fullName();
	out += "(";
	out += mystate.name();
	out += "), type ";
	out += mystate.type();
	return out;
}

void MStateReq::assign(ProcMemory& proc, unsigned addr) {
	((AsmStar*)(mystate.parent()))->addEntry(mystate,proc,addr);
}

// state returns the first state on the list
const State* MConsecStateReq::state() {
	MReqListIter next(lis);
	MReq* m = next++;
	return m ? m->state() : 0;
}

StringList MConsecStateReq::print() {
	StringList out = "consecutive states:";
	MReqListIter next(lis);
	int first = 1;
	MReq* m;
	const char* sep = "(";
	while ((m = next++) != 0) {
		const State* s = m->state();
		if (!s) { out += "???"; return out;}
		if (first) {
			out += s->parent()->fullName();
			first = 0;
		}
		out += sep;
		out += s->name();
		sep = ", ";
	}
	out += ")";
	return out;
}

void MConsecStateReq::assign(ProcMemory& proc, unsigned addr) {
	MReqListIter next(lis);
	MReq* m;
	while ((m = next++) != 0) {
		m->assign(proc,addr);
		addr += m->size();
	}
}

void MConsecStateReq::zero() {
	lis.zero();
	sz = 0;
}
