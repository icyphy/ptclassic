#ifndef _CGGeodesic_h
#define _CGGeodesic_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck
 Modified by: E. A. Lee

 geodesics for CGCode stars.  Their main role is to figure out how
 big a buffer must be allocated in code generation.

 There is also support for fork buffers -- an attempt to allow all
 the buffers of a fork star to be allocated as only a single buffer.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "CGPortHole.h"
#include "Geodesic.h"
#include <minmax.h>

const int F_SRC = 1;
const int F_DEST = 2;

class CGGeodesic : public Geodesic {
public:
	CGGeodesic () : maxNumParticles(0) {}
	void initialize();
	void incCount(int);

	// class identification
	int isA(const char*) const;

	// return the size of the buffer itself (zero is returned for
	// fork output geodesics)
	int localBufSize() const;

	// return the size of the buffer, or the size of the associated
	// fork buffer for fork destinations.
	int bufSize() const;

	// return my "fork type"
	int forkType() const;

	// return the value of any delay on the outputs of forks --
	// return zero if not associated with a fork buffer.
	int forkDelay() const;

	// return the maxNumParticles
	int getMaxNum() { return maxNumParticles; }

protected:
	CGPortHole* src() {
		return ((CGPortHole*)originatingPort)->forkSrc;
	}
	const CGPortHole* src() const {
		return ((const CGPortHole*)originatingPort)->forkSrc;
	}
	// minimum size needed considering only local information
	int minNeeded() const {
		const CGPortHole* dest = (const CGPortHole*)destinationPort;
		int nOld = max(dest->maxDelay() + 1 - dest->numXfer(),0);
		return maxNumParticles + nOld;
	}
	// this function returns a value giving the maximum fraction of
	// memory internalBufSize should "waste" to arrange for linear
	// buffering between stars that do a non-integral rate change
	// without delay.  Setting it to 1.0 forces the use of the minimum
	// possible size.  Setting it huge means that, say, a 41->42
	// connection allocates a buffer length of 41*42.
	virtual double wasteFactor() const;

	// internal buffer size -- used by bufSize.  Main difference is
	// that all arcs of a fork have an internal size, but bufSize
	// returns 0 for all but the source arc.
	virtual int internalBufSize() const;
private:
	int maxNumParticles;
	SequentialList dests;
};

#endif
