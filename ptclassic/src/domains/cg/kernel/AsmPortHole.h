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

// PortHole class specific to assembly code generation.
// It contains methods for allocating memory for inputs and outputs.
class AsmPortHole : public SDFPortHole {
	int offset;
public:
	// Allocate a geodesic and give it a name
	virtual Geodesic* allocateGeodesic();

	// Return the geodesic connected to this PortHole.
	// This is typesafe because allocateGeodesic
	// makes myGeodesic of this type.
	AsmGeodesic& geo() const { return *(AsmGeodesic*)myGeodesic;}

	// Return the size of the buffer connected to this PortHole.
	int bufSize() const;

	// Assign a memory address to the geodesic connected to this PortHole.
	void assignAddr(ProcMemory& m, unsigned a);

	// Return the base address of the memory allocated to the
	// geodesic connected to this PortHole.
	unsigned baseAddr() const;

	// Return the memory allocated to the
	// geodesic connected to this PortHole.
	ProcMemory* memory() const;

	// Return a string indicating the address with an offset for
	// the current access.
	virtual StringList location();

	// Advance the offset by the number of tokens produced or
	// consumed in this PortHole when the Star fires.
	virtual void advance() {
		offset += numberTokens;
		if (offset >= bufSize()) offset -= bufSize();
	}

	// Return TRUE if a circular buffer access is ever required
	// for this buffer.  This was either specified by the programmer,
	// or required because the number of tokens produced or consumed
	// does not evenly divide the buffer size.
	int circAccess() const;

	// Return TRUE if an advance now will cause a wraparound.
	// This indicates that circular access should be used this time.
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

// Since MultiAsmPort is no different from MultiSDFPort, we just use an alias
#define MultiAsmPort MultiSDFPort

class MultiInAsmPort : public MultiAsmPort {
public:
	int isItInput() const;
	// PortHole& newPort();
};

class MultiOutAsmPort : public MultiAsmPort {
public:
	int isItOutput() const;
	// PortHole& newPort();
};

#endif
