static const char file_id[] = "AsmGeodesic.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck

 geodesics for AsmCode stars.  Their main role is to figure out how
 big a buffer must be allocated in code generation.

 There is also support for fork buffers -- a mechanism that allows all
 the buffers of a fork star to be allocated as only a single buffer.

 There are four kinds of AsmGeodesics, depending on the value of forkType():

0:	an ordinary buffer, with one reader and one writer.  Its size
	is lcm(nread,nwrite).

F_DEST:	the output (destination) of a Fork star.  No memory is allocated
	for this buffer; accesses to this buffer refer to the corresponding
	fork buffer instead.

F_SRC:	a fork buffer -- the input, or source, of a Fork star.  This buffer
	has one writer and multiple readers.  Its size is the LCM of nread
	and all the nwrite values.

F_SRC|F_DEST:
	a buffer that sits between two fork stars (it is the output of
	one and the input of another).  No memory is allocated for it;
	it has pointers that "chain" the F_DEST buffers back to the
	corresponding F_SRC buffer.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "AsmGeodesic.h"
#include "Error.h"
#include <builtin.h>

int AsmGeodesic::forkType() const {
	int type = 0;
	if (src() != 0)	type |= F_DEST;
	if (destinationPort && ((AsmPortHole*)destinationPort)->fork() != 0)
		type |= F_SRC;
	return type;
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

// recursive function to compute buffer and forkbuf sizes.  Note that
// buffers are only actually allocated for types 0 and F_SRC.  For others,
// the return value represents a partial result, since we must compute
// the LCM of all reader and writer values of numberTokens.
int AsmGeodesic :: internalBufSize() const {
	int bsiz;
	int type = forkType();
	switch (type) {
	case 0:
		// a normal buffer
		bsiz = lcm(originatingPort->numberTokens,
			   destinationPort->numberTokens);
		break;
		// output of fork: no buffer at all (we share the forkbuf)
	case F_DEST:
		return destinationPort->numberTokens;
	default:
		// an F_SRC (true fork buffer) or F_SRC|F_DEST buffer
		// (a buffer that sits between fork stars).  Want the lcm
		// of all connected ports.
		{
			AsmPortHole* dPort = (AsmPortHole*)destinationPort;
			ListIter next(dPort->forkDests);
			bsiz = (type == F_SRC ?
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
int AsmGeodesic :: forkDelay() const {
	int n = 0;
	const AsmGeodesic *s = this;
	const AsmPortHole* p;
	while ((p = s->src()) != 0) {
		n += s->numInitialParticles;
		s = &p->geo();
	}
	return n;
}

int AsmGeodesic :: bufSize() const {
	if (src() != 0) return 0;
	else return internalBufSize();
}

// Return the address assigned to the geodesic.
// if I am a fork destination, my address is that of my source.
unsigned AsmGeodesic::address() const {
	const AsmPortHole* p = src();
	return p ? p->geo().address() : addr;
}


ProcMemory* AsmGeodesic::memory() const {
	const AsmPortHole* p = src();
	return p ? p->geo().memory() : mem;
}

ISA_FUNC(AsmGeodesic,Geodesic);




