#ifndef _CGCConnect_h
#define _CGCConnect_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: E. A. Lee

 These classes are portholes for stars that generate C code.  

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif
#include "SDFConnect.h"
#include "CGConnect.h"
#include "CGCGeodesic.h"

class CGCPortHole : public CGPortHole {
friend class ForkDestIter;
public:
	CGCPortHole() : maxBuf(1), prevOffset(0) {}

	CGCPortHole* getForkSrc() { return (CGCPortHole*) forkSrc; }

	// return the far port bypassing the fork stars
	CGCPortHole* realFarPort();

	// make a complete list of forkDests after resolving the indirect
	// list via fork stars
	void setupForkDests();

	// return the buffer requirements for static buffering.
	int maxBufReq() 
		{ return isItOutput()? maxBuf: realFarPort()->maxBufReq(); }

	void setGeoName(char* n) const;
	const char* getGeoName();

	// Return the geodesic connected to this PortHole.
	// This is typesafe because allocateGeodesic
	// makes myGeodesic of this type.
	CGCGeodesic& geo() const { return *(CGCGeodesic*)myGeodesic;}

	// buf Size
	int bufSize() { return maxBufReq(); }

	// return bufferSize
	int inBufSize() { return bufferSize; }

	// initialize offset
	int initOffset();

	// set and get prevOffset
	void setPrevOffset() { prevOffset = bufPos(); }
	int wrapAround() { return (bufPos() < prevOffset); }

	// determine the buffer size finally
	void finalBufSize();

private:
	int maxBuf;
	int prevOffset;

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
