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
	SequentialList spliceList;
protected:
	ProcMemory* mem;

	// what it says
	int allocateMemory(Galaxy&);

	// redefine init routine for code generation
	int codeGenInit(Galaxy&);

	// Request memory for all structures in a Star
	int allocReq(AsmStar&);

	// make modifications to interface circular to linear buffers
	// and handling of long delays.
	int modifyGalaxy(Galaxy&);

	// splice in a new star, returning a pointer to its input
	// porthole.  Spliced stars are assumed to have one input
	// named "input" and one output named "output".
	PortHole* spliceStar(PortHole*, const char*, int delayBefore);
       
	void doInitialization(CGStar&);

public:
	AsmTarget(const char* nam, const char* desc,
		  const char* stype, ProcMemory* m = 0) :
		CGTarget(nam,stype,desc), mem(m) {}

	~AsmTarget();

	Block* clone() const = 0;

	// output a directive that switches to the code section
	virtual void codeSection() = 0;

	// generate file of stuff named base.suffix.  If mode is
	// specified, the mode of the file generated is set to 'mode.'
	int genFile(StringList& stuff, char* base,
		    const char* suffix=NULL,const char* mode=NULL);

	// generate file of stuff named base.suffix and display it.
	// If mode is specified, the mode of the file generated is 
	// set to 'mode.'
	int genDisFile(StringList& stuff,char* base,
		       const char* suffix=NULL,const char* mode=NULL);
  
	// Return full file name including path.  
	// NOTE: User must delete the char* returned after use.
	char* fullFileName(char* base, const char* suffix=NULL);

	// Return file name w/o path.  If the char* returned is not
	// for immediate use, the calling function must copy the string
	// into another location.
	char* fileName(char* base, const char* suffix=NULL);

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
