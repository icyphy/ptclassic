static const char file_id[] = "AsmGeodesic.cc";
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
#pragma implementation
#endif

#include "AsmGeodesic.h"
#include "Error.h"
#include <builtin.h>

PortHole* AsmGeodesic::setSourcePort (GenericPort & sp, int delay) {
	AsmPortHole* p = (AsmPortHole*)Geodesic::setSourcePort (sp, delay);
	if (p->forkSource()) {
		forkType |= F_DEST;
		src = p->forkSource();
	}
	return p;
}

PortHole* AsmGeodesic::setDestPort (GenericPort& dp) {
	AsmPortHole* p = (AsmPortHole*)Geodesic::setDestPort (dp);
	if (p->fork()) {
		forkType |= F_SRC;
	}
	return p;
}

void AsmGeodesic :: initialize() {
	Geodesic :: initialize();
	maxNumParticles = size();
}

void AsmGeodesic :: incCount(int n) {
	Geodesic :: incCount(n);
	if (size() > maxNumParticles) maxNumParticles = size();
}

static int gcd(int a, int b) {
	// swap to make a > b if needed
	if (a < b) { int t = a; a = b; b = t;}
	while (1) {
		if (b <= 1) return b;
		int rem = a%b;
		if (rem == 0) return b;
		a = b;
		b = rem;
	}
}

inline int lcm(int a, int b) { return a * b / gcd(a,b);}

// recursive function to compute buffer and forkbuf sizes
int AsmGeodesic :: internalBufSize() const {
	int bsiz;
	switch (forkType) {
	case 0:
		// a normal buffer
		bsiz = lcm(originatingPort->numberTokens,
			   destinationPort->numberTokens);
		break;
		// output of fork: no buffer at all (we share the forkbuf)
	case F_DEST:
		return 0;
	default:
		// an F_SRC (true fork buffer) or F_SRC|F_DEST buffer
		// (a buffer that sits between fork stars).  Want the lcm
		// of all connected ports.
		{
			ListIter next(src->forkDests);
			bsiz = (forkType == F_SRC ?
				originatingPort->numberTokens : 1);
			AsmPortHole* p;
			while ((p = (AsmPortHole*)next++) != 0)
				bsiz = lcm(bsiz,p->geo().internalBufSize());
		}
	}
	if (maxNumParticles > bsiz) {
		Error::abortRun(*destinationPort,
				"number of delay tokens exceeds buffer size",
				" (not yet supported)");
	}
	return bsiz;
}

// return the number of delays on all geodesics that are outputs of forks
// in the path back to the fork buffer.
int AsmGeodesic :: forkDelay() {
	int n = 0;
	AsmGeodesic *s = this;
	while (s->srcGeo()) {
		n += s->numInitialParticles;
		s = s->srcGeo();
	}
	return n;
}

int AsmGeodesic :: bufSize() const {
	if (forkType == (F_DEST|F_SRC)) return 0;
	else return internalBufSize();
}

// Return the address assigned to the geodesic.
// if I am a fork destination, my address is that of my source.
unsigned AsmGeodesic::address() const {
	return (forkType & F_DEST) ? srcGeo()->address() : addr;
}


ProcMemory* AsmGeodesic::memory() const {
	return (forkType & F_DEST) ? srcGeo()->memory() : mem;
}

ISA_FUNC(AsmGeodesic,Geodesic);




