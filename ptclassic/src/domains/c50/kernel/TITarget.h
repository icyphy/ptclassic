#ifndef _TITarget_h
#define _TITarget_h 1
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-1994 The Regents of the University of California.
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

Programmer; Andreas Baensch
Date of creation: 30 April 1995

Based on code by J. Buck, J. Pino, and T. M. Parks. and A. Baensch

Base target for TI DSP assembly code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "AsmTarget.h"
#include "ProcMemory.h"
#include "StringState.h"

class TIMemory : public DualMemory {
public:
	TIMemory(const char* b_map, const char* u_map);
	TIMemory(const TIMemory& arg) : DualMemory(arg) {}
};

class TITarget : public AsmTarget {
public:
	TITarget(const char* nam, const char* desc, const char* stype);
	// copy constructor
	TITarget(const TITarget&);
	Block* makeNew() const;
	void setup();
	void headerCode();
	void beginIteration(int repetitions, int depth);
	void endIteration(int repetitions, int depth);
	virtual double limitFix(double val);
	/*virtual*/ StringList comment(const char*,const char*,const char*,const char*);
	/*virtual*/ void writeFiring(Star&,int);

//FIXME
//#ifdef __GNUG__
	// Workaround a bug in gcc-2.6.0.  Otherwise DSK320Target.cc 
	// won't compile
	//void trailerCode() { CGTarget::trailerCode();}
//#endif

	~TITarget();
protected:
	StringState bMemMap;
	StringState uMemMap;

	// Write star firings as subroutine calls.
	IntState subFire;

#ifdef __GNUG__
	// Workaround a bug in gcc-2.6.0.  Otherwise DSK320Target.cc 
	// won't compile
	Block* copyStates(const Block& arg) { return Block::copyStates(arg);}
#endif

	void trailerCode();
	void codeSection();
	void orgDirective(const char* memName, unsigned addr);
	void writeInt(int);
	void writeFix(double);
	void writeFloat(double);
	void disableInterrupts();
	void enableInterrupts();
	void frameCode();
	int inProgSection;
private:
	void initStates();
};

#endif
