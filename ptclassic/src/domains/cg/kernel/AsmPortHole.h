#ifndef _AsmConnect_h
#define _AsmConnect_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck
 Modified by: E. A. Lee

 These classes are portholes for stars that generate assembly language code.  

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif
#include "CGConnect.h"

// portholes for AsmCodeStars and derived stars
const bitWord PB_CIRC = 0x40;
const bitWord PB_SYMMETRIC = 0x200;

extern const Attribute P_CIRC;
extern const Attribute P_SYMMETRIC;

// attributes for code generation portholes

class ProcMemory;
class AsmGeodesic;

// PortHole class specific to assembly code generation.
// It contains methods for allocating memory for inputs and outputs.
class AsmPortHole : public CGPortHole {
	friend class AsmGeodesic;
public:
	// Allocate a geodesic and give it a name
	Geodesic* allocateGeodesic();

	// Return the geodesic connected to this PortHole.
	// This is typesafe because allocateGeodesic
	// makes myGeodesic of this type.
	AsmGeodesic& geo() const { return *(AsmGeodesic*)myGeodesic;}

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

	// Initialize the offset member -- must be called after
	// any setSDFParams calls on ports take place (e.g. start
	// funcs in AsmStars).  This is handled by doing it from
	// CGTarget after everything has been init-ed.
	// it returns TRUE on success, else FALSE (0)
	virtual int initOffset();
};

class InAsmPort : public AsmPortHole {
public:
	int isItInput() const;
};

class OutAsmPort : public AsmPortHole {
public:
	int isItOutput() const;

};

#define MultiAsmPort MultiCGPort

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
