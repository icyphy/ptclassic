static const char file_id[] = "Fraction.cc";
#include "Fraction.h"
#include <stream.h>

// SCCS version identification
// $Id$

/*

 Copyright (c) 1989 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Edward A. Lee
 Date: 12/7/89

 This class represents fractions using integers for the myNum
 and myDen.

 Rewritten by Joe Buck to implement assignment operators and to be
 more efficient.

 This file contains the member function definitions.

*/

// greatest common divisor function.  If 2nd arg is negative, result is
// negative.  Magnitude of result equals gcd(abs(a),abs(b)).
// these are so simplify is easy to write.
int gcd(int a, int b) {
	int sign = 1;
	// record signs
	if (a < 0) {
		a = -a;
	}
	if (b < 0) {
		sign = -1;
		b = -b;
	}
	// swap to make a > b if needed
	if (a < b) { int t = a; a = b; b = t;}
	if (b == 0) return a;
	while (1) {
		if (b <= 1) return b*sign;
		int rem = a%b;
		if (rem == 0) return b*sign;
		a = b;
		b = rem;
	}
}

Fraction&
Fraction :: simplify() {
	int g = gcd(myNum,myDen);
	myNum /= g;
	myDen /= g;
	return *this;
}

Fraction&
Fraction:: operator += (const Fraction& a)
{
	myNum = myNum * a.myDen + a.myNum * myDen;
	myDen *= a.myDen;
	return *this;
}

Fraction&
Fraction::operator -= (const Fraction& a)
{
	myNum = myNum * a.myDen - a.myNum * myDen;
	myDen *= a.myDen;
	return *this;
}

// print
ostream& operator<<(ostream& o,const Fraction& f) {
	return o << f.num() << '/' << f.den();
}
