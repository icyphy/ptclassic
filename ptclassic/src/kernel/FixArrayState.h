#ifndef _FixArrayState_h
#define _FixArrayState_h 1
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 10/17/91
 Revisions:

 A FixArrayState is conceptually represented as an array of fixed
 point numbers.  Legal values for elements range from -1 to just under 1
 (an exact one is allowed and converted to the largest possible
 fixed point number).

 Its initialization expression can have arbitrary floating values,
 as long as the final value for each element is within the [-1,1] range.

 bitVal(int el, int nBits) returns the value, as an n-bit 2's complement
 integer, of the el'th element.

 nBits can be no more than the number of bits in a long.

**************************************************************************/
#include "FloatArrayState.h"
#ifdef __GNUG__
#pragma interface
#endif

// arraystate of values that are conceptually represented as twos
// complement fixed point numbers.  They range from -1 to just under
// 1 (an exact one is allowed and converted to the largest possible
// fixed point number).

class FixArrayState : public FloatArrayState {
public:
	void initialize();

	const char* type() const; // return "FIXARRAY"

	// value as string is same as for floatstate

	// return element el as an integer, assuming represented by nBits bits
	long bitVal(int el, int nBits);

	// class identification
	int isA(const char*) const;
	const char* ClassName() const {return "FixArrayState";}

	State* clone () const; // { return new FixArrayState;}
};
#endif
