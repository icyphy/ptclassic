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

#include "AsmTarget.h"
#include "ProcMemory.h"
#include "StringState.h"
#include "IntState.h"

class CG56Memory : public DualMemory {
public:
	CG56Memory(const char* x_map, const char* y_map);
	CG56Memory(const CG56Memory& arg) : DualMemory(arg) {}
};

class CG56Target : public AsmTarget {
private:
	NestedSymbol targetNestedSymbol;
	void initStates();
protected:
	StringState xMemMap;
	StringState yMemMap;

	void codeSection();
	void orgDirective(const char* memName, unsigned addr);
	void writeInt(int);
	void writeFix(double);
	void writeFloat(double);
	void disableInterrupts();
	void enableInterrupts();
	void saveProgramCounter();
	void restoreProgramCounter();

	IntState disCode;
	IntState runCode;
	int inProgSection;
	StringState targetHost;
	char* uname;

	virtual void initializeCmds();
	virtual char downloadCmdFlag() const{return '!';}
	virtual char miscCmdFlag() const{return '@';}
	StringList downloadCmds;
	StringList assembleCmds;
	StringList miscCmds;
	virtual const char* asmSuffix() const {return ".asm";}
public:
	CG56Target (const char* nam, const char* desc);
	// copy constructor
	CG56Target(const CG56Target&);
	Block* clone() const;
	void headerCode();
	int setup(Galaxy &g);
	void wrapup();
	void beginIteration(int repetitions, int depth);
	void endIteration(int repetitions, int depth);
	void addCode(const char*);
	virtual int assembleCode();
	virtual int downloadCode();
	const char* readClassName() const{return "CG56Target";}
	~CG56Target();
};

#endif
