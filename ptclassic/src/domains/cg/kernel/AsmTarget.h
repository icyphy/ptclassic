#ifndef _AsmTarget_h
#define _AsmTarget_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck, J.Pino

 Basic assembly language code Target for a single processor.
 Either an SDFScheduler or a LoopScheduler object can be used as
 the scheduler.  More specialized Targets (for a specific
 processor, etc) can be derived from this.

*******************************************************************/

#include "CGTarget.h"
#include "SDFScheduler.h"

class ProcMemory;
class AsmStar;

class AsmTarget : public CGTarget {
private:
	const char* starClass;
protected:
	ProcMemory* mem;

	// redefine buffer assignment
	int decideBufSize(Galaxy&);

	// redefine init routine for code generation
	int codeGenInit(Galaxy&);

	// Request memory for all structures in a Star
	int allocReq(AsmStar&);

	void doInitialization(CGStar&);

public:
	AsmTarget(const char* nam, const char* desc,
		  const char* stype, ProcMemory* m = 0) :
		CGTarget(nam,stype,desc), mem(m) {}

	Block* clone() const = 0;

	// output a directive that switches to the code section
	virtual void codeSection() = 0;

	// generate file of stuff named base.suffix
	int genFile(StringList& stuff, char* base,const char* suffix);

	// generate file of stuff named base.suffix and display it.
	int genDisFile(StringList& stuff, char* base,const char* suffix);

	// Return file name.  User must delete the char* returned after 
	// use.
	char* fullFileName(char* base, const char* suffix);

	// output an "org" directive that switches to the specified
	// memory and address
	virtual void orgDirective(const char* mem, unsigned addr) = 0;

	// output an integer
	virtual void writeInt (int) = 0;

	// output a fixed-point value, passed as a double argument.
	virtual void writeFix (double) = 0;

	// output a floating-point value.
	virtual void writeFloat (double) = 0;

	// output a comment.  Default form uses "outputLineOrientedComment"
	// to make comments beginning with semicolons.
	void outputComment (const char*);

	// useful function for comment generation
	void outputLineOrientedComment(const char* prefix,
				       const char* msg,
				       int lineLen);

	//Disable interrupts
	virtual void disableInterrupts();

	//Enable interrupts
	virtual void enableInterrupts();

	//Flag that is set to true when the target has generated interrupts
	int	interruptFlag = FALSE;

	//Save program counter
	virtual void saveProgramCounter();

	//restore program counter
	virtual void restoreProgramCounter();

	//Derived AsmTargets should call this headercode at the 
	//beginning to insure that interrupts are turned off
	//prior to all the header code and init code
	void headerCode() {disableInterrupts();}
};

#endif
