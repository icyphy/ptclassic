#include "Fraction.h"

/*

 Copyright (c) 1989 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Edward A. Lee
 Date: 12/7/89

 This class represents fractions using integers for the numerator
 and denominator.

 This file contains the member function definitions.

*/

# include <stream.h>	// needed only for the print function

Fraction operator + (Fraction i, Fraction j)
{
    Fraction temp;

    temp.numerator = i.numerator * j.denominator + j.numerator * i.denominator;
    temp.denominator = i.denominator * j.denominator;

    return (temp);
}

Fraction operator - (Fraction i, Fraction j)
{
    Fraction temp;

    temp.numerator = i.numerator * j.denominator - j.numerator * i.denominator;
    temp.denominator = i.denominator * j.denominator;

    return (temp);
}

Fraction operator * (Fraction i, Fraction j)
{
    Fraction temp;

    temp.numerator = i.numerator * j.numerator;
    temp.denominator = i.denominator * j.denominator;

    return (temp);
}

Fraction operator / (Fraction i, Fraction j)
{
    Fraction temp;

    temp.numerator = i.numerator * j.denominator;
    temp.denominator = i.denominator * j.numerator;

    return (temp);
}

int operator == (Fraction i, Fraction j)
{
    return ((i.numerator == j.numerator) && (i.denominator == j.denominator));
}

void Fraction :: print()
{
	cout << numerator << "/" << denominator << "\n";
}

// This routine simplifies a Fraction so that its numerator and denominator
// are relatively prime.

void Fraction::simplify ()
{
	GcdLcm d;

	// by convention, if the numerator is 0, the denominator is set to 1
	if (numerator == 0) denominator = 1;
	else {
	  // find the greatest common divisor of the numerator and denominator
	  d = this->computeGcdLcm();

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

*/

GcdLcm Fraction::computeGcdLcm ()
{
	// initialize the matrix to the identity
	int a12 = 0;
	int a21 = 0;
	int a11 = 1;
	int a22 = 1;

	int s0 = abs(numerator);
	int t0 = abs(denominator);

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

	GcdLcm toReturn;
	toReturn.gcd = sr;
	toReturn.lcm = abs(a21)*s0;
	// abs(a21)*s0 should also equal abs(a22)*t0

	return toReturn;
}
