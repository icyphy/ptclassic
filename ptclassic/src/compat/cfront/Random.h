// $Id$
#ifndef _Random_h
#define _Random_h 1
// This version substitutes for libg++ random-number classes.
// This is the abstract base class for other random-number types.

#if defined(hppa) || defined(SVR4) || defined(SYSV)
#ifndef __GNUG__

extern "C" long int lrand48();

// returns a random value between 0 and 1.

class Random {
protected:
	double unif01() { return double(lrand48())/2147483648.0;}
public:
	virtual double operator()() = 0;
};
#endif /* __GNUG__ */
#else /* hppa */

extern "C" long random();

// returns a random value between 0 and 1.

class Random {
protected:
	double unif01() { return double(random())/2147483648.0;}
public:
	virtual double operator()() = 0;
};
#endif /* hppa */
#endif

