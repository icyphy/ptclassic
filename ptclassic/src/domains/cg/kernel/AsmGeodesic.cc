static const char file_id[] = "AsmGeodesic.cc";
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
#pragma implementation
#endif

#include "AsmGeodesic.h"
#include "Error.h"
#include "ProcMemory.h"
#include <builtin.h>

ISA_FUNC(AsmGeodesic,CGGeodesic);

// Assign a memory and address to the Geodesic
void AsmGeodesic :: assignAddr(ProcMemory& m, unsigned a) {
	mem = &m; addr = a;
}

// Return the address assigned to the geodesic.
// if I am a fork destination, my address is that of my source.
unsigned AsmGeodesic::address() const {
	const AsmPortHole* p = (const AsmPortHole*)src();
	return p ? p->geo().address() : addr;
}


ProcMemory* AsmGeodesic::memory() const {
	const AsmPortHole* p = (const AsmPortHole*)src();
	return p ? p->geo().memory() : mem;
}

// recursive function to compute buffer and forkbuf sizes.  Note that
// buffers are only actually allocated for types 0 and F_SRC.  For others,
// the return value represents a partial result, since we must compute
// the LCM of all reader and writer values of numberTokens.
int AsmGeodesic :: internalBufSize() const {
	int size = CGGeodesic::internalBufSize();
	if (maxNumParticles > size) {
		Error::abortRun(*destinationPort,
				"number of delay tokens exceeds buffer size",
				" (not yet supported)");
	}
	return size;
}
