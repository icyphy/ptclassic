// A small complex class -- a subset of the cfront and libg++ versions,
// but written from scratch.
// $Id$

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


