static const char file_id[] = "CG56Star.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: E. A. Lee

 This is the baseclass for stars that generate assembly language code
 for the Motorola DSP 56000 family

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "CG56Star.h"

extern const Attribute A_XMEM = {AB_MEMORY|AB_XMEM,AB_YMEM};
extern const Attribute A_YMEM = {AB_MEMORY|AB_YMEM,AB_XMEM};

// const char* CG56Star::format(unsigned addr) {
	// static char* tmp = "ADDRESS";
	// return tmp;
// }

// The following is defined in CG56Domain.cc -- this forces that module
// to be included if any CG56 stars are linked in.
extern const char CG56domainName[];

const char* CG56Star :: domain () const { return CG56domainName;}

// isa

ISA_FUNC(CG56Star, CGStar);

// fire: prefix the code with a comment

int CG56Star::fire() {
	StringList code = "; code from star ";
	code += readFullName();
	code += " (class ";
	code += readClassName();
	code += ")\n";
	addCode(code);
	return AsmStar::fire();
}

// This is a static function.
// printFixValue: limit the fixedpoint value to the range.
StringList CG56Star::printFixedPointValue(double val) {
	const double maxVal = 1.0 - 1.0/double(1<<23);
	// write maxVal if given 1.0
	if (val > maxVal) val = maxVal;
	// make sure to write -1.0, not -1, for largest negative value.
	if (val < -maxVal) return "-1.0";
	char buf[20];
	sprintf(buf,"%.9g",val);
	return buf;
}

