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
represented by the origin and the length of the interval.

**************************************************************************/
#include "IntervalList.h"

int Interval::isBefore(Interval* i1) { 
	return (origin <= end() < i1->origin); 
}

int Interval::isAfter(Interval* i1) { 
	return (origin >= i1->end()); 
}

int Interval::endsBefore(Interval *i1) {
	return (end() < i1->origin);
}

int Interval::extendsBefore(Interval *i1) { 
	return (i1->origin <= end() <= i1->end());
}

int Interval::extendsAfter(Interval *i1) {
	return (i1->origin <= origin <= i1->end()) ;
}

int Interval::contains(Interval *i1) {
	return (i1->origin <= origin && i1->end() <= end());
}

int Interval::doesIntersect(Interval *i1) {
	return ( (i1->origin < end() <= i1->end()) ||
		 (i1->origin <= origin < i1->end()) );
}

Interval& Interval::operator=(Interval *il) {
	origin = i1->origin;
	length = i1->length;
	next = i1->next;
	return next;
}
	
Interval& Interval::operator&=(Interval *i1) {
	unsigned end = min(i1->end(),end());
	origin = max(origin,i1->origin);
	length = max(end-origin,0);
	return this;
}

Interval Interval::operator&(Interval* i1,Interval* i2) {
	Interval intersection = Interval(i1->;
	i1 &= i2;
	return intersection;
}

unsigned Interval::end() { 
	return origin+length;
}

Interval* Interval::operator++ () {
	return next;
}

Interval::~Interval() {
	LOG_DEL; delete this;
}

	//////////////////////////////////////
	// class IntervalList
	//////////////////////////////////////

StringList IntervalList::print () {
	Interval* p = head;
	StringList out = "";
	while (p) {
		out += " [";
		out += int(p->origin);
		out += ":";
		out += int(p->origin+p->length-1);
		out += "]";
		p = p->next;
	}
	return out;
}

// copy constructor
void IntervalList::copy(const IntervalList& src) {
	Interval* q = src.head;
	LOG_NEW; head = new Interval(q->origin,q->length);
	Interval* p = head;
	while (q++) {
		LOG_NEW; p->next = new Interval(q->origin,q->length);
		p++;
	}
	p->next = 0;
}

void IntervalList::zero() {
	Interval* p = head->next;
	Interval* q = head;
	while (q) {
		p = q->next;
		q.~Interval();
		q = p;
	}
}

IntervalList& IntervalList::operator|=(unsigned origin, unsigned length) {
	Interval* current = this->head;
	Interval* previous = NULL;
	Interval* input;
	LOG_NEW; input = new Interval(origin,length);
	while (input->isAfter(current) && current != NULL) {
		previous = current;
		current++;
	}
	if (current == NULL) previous->next = input;
	else {	
		if (previous == NULL) {
			input->next = head;
			head = input;
		}
		else {
			previous->next = input;
			input->next = current;
		}
		while (!input->endsBefore(current) && current != NULL) {
			input->length = 
				max(input->end(),current->end())-origin;
			input->next = current->next;
			current->~Interval();
			current = input->next;
		}
	}	
	return this;
}
 
IntervalList& IntervalList::operator|=(IntervalList& src) {
	Interval* current==src->head;
	while (current++ != NULL) this |= current;
	return this;
}

IntervalList operator|(IntervalList& src1,IntervalList& src2) {
	IntervalList union = src1;
	union |= src2;
	return union;
}
