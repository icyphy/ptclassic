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
#include <builtin.h>

PortHole* AsmGeodesic::setSourcePort (GenericPort & sp, int delay) {
	AsmPortHole* p = (AsmPortHole*)Geodesic::setSourcePort (sp, delay);
	if (p->forkSource()) {
		forkType |= F_DEST;
		src = p->forkSource();
	}
}

PortHole* AsmGeodesic::setDestPort (GenericPort& dp) {
	AsmPortHole* p = (AsmPortHole*)Geodesic::setDestPort (dp);
	if (p->fork()) {
		forkType |= F_SRC;
	}
}

void AsmGeodesic :: initialize() {
	Geodesic :: initialize();
	maxNumParticles = size();
}

void AsmGeodesic :: incCount(int n) {
	Geodesic :: incCount(n);
	if (size() > maxNumParticles) maxNumParticles = size();
}

// recursive function to sum up fork buffer sizes
int AsmGeodesic :: internalBufSize() const {
	switch (forkType) {
	case 0:
		return maxNumParticles;
	case F_DEST:
		return 0;
	default:{
		ListIter next(src->forkDests);
		int omax = 0;
		AsmGeodesic *d;
		while ((d = (AsmGeodesic*)next++) != 0)
			omax = max(omax,d->internalBufSize());
		return omax + maxNumParticles;
		}
	}
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




