#ifndef _CG56Target_h
#define _CG56Target_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck and J. Pino

 Base target for Motorola 56000 assembly code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "MotorolaTarget.h"
#include "ProcMemory.h"
#include "StringState.h"
#include "IntState.h"
#include "FixState.h"

extern StringList CG56ONE;

class CG56Target : public virtual MotorolaTarget {
protected:
	void writeFloat(double);
	FixState ONE;
public:
	CG56Target (const char* nam, const char* desc) :
		MotorolaTarget(nam,desc,"CG56Star") {}
	// copy constructor
	CG56Target(const CG56Target& src) : 
	  MotorolaTarget(src.readName(),src.readDescriptor(),"CG56Star") {}
	int setup(Galaxy &g);
	Block* clone() const;
	void headerCode();
	const char* readClassName() const{return "CG56Target";}
};

#endif
