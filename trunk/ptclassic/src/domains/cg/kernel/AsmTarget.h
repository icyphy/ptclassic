#ifndef _AsmTarget_h
#define _AsmTarget_h 1
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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
#include "SDFScheduler.h"

class ProcMemory;
class AsmStar;
class AsmTargetPtr;

class AsmTarget : public CGTarget {
public:
	AsmTarget(const char* nam, const char* desc,
		  const char* stype, ProcMemory* m = 0);

	Block* makeNew() const = 0;

	// output a directive that switches to the code section
	virtual void codeSection() = 0;

	/*virtual*/ void writeCode();

	// output an "org" directive that switches to the specified
	// memory and address
	virtual void orgDirective(const char*, unsigned) {}

	// output an integer
	virtual void writeInt (int) {}

	// output a fixed-point value, passed as a double argument.
	virtual void writeFix (double) {}

	// output a floating-point value.
	virtual void writeFloat (double) {}

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

protected:
	ProcMemory* mem;

	// what it says
	int allocateMemory();

	// redefine init routine for code generation
	int codeGenInit();

	// Request memory for all structures in a Star
	int allocReq(AsmStar&);

	// make modifications to interface circular to linear buffers
	// and handling of long delays.
	int modifyGalaxy();

	virtual void doInitialization(CGStar&);

	// methods for generating code for reading and writing
	// wormhole ports.  Argument is the "real port" of the interior
	// star that is attached to an event horizon.
	void wormInputCode(PortHole&);
	void wormOutputCode(PortHole&);

	// AsmTargets always support "AnyAsmStar" stars.
	const char* auxStarClass() const;

	/*virtual*/ void frameCode();

private:
	void initStates();

};

#endif
