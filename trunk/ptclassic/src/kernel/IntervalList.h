/**************************************************************************
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

 Programmer:  J. Buck, E. A. Lee and J. Pino
 Date of creation: 3/3/92

The interval list class supports an ordered unsigned interval list.
When a new interval is added to the list, it is merged to a predefined
interval if possible or added as a new interval.  Each interval is
represented by the origin and the length of the interval.

The Interval objects in an IntervalList are always sorted in increasing
order, and there are always gaps between intervals.

**************************************************************************/
#ifndef _IntervalList_h
#define _IntervalList_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "type.h"
 
class istream;
class ostream;

	//////////////////////////////////////
	// class Interval
	//////////////////////////////////////

class Interval {
	friend class IntervalList;
	friend class IntervalListIter;
	friend class CIntervalListIter;

	// i/o functions
	friend istream& operator>>(istream&,Interval&);
	friend ostream& operator<<(ostream&,const Interval&);
	friend istream& operator>>(istream&,IntervalList&);
	friend ostream& operator<<(ostream&,const IntervalList&);


	// implementation
	unsigned pOrigin, pLength;
	Interval* next;
public:
	Interval(unsigned o=0, unsigned l=0, Interval* nxt = 0) :
		pOrigin(o), pLength(l), next(nxt) {}
	Interval(const Interval& i1) : pOrigin(i1.pOrigin),
		pLength(i1.pLength), next(0) {}
	Interval(const Interval& i1,Interval* nxt) :
		pOrigin(i1.pOrigin), pLength(i1.pLength), next(nxt) {}

	unsigned origin() const { return pOrigin;}
	unsigned length() const { return pLength;}

	// isAfter returns true if current interval begins after
	// interval i1.
	int isAfter(const Interval &i1) const;

	// endsBefore returns true if current interval ends before
	// the pOrigin of interval i1.
	int endsBefore(const Interval &i1) const;

	// completelyBefore returns true if endsBefore is true and there
	// is space between the intervals (they cannot be merged)
	int completelyBefore(const Interval &i1) const;

	// mergeableWith returns true if two intervals overlap or are
	// adjacent, so that their union is also an interval.
	int mergeableWith(const Interval& i1) const;

	// intersects returns true if two intervals have a non-null
	// intersection.
	int intersects(const Interval& i1) const;

	// subsetOf returns true if the argument is a subset of me.
	int subsetOf(const Interval& i1) const;

	// merge alters the interval to the result of merging it with i1.
	void merge(const Interval& i1);

	// return the end location of the interval.
	unsigned end() const;

	Interval& operator=(const Interval&);
	Interval& operator&=(const Interval&);
};

Interval operator&(const Interval&, const Interval&);

	//////////////////////////////////////
	// class IntervalList
	//////////////////////////////////////

class IntervalList
{
	friend class IntervalListIter;
	friend class CIntervalListIter;

	// I/O functions
	friend istream& operator>>(istream&,IntervalList&);
	friend ostream& operator<<(ostream&,const IntervalList&);

	// Remove all Intervals.
	void zero();

	// body of copy constructor
	void copy(const IntervalList& src);

protected:
	Interval* head;
	
public:
	IntervalList() : head(0) {}

 	IntervalList(unsigned origin,unsigned length);

	// copy constructor
	IntervalList(const IntervalList& src) { copy(src);}

	// constructor that takes a string value and parses it
	IntervalList(const char* definition);

	// assignment operator
	IntervalList& operator=(const IntervalList& src) {
		zero();
		copy(src);
		return *this;
	}

	// destructor
	~IntervalList() { zero();}

	// contains: return 0 if no part of i1 is in the IntervalList,
	// 1 if i1 is completely contained in the IntervalList, and
	// -1 if i1 is partially contained (has a non-null intersection)
	int contains(const Interval& i1) const;

	// Add a new interval to the interval list.
	IntervalList& operator|=(const Interval& i1);

	// Set current IntervalList to the union of itself and src 
	IntervalList& operator|=(const IntervalList& src);

	// intersection (nondestructive)
	IntervalList operator&(const IntervalList&) const;

	// subtract an interval from the list
	IntervalList& subtract(const Interval& i1);

	// alternate name
	IntervalList& operator-=(const Interval& i1) {
		return subtract(i1);
	}

	// subtract an IntervalList from the list
	IntervalList& operator-=(const IntervalList &toSub);

	// predicate for empty list
	int empty() const { return (head == 0);}
};

// difference of two interval lists.
inline IntervalList operator-(const IntervalList& a,const IntervalList& b) {
	IntervalList tmp(a);
	return tmp -= b;
}

// Take the union of two interval lists.
inline IntervalList operator|(const IntervalList& a,const IntervalList& b) {
	IntervalList tmp(a);
	return tmp |= b;
}

class IntervalListIter {
private:
	IntervalList& ref;
	Interval* p;
public:
	IntervalListIter(IntervalList& list) : ref(list),p(list.head) {}
	void reset() { p = ref.head;}
	Interval* next() {
		Interval* tmp = p;
		if (p) p = p->next;
		return tmp;
	}
	Interval* operator++(POSTFIX_OP) { return next();}
};

class CIntervalListIter {
private:
	const IntervalList& ref;
	Interval* p;
public:
	CIntervalListIter(const IntervalList& list)
		: ref(list),p(list.head) {}
	void reset() { p = ref.head;}
	const Interval* next() {
		const Interval* tmp = p;
		if (p) p = p->next;
		return tmp;
	}
	const Interval* operator++(POSTFIX_OP) { return next();}
};


#endif
