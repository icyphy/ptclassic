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
represented by the origin and the length of the interval.

**************************************************************************/
#ifndef _IntervalList_h
#define _IntervalList_h 1

#include "type.h"
#include "StringList.h"
 

	//////////////////////////////////////
	// class Interval
	//////////////////////////////////////

class Interval {
	friend class IntervalList;
	unsigned origin, length;
	Interval* next;
	Interval(unsigned o=0, unsigned l=0, Interval* nxt = NULL) :
		origin(o), length(l), next(nxt) {};
	Interval(Interval* i1) : origin(i1->origin), length(i1->length), 
		next(i1->next) {};
	Interval(Interval& i1) : origin(i1.origin), length(i1.length), 
		next(i1.next) {};
	// isAfter returns true if current interval begins after
	// interval i1.
	int isAfter(Interval* i1);

	// isBefore returns true if current interval comes before
	// interval i1.
	int isBefore(Interval* i1);

	// endsBefore returns true if current interval ends before
	// the origin of interval i1.
	int endsBefore(Interval *i1);

	// extendsBefore returns true if the current interval
	// can be be 'fused' to the beginning of interval i1.
	// Two intervals can be fused together only if the intervals
	// are right next to each other or they intersect.
	int extendsBefore(Interval *i1);

	// extendsAfter returns true if the current interval
	// can be 'fused' to the end of interval i1.
	int extendsAfter(Interval *i1);

	// contains returns true if the current interval contains i1.
	int contains(Interval*);
	
	// doesIntersect returns true if the current interval intersects i1.
	int doesIntersect(Interval*);

	// return the end location of the interval + 1.
	unsigned end();

	// return pointer to next interval, if no more intervals return NULL
	Interval* operator++ ();

	Interval& operator=(Interval*);
	Interval& operator&=(Interval*);
	friend Interval operator&(Interval&, Interval&);

	~Interval();
};

	//////////////////////////////////////
	// class IntervalList
	//////////////////////////////////////

class IntervalList
{
	Interval* head;

	// body of copy constructor
	void copy(const IntervalList& src);

	// Remove all Intervals.
	void zero();

public:
 	IntervalList(unsigned origin,unsigned length) {
		INC_LOG_NEW; head = new Interval(origin,length);
	}

	// copy constructor
	IntervalList(IntervalList& src) { copy(src);}

	// assignment operator
	IntervalList& operator=(const IntervalList& src) {
		zero();
		copy(src);
		return *this;
	}

	// destructor
	~IntervalList() { zero();}

	// Take the union of two interval lists.
	friend IntervalList operator|(IntervalList&,IntervalList&);

	// Add a new interval to the interval list.
	IntervalList& operator|=(Interval *i1);

	// Set current IntervalList to the union of itself and src 
	IntervalList& operator|=(IntervalList& src);

	// print (debug)
	StringList print();
};

#define MAX(A, B)	((A) > (B) ? (A) : (B))
#define MIN(A, B)	((A) < (B) ? (A) : (B))

#endif
