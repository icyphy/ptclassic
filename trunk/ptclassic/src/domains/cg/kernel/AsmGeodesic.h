#ifndef _AsmGeodesic_h
#define _AsmGeodesic_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck

 geodesics for AsmCode stars.  Their main role is to figure out how
 big a buffer must be allocated in code generation.

 There is also support for fork buffers -- an attempt to allow all
 the buffers of a fork star to be allocated as only a single buffer.

*******************************************************************/
#ifdef __GNUG__
#pragma once
#pragma interface
#endif

#include "Geodesic.h"

const int F_SRC = 1;
const int F_DEST = 2;

class ProcMemory;

class AsmGeodesic : public Geodesic {
public:
	void initialize();
	void incCount(int);
	// class identification
	int isA(const char*) const;
	int bufSize() const;
	void initDestList() { forkType |= F_SRC; dests.initialize(); }
	void addDest(AsmGeodesic &d) {
		d.forkType |= F_DEST;
		d.src = this;
		dests.put(&d);
	}

	// Constructor
	AsmGeodesic() : maxNumParticles(0), forkType(0), src(0), mem(0) {}

	// Assign a memory and address to the geodesic
	void assignAddr(ProcMemory& m, unsigned a) {
		mem = &m; addr = a;
	}

	// Return the address assigned to the geodesic.
	// if I am a fork destination, my address is that of my source.
	unsigned address() const;

	// Return a pointer to the memory assigned to the geodesic
	// if I am a fork destination, my memory is that of my source.

	ProcMemory* memory() const;
	
private:
	int internalBufSize() const;
	int maxNumParticles;
	SequentialList dests;
	AsmGeodesic *src;
	int forkType;
	ProcMemory* mem;
	unsigned addr;
};

#endif
