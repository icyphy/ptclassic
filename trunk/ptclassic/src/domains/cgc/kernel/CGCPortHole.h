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
public:
	void setGeoName(char* n) const;
	const char* getGeoName();

	// Return the geodesic connected to this PortHole.
	// This is typesafe because allocateGeodesic
	// makes myGeodesic of this type.
	CGCGeodesic& geo() const { return *(CGCGeodesic*)myGeodesic;}
};

class InCGCPort : public InCGPort {
public:
	int isItInput() const;
};

class OutCGCPort: public OutCGPort {
public:
	int isItOutput() const;
};

// is anything here?
class MultiCGCPort : public MultiCGPort {
public:
	int someFunc();
};

class MultiInCGCPort : public MultiInCGPort {
public:
	int isItInput() const;
	PortHole& newPort();
};

class MultiOutCGCPort : public MultiOutCGPort {
public:
	int isItOutput() const;
	PortHole& newPort();
};

#endif
