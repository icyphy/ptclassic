#ifndef _MotorolaTarget_h
#define _MotorolaTarget_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck and J. Pino

 Base target for Motorola DSP assembly code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "AsmTarget.h"
#include "ProcMemory.h"
#include "StringState.h"
#include "MotorolaAttributes.h"

class MotorolaMemory : public DualMemory {
public:
	MotorolaMemory(const char* x_map, const char* y_map);
	MotorolaMemory(const MotorolaMemory& arg) : DualMemory(arg) {}
};

class MotorolaTarget : public AsmTarget {
private:
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

	int inProgSection;
public:
	MotorolaTarget(const char* nam, const char* desc, const char* stype);
	// copy constructor
	MotorolaTarget(const MotorolaTarget&);
	Block* clone() const;
	int setup(Galaxy &g);
	void wrapup();
	void beginIteration(int repetitions, int depth);
	void endIteration(int repetitions, int depth);
	const char* readClassName() const{return "MotorolaTarget";}
	virtual double limitFix(double val) { 
		return (val==1 ? 1.0 - 1.0/double(1<<23) : val);
	}
	~MotorolaTarget();
};

#endif
