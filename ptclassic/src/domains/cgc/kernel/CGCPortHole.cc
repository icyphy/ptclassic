static const char file_id[] = "CGCConnect.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: E. A. Lee and Soonhoi Ha

 These classes are portholes for stars that generate C code.  

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "CGCConnect.h"
#include "CGCGeodesic.h"
#include "SDFStar.h"
#include "Error.h"

#define SETFLAGS(p) \
	if ((p->asLinearBuf == TRUE) && (maxBuf % p->numXfer() != 0)) \
		p->asLinearBuf = FALSE; \
	if (p->asLinearBuf == FALSE) p->hasStaticBuf = FALSE; \
	else if (maxBuf != p->numXfer() * ((SDFStar*) p->parent())->reps()) \
		p->hasStaticBuf = FALSE; 

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
	if (!del) del = numTokens();
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

// Decide the buffer size finally
// reserve the buffer for initial offsets which the farSide input port
// will read.
void CGCPortHole :: finalBufSize(int statBuf) {
	if (isItInput()) return;

	int reqSize = cgGeo().getMaxNum();
	int staticSize = localBufSize();
	int tryFlag = TRUE;	// try to achieve static buffering

	if (far()->isItOutput()) {
		maxBuf = staticSize;
		return;	// check wormhole boundary.
	}

	if (bufferSize > numXfer()) {
		asLinearBuf = FALSE;
		tryFlag = FALSE;
		if (reqSize < bufferSize) reqSize = bufferSize;
	}

	CGCPortHole* p = realFarPort();
	if (p->fork()) {
		// determine the maximum offset.
		ForkDestIter next(p);
		CGCPortHole* outp;
		while ((outp = next++) != 0) {
			CGCPortHole* inp = outp->realFarPort();
			int myTry = TRUE;
			// access to the past Particles
			int temp = inp->inBufSize() - inp->numXfer();
			if (temp) myTry = FALSE;
			// delays
			temp += inp->cgGeo().forkDelay();
			if (temp % inp->numXfer() != 0) myTry = FALSE;

			temp += inp->cgGeo().getMaxNum();
			if (temp > reqSize) reqSize = temp;

			// determine whether p can be of static buffering
			if (!myTry) {
				tryFlag = FALSE;
				inp->asLinearBuf = FALSE;
			}
		}
	} else {
		int temp = p->inBufSize() - p->numXfer();
		if (temp || (p->numTokens() % p->numXfer() != 0)) { 
			tryFlag = FALSE;
			p->asLinearBuf = FALSE;
		}
		reqSize += temp;
	}

	// check whether this size is set manually or not.
	// If yes, range check.
	if (manualFlag) {
		if (maxBuf < reqSize) {
			Error::warn(*this, "buffer request is too small.");
			maxBuf = reqSize;
		}
	} else if (statBuf == 0) {
		maxBuf = reqSize;
	} else { // static buffering option.
		if (tryFlag && (reqSize % staticSize != 0)) {
			maxBuf = ((reqSize/staticSize)+1) * staticSize;
		} else if ((asLinearBuf != 0) && (reqSize % numXfer() != 0)) {
			maxBuf = ((reqSize / numXfer()) + 1) * numXfer();
		} else {
			maxBuf = reqSize;
		}
	}

	// set the static buffering option and offserReq flag
	SETFLAGS(this)

	// confirm the asLinearBuf flag of fork-destination ports
	if (p->fork()) {
		// determine the maximum offset.
		ForkDestIter next(p);
		CGCPortHole* outp;
		while ((outp = next++) != 0) {
			CGCPortHole* inp = outp->realFarPort();
			SETFLAGS(inp);
		}
	} else {
		SETFLAGS(p);
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
	geo().setBufName(n);
}

const char* CGCPortHole :: getGeoName() const {
	return geo().getBufName();
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
