#ifndef _AsmGeodesic_h
#define _AsmGeodesic_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck
 Modified by: E. A. Lee

 geodesics for AsmCode stars.  In addition to the functions of CG
 geodesics, they manage memory allocation for buffers.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "AsmConnect.h"
#include "CGGeodesic.h"

class ProcMemory;

class AsmGeodesic : public CGGeodesic {
public:
	AsmGeodesic () : mem(0), addr(0) {}

	// class identification
	int isA(const char*) const;

	// Assign a memory and address to the geodesic
	void assignAddr(ProcMemory& m, unsigned a);

	// Return the address assigned to the geodesic.
	// if I am a fork destination, my address is that of my source.
	unsigned address() const;

	// Return a pointer to the memory assigned to the geodesic
	// if I am a fork destination, my memory is that of my source.

	ProcMemory* memory() const;
protected:
	// determine whether extra memory should be used to obtain
	// linear access with no wraparound.
	double wasteFactor() const;
private:
	int internalBufSize() const;
	ProcMemory* mem;
	unsigned addr;
};

#endif
