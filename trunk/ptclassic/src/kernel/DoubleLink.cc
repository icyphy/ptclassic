static const char file_id[] = "DoubleLink.cc";

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

 Programmer:  Soonhoi Ha, based on S.  Bhattacharyya's code.
 
*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "DoubleLink.h"

////////////////////////
// DoubleLink Methods //
////////////////////////

void DoubleLink::unlinkMe() {
  if (prev!=0) prev->next=next;
  if (next!=0) next->prev=prev;
}

DoubleLink::~DoubleLink() {}

///////////////////////////////
// DoubleLinkList    Methods //
///////////////////////////////

// destructor
DoubleLinkList::~DoubleLinkList() { initialize();}

// insert y immediately ahead of x
void DoubleLinkList::insertAhead(DoubleLink *y, DoubleLink *x) { 
	DoubleLink* tmp = x->prev;
	if (tmp) tmp->next = y;
	y->next = x;
	y->prev = tmp;
	x->prev = y;
	if (myHead==x) myHead=y;
	mySize++;
}

// insert y immediately behind of x
void DoubleLinkList::insertBehind(DoubleLink *y, DoubleLink *x) {
	DoubleLink* tmp = x->next;
	if (tmp) tmp->prev = y;
	y->prev = x;
	y->next = tmp;
	x->next = y;
	if (myTail==x) myTail=y;
	mySize++;
}

// make p be the first (only) node of this list 
void DoubleLinkList :: firstNode(DoubleLink *p) {
	myHead=p; 
	myTail=p; 
	mySize=1; 
	p->next=0; 
	p->prev=0; 
}

void DoubleLinkList :: insertLink(DoubleLink  *p) {
	if (myHead==0) firstNode(p);
	else insertAhead(p,myHead);
}

void DoubleLinkList :: appendLink(DoubleLink *p) {
	if (myHead==0) firstNode(p);
	else insertBehind(p,myTail);
}

void DoubleLinkList::remove(Pointer x) {
	DoubleLinkIter next(*this);
	DoubleLink* p;

	while (( p = next.nextLink()) != 0) {
		if (p->e == x) {
			removeLink(p);
			return;
		}
	}
	return;
}

int DoubleLinkList :: find(Pointer e) {
	DoubleLinkIter next(*this);
	Pointer p;

	while (( p = next++) != 0) {
		if (p == e) return TRUE;
	}
	return FALSE;
}

DoubleLink* DoubleLinkList::unlink(DoubleLink *x) {
	if (mySize == 0 || x == 0) return 0;
	mySize--;
	x->unlinkMe();
	if (myHead==x) myHead = x->next;
	if (myTail==x) myTail = x->prev;
	return x;
}

void DoubleLinkList :: initialize() {
	while (myHead!=0) removeLink(myHead);
	myHead = 0; myTail = 0; mySize = 0;
}

void DoubleLinkList :: reset() {
	myHead = 0; myTail = 0; mySize = 0;
}

Pointer DoubleLinkList :: takeFromFront() { 
	DoubleLink* tmp = getHeadLink();
	if (!tmp) return 0;
	Pointer t = tmp->e;
	LOG_DEL; delete tmp;
	return t; 
}

Pointer DoubleLinkList :: takeFromBack() { 
	DoubleLink* tmp = getTailLink();
	if (!tmp) return 0;
	Pointer t = tmp->e;
	LOG_DEL; delete tmp;
	return t; 
}

///////////////////////////////
// DoubleLinkIter    Methods //
///////////////////////////////

DoubleLink* DoubleLinkIter :: nextLink() {
	DoubleLink* temp = ref;
	if (ref != 0) ref = ref->next;
	return temp;
}

Pointer DoubleLinkIter :: next() {
	DoubleLink* temp = nextLink();
	if (temp) return temp->e;
	return NULL;
}

