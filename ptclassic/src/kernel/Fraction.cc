static const char file_id[] = "Fraction.cc";
#include "Fraction.h"
#include <stream.h>

// SCCS version identification
// $Id$

/*

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
