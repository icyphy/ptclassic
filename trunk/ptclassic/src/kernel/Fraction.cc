#include "Fraction.h"
#include "StringList.h"

// SCCS version identification
// $Id$

/*

 Copyright (c) 1989 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Edward A. Lee
 Date: 12/7/89

 This class represents fractions using integers for the numerator
 and denominator.

 Rewritten by Joe Buck to implement assignment operators and to be
 more efficient.

 This file contains the member function definitions.

*/

Fraction&
Fraction:: operator += (const Fraction& a)
{
    numerator = numerator * a.denominator + a.numerator * denominator;
    denominator *= a.denominator;
    return *this;
}

Fraction&
Fraction::operator -= (const Fraction& a)
{
    numerator = numerator * a.denominator - a.numerator * denominator;
    denominator *= a.denominator;
    return *this;
}

Fraction&
Fraction::operator *= (const Fraction& a)
{
    numerator *= a.numerator;
    denominator *= a.denominator;
    return *this;
}

Fraction&
Fraction::operator /= (const Fraction& a)
{
    numerator *= a.denominator;
    denominator *= a.numerator;
    return *this;
}

int operator == (const Fraction& i, const Fraction& j)
{
    return ((i.numerator == j.numerator) && (i.denominator == j.denominator));
}

Fraction :: operator StringList () const
{
	StringList out;
	out = numerator;
	out += "/";
	out += denominator;
	out += "\n";
	return out;
}

// This routine simplifies a Fraction so that its numerator and denominator
// are relatively prime.

void Fraction::simplify ()
{
	// by convention, if the numerator is 0, the denominator is set to 1
	if (numerator == 0) denominator = 1;
	else {
	  // find the greatest common divisor of the numerator and denominator
	  GcdLcm d(*this);

	  numerator = numerator / d.gcd;
	  denominator = denominator / d.gcd;
	}
}


/********************************************************************
			computeGcdLcm

This routine computes and returns the greatest common divisor of two
integers numerator and denominator in the class Fraction.
It uses Euclid's algorithm, as described in Richard Blahut,
"Fast Algorithms for Digital Signal Processing", Addison-Wesley, 1985.

The technique used is to update the matrix A(r), where
           | 0      1 |
    A(r) = |          | A(r-1)
           | 1  -q(r) |
Where
    q(r) = floor ( s(r-1) / t(r-1))
And
    | s(r) |        |  numerator  |
    |      | = A(r) |             |
    | t(r) |        | denominator |

Begin with r=1, s(0)=abs(numerator), t(0)=abs(denominator), A(0)=I.
Terminate when t(r)=0, and return (s(r), A21(r), A22(r)).

This is implemented as a constructor for GcdLcm.

*/

GcdLcm::GcdLcm(const Fraction& f)
{
	// initialize the matrix to the identity
	int a12 = 0;
	int a21 = 0;
	int a11 = 1;
	int a22 = 1;

	int s0 = abs(f.numerator);
	int t0 = abs(f.denominator);

	// initialize the iteration vector
	int sr = s0;
	int tr = t0;

	int qr,tmp1;

	while (tr != 0) {

		// update the quotient
		qr = sr/tr;

		// update the matrix
		tmp1 = a22;
		a22 = a12 - qr*tmp1;
		a12 = tmp1;
		tmp1 = a21;
		a21 = a11 - qr*tmp1;
		a11 = tmp1;

		// update the vector
		sr = a11*s0 + a12*t0;
		tr = a21*s0 + a22*t0;
	}

	gcd = sr;
	lcm = abs(a21)*s0;
	// abs(a21)*s0 should also equal abs(a22)*t0
}
