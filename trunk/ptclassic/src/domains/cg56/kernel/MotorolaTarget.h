#ifndef _MotorolaTarget_h
#define _MotorolaTarget_h 1
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

 Programmer: J. Buck, J. Pino, and T. M. Parks

 Base target for Motorola DSP assembly code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "AsmTarget.h"
#include "ProcMemory.h"
#include "StringState.h"
#include "IntState.h"
#include "StringList.h"

class MotorolaMemory : public DualMemory {
public:
	MotorolaMemory(const char* x_map, const char* y_map);
	MotorolaMemory(const MotorolaMemory& arg) : DualMemory(arg) {}
};

class MotorolaTarget : public AsmTarget {
public:
	MotorolaTarget(const char* nam, const char* desc, const char* stype);
	// copy constructor
	MotorolaTarget(const MotorolaTarget&);
	Block* makeNew() const;
	void setup();
	void beginIteration(int repetitions, int depth);
	void endIteration(int repetitions, int depth);
	virtual double limitFix(double val);
	/*virtual*/ StringList comment(const char*,const char*,const char*,const char*);
	/*virtual*/ void writeFiring(Star&,int);

#ifdef __GNUG__
	// Workaround a bug in gcc-2.6.0.  Otherwise Sim56Target.cc 
	// won't compile
	void trailerCode() { CGTarget::trailerCode();}
#endif

	int computeMemoryUsage();
	inline int programMemoryUsage() { return ProgramDataMemory; }
	inline int xdataMemoryUsage() { return XDataMemory; }
	inline int ydataMemoryUsage() { return YDataMemory; }
	const char* memoryUsageString();
	void resetMemoryUsage();

	inline const char* setAssemblerOptions(const char* options) {
		assemblerOptions = options;
		return assemblerOptions;
	}
	inline const char* getAssemblerOptions() {
		return assemblerOptions;
	}

	~MotorolaTarget();

protected:
	// Target parameters (states)
	StringState xMemMap;
	StringState yMemMap;
	IntState reportMemoryUsage;
	IntState subFire;	// Write star firings as subroutine calls.

#ifdef __GNUG__
	// Workaround a bug in gcc-2.6.0.  Otherwise Sim56Target.cc 
	// won't compile
	Block* copyStates(const Block& arg) { return Block::copyStates(arg);}
#endif

	/*virtual*/ void tailerCode();
	void codeSection();
	void orgDirective(const char* memName, unsigned addr);
	void writeInt(int);
	void writeFix(double);
	void writeFloat(double);
	void disableInterrupts();
	void enableInterrupts();
	void saveProgramCounter();
	void restoreProgramCounter();

	// Other data members
	int inProgSection;
	int ProgramDataMemory;
	int XDataMemory;
	int YDataMemory;
	StringList MemoryUsageString;
	StringList assemblerOptions;

private:
	void initStates();
};

#endif
