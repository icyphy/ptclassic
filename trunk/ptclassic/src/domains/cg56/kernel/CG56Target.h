#ifndef _CG56Target_h
#define _CG56Target_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck

 Base target for Motorola 56000 assembly code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "AsmTarget.h"
#include "ProcMemory.h"
#include "StringState.h"

class CG56Memory : public DualMemory {
public:
	CG56Memory(const char* x_map, const char* y_map);
	CG56Memory(const CG56Memory& arg) : DualMemory(arg) {}
};

class CG56Target : public AsmTarget {
private:
	void initStates();
protected:
	StringState xMemMap;
	StringState yMemMap;
public:
	CG56Target (const char* nam, const char* desc);
	// copy constructor
	CG56Target(const CG56Target&);
	Block* clone() const;
	void headerCode();
	void wrapup();
	int setup(Galaxy&);
	StringList beginIteration(int repetitions, int depth);
	StringList endIteration(int repetitions, int depth);
	~CG56Target();
protected:
	void codeSection();
	void orgDirective(const char* memName, unsigned addr);
	void writeInt(int);
	void writeFix(double);
	void writeFloat(double);

	int inProgSection;
};

#endif
