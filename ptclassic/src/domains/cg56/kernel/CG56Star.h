#ifndef _CG56Star_h
#define _CG56Star_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: E. A. Lee and J. T. Buck

 This is the baseclass for stars that generate assembly language code
 for the Motorola DSP 56000 family

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "AsmStar.h"
#include "CG56Connect.h"
#define CG56CodeBlock CodeBlock

// 56k - specific attributes.

const bitWord AB_XMEM = 0x10000; // allocate in X memory bank
const bitWord AB_YMEM = 0x20000; // allocate in Y memory bank

// note that attributes may combine several attribute bits.

extern const Attribute A_XMEM;
extern const Attribute A_YMEM;

class CG56Star : public AsmStar {
protected:
	// const char* format(unsigned addr);
public:
	// my domain
	const char* domain() const;

	int fire();

	// class identification
	int isA(const char*) const;

	// print fixed point values
	StringList printFixValue(double arg) const {
		return printFixedPointValue(arg);
	}

	// body of above function
	static StringList printFixedPointValue(double);
};

#endif
