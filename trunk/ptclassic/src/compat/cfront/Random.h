/*
Copyright (c) 1990-%Q% The Regents of the University of California.
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

