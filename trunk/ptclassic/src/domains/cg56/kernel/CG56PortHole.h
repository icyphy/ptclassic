#ifndef _CG56Connect_h
#define _CG56Connect_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck

 These classes are portholes for stars that generate assembly language code.  

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif
#include "SDFConnect.h"
#include "AsmConnect.h"

// 56k - specific attributes.

const bitWord PB_XMEM = 0x10000; // allocate in X memory bank
const bitWord PB_YMEM = 0x20000; // allocate in Y memory bank

// note that attributes may combine several attribute bits.

extern const Attribute P_XMEM;
extern const Attribute P_YMEM;

class ProcMemory;
class AsmGeodesic;

class CG56PortHole : public AsmPortHole {
public:
	// constructor sets default attributes.
	CG56PortHole();
};

class InCG56Port : public InAsmPort {
public:
	int isItInput() const;
};

class OutCG56Port: public OutAsmPort {
public:
	int isItOutput() const;
};

// is anything here?
class MultiCG56Port : public MultiAsmPort {
public:
	int someFunc();
};

class MultiInCG56Port : public MultiInAsmPort {
public:
	int isItInput() const;
	PortHole& newPort();
};

class MultiOutCG56Port : public MultiOutAsmPort {
public:
	int isItOutput() const;
	PortHole& newPort();
};

#endif
