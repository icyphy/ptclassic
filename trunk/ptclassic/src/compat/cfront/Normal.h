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
