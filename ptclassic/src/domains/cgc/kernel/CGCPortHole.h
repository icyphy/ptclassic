#ifndef _CGCPortHole_h
#define _CGCPortHole_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: E. A. Lee, S. Ha

 These classes are portholes for stars that generate C code.  

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif
#include "SDFPortHole.h"
#include "CGPortHole.h"

// three different types of buffer.
// If owner, this porthole owns buffer.
// If embedded, no physical buffer is assigned to that porthole.
// If copied, two different set of buffer is assigned. It is needed when
// the output is embedding, and its far() requires past samples, or delays.
enum BufType { NA, OWNER, EMBEDDED, COPIED };

class CGCGeodesic;

class CGCPortHole : public CGPortHole {
friend class ForkDestIter;
public:
	CGCPortHole() : maxBuf(1), manualFlag(0), asLinearBuf(1), 
			hasStaticBuf(1), myType(NA), converted(-1) {}

	void initialize();

	CGCPortHole* getForkSrc() { return (CGCPortHole*) forkSrc; }

	// const version
	const CGCPortHole* getForkSrc() const {
		return (const CGCPortHole*) forkSrc;
	}

	// return the far port bypassing the fork stars
	CGCPortHole* realFarPort();
	// const version
	const CGCPortHole* realFarPort() const;

	// make a complete list of forkDests after resolving the indirect
	// list via fork stars
	void setupForkDests();

	// return the buffer requirements. Indicates whether static buffering
	// is achieved or not.
	// return 1 if on the wormhole boundary
	int maxBufReq() const;

	// return TRUE if a star can see this port as a linear buffer.
	int linearBuf() const { return asLinearBuf; }

	// return TRUE if the actual static buffering achieved.
	int staticBuf() const { return hasStaticBuf; }

	// set and get the buffer type
	void setBufType(BufType b) { myType = b; }
	BufType  bufType() const { 
		if (atBoundary()) return myType;
		return isItOutput()? myType: realFarPort()->bufType(); 
	}

	void setGeoName(char* n);
	const char* getGeoName() const;
	const char* getLocalGeoName();

	// Return the geodesic connected to this PortHole.
	// This is typesafe because allocateGeodesic
	// makes myGeodesic of this type.
	CGCGeodesic& geo() { return *(CGCGeodesic*)myGeodesic;}

	// const version
	const CGCGeodesic& geo() const {
		return *(const CGCGeodesic*)myGeodesic;}

	// buf Size
	int bufSize() const { return maxBufReq(); }

	// return bufferSize
	int inBufSize() const { return bufferSize; }

	// initialize offset
	int initOffset();

	// determine the buffer size finally
	void finalBufSize(int statBuf);

	// Set the maxBuf manually. In finalBufSize() method, we will
	// compare this manual value with what the current scheduler
	// calculates. If this manual value is smaller, signal an error.
	void requestBufSize(int i) { maxBuf = i; manualFlag = TRUE; }

	// check whether automatic type conversion should be performed.
	// Currently, Complex and float/int type data are converted.
	int isConverted();
	
protected:
	void setFlags();

private:
	int maxBuf;		// Final buffer size.
	short manualFlag;	// set if buffer size is manually chosen.
	short hasStaticBuf;	// set if static buffer is achieved.
	short asLinearBuf;	// set if acts as a linear buf inside a star
	BufType myType;		// buffer Type.
	
	int converted;		// set TRUE if the explicit type conversion 
				// is required.

	SequentialList& myDest() { return forkDests; }
};

class InCGCPort : public CGCPortHole {
public:
	int isItInput() const;
};

class OutCGCPort: public CGCPortHole {
public:
	int isItOutput() const;
};

// is anything here?
class MultiCGCPort : public MultiCGPort {
public:
	int someFunc();
};

class MultiInCGCPort : public MultiCGCPort {
public:
	int isItInput() const;
	PortHole& newPort();
};

class MultiOutCGCPort : public MultiCGCPort {
public:
	int isItOutput() const;
	PortHole& newPort();
};

class ForkDestIter : private ListIter {
public:
	ForkDestIter(CGCPortHole* p) : ListIter(p->myDest()) {}
	CGCPortHole* next() { return (CGCPortHole*) ListIter::next(); }
	CGCPortHole* operator++ () { return next(); }
	ListIter::reset;
};

#endif
