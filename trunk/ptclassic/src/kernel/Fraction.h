
#ifndef _Fraction_h
#define _Fraction_h 1


// SCCS version identification
// $Id$

/*

 Copyright (c) 1990 The Regents of the University of California.
			All Rights Reserved.
 Programmer:  Edward A. Lee and J. Buck
 Date: 12/7/89

 This class represents fractions using integers for the numerator
 and denominator.

 Thoroughly rewritten by Joe Buck to be more efficient, implement
 assignment operators, and to work around some g++ bugs.

*/
#ifdef __GNUG__
#pragma interface
#endif

#include "StringList.h"

// The following class is defined so that Euclid's algorithm
// can return both the least common multiple and greatest common
// divisor.

// The only way to build a GcdLcm is to start with a fraction: the
// constructor builds one.

// I changed data members back to public to support the ugly SDF scheduler,
// which insists on violating information hiding.
class GcdLcm {
	friend class Fraction;
public:
	int gcd;
	int lcm;
	GcdLcm(const Fraction& f);
};

class Fraction
{
	friend class GcdLcm;
public:
	int numerator;
	int denominator;
	// Constructors
	Fraction () : numerator(0), denominator(1) { }

	Fraction (int i) : numerator(i), denominator(1) { }

	Fraction (int i, int j) : numerator(i), denominator(j) { }

	// Copy constructor
	Fraction (const Fraction& a) : numerator(a.numerator),
				       denominator(a.denominator) {}

	// Cast to type double:
	operator double() const {
		return double(numerator)/double(denominator);
	}

	// Assignment operators
	Fraction& operator += (const Fraction& f);
	Fraction& operator -= (const Fraction& f);
	Fraction& operator *= (const Fraction& f);
	Fraction& operator /= (const Fraction& f);

	// Equality test operator
	// This operator assumes the fractions have been simplified!!
	friend int operator == (const Fraction&,const Fraction&);

	// Print operator
	operator StringList () const;

	// Simplify the fraction (compute an equivalent fraction with
	// numerator and denominator relatively prime).
	// The simplified fraction replaces the original.
	void simplify();

	// Compute gcd,lcm of numerator, denominator
	GcdLcm computeGcdLcm() const { return GcdLcm(*this);}
};

// Notice: these functions don't have to be friends!
// Design based on Andrew Koenig's method -- the temporaries
// (res in the functions below) will almost always be optimized away

inline Fraction operator+ (const Fraction& a,const Fraction& b) {
	Fraction res(a);
	res += b;
	return res;
}

inline Fraction operator- (const Fraction& a,const Fraction& b) {
	Fraction res(a);
	res -= b;
	return res;
}

inline Fraction operator* (const Fraction& a,const Fraction& b) {
	Fraction res(a);
	res *= b;
	return res;
}

inline Fraction operator/ (const Fraction& a,const Fraction& b) {
	Fraction res(a);
	res /= b;
	return res;
}

#endif
