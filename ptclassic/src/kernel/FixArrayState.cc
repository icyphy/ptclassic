static const char file_id[] = "FixArrayState.cc";
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 10/17/91
 Revisions:

 Implementation for FixArrayState class.

**************************************************************************/


#ifdef __GNUG__
#pragma implementation
#endif

#include "FixArrayState.h"
#include "KnownState.h"

extern long scaleUpArgument(double val, int nBits);

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

const char* FixArrayState :: type() const {
	return "FIXARRAY";
}

State* FixArrayState :: clone() const {
	return new FixArrayState;
}

ISA_FUNC(FixArrayState,FloatArrayState);

static FixArrayState proto;
static KnownState entry(proto,"FIXARRAY");
