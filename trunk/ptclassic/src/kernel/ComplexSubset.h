// A small complex class -- a subset of the cfront and libg++ versions,
// but written from scratch.
// $Id$

#ifndef _ComplexSubset_h
#define _ComplexSubset_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include <math.h>

class Complex {
private:
	double r;
	double i;
public:
	// retrieve members
	double real() const { return r;}
	double imag() const { return i;}

	// constructor, copy constructor
	Complex() : r(0.0), i(0.0) {}
	Complex(double rp, double ip = 0.0) : r(rp), i(ip) {}
	Complex(const Complex& arg) : r(arg.r), i(arg.i) {}

	// assignment operator
	Complex& operator = (const Complex& arg) {
		r = arg.r;
		i = arg.i;
		return *this;
	}

	// op-assign operators
	Complex& operator += (const Complex& arg) {
		r += arg.r;
		i += arg.i;
		return *this;
	}

	Complex& operator -= (const Complex& arg) {
		r -= arg.r;
		i -= arg.i;
		return *this;
	}

	Complex& operator *= (const Complex& arg) {
		double nr = r * arg.r - i * arg.i;
		i = r * arg.i + i * arg.r;
		r = nr;
		return *this;
	}

	// this one is not inline: too big
	Complex& operator /= (const Complex& arg);

	// special ones for double args
	Complex& operator *= (double arg) {
		r *= arg;
		i *= arg;
		return *this;
	}

	Complex& operator /= (double arg) {
		r /= arg;
		i /= arg;
		return *this;
	}
};

// comparisons
inline int operator != (const Complex& x, const Complex& y) {
	return x.real() != y.real() || x.imag() != y.imag();
}

inline int operator == (const Complex& x, const Complex& y) {
	return x.real() == y.real() && x.imag() == y.imag();
}

// basic operations
inline Complex operator + (const Complex& x, const Complex& y) {
	return Complex(x.real()+y.real(),x.imag()+y.imag());
}

inline Complex operator - (const Complex& x, const Complex& y) {
	return Complex(x.real()-y.real(),x.imag()-y.imag());
}

inline Complex operator - (const Complex& y) {
	return Complex(-y.real(),-y.imag());
}

inline Complex conj (const Complex& x) {
	return Complex(x.real(),-x.imag());
}

inline double real (const Complex& x) { return x.real();}
inline double imag (const Complex& x) { return x.imag();}

inline Complex operator * (const Complex& x, const Complex& y) {
	return Complex(x.real()*y.real() - x.imag()*y.imag(), 
		       x.real()*y.imag() + x.imag()*y.real());
}

inline Complex operator * (double x, const Complex& y) {
	return Complex(x*y.real(),x*y.imag());
}

inline Complex operator * (const Complex& x, double y) {
	return y*x;
}

inline Complex operator / (const Complex& x, double y) {
	return Complex(x.real()/y, x.imag()/y);
}

inline double abs(const Complex& arg) {
	return hypot(arg.real(), arg.imag());
}

inline double arg(const Complex& x) {
	return atan2(x.imag(), x.real());
}

inline double norm(const Complex& arg) {
	return arg.real() * arg.real() + arg.imag() * arg.imag();
}

// math functions.
Complex operator / (const Complex&, const Complex&);
Complex sin(const Complex&);
Complex cos(const Complex&);
Complex exp(const Complex&);
Complex log(const Complex&);
Complex sqrt(const Complex&);
Complex pow(double base,const Complex& expon);
Complex pow(const Complex& base, const Complex& expon);
Complex pow(double base, const Complex& expon);

class ostream;

ostream& operator<<(ostream&, const Complex&);
#endif
