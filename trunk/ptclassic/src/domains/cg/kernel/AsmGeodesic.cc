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
#include "miscFuncs.h"

void AsmGeodesic :: initialize() {
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
		ListIter next(dests);
		int omax = 0;
		AsmGeodesic *d;
		while ((d = (AsmGeodesic*)next++) != 0)
			omax = max(omax,d->internalBufSize());
		return omax + maxNumParticles;
		}
	}
}

int AsmGeodesic :: bufSize() const {
	if (forkType == (F_DEST|F_SRC)) return 0;
	else return internalBufSize();
}

ISA_FUNC(AsmGeodesic,Geodesic);




