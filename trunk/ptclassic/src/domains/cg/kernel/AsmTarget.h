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

#ifdef __GNUG__
#pragma interface
#endif

#include "CGTarget.h"
#include "IntState.h"
#include "StringState.h"
#include "SDFScheduler.h"

class ProcMemory;
class AsmStar;
class AsmTargetPtr;

class AsmTarget : public CGTarget {
private:
	SequentialList spliceList;
	void initStates();
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
       
	virtual void doInitialization(CGStar&);

	// host through which the target should be accessed.
	StringState targetHost;

	// if state displayFlag=TRUE then display generated code.
	IntState displayFlag;

	// if state runFlag=TRUE compile & run code
	IntState runFlag;		

	// 'uname' points to the universe name in all lowercase letters.
	// This is typically used for the base name for a file produced
	// by the target
	char* uname;

	// The filename for the assembly code produced is specified 
	// by 'uname'asmSuffix().
	virtual const char* asmSuffix() const {return ".asm";}

	// 'runCmds' stores commands that are to be executed at run
	// time.  It's use is strictly defined in the child domain's
	// target & stars.  'miscCmds' stores misc commands specific
	// to a child target.  To augment these strings use the
	// addRunCmd & addMiscCmd methods.
	// To augment these 
	StringList runCmds;
	StringList miscCmds;

	// methods for generating code for reading and writing
	// wormhole ports.  Argument is the "real port" of the interior
	// star that is attached to an event horizon.
	void wormInputCode(PortHole&);
	void wormOutputCode(PortHole&);

public:
	AsmTarget(const char* nam, const char* desc,
		  const char* stype, ProcMemory* m = 0);

	~AsmTarget();

	// Execute system call on host, with DISPLAY variables
	// set and inside the destDirectory.  If err is specified,
	// err is displayed if system call is unsuccessful.
	virtual int hostSystemCall(const char*,const char* err = NULL);

	Block* clone() const = 0;

	int setup(Galaxy &g);

	// output a directive that switches to the code section
	virtual void codeSection() = 0;

	// generate file of stuff named base.suffix.
	int genFile(StringList& stuff, char* base,const char* suffix=NULL);

	// generate file of stuff named base.suffix and display it.
	int genDisFile(StringList& stuff,char* base,const char* suffix=NULL);
  
	// Return full file name including path.  
	// NOTE: User must delete the char* returned after use.
	char* fullFileName(const char* base, const char* suffix=NULL);

	// Return file name w/o path.  If the char* returned is not
	// for immediate use, the calling function must copy the string
	// into another location.
	char* fileName(const char* base, const char* suffix=NULL);

	// output an "org" directive that switches to the specified
	// memory and address
	virtual void orgDirective(const char* mem, unsigned addr) {};

	// output an integer
	virtual void writeInt (int) {};

	// output a fixed-point value, passed as a double argument.
	virtual void writeFix (double) {};

	// output a floating-point value.
	virtual void writeFloat (double) {};

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
	int	interruptFlag;

	//Save program counter
	virtual void saveProgramCounter();

	//restore program counter
	virtual void restoreProgramCounter();

	void headerCode();

	void wrapup();

	// commands to augment the runCmds & miscCmds StringLists
	void  addRunCmd(const char* cmd) { runCmds += cmd;}
	void addMiscCmd(const char* cmd) {miscCmds += cmd;}
};

#endif
