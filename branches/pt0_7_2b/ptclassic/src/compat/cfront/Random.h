// $Id$
#ifndef _Random_h
#define _Random_h 1
// This version substitutes for libg++ random-number classes.
// This is the abstract base class for other random-number types.
extern "C" long random();

// returns a random value between 0 and 1.

class Random {
protected:
	double unif01() { return double(random())/2147483648.0;}
public:
	virtual double operator()() = 0;
};
#endif

