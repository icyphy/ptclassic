static const char file_id[] = "MemMap.cc";
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

 Programmer: E. A. Lee

 This is support for lists of allocated memory arranged sequentially.

*******************************************************************/
#include "MemMap.h"

// Append an allocation sorted in order of memory address.
void MemMap::appendSorted(unsigned start, MReq& mreq) {
	LOG_NEW; MemAssignment *m = new MemAssignment(start,mreq);

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
