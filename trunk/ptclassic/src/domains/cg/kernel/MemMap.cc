static const char file_id[] = "MemMap.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: E. A. Lee

 This is support for lists of allocated memory arranged sequentially.

*******************************************************************/
#include "MemMap.h"

// Append an allocation sorted in order of memory address.
void MemMap::appendSorted(unsigned start, unsigned length,
                          const State* state, AsmPortHole* port,
                          int circular, DataType type) {
	LOG_NEW; MemAssignment *m = new MemAssignment;
	m->start = start;
	m->length = length;
	m->state = state;
	m->port = port;
	m->circular = circular;
	m->type = type;

	if (first == 0) {
		first = m;
		m->next = 0;
		return;
	}
	if (first->start > start) {
		// new guy is earliest, put it first
		m->next = first;
		first = m;
		return;
	}
	MemAssignment* q = first; MemAssignment* p = first->next;
	while (p && p->start < start) {
		q = p;
		p = p->next;
	}
	// insert new item between p and q
	m->next = p;
	q->next = m;
	return;
}

void MemMap::zero() {
	while (first) {
		MemAssignment* p = first;
		first = p->next;
		LOG_DEL; delete p;
	}
}
