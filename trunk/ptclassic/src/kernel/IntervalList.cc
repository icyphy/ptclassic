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
#include "streamCompat.h"
#include "miscFuncs.h"

int Interval::isAfter(const Interval &i1) const { 
	return (pOrigin >= i1.end()); 
}

int Interval::endsBefore(const Interval &i1) const {
	return (end() < i1.pOrigin);
}

int Interval::completelyBefore(const Interval &i1) const {
	return i1.pOrigin == 0? FALSE : (end() < i1.pOrigin-1);
}

int Interval::mergeableWith(const Interval& i1) const {
	return (pOrigin <= i1.end() + 1 &&
		i1.pOrigin <= end() + 1);
}

int Interval::subsetOf(const Interval& i1) const {
	return pOrigin >= i1.pOrigin && end() <= i1.end();
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
	return pOrigin == 0 && pLength == 0? 0 : pOrigin+pLength-1;
}

// I/O functions: read and write intervals.

// write an Interval.
ostream& operator<<(ostream& o,const Interval& ival) {
	if (ival.length() == 0) {
		o << "<empty>";
		return o;
	}
	o << ival.origin();
	if (ival.length() > 1)
		o << "-" << ival.end();
	return o;
}

// Read an Interval.
// an interval either has the form "n" or "m-n".  No spaces are
// allowed between m and "-" in the latter form.  Space is allowed
// AFTER the "-" because it is not ambiguous.

istream& operator>>(istream& s,Interval& ival) {
	char ch;
	if (! (s >> ival.pOrigin)) {
		s.clear(ios::failbit);
		return s;
	}
	// we do not permit whitespace between the first value and
	// a "-" sign.  If we now see a whitespace character, we assume
	// that we have the single-value form.
	if (!s.get(ch)) return s;
	if (ch == '-') {
		unsigned int eval;
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

// Write an IntervalList.
ostream& operator<<(ostream& o,const IntervalList& l) {
	if (l.empty())
		o << "<>";
	CIntervalListIter next(l);
	const Interval* p;
	while ((p = next++) != 0) {
		o << *p;
		p = p->next;
		if (p) o << ",";
	}
	return o;
}

// Read an IntervalList.
// the reading function allows intervals to be specified in any order;
// the interpretation is that we form the union of all intervals specified.
// The resulting list is in the proper internal form (sorted, gaps, no
// null intervals).
// The form is a sequence of comma-separated intervals, e.g.
//
// 1,3-8,10,12-14
//
// No spaces are permitted before a comma; that's how we know when we
// have the end of the IntervalList.  Spaces are permitted before any
// number, but not after.
// We allow the list to be surrounded by "<>".  In this form, spaces
// may appear before commas.

istream& operator>>(istream& s,IntervalList &l) {
	char ch;
	int braceflag = 0;
	Interval ival;

	l.zero();
	if (!s.ipfx(0)) {
		s.clear(ios::failbit);
		return s;
	}

	if (!(s >> ch)) return s;
	if (ch == '<') braceflag++;
	else s.putback(ch);

	if (!(s >> ival)) return s;
	LOG_NEW; l.head = new Interval(ival);
	/*Interval* current = l.head;*/
	if (braceflag) s >> ws;
	if (!s.get(ch)) return s;
	while (ch == ',') {
		s >> ival;
		if (ival.length() == 0) {
			// nothing, ignore.
		}
		else 
			l |= ival;
		if (braceflag) s >> ws;
		if (!s.get(ch)) return s;
	}
	// if we get here, ch is not a comma.  put it back
	if (!braceflag || ch != '>') s.putback(ch);
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
	if (i1.subsetOf(*p))
		return 1;
	else return -1;
}

// add a single interval.
IntervalList& IntervalList::operator|=(const Interval& ival) {
    if (head==NULL)
	head = new Interval(ival);
    else {
	IntervalListIter next(*this);
	Interval* current;
	Interval* last = NULL;
	while ((current=next++) != 0 ) {
	    if (ival.completelyBefore(*current)) {
		if (last!=NULL) last->next = new Interval(ival,current);
		else head = new Interval(ival,current);
		return *this;
	    }
	    else if (ival.mergeableWith(*current)) {
	        current->merge(ival);
		while(current->next!=0&&current->mergeableWith(*current->next)){
		    current->merge(*current->next);
		    Interval* toDel = current->next;
	  	    current->next = current->next->next;
		    LOG_DEL; delete toDel;
		}
		return *this;
	    } 
	    last = current;
	}
	last->next = new Interval(ival); 
    }
    return *this;
}

// Compute the union of two interval lists, replacing the first
// by the union.
IntervalList& IntervalList::operator|=(const IntervalList& toAdd) {
	CIntervalListIter next(toAdd);
	const Interval* p;
	while ((p=next++) != 0)
	    *this |= *p;
	return *this;
}

// subtract an interval from the list
IntervalList& IntervalList::subtract(const Interval& i1) {
	Interval* current = head;
	Interval* prev = 0;
	while (current && current->endsBefore(i1)) {
		prev = current;
		current = current->next;
	}
	// return immediately if no overlap
	if (current == 0 || i1.endsBefore(*current))
		return *this;
	while (1) {
		// delete any intervals on list that are completely contained
		// in i1.
		while (current && current->subsetOf(i1)) {
			if (prev == 0) {
				head = current->next;
				LOG_DEL; delete current;
				current = head;
			}
			else {
				prev->next = current->next;
				LOG_DEL; delete current;
				current = prev->next;
			}
		}
		// return if we are now done, because nothing left or
		// next interval comes after
		if (current == 0 || i1.endsBefore(*current))
			return *this;
		// partial overlap between current node and i1.
		// see if we must split the node in two.
		if (i1.origin() > current->origin() &&
		    i1.end() < current->end()) {
			LOG_NEW; Interval *i = new Interval(i1.end()+1,
						   current->end()-i1.end());
			current->pLength = i1.origin() - current->origin();
			i->next = current->next;
			current->next = i;
		}
		// one-sided overlap, two cases.
		else if (i1.origin() > current->origin())
			current->pLength = i1.origin() - current->origin();
		else {
			unsigned tEnd = current->end();
			current->pOrigin = i1.end() + 1;
			current->pLength = tEnd - current->pOrigin + 1;
		}
		prev = current;
		current = current->next;
		// continue because next interval may overlap or
		// be contained in i1.
	}
}

// subtraction of a list (could be faster)
IntervalList& IntervalList::operator-=(const IntervalList& toSub) {
	const Interval* p = toSub.head;
	while (p) {
		subtract(*p);
		p = p->next;
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

// constructor reads an argString from a string using stream functions.
// We must copy the argString because istrstream wants a char (and
// presumably putback writes stuff back into the buffer?)

IntervalList::IntervalList(const char* argString) : head(0) {
	const int BUFLEN = 80;
	char buf[BUFLEN], *p = buf;
	int l = strlen(argString);
	if (l >= BUFLEN) p = savestring(argString);
	else strcpy(buf,argString);
	istrstream strm(p, l);
	strm >> *this;
	if (l >= BUFLEN) {
		LOG_DEL; delete p;
	}
}
