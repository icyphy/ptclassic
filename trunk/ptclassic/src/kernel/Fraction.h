#ifndef _Fraction_h
#define _Fraction_h 1


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
 Programmer:  Edward A. Lee and J. Buck
 Date: 12/7/89

 This class represents fractions using integers for the myNum
 and myDen.

 Thoroughly rewritten by Joe Buck to be more efficient, implement
 assignment operators, and to work around some g++ bugs.

 Definitions of "gcd" and "lcm" are here too, since fraction code
 uses them.

 All operations keep the fraction relatively prime, with positive
 denominator.
*/
#ifdef __GNUG__
#pragma interface
#endif

// gcd and lcm functions.  The gcd function has the property that
// gcd(0,n) = gcd(n,0) = n

int gcd(int a,int b);

// order of the multiplication and division is chosen to avoid overflow
// in cases where a*b > MAXINT but lcm(a,b) < MAXINT.  The division always
// goes evenly.

inline int lcm(int a,int b) { return a == b ? a : a * (b / gcd(a,b));}

class Fraction
{
public:
	int num() const {return myNum;}
	int den() const {return myDen;}
	// Constructors
	Fraction () : myNum(0), myDen(1) { }

	Fraction (int i, int j=1) : myNum(i), myDen(j) {}

	Fraction (const Fraction& a) : myNum(a.myNum), myDen(a.myDen) {}

	Fraction& operator= (const Fraction& a) {
		myNum = a.myNum;
		myDen = a.myDen;
		return *this;
	}

	// Cast to type double:
	operator double() const {
		return double(myNum)/double(myDen);
	}

	// Assignment operators
	Fraction& operator += (const Fraction& f);
	Fraction& operator -= (const Fraction& f);

	Fraction& operator *= (const Fraction& a) {
		myNum *= a.myNum;
		myDen *= a.myDen;
		return *this;
	}

	Fraction& operator /= (const Fraction& a) {
		myNum *= a.myDen;
		myDen *= a.myNum;
		return *this;
	}

	// binary operators that need friendship

	friend int operator == (const Fraction&,const Fraction&);
	friend int operator != (const Fraction&,const Fraction&);
	friend Fraction operator * (const Fraction&,const Fraction&);
	friend Fraction operator / (const Fraction&,const Fraction&);

	// Simplify the fraction (compute an equivalent fraction with
	// myNum and myDen relatively prime).
	// The simplified fraction replaces the original.
	Fraction& simplify();
private:
	int myNum;
	int myDen;
};

// print a Fraction on an ostream
class ostream;
ostream& operator<<(ostream&,const Fraction&);

// Notice: these functions don't have to be friends!
// Design based on Andrew Koenig's method -- the temporaries
// (res in the functions below) can almost always be optimized away

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

// these can be done most efficiently with a constructor call.
inline Fraction operator* (const Fraction& a,const Fraction& b) {
	return Fraction(a.myNum*b.myNum,a.myDen*b.myDen);
}

inline Fraction operator/ (const Fraction& a,const Fraction& b) {
	return Fraction(a.myNum*b.myDen,a.myDen*b.myNum);
}

// Equality test.  Works for 2/3, 4/6.

inline int operator == (const Fraction& i, const Fraction& j)
{
	return i.myNum*j.myDen == i.myDen*j.myNum;
}

inline int operator != (const Fraction& i, const Fraction& j)
{
	return i.myNum*j.myDen != i.myDen*j.myNum;
}

#endif
