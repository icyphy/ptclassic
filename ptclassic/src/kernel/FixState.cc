/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 10/17/91
 Revisions:

 Implementation for FixState and FixArrayState classes.

**************************************************************************/


#include "FixState.h"
#include "FixArrayState.h"

#ifdef __GNUG__
#pragma implementation
#pragma implementation "FixArrayState.h"
#endif

void FixState :: initialize() {
	FloatState :: initialize();
	double v = *this;
	if (v < -1.0 || v > 1.0)
		parseError ("value out of range for a fixed-point State");
}

ISA_FUNC(FixState,FloatState);


static long scaleUpArgument(double val, int nBits) {
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

static FixState proto_1;
static KnownState entry_1(proto_1,"FIX");

void FixArrayState :: initialize() {
	FloatArrayState :: initialize();
	for (int i = 0; i < size(); i++) {
		if (val[i] < -1.0 || val[i] > 1.0) {
			parseError ("value(s) out of range for a fixed-point State");
			return;
		}
	}
}

long FixArrayState :: bitVal(int el, int nBits) {
	return scaleUpArgument(val[el], nBits);
}

ISA_FUNC(FixArrayState,FloatArrayState);

static FixArrayState proto_2;
static KnownState entry_2(proto_2,"FIXARRAY");
