static const char file_id[] = "IntervalList.cc";
/**************************************************************************
Version identification:
$Id$
 
 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. Buck, E. A. Lee and J. Pino
 Date of creation: 3/3/92

The interval list class supports an ordered unsigned interval list.
When a new interval is added to the list, it is merged to a predefined
interval if possible or added as a new interval.  Each interval is
internally represented by the origin and the length of the interval.

Intervals on an interval list are always sorted in increasing order,
and they always have gaps between them.

**************************************************************************/
#include "IntervalList.h"
#include <minmax.h>
#include <iostream.h>

int Interval::isAfter(const Interval &i1) const { 
	return (pOrigin >= i1.end()); 
}

int Interval::endsBefore(const Interval &i1) const {
	return (end() < i1.pOrigin);
}

int Interval::completelyBefore(const Interval &i1) const {
	return (end() < i1.pOrigin-1);
}

int Interval::mergeableWith(const Interval& i1) const {
	return (pOrigin <= i1.end() + 1 &&
		i1.pOrigin <= end() + 1);
}

// form the union of the two intervals, assuming that they overlap or
// adjoin (otherwise everything in between is included as well)
void Interval::merge(const Interval& i1) {
	unsigned nEnd = max(end(),i1.end());
	pOrigin = min(pOrigin,i1.pOrigin);
	pLength = nEnd - pOrigin + 1;
}

int Interval::intersects(const Interval& i1) const {
	return (pOrigin <= i1.end() &&
		i1.pOrigin <= end());
}

Interval& Interval::operator=(const Interval& i1) {
	pOrigin = i1.pOrigin;
	pLength = i1.pLength;
	next = NULL;
	return *this;
}
	
Interval& Interval::operator&=(const Interval& i1) {
	unsigned endv = min(i1.end(),end());
	pOrigin = max(pOrigin,i1.pOrigin);
	if (endv >= pOrigin)
		pLength = endv-pOrigin+1;
	else pLength = 0;
	return *this;
}

Interval operator&(const Interval& i1,const Interval& i2) {
	Interval intersection(i1);
	intersection &= i2;
	return intersection;
}

unsigned Interval::end() const { 
	return pOrigin+pLength-1;
}

// I/O functions: read and write intervals.

ostream& operator<<(ostream& o,const Interval& ival) {
	if (ival.length() <= 0)
		o << "<empty>";
	o << ival.origin();
	if (ival.length() > 1)
		o << "-" << ival.end();
	return o;
}

istream& operator>>(istream& s,Interval& ival) {
	char ch;
	if (! (s >> ival.pOrigin)) {
		s.clear(ios::failbit);
		return s;
	}
	s >> ws;
	s >> ch;
	if (ch == '-') {
		int eval;
		s >> eval;
		if (eval < ival.pOrigin)
			ival.pLength = 0;
		else ival.pLength = eval - ival.pOrigin + 1;
	}
	else {
		s.putback(ch);
		ival.pLength = 1;
	}
	return s;
}
		
	//////////////////////////////////////
	// class IntervalList
	//////////////////////////////////////

// I/O functions.

ostream& operator<<(ostream& o,const IntervalList& l) {
	o << "[";
	CIntervalListIter next(l);
	const Interval* p;
	while ((p = next++) != 0) {
		o << *p;
		p = p->next;
		if (p) o << ",";
	}
	o << "]";
	return o;
}

// the reading function allows intervals to be specified in any order;
// the interpretation is that we form the union of all intervals specified.
// The resulting list is in the proper internal form (sorted, gaps, no
// null intervals)
istream& operator>>(istream& s,IntervalList &l) {
	char ch;
	Interval ival;
	l.zero();
	if (!s.ipfx(0)) {
		s.clear(ios::failbit);
		return s;
	}
	s >> ws;
	s.get(ch);
	if (ch == '[') {
		if (!(s >> ival)) return s;
		LOG_NEW; l.head = new Interval(ival);
		Interval* current = l.head;
		s >> ws;
		s.get(ch);
		while (ch == ',') {
			s >> ival;
			if (ival.length() == 0) {
				// nothing, ignore.
			}
			else if (ival.completelyBefore(*current)) {
				// specified out of order, use union operator
				// to insert starting at head
				l |= ival;
			}
			else if (ival.mergeableWith(*current))
				current->merge(ival);
			else {
				LOG_NEW; current->next = new Interval(ival);
				current = current->next;
			}
			s >> ws;
			s.get(ch);
		}
		if (ch != ']') {
			s.putback(ch);
			s.clear(ios::failbit);
		}
	}
// fix up head slot: if it's null, delete it and attach the next one.
	if (l.head->length() == 0) {
		Interval* p = l.head;
		l.head = p->next;
		LOG_DEL; delete p;
	}
	return s;
}

