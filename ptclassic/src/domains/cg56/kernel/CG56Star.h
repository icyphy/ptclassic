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
#include "CG56PortHole.h"
#include "MotorolaAttributes.h"

#define CG56CodeBlock CodeBlock

const double CG56_ONE = 1.0 - 1.0/double(1<<23);

class CG56Star : public AsmStar {
public:
	// my domain
	const char* domain() const;

	// class identification
	int isA(const char*) const;
};

#endif
