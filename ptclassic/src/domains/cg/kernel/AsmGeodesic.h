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
	void addDest(AsmGeodesic *d) {
		d->forkType |= F_DEST;
		d->src = this;
		dests.append(d);
	}
	AsmGeodesic() : maxNumParticles(0), forkType(0), src(0), mem(0) {}
	void assignAddr(ProcMemory& m, unsigned a) {
		mem = &m; address = a;
	}
	unsigned addr() const { return address;}
private:
	int internalBufSize() const;
	int maxNumParticles;
	SequentialList dests;
	AsmGeodesic *src;
	int forkType;
	ProcMemory* mem;
	unsigned address;
};

#endif
