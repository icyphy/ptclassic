static const char file_id[] = "BDFBool.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 1/8/91

// methods for BoolTerm and BoolFraction.

*****************************************************************/
#include "BDFBool.h"
#include "StringList.h"
#include <stream.h>

// return true if the BoolSignal occurs in the BoolTerm.
int BoolTerm::member (const BoolSignal& term) const {
	const BoolSignal* l = bList;
	while (l) {
		if (*l == term) return TRUE;
		l = l->link;
	}
	return FALSE;
}

// add in the BoolSignal if it does not appear.
BoolTerm& BoolTerm::add (const BoolSignal& term) {
	if (!member(term))
		return fastAdd (term);
	else return *this;
}

// eliminate the BoolSignal from the BoolTerm; return true if found.
// if not found, return 0 and do nothing.
int BoolTerm::elim (const BoolSignal& term) {
	if (!bList) return 0;
	if (*bList == term) {
		BoolSignal* t = bList;
		bList = t->link;
		LOG_DEL; delete t;
		return 1;
	}
	BoolSignal* p = bList;
	BoolSignal* q = p->link;
	while (q) {
		if (*q == term) {
			p->link = q->link;
			LOG_DEL; delete q;
			return 1;
		}
		p = q;
		q = q->link;
	}
	return 0;
}

// find the gcd of two integers.
static int gcd(int a, int b) {
	if (a < b) {
		int t = a; a = b; b = t;
	}
	// now b is less.  Check for zero
	if (b <= 0) return 0;
	while (1) {
		int rem = a % b;
		if (rem == 0) return b;
		a = b;
		b = rem;
	}
}

// determine whether a contradiction is present in a BoolTerm
// (meaning that the BoolTerm is always false).
int BoolTerm::contradiction() const {
	const BoolSignal* q = list();
	while (q) {
		BoolSignal b(q->p, !q->negated);
		if (member (b)) return TRUE;
		q = q->link;
	}
	return FALSE;
}

// convert a BoolTerm to zero, by zapping all the BoolSignals and setting
// the leading term to zero.
void BoolTerm::zerofy() {
	BoolSignal* t;
	while (bList) {
		t = bList;
		bList = t->link;
		LOG_DEL; delete t;
	}
	constTerm = 0;
}

// Find the lcm of two BoolTerms.
BoolTerm&
BoolTerm::lcm(const BoolTerm& arg) {
	// make constant term the lcm of the two
	int g = gcd(constTerm, arg.constTerm);
	constTerm *= arg.constTerm / g;
	// add in terms from the argument
	for (BoolSignal *q = arg.bList; q; q = q->link)
		add(*q);
	return *this;
}

// Product of two BoolTerms.
BoolTerm&
BoolTerm::operator *= (const BoolTerm& arg) {
	for (const BoolSignal* q = arg.bList; q; q = q->link)
		add (*q);
	constTerm *= arg.constTerm;
	return *this;
}

// length of a BoolTerm.
int BoolTerm::length() const {
	int cnt = 0;
	for (BoolSignal*q = bList; q; q = q->link) cnt++;
	return cnt;
}
	
// copy constructor "guts" (also used in assignment op)
void BoolTerm::copy(const BoolTerm& arg) {
	constTerm = arg.constTerm;
	bList = 0;
	// add each term to the new list
	for (BoolSignal*q = arg.bList; q; q = q->link)
		fastAdd (*q);
}

// equality operator
int operator == (const BoolTerm& a, const BoolTerm& b) {
	if (a.constTerm != b.constTerm) return 0;
	if (a.length() != b.length()) return 0;
	for (const BoolSignal* q = a.bList; q; q = q->next()) {
		if (!b.member(*q)) return 0;
	}
	return 1;
}

// simplify a BoolFraction
BoolFraction& BoolFraction::simplify() {
	// eliminate all common terms from numerator and denominator
	const BoolSignal* q = den().list();
	while (q) {
		// order here is important, since list may change
		const BoolSignal& term = *q;
		q = q->next();
		if (num().elim(term)) den().elim(term);
	}
	// reduce integer parts
	int g = gcd (num().constTerm, den().constTerm);
	if (g > 1) {
		num().constTerm /= g;
		den().constTerm /= g;
	}
// check for "contradiction" (BoolTerm contains contradictory terms) in
// numerator, or leading term of 0.  If so, turn fraction into 0/1.
	if (num().constTerm == 0 || num().contradiction()) {
		num().zerofy();
		den().zerofy();
		den().constTerm = 1;
	}
	return *this;
}

// print a BoolSignal
StringList& operator+=(StringList& msg, const BoolSignal& b) {
	if (b.negated) msg += "!";
	msg += b.p.fullName();
	return msg;
}

// print a BoolTerm
StringList& operator+=(StringList& msg, const BoolTerm& t) {
	if (t.pureNumber()) msg += t.constTerm;
	else {
		if (t.constTerm != 1) {
			msg += t.constTerm;
			msg += "*";
		}
		const BoolSignal *s = t.list();
		while (s) {
			msg += *s;
			s = s->next();
			if (s) msg += "&";
		}
	}
	return msg;
}

// print a BoolFraction
StringList& operator+=(StringList& msg, const BoolFraction& f) {
	msg += f.num();
	if (f.den().constTerm == 1 && f.den().pureNumber()) return msg;
	msg += "/";
	msg += f.den();
	return msg;
}

// print to an ostream
ostream& operator<<(ostream& o, const BoolTerm& t) {
	StringList q;
	q += t;
	return o << q;
}

ostream& operator<<(ostream& o, const BoolFraction& f) {
	StringList q;
	q += f;
	return o << q;
}

