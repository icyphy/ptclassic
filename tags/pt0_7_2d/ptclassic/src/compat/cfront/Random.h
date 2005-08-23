/*
Copyright (c) 1990-1999 The Regents of the University of California.
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
*/
// @(#)Random.h	1.12 01/28/99
#ifndef _Random_h
#define _Random_h 1
// This version substitutes for libg++ random-number classes.
// This is the abstract base class for other random-number types.

#if defined(hppa) || defined(SVR4) || defined(SYSV) || defined(linux)
#if !defined(__GNUG__) || defined(PT_EGCS)

extern "C" long int lrand48();

// returns a random value between 0 and 1.

class Random {
protected:
	double unif01() { return double(lrand48())/2147483648.0;}
public:
	virtual double operator()() = 0;
};
#endif /* !__GNUG__ || PT_EGCS */
#else /* hppa */
#ifdef PT_USE_RAND

#if defined(PT_NT4VC)
extern "C" {
#include <stdlib.h>    
}
#endif /* PT_NT4VC */

//extern "C" int rand();

// returns a random value between 0 and 1.

class Random {
protected:
#ifdef PT_NT4VC
    double unif01() { return double(rand())/RAND_MAX;}
#else
    double unif01() { return double(rand())/2147483648.0;}
#endif
public:
	virtual double operator()() = 0;
};
#else /* PT_USE_RAND */

#if defined(PTNT) && !defined(PT_NT4VC)
extern "C" {
#include <stdlib.h>
}
#else
extern "C" long random();
#endif

// returns a random value between 0 and 1.

class Random {
protected:
	double unif01() { return double(random())/2147483648.0;}
public:
	virtual double operator()() = 0;
};
#endif /* PT_USE_RAND */
#endif /* hppa */
#endif

