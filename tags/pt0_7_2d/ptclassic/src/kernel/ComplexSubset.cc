static const char file_id[] = "ComplexSubset.cc";
/**************************************************************************
Version identification:
@(#)ComplexSubset.cc	1.7 3/2/95

Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

A small complex class -- a subset of the cfront and libg++ versions,
but written from scratch.
**************************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "ComplexSubset.h"
#include <stream.h>

Complex& Complex::operator /= (const Complex& arg) {
	// multiply by the conjugate of arg
	double nr = r * arg.r + i * arg.i;
	i = i * arg.r - r * arg.i;
	r = nr;
	// divide by the magnitude squared to get the quotient
	double na = norm(arg);
	r /= na;
	i /= na;
	return *this;
}

Complex operator / (const Complex& x, const Complex& y) {
	Complex tmp(x);
	return tmp /= y;
}

Complex exp(const Complex& arg) {
	double angle = arg.imag();
	double mag = exp(arg.real());
	return Complex(mag*cos(angle), mag*sin(angle));
}

Complex log(const Complex& x) {
	return Complex(log(abs(x)), arg(x));
}

Complex sin(const Complex& x) {
	double nr = sin(x.real()) * cosh(x.imag());
	double ni = cos(x.real()) * sinh(x.imag());
	return Complex(nr,ni);
}

Complex cos(const Complex& x) {
	double nr = cos(x.real()) * cosh(x.imag());
	double ni = -sin(x.real()) * sinh(x.imag());
	return Complex(nr,ni);
}

Complex pow(const Complex& base, const Complex& expon) {
	return exp(expon*log(base));
}

Complex pow(double base, const Complex& expon) {
	if (base > 0) {
		return exp(expon*log(base));
	}
	else return exp(expon*log(Complex(base)));
}

Complex pow(const Complex& x, double expon) {
	double mag = pow(abs(x), expon);
	double angle = arg(x) * expon;
	return Complex(mag * cos(angle), mag * sin(angle));
}

Complex sqrt(const Complex& x) {
	double mag = sqrt(abs(x));
	double angle = arg(x) / 2;
	return Complex(mag * cos(angle), mag * sin(angle));
}

ostream& operator << (ostream& s, const Complex& x) {
	return s << "(" << x.real() << ", " << x.imag() << ")" ;
}


