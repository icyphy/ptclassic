// $Id$
#ifndef _NegExp_h
#define _NegExp_h 1
#include <Random.h>
#include <math.h>
// This version substitutes for libg++ random-number classes.
// Return an exponentially distributed r.v. with mean "arg".

class ACG;

class NegativeExpntl : public Random {
private:
	double mean;
public:
	NegativeExpntl(double arg,ACG*) : mean(arg) {}
	double operator()() {
		return -log(unif01()) * mean;
	}
};

#endif
