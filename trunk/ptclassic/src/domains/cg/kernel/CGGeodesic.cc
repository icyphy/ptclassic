static const char file_id[] = "CGGeodesic.cc";
/******************************************************************
Version identification:
@(#)CGGeodesic.cc	1.17	1/1/96

Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

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
#include "SimControl.h"
#include "IntState.h"

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
		n += s->numInit();
		s = &p->cgGeo();
		if (s->sourcePort()->isItInput()) return n;
	}
	return n;
}

// minimum size needed considering only local information
int CGGeodesic :: minNeeded() const
{
	const CGPortHole* dest = (const CGPortHole*)destinationPort;
	int nOld = 0;
	int type = forkType();
	int maxN = maxNumParticles();
	int bufsize;
	if ((type & F_SRC) != 0) {
		// consider past sample access.
		CGPortHole* dPort = (CGPortHole*)destinationPort;
		ListIter next(dPort->forkDests);
		CGPortHole* p;
		while ((p = (CGPortHole*)next++) != 0) {
			CGGeodesic* g = (CGGeodesic*) p->geo();
			int temp = g->minNeeded();
	//		int temp = p->maxDelay() + 1 - p->numXfer() + 
	//			p->numInitDelays();
			if (temp > maxN) maxN = temp;
		}
		bufsize = maxN;
	} else {
		nOld = max(dest->maxDelay() + 1 - dest->numXfer(),0);
		bufsize = maxN+nOld;
	}
	return bufsize;
}

// recursive function to compute buffer and forkbuf sizes.  Note that
// buffers are only actually allocated for types 0 and F_SRC.

// We return 0 if the schedule has not yet been run, and also print an
// error message.

int CGGeodesic :: internalBufSize() const {
	CGPortHole* dest = (CGPortHole*)destinationPort;
	int drep = dest->parentReps();
	if (dest->atBoundary()) drep = 1;
	if (drep == 0) {
		Error::abortRun(*dest,
"Attempt to determine buffer size before schedule has been computed\n"
"(possibly from calling bufSize() in a star's setup() method)");
		return 1;
	}
	int bsiz = minNeeded();
	int type = forkType();
	if ((type & F_SRC) != 0) {
		// size must be >= max of all destbuf sizes.
		CGPortHole* dPort = (CGPortHole*)destinationPort;
		ListIter next(dPort->forkDests);
		CGPortHole* p;
		while ((p = (CGPortHole*)next++) != 0) {
			bsiz = max(bsiz,p->cgGeo().internalBufSize());
			if (SimControl::haltRequested()) return FALSE;
		}
	}
// if there are delays or old values are used, it may be better to
// use a larger size so that compile-time indexing is supportable.
// The buffer size must divide the total number of tokens produced in
// one execution.
	if (numInit() > 0 || dest->usesOldValues()) {
		// cannot determine size without schedule.
		if (maxNumParticles() == 0) return 0;
		int total = drep * dest->numXfer();
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


// Prevent delay warning messages from being repeated.
static int alreadyWarned = FALSE;

// Reset the alreadyWarned flag.
void CGGeodesic::initialize()
{
    Geodesic::initialize();
    alreadyWarned = FALSE;
}

// Helper function for setDelay() and setSourcePort().
void checkNumDelays(const char* values)
{
    if (values && *values)
    {
	StringList msg;

	if (*values == '*')
	{
	    msg << "Use of variable delays may result in the generation of incorrect code.\n"
	        << "Please use a constant expression for the number of delays.";
	}
	else
	{
	    msg << "Use of initializable delays may result in the generation of incorrect code.\n"
		<< "Please use non-initializable delays.";
	}

	if (!alreadyWarned) Error::warn(msg);
	alreadyWarned = TRUE;
    }
}

void CGGeodesic::setDelay(int num, const char* values)
{
    checkNumDelays(values);
    Geodesic::setDelay(num, values);
}

PortHole* CGGeodesic::setSourcePort(GenericPort& port, int num, const char* values)
{
    checkNumDelays(values);
    return Geodesic::setSourcePort(port, num, values);
}