// some constructors
IntervalList::IntervalList(unsigned origin,unsigned length) {
	if (length > 0) {
		LOG_NEW; head = new Interval(origin,length);
	}
	else head = 0;
}

// copy constructor body.  Important: if head already points to some Interval
// objects, they won't be deleted by this function.
void IntervalList::copy(const IntervalList& src) {
	Interval* q = src.head;
	if (!q) {
		head = 0;
		return;
	}
	LOG_NEW; head = new Interval(*q);
	Interval* p = head;
	q = q->next;
	while (q) {
		LOG_NEW; p->next = new Interval(*q);
		p = p->next;
		q = q->next;
	}
}

// destructor body: all Interval objects are deleted.
void IntervalList::zero() {
	Interval* p;
	Interval* q = head;
	while (q) {
		p = q->next;
		LOG_DEL; delete q;
		q = p;
	}
}

// Determine membership of Interval i1.  Return 0 for no overlap,
// 1 for complete inclusion, -1 for partial inclusion.
int IntervalList::contains(const Interval& i1) const {
	const Interval* p = head;
	while (p && p->endsBefore(i1)) p = p->next;
	if (p == 0 || i1.endsBefore(*p)) return 0;
	// neither ends before the other, we have overlap.
	// see if it is complete.
	if (p->origin() <= i1.origin() && p->end() >= i1.end())
		return 1;
	else return -1;
}

// add a single interval.
IntervalList& IntervalList::operator|=(const Interval& i1) {
	IntervalList toAdd(i1.origin(),i1.length());
	return *this |= toAdd;
}

// Compute the union of two interval lists, replacing the first
// by the union.
IntervalList& IntervalList::operator|=(const IntervalList& toAdd) {
	if (head == 0) return (*this = toAdd);
	const Interval* addList = toAdd.head;
	if (addList == 0) return *this;
	// special case: toAdd first node completely precedes list.
	if (addList->completelyBefore(*head)) {
		LOG_NEW; head = new Interval(*addList,head);
		addList = addList->next;
	}
	Interval* current = head;
	// initial condition: first node on addList overlaps or follows
	// first node on current.  Advance current until *addList is
	// either between current and current->next, or overlaps with
	// one or the other of them.
	while (addList) {
		while (current->next &&
		       current->next->completelyBefore(*addList))
			current = current->next;
		if (current->mergeableWith(*addList)) {
			current->merge(*addList);
			if (current->next &&
			    current->next->mergeableWith(*current)) {
				Interval* p = current->next->next;
				current->merge(*current->next);
				LOG_DEL; delete current->next;
				current->next = p;
			}
		}
		else if (current->next &&
			 current->next->mergeableWith(*addList)) {
			current->next->merge(*addList);
		}
		else {
			LOG_NEW; current->next = new Interval(*addList,current->next);
		}
		addList = addList->next;;
	}
	return *this;
}

// intersection.
IntervalList IntervalList::operator&(const IntervalList& bL) const {
	IntervalList tmp;
	const Interval* a = head;
	const Interval* b = bL.head;
	// need to get head node first
	while (a && b && !a->intersects(*b)) {
		if (a->endsBefore(*b)) a = a->next;
		else if (b->endsBefore(*a)) b = b->next;
	}
	if (a == 0 || b == 0) return tmp;
	LOG_NEW; tmp.head = new Interval(*a);
	*(tmp.head) &= *b;
	Interval* out = tmp.head;
	while (1) {
		if (a->end() <= b->end()) a = a->next;
		else b = b->next;
		while (a && b && !a->intersects(*b)) {
			if (a->endsBefore(*b)) a = a->next;
			else if (b->endsBefore(*a)) b = b->next;
		}
		if (a == 0 || b == 0) return tmp;
		LOG_NEW; out->next = new Interval(*a);
		*(out->next) &= *b;
		out = out->next;
	}
}
