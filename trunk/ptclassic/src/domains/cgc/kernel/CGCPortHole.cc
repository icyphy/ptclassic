static const char file_id[] = "CGCPortHole.cc";
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

 Programmer: S. Ha and E. A. Lee

 These classes are portholes for stars that generate C code.  

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "CGCPortHole.h"
#include "CGCGeodesic.h"
#include "SDFStar.h"
#include "Error.h"

CGCPortHole :: ~CGCPortHole() {
	LOG_DEL; delete bufName;
}

void CGCPortHole :: initialize() {
	CGPortHole :: initialize();

	// member initialize.
	myType = NA;
	hasStaticBuf = TRUE;
	asLinearBuf = TRUE;
	manualFlag = FALSE;
	maxBuf = 1;
}

// allocate a CGCGeodesic.  Use hashstring for the name since we expect
// repeated names to occur (lots of Node_input and Node_output, for example)
Geodesic* CGCPortHole::allocateGeodesic() {
	char nm[80];
	strcpy (nm, "Node_");
	strcat (nm, name());
	LOG_NEW; Geodesic *g = new CGCGeodesic;
	g->setNameParent(hashstring(nm), parent());
	return g;
}

void CGCPortHole::setFlags() {
	if (bufSize() % numXfer() != 0)
		asLinearBuf = FALSE;
	if ((numXfer() * parentReps()) % bufSize() != 0)
		hasStaticBuf = FALSE; 
}

BufType CGCPortHole::bufType() const { 
	if (switched()) {
		CGCPortHole* cp = (CGCPortHole*) cgGeo().sourcePort();
		return cp->bufType();
	}
	if ((far() == 0) || atBoundary()) return myType;
	return isItOutput()? myType: realFarPort()->bufType(); 
}

		////////////////////////////////////////
		// Buffer size determination routines
		////////////////////////////////////////

// initialize the offset member. If there is no fork involved, output
// porthole start writing from offset 0, and the input porthole
// start reading from the (maxBuf - offset).
int CGCPortHole :: initOffset() {
	if (isItOutput()) {
		offset = numXfer() - 1;
		return TRUE;
	}
	int del = cgGeo().forkDelay();
	if (!del) del = numInitDelays();
	if (!del) offset = numXfer() - 1;
	else {
		offset = numXfer() - del - 1;
		if (offset < 0)  offset += maxBufReq();
	}
	if (del > maxBufReq()) {
		Error :: abortRun(*this, " delay is too large\n");
		return FALSE;
	}
	return TRUE;
}

// return the buffer requirements. Indicates whether static buffering
// is achieved or not.
// return 1 if on the wormhole boundary
int CGCPortHole :: maxBufReq() const {
	if (switched()) {
		CGCPortHole* cp = (CGCPortHole*) cgGeo().sourcePort();
		return cp->maxBufReq();
	}
	if ((far() == 0) || atBoundary()) return maxBuf;
	return isItOutput()? maxBuf: realFarPort()->maxBufReq();
}

// Decide the buffer size finally
// reserve the buffer for initial offsets which the farSide input port
// will read.
void CGCPortHole :: finalBufSize(int statBuf) {
	if (isItInput()) return;
	else if (far() == 0) {
		maxBuf = numXfer();
		return;
	}

	if (far()->isItOutput()) {
		maxBuf = localBufSize();
		return; // check wormhole boundary.
	}

	// Try best to realize Linear or static buffering.
	// Look at CGGeodesic.cc to see the actual code for buffer size
	// determination.
	if (usesOldValues()) {
		// if past values are used. give up linear buffering.
		asLinearBuf = FALSE;
		statBuf = 0;
	}
	geo().preferLinearBuf(statBuf);
	int reqSize = localBufSize();

	// check whether this size is set manually or not.
	// If yes, range check.
	if (manualFlag) {
		if (maxBuf < reqSize) {
			Error::warn(*this, "buffer request is too small.");
			maxBuf = reqSize;
		}
	} else { // static buffering option.
		if (asLinearBuf && (reqSize % numXfer() != 0)) {
			// for maximum chance of linear buffering.
			maxBuf = ((reqSize/numXfer())+1) * numXfer();
		} else {
			maxBuf = reqSize;
		}
	}

	if ((!statBuf) && (maxBuf > numXfer()))
		giveUpStatic();

	// set the flags
	setFlags();
	CGCPortHole* p = realFarPort();
	if (p->fork()) {
		// determine the maximum offset.
		ForkDestIter next(p);
		CGCPortHole* outp;
		while ((outp = next++) != 0) {
			CGCPortHole* inp = outp->realFarPort();
			if ((!statBuf) && (maxBuf > inp->numXfer()))
				inp->giveUpStatic();
			inp->setFlags();
			// access to the past Particles
			if (inp->usesOldValues() ||
				(inp->numInitDelays() % inp->numXfer() != 0))
				inp->asLinearBuf = FALSE;
		}
	} else {
		if ((!statBuf) && (maxBuf > p->numXfer()))
			p->giveUpStatic();
		p->setFlags();
		if (p->usesOldValues() ||
			(p->numInitDelays() % p->numXfer() != 0)) {
			p->asLinearBuf = FALSE;
		}
	}
}

