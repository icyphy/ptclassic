static const char file_id[] = "FixArrayState.cc";
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
