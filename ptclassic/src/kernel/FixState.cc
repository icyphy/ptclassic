static const char file_id[] = "FixState.cc";
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

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
	return new FixState;
}

// assignment operator: truncate the value
double FixState :: operator = (double rvalue) {
	if (rvalue > 1.0) rvalue = 1.0;
	else if (rvalue < -1.0) rvalue = -1.0;
	return FloatState::operator=(rvalue);
}

static FixState proto;
static KnownState entry(proto,"FIX");

