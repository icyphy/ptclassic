#ifndef _AsmTarget_h
#define _AsmTarget_h 1
/******************************************************************
Version identification:
@(#)AsmTarget.h	1.50	03/28/97

Copyright (c) 1990-1997 The Regents of the University of California.
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
#include "RepStateList.h"

class ProcMemory;
class AsmStar;

class AsmTarget : public CGTarget {
public:
	// Constructor
	AsmTarget(const char* nam, const char* desc, const char* stype,
		  const char* assocDomain = "AnyAsm", ProcMemory* m = 0);

	// Destructor
	~AsmTarget();

	// Return a copy of itself
	/*virtual*/ Block* makeNew() const {
		return new AsmTarget(name(),starType(), descriptor(),
				     getAssociatedDomain());
	}

	// Class identification.
	/*virtual*/ int isA(const char*) const;

	// output a directive that switches to the code section
	virtual void codeSection() {}

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

	//lookup a shared state
	const State* lookupSharedState(const State& s) const {
		return sharedStarStates.equivalentState(s);	
	}

	// Look up an address for a Shared State entry by name.
	// Return a pointer to the memory in which the state will be stored,
	// or NULL if no entry is found.  Set the second argument
	// equal to the address.
	ProcMemory* lookupSharedEntry(State&,unsigned&);

	/*virtual*/ void frameCode();

	// Re-define this CGTarget method to produce comments
	// with ";" as the first character

	/* virtual */  StringList comment(const char* cmt,
		const char* begin=NULL, const char* end=NULL,
		const char* cont=NULL);


protected:
	/*virtual*/ void setup();
	/*virtual*/ void mailLoopCode();

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

	/*virtual*/ void mainLoopCode();

	CodeStream mainLoop;
	CodeStream trailer;

private:
	void initStates();
	RepStateList sharedStarStates;
};

#endif
