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
#pragma interface
#endif

#include "AsmConnect.h"
#include "Geodesic.h"

const int F_SRC = 1;
const int F_DEST = 2;

class ProcMemory;

class AsmGeodesic : public Geodesic {
public:
	AsmGeodesic () : maxNumParticles(0), mem(0), addr(0) {}
	void initialize();
	void incCount(int);
	// class identification
	int isA(const char*) const;

	// return the size of the buffer, or the size of the associated
	// fork buffer for fork destinations.
	int bufSize() const;

	// return the size of the buffer itself (zero is returned for
	// fork output geodesics)
	int localBufSize() const;

	// return my "fork type"
	int forkType() const;

	// return the value of any delay on the outputs of forks --
	// return zero if not associated with a fork buffer.
	int forkDelay() const;

	// Assign a memory and address to the geodesic
	void assignAddr(ProcMemory& m, unsigned a);

	// Return the address assigned to the geodesic.
	// if I am a fork destination, my address is that of my source.
	unsigned address() const;

	// Return a pointer to the memory assigned to the geodesic
	// if I am a fork destination, my memory is that of my source.

	ProcMemory* memory() const;
	
protected:
	AsmPortHole* src() {
		return ((AsmPortHole*)originatingPort)->forkSrc;
	}
	const AsmPortHole* src() const {
		return ((const AsmPortHole*)originatingPort)->forkSrc;
	}
private:
	int internalBufSize() const;
	int maxNumParticles;
	SequentialList dests;
	ProcMemory* mem;
	unsigned addr;
};

#endif
