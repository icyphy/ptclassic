/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

 Programmer:  J. Buck, E. A. Lee

Code for CG-type portholes and multiportholes.

**************************************************************************/

static const char file_id[] = "CGPortHole.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "Block.h"
#include "CGPortHole.h"
#include "CircularBuffer.h"
#include "CGGeodesic.h"


// constructor
CGPortHole :: CGPortHole() : offset(0), forkSrc(0), embeddedPort(0),
	embeddedLoc(-1), embeddingFlag(0), switchFlag(0) {}

// destructor: remove forklist references.
CGPortHole :: ~CGPortHole() {
	ListIter next(forkDests);
	OutCGPort* p;
	while ((p = (OutCGPort*)next++) != 0) p->setForkSource(0);

	if (forkSrc) forkSrc->forkDests.remove(this);

	// If myGeodesic is switched, the pointer is set to zero to prevent
	// deleting the same geodesic multiple times.  Make sure that
	// original geodesic is destroyed when switching geodesics.
	if (switched()) myGeodesic = 0;
}

// Advance the offset by the number of tokens produced or
// consumed in this PortHole when the Star fires.
void CGPortHole::advance() {
	offset += numberTokens;
	int sz = bufSize();
	if (offset >= sz) offset -= sz;
}

// make me a fork destination; set my source.
void CGPortHole::setForkSource(CGPortHole* p) {
	// set new forkSrc
	forkSrc = p;
	// add me as one of forkSrc's destinations
	if (forkSrc) forkSrc->forkDests.put(this);
}

// processing for each port added to a fork buffer
// call this from OutXXX
void MultiCGPort :: forkProcessing (CGPortHole& p) {
	if (forkSrc) p.setForkSource(forkSrc);
}

// this avoids having cfront generate many copies of the destructor
MultiCGPort :: ~MultiCGPort() {}

int CGPortHole :: bufSize() const {
	return atBoundary() ? (parentReps()*numberTokens) : cgGeo().bufSize();
}

int CGPortHole :: localBufSize() const {
	return atBoundary() ? (parentReps()*numberTokens) : cgGeo().localBufSize();
}

int InCGPort :: isItInput () const { return TRUE; }
int OutCGPort :: isItOutput () const { return TRUE; }
int MultiInCGPort :: isItInput () const { return TRUE; }
int MultiOutCGPort :: isItOutput () const { return TRUE; }

PortHole& MultiInCGPort :: newPort () {
	LOG_NEW; CGPortHole& p = *new InCGPort;
	p.setSDFParams(numberTokens,numberTokens-1);
	return installPort(p);
}

PortHole& MultiOutCGPort :: newPort () {
	LOG_NEW; CGPortHole& p = *new OutCGPort;
	p.setSDFParams(numberTokens,numberTokens-1);
	return installPort(p);
}
