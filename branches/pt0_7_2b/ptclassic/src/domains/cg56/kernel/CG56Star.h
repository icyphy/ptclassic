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
#pragma once
#pragma interface
#endif

#include "AsmStar.h"
#include "CG56Connect.h"
#define CG56CodeBlock CodeBlock

// 56k - specific attributes.

const bitWord AB_XMEM = 0x10000; // allocate in X memory bank

// note that attributes may combine several attribute bits.

extern const Attribute A_XMEM;
extern const Attribute A_YMEM;
extern const Attribute A_XMEM_BIT;

class CG56Star : public AsmStar {
public:
	void fire();
};

#endif