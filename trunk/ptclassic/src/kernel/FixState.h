#ifndef _FixState_h
#define _FixState_h 1
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