// setup ForkDests
void CGCPortHole :: setupForkDests() {
	SequentialList temp;
	temp.initialize();

	ForkDestIter next(this);
	CGCPortHole *outp, *inp;
	while ((outp = next++) != 0) {
		//  check wormhole boundary
		if (outp->far()->isItOutput()) continue;

		inp = outp->realFarPort();
		if (inp->fork()) temp.put(inp);
	}

	ListIter nextPort(temp);
	while ((inp = (CGCPortHole*) nextPort++) != 0) {
		inp->setupForkDests();
		forkDests.remove(inp->far());
		ForkDestIter realNext(inp);
		while ((outp = realNext++) != 0)
			forkDests.put(outp);
	}
}

// Need modification if we allow wormholes!!
CGCPortHole* CGCPortHole :: realFarPort() {
	CGCPortHole* p = (CGCPortHole*) far();
	if (p->getForkSrc()) return p->getForkSrc()->realFarPort();
	return p;
}

// const version: hate this duplication.
const CGCPortHole* CGCPortHole :: realFarPort() const {
	const CGCPortHole* p = (const CGCPortHole*) far();
	if (p->getForkSrc()) return p->getForkSrc()->realFarPort();
	return p;
}

void CGCPortHole :: setGeoName(char* n) {
	if (myGeodesic == 0) bufName = n;
	else geo().setBufName(n);
}

const char* CGCPortHole :: getGeoName() const {
	if (bufName) return bufName;
	return geo().getBufName();
}

// If type conversion between complex to float/int is required when
// it is an output.
int CGCPortHole :: isConverted(){
	if ((converted >= 0) || isItInput()) return converted;
	else if (strcmp(type(),resolvedType()) == 0) converted = FALSE;
	else if (strcmp(type(), ANYTYPE) == 0) converted = FALSE;
	else if ((strcmp(type(),COMPLEX) == 0) || 
	    (strcmp(type(),"COMPLEXARRAY") == 0)) converted = TRUE;
	else if ((strcmp(resolvedType(),COMPLEX) == 0) || 
	    (strcmp(resolvedType(),"COMPLEXARRAY") == 0)) converted = TRUE;
	else converted = FALSE;

	return converted;
}

const char* CGCPortHole :: getLocalGeoName() {
	if (bufName) return bufName;
	if (isItInput() || (isConverted() == FALSE))
		 return geo().getBufName();
	StringList temp = geo().getBufName();
	temp << "_local";
	return (const char*) temp;
}
	
// Dummy
int MultiCGCPort :: someFunc() { return 1; }

int InCGCPort :: isItInput() const { return TRUE; }
int OutCGCPort :: isItOutput() const { return TRUE; }
int MultiInCGCPort :: isItInput() const { return TRUE; }
int MultiOutCGCPort :: isItOutput() const { return TRUE; }

PortHole& MultiInCGCPort :: newPort () {
        LOG_NEW; InCGCPort* p = new InCGCPort;
        p->numberTokens = numberTokens;
	forkProcessing(*p);
        return installPort(*p);
}

PortHole& MultiOutCGCPort :: newPort () {
	LOG_NEW; OutCGCPort* p = new OutCGCPort;
        p->numberTokens = numberTokens;
	forkProcessing(*p);
        return installPort(*p);
}
