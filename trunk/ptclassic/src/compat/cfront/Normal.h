// $Id$
#ifndef _Normal_h
#define _Normal_h 1

#include "Random.h"
// This version substitutes for libg++ random-number classes.
// return a normally-distributed r.v. with given mean and variance.
// The "polar method" is used.

class ACG;

class Normal : public Random {
private:
	double mean;
	double var;
	double val2;
	int have2;
public:
	Normal(double m,double v,ACG*) : mean(m), var(v), have2(0) {}
	double operator()();
};

inline double Normal :: operator()() {
	if (have2) {
		have2 = 0;
		return val2;
	}
	double x, y, r;
	do {
		// get two uniformly distributed r.v.'s in [-1,1]
		x = unif01() * 2.0 - 1.0;
		y = unif01() * 2.0 - 1.0;
		// form the magnitude squared.
		r = x * x + y * y;
	} while (r >= 1.0);
	r = sqrt(-2.0 * var *log(r) / r);
	val2 = r * x + mean;
	have2 = 1;
	return r * y + mean;
};

#endif
