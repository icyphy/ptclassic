#ifndef _CG56PortHole_h
#define _CG56PortHole_h 1
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
#include "SDFPortHole.h"
#include "MotorolaPortHole.h"
#include "MotorolaAttributes.h"

class ProcMemory;
class AsmGeodesic;

#define CG56PortHole	MotorolaPortHole

class InCG56Port : public CG56PortHole {
	int isItInput() const;
};

class OutCG56Port: public CG56PortHole {
	int isItOutput() const;
};


#define MultiCG56Port	MultiMotorolaPort

class MultiInCG56Port : public MultiMotorolaPort {
public:
	PortHole& newPort();
	int isItInput() const;
};

class MultiOutCG56Port : public MultiMotorolaPort {
public:
	PortHole& newPort();
	int isItOutput() const;
};

#endif
