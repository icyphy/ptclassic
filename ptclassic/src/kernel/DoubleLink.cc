static const char file_id[] = "DoubleLink.cc";

/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha, based on S.  Bhattacharyya's code.
 
*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "DoubleLink.h"

/////////////////////////
// Double Link Methods //
/////////////////////////

void DoubleLink::unlinkMe() {
  if (prev!=0) prev->next=next;
  if (next!=0) next->prev=prev;
}

///////////////////////////////
// DoubleLinkList    Methods //
///////////////////////////////

// insert y immediately ahead of x
void DoubleLinkList::insertAhead(DoubleLink *y, DoubleLink *x) { 
	DoubleLink* tmp = x->prev;
	if (tmp) tmp->next = y;
	y->next = x;
	y->prev = tmp;
	x->prev = y;
	if (head==x) head=y;
	size++;
}

// insert y immediately behind of x
void DoubleLinkList::insertBehind(DoubleLink *y, DoubleLink *x) {
	DoubleLink* tmp = x->next;
	if (tmp) tmp->prev = y;
	y->prev = x;
	y->next = tmp;
	x->next = y;
	if (tail==x) tail=y;
	size++;
}

// make p be the first (only) node of this list 
void DoubleLinkList :: firstNode(DoubleLink *p) {
	head=p; 
	tail=p; 
	size=1; 
	p->next=0; 
	p->prev=0; 
}

void DoubleLinkList :: insertLink(DoubleLink  *p) {
	if (head==0) firstNode(p);
	else insertAhead(p,head);
}

void DoubleLinkList :: appendLink(DoubleLink *p) {
	if (head==0) firstNode(p);
	else insertBehind(p,tail);
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
	if (size == 0 || x == 0) return 0;
	size--;
	x->unlinkMe();
	if (head==x) head = x->next;
	if (tail==x) tail = x->prev;
	return x;
}

void DoubleLinkList :: initialize() {
	while (head!=0) removeLink(head);
	head = 0; tail = 0; size = 0;
}

void DoubleLinkList :: reset() {
	head = 0; tail = 0; size = 0;
}

Pointer DoubleLinkList :: takeFromFront() { 
	DoubleLink* tmp = getHeadLink();
	if (!tmp) return 0;
	Pointer t = tmp->e;
	INC_LOG_DEL; delete tmp;
	return t; 
}

Pointer DoubleLinkList :: takeFromBack() { 
	DoubleLink* tmp = getTailLink();
	if (!tmp) return 0;
	Pointer t = tmp->e;
	INC_LOG_DEL; delete tmp;
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

