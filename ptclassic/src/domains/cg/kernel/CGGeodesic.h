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

#include "CGConnect.h"
#include "Geodesic.h"

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
	int maxNumParticles;
	virtual int internalBufSize() const;
private:
	SequentialList dests;
};

#endif
