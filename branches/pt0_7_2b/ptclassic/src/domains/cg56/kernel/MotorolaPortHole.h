#ifndef _MotorolaConnect_h
#define _MotorolaConnect_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck

 These classes are portholes for stars that generate assembly language code.  

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif
#include "SDFConnect.h"
#include "AsmConnect.h"
#include "MotorolaAttributes.h"

class ProcMemory;
class AsmGeodesic;

class MotorolaPortHole : public AsmPortHole {
public:
	// constructor sets default attributes.
	MotorolaPortHole();
};

class InMotorolaPort : public InAsmPort {
public:
	int isItInput() const;
};

class OutMotorolaPort: public OutAsmPort {
public:
	int isItOutput() const;
};

// is anything here?
class MultiMotorolaPort : public MultiAsmPort {
public:
	int someFunc();
};

class MultiInMotorolaPort : public MultiInAsmPort {
public:
	int isItInput() const;
	PortHole& newPort();
};

class MultiOutMotorolaPort : public MultiOutAsmPort {
public:
	int isItOutput() const;
	PortHole& newPort();
};

#endif
