static const char file_id[] = "AsmGeodesic.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY

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

inline int hasCirc(const PortHole* p) {
	return (p->attributes() & PB_CIRC) != 0;
}

// recursive function to compute buffer and forkbuf sizes.  Note that
// buffers are only actually allocated for types 0 and F_SRC.  For others,
// the return value represents a partial result, since we must compute
// the LCM of all reader and writer values of numberTokens.
int AsmGeodesic :: internalBufSize() const {
	int size = CGGeodesic::internalBufSize();
	if (size == 0) return 0; // scheduler has not been run, indeterminate.
	CGPortHole* dest = (CGPortHole*)destinationPort;
	CGPortHole* src = (CGPortHole*)originatingPort;
	if ((numInit() > 0 || dest->usesOldValues()) &&
	    !hasCirc(dest) && !hasCirc(src)) {
		int total = src->parentReps() * src->numXfer();
		if (total < size)
			Error::abortRun (*destinationPort,
		 "maximum buffer size exceeds number of particles produced\n",
			 "in one schedule iteration (not yet supported)");
	}
	return size;
}

// allocation control heuristic.  We prefer the smallest possible
// memory if each end has P_CIRC; otherwise, we always want linear
// memory.
double AsmGeodesic :: wasteFactor() const {
	if (hasCirc(originatingPort) && hasCirc(destinationPort))
		return 1.0;
	else return 1.0e9;
}
