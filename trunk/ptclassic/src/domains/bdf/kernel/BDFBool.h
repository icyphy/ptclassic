#ifndef _BDFBool_h
#define _BDFBool_h 1
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-1994 The Regents of the University of California.
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

 Programmer:  J. T. Buck
 Date of creation: 1/8/91

 Special Boolean expression classes for use with BDF.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "PortHole.h"

class StringList;

// a BoolSignal represents a single boolean signal, which may be negated.
struct BoolSignal {
	friend int operator==(const BoolSignal& a, const BoolSignal& b);
	friend StringList& operator+=(StringList&, const BoolSignal&);
	friend class BoolTerm;
private:
	int negated;
	BoolSignal* link;
public:
	const PortHole& p;
	int neg() const { return negated;}
	BoolSignal(const PortHole& ph, int n, BoolSignal* lnk = 0) :
		negated(n), link(lnk), p(ph) {}
	BoolSignal(const BoolSignal& a) :
	        negated(a.negated), link(0), p(a.p) {}
	BoolSignal(const BoolSignal& a, BoolSignal* lnk) :
		negated(a.negated), link(lnk), p(a.p) {}
	const BoolSignal* next() const { return link;}
};

inline int operator==(const BoolSignal& a, const BoolSignal& b) {
	return &(a.p) == &(b.p) && a.negated == b.negated;
}

// a BoolTerm represents the product of a constant integer and an indefinite
// number of BoolSignal terms.
class BoolTerm {
	friend int operator == (const BoolTerm& a, const BoolTerm& b);
private:
	BoolSignal* bList;
	BoolTerm& fastAdd(const BoolSignal& term) {
		INC_LOG_NEW; bList = new BoolSignal(term,bList);
		return *this;
	}
	void copy(const BoolTerm&);
public:
	int constTerm;	// allow this to be written directly.
	int pureNumber() const { return !bList;}
	const BoolSignal* list() const { return bList;}
	BoolTerm& add(const BoolSignal&);
	int elim(const BoolSignal&);
	int member(const BoolSignal&) const;
	int length() const;
	void zerofy();
	int contradiction() const;
	BoolTerm& lcm(const BoolTerm&);
	BoolTerm(int i=1) : bList(0), constTerm(i) {}
	BoolTerm(int i,const BoolSignal& term) : constTerm(i) {
		INC_LOG_NEW; bList = new BoolSignal(term);
	}
	BoolTerm(const BoolTerm& arg) { copy(arg);}
	BoolTerm& operator=(const BoolTerm& arg) {
		if (this != &arg) {
			zerofy();
			copy(arg);
		}
		return *this;
	}
	~BoolTerm() {
		zerofy();
	}
	BoolTerm& operator *= (const BoolTerm&);
};

// A BoolFraction formally represents the ratio of two BoolTerms.  This
// representation is only formal since the fraction would be undefined
// if any contained BoolSignal in the denominator is false, but for our
// purposes this doesn't matter.
class BoolFraction {
private:
	BoolTerm numerator;
	BoolTerm denominator;
public:
	// constructors
	BoolFraction(int i = 0) : numerator(i), denominator(1) {}
	BoolFraction(const BoolTerm& n, const BoolTerm& d) :
		numerator(n), denominator(d) {}
	BoolFraction(const BoolFraction& f) :
		numerator(f.numerator), denominator(f.denominator) {}
	BoolFraction& operator = (const BoolFraction& f) {
		numerator = f.numerator;
		denominator = f.denominator;
		return *this;
	}
	// const forms
	const BoolTerm& num() const { return numerator;}
	const BoolTerm& den() const { return denominator;}
	// non-const forms
	BoolTerm& num() { return numerator;}
	BoolTerm& den() { return denominator;}

	BoolFraction& simplify();
	BoolFraction& add(const BoolSignal& s) {
		numerator.add(s); return *this;
	}
	BoolFraction& operator*= (const BoolFraction& arg) {
		numerator *= arg.numerator;
		denominator *= arg.denominator;
		return *this;
	}
	BoolFraction& operator*= (const BoolTerm& arg) {
		numerator *= arg;
		return *this;
	}
	BoolFraction& operator/= (const BoolFraction& arg) {
		numerator *= arg.denominator;
		denominator *= arg.numerator;
		return *this;
	}
};

inline BoolFraction operator * (const BoolFraction& a, const BoolFraction& b) {
	BoolFraction tmp(a);
	return tmp *= b;
}

inline BoolFraction operator / (const BoolFraction& a, const BoolFraction& b) {
	BoolFraction tmp(a);
	return tmp /= b;
}

// equality operators
int operator == (const BoolTerm& a, const BoolTerm& b);

// IMPORTANT!  You must call simplify() before testing two BoolFractions
// for equality!
inline int operator == (const BoolFraction& a, const BoolFraction& b) {
	return a.num() == b.num() && a.den() == b.den();
}

// methods for printing these classes as strings
StringList& operator+=(StringList&, const BoolTerm&);
StringList& operator+=(StringList&, const BoolFraction&);

// methods for printing on ostream
class ostream;
ostream& operator<<(ostream&, const BoolTerm&);
ostream& operator<<(ostream&, const BoolFraction&);

#endif
