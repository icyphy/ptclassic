#ifndef _FixState_h
#define _FixState_h 1
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 10/17/91
 Revisions:

 A FixState is conceptually represented as a fixed  point number.
 Its legal values range from -1 to just under 1
 (an exact one is allowed and converted to the largest possible
 fixed point number).

 Its initialization expression can have arbitrary floating values,
 as long as the final value is within the [-1,1] range.

 bitVal(int nBits) returns the value as an n-bit 2s complement integer.
 nBits can be no more than the number of bits in a long.

**************************************************************************/
#include "FloatState.h"
#ifdef __GNUG__
#pragma interface
#endif

class FixState : public FloatState {
public:

	void initialize();

	const char* type() const; // return "FIX"

	// value as string is same as for floatstate

	// return value as an integer, assuming represented by nBits bits
	long bitVal(int nBits);

	// class identification
	int isA(const char*) const;
	const char* className() const {return "FixState";}

	State* clone () const; // { return new FixState;}

	// assignment: does check and truncation
	double operator=(double rvalue);

};
#endif
