#ifndef _AsmConnect_h
#define _AsmConnect_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck

 These classes are portholes for stars that generate assembly language code.  

*******************************************************************/
#ifdef __GNUG__
#pragma once
#pragma interface
#endif
#include "SDFConnect.h"

// portholes for AsmCodeStars and derived stars
const bitWord PB_CIRC = 0x40;

// attributes for code generation portholes

class ProcMemory;
class AsmGeodesic;

class AsmPortHole : public SDFPortHole {
	int offset;
public:
	int bufSize() const;
	void assignAddr(ProcMemory& m, unsigned a);
	int circAccess() const;
	virtual Geodesic* allocateGeodesic();

	// the following is typesafe because allocateGeodesic
	// makes myGeodesic of this type.
	AsmGeodesic& geo() const { return *(AsmGeodesic*)myGeodesic;}
	unsigned baseAddr() const;
	virtual StringList location(int update);
	virtual void advance() {
		offset += numberTokens;
		if (offset >= bufSize()) offset -= bufSize();
	}
	int circAccessThisTime() const {
		return offset + numberTokens > bufSize();
	}
};

class InAsmPort : public AsmPortHole {
public:
	int isItInput() const;
};

class OutAsmPort : public AsmPortHole {
public:
	int isItOutput() const;
};

// is anything here?
class MultiAsmPort : public MultiSDFPort {
public:
	int someFunc();
};

class MultiInAsmPort : public MultiAsmPort {
public:
	int isItInput() const;
	PortHole& newPort();
};

class MultiOutAsmPort : public MultiAsmPort {
public:
	int isItOutput() const;
	PortHole& newPort();
};

#endif
