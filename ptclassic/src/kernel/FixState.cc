static const char file_id[] = "FixState.cc";
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY

 Programmer:  J. T. Buck
 Date of creation: 10/17/91
 Revisions:

 Implementation for FixState class.

**************************************************************************/


#ifdef __GNUG__
#pragma implementation
#endif

#include "FixState.h"
#include "KnownState.h"

void FixState :: initialize() {
	FloatState :: initialize();
	double v = *this;
	if (v < -1.0 || v > 1.0)
		parseError ("value out of range for a fixed-point State");
}

ISA_FUNC(FixState,FloatState);


long scaleUpArgument(double val, int nBits) {
	// clip argument
	if (val > 1.0) val = 1.0;
	if (val < -1.0) val = 1.0;

	// compute upper limit so it is safe even if nBits == (# bits in
	// a long)
	long k = 1L << (nBits-2);
	long limit = k - 1L;
	limit += k;

	double result = k * val * 2.0;
	if (result >= limit) return limit;
	// round to nearest value, regardless of how the processor
	// handles negative truncation (towards 0 or -inf)
	if (result > 0) return long(result+0.5);
	return -long(0.5-result);
}

long FixState::bitVal(int nBits) {
	return scaleUpArgument(*this,nBits);
}

const char* FixState :: type() const {
	return "FIX";
}

State* FixState :: clone() const {
	LOG_NEW; FixState* s = new FixState;
	s->val = val;
	return s;
}

// assignment operator: truncate the value
double FixState :: operator = (double rvalue) {
	if (rvalue > 1.0) rvalue = 1.0;
	else if (rvalue < -1.0) rvalue = -1.0;
	return FloatState::operator=(rvalue);
}

static FixState proto;
static KnownState entry(proto,"FIX");

