static const char file_id[] = "CGGeodesic.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck
 Modified by: E. A. Lee

 geodesics for CGCode stars.  Their main role is to figure out how
 big a buffer must be allocated in code generation.

 There is also support for fork buffers -- a mechanism that allows all
 the buffers of a fork star to be allocated as only a single buffer.

 There are four kinds of CGGeodesics, depending on the value of forkType():

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

#include "CGGeodesic.h"
#include "Error.h"
#include "Fraction.h"

void CGGeodesic :: initialize() {
	Geodesic :: initialize();
	maxNumParticles = size();
}

void CGGeodesic :: incCount(int n) {
	Geodesic :: incCount(n);
	if (size() > maxNumParticles) maxNumParticles = size();
}

ISA_FUNC(CGGeodesic,Geodesic);

int CGGeodesic :: localBufSize() const {
	if (src() != 0) return 0;
	else return internalBufSize();
}

int CGGeodesic :: bufSize() const {
	const CGPortHole* p = src();
	return p ? p->cgGeo().bufSize() : internalBufSize();
}

int CGGeodesic::forkType() const {
	int type = 0;
	if (src() != 0)	type |= F_DEST;
	if (destinationPort && ((CGPortHole*)destinationPort)->fork() != 0)
		type |= F_SRC;
	return type;
}

// return the number of delays on all geodesics that are outputs of forks
// in the path back to the fork buffer.
int CGGeodesic :: forkDelay() const {
	int n = 0;
	const CGGeodesic *s = this;

	if (s->sourcePort()->isItInput()) return n;	// check wormhole boundary.

	const CGPortHole* p;
	while ((p = s->src()) != 0) {
		n += s->numInitialParticles;
		s = &p->cgGeo();
		if (s->sourcePort()->isItInput()) return n;
	}
	return n;
}

// recursive function to compute buffer and forkbuf sizes.  Note that
// buffers are only actually allocated for types 0 and F_SRC.

// We return 0 if the schedule has not yet been run, and also print an
// error message.

int CGGeodesic :: internalBufSize() const {
	CGPortHole* dest = (CGPortHole*)destinationPort;
	if (dest->parentReps() == 0) {
		Error::abortRun(*dest,
"Attempt to determine buffer size before schedule has been computed\n"
"(possibly from calling bufSize() in a star's setup() method)");
		return 0;
	}
	int bsiz = minNeeded();
	int type = forkType();
	if ((type & F_SRC) != 0) {
		// size must be >= max of all destbuf sizes.
		CGPortHole* dPort = (CGPortHole*)destinationPort;
		ListIter next(dPort->forkDests);
		CGPortHole* p;
		while ((p = (CGPortHole*)next++) != 0)
			bsiz = max(bsiz,p->cgGeo().internalBufSize());
	}
// if there are delays or old values are used, it may be better to
// use a larger size so that compile-time indexing is supportable.
// The buffer size must divide the total number of tokens produced in
// one execution.
	if (numInit() > 0 || dest->usesOldValues()) {
		// cannot determine size without schedule.
		if (maxNumParticles == 0) return 0;
		int total = dest->parentReps() * dest->numXfer();
		if (total >= bsiz) {
			// return the smallest factor of total
			// that is >= size
			int rval = total;
			for (int d = 2; bsiz*d <= total; d++) {
				if (total % d == 0)
					rval = total / d;
			}
			bsiz = rval;
		}
		// if we fall out of this loop, run-time addressing
		// will be required, so we might as well use the smaller
		// size.
	}
	// we still prefer to use lcm(nread,nwrite)
	// (avoiding modulo addressing) unless it is very
	// wasteful.  Heuristic: OK if <= wasteFactor() times the
	// sum of nread and nwrite.
	// example: 5->6 gives 11; 30 > 2*11 so 11 is used.
	// 2->n, n odd, gives n+2; 2n < 2*(n+2) so 2n is used.
	// to force use of lcm, make wasteFactor huge; to force
	// minimum memory, make wasteFactor 1.

	int nread = dest->numXfer();
	int nwrite = originatingPort->numXfer();
	int lcmValue = lcm(nread,nwrite);
	if (lcmValue >= bsiz && lcmValue <= wasteFactor() * (nread+nwrite))
			return lcmValue;
	else return bsiz;
}

// default waste factor.
double CGGeodesic::wasteFactor() const { return 2.0;}

