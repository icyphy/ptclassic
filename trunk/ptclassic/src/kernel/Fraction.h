#ifndef _Fraction_h
#define _Fraction_h 1

/*
 Programmer:  Edward A. Lee
 Date: 12/7/89

 This class represents fractions using integers for the numerator
 and denominator.

*/

// The following structure is defined so that Euclid's algorithm
// can return both the least common multiple and greatest common
// divisor.

struct GcdLcm {
	int gcd;
	int lcm;
};

class Fraction
{
public:
	int numerator;
	int denominator;

	// Constructors
	Fraction () {
		numerator = 0;
		denominator = 1;
	}
	Fraction (int i) {
		numerator = i;
		denominator = 1;
	}
	Fraction (int i, int j) {
		numerator = i;
		denominator = j;
	}

	// Cast to type double:
	operator double() { return (double)numerator/(double)denominator; }

	// Addition operator
	friend Fraction operator + (Fraction, Fraction);

	// Subtraction operator
	friend Fraction operator - (Fraction, Fraction);

	// Multiplication operator
	friend Fraction operator * (Fraction, Fraction);

	// Division operator
	friend Fraction operator / (Fraction, Fraction);

	// Equality test operator
	// This operator assumes the fractions have been simplified!!
	friend int operator == (Fraction, Fraction);

	// Print operator
	operator char* ();

	// Simplify the fraction (compute an equivalent fraction with
	// numerator and denominator relatively prime).
	// The simplified fraction replaces the original.
	void simplify();

	// Greatest common divisor and least common multiple function
	GcdLcm computeGcdLcm();
};

#endif
