#ifndef _CGCPortHole_h
#define _CGCPortHole_h 1
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
#pragma interface
#endif

#include "CGPortHole.h"

// three different types of buffer.
// If owner, this porthole owns buffer.
// If embedded, no physical buffer is assigned to that porthole.
enum BufType { NA, OWNER, EMBEDDED };

class CGCGeodesic;

class CGCPortHole : public CGPortHole {
friend class ForkDestIter;
public:
	CGCPortHole() : maxBuf(1), manualFlag(0), asLinearBuf(1), bufName(0),
			hasStaticBuf(1), myType(NA), converted(-1) {}
	~CGCPortHole();

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
	// return numTokens if on the wormhole boundary
	int maxBufReq() const;

	// return TRUE if a star can see this port as a linear buffer.
	int linearBuf() const { return asLinearBuf; }

	// return TRUE if the actual static buffering achieved.
	int staticBuf() const { return hasStaticBuf; }
	// give up static buffering for loop scheduling
	void giveUpStatic() { hasStaticBuf = FALSE; }

	// set and get the buffer type
	void setBufferType();

	BufType  bufType() const;

	// name the porthole in the data structure.
	void setGeoName(char* n);
	const char* getGeoName() const;

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

	// determine the buffer size for this porthole finally
	// Consider delay and need of past particles.
	void finalBufSize(int statBuf);

	// Set the maxBuf manually. In finalBufSize() method, we will
	// compare this manual value with what the current scheduler
	// calculates. If this manual value is smaller, signal an error.
	void requestBufSize(int i) { maxBuf = i; manualFlag = TRUE; }

	// check whether automatic type conversion should be performed.
	// Currently, Complex and float/int type data are converted.
	int isConverted();
	
	// allocate a geodesic
	/* virtual */ Geodesic* allocateGeodesic();

	// determine the property of the buffer associated with this port.
	void setFlags();

	// set output offset manually
	void setOffset(int v) { manualOffset = v; }

private:
	int maxBuf;		// Final buffer size.
	short manualFlag;	// set if buffer size is manually chosen.
	short hasStaticBuf;	// set if static buffer is achieved.
	short asLinearBuf;	// set if acts as a linear buf inside a star
	BufType myType;		// buffer Type.
	
	int converted;		// set TRUE if the explicit type conversion 
				// is required.

	char* bufName;		// set if no geodesic is assigned.

	SequentialList& myDest() { return forkDests; }
	
	int manualOffset;	// set the offset of the output manually.
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
	CGCPortHole* operator++ (POSTFIX_OP) { return next(); }
	ListIter::reset;
};

#endif
