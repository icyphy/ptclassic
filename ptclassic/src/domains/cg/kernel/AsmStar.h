#ifndef _AsmStar_h
#define _AsmStar_h 1
/******************************************************************
Version identification:
@(#)AsmStar.h	1.33	3/2/95

Copyright (c) 1990-1995 The Regents of the University of California.
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

 Programmer: J. Buck, J. Pino, T. M. Parks

 This is the baseclass for stars that generate assembly language code.  

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "Attribute.h"
#include "AsmPortHole.h"

// Define attribute bit masks for code generation states
const bitWord AB_MEMORY  =  0x100; // state is in memory
const bitWord AB_CIRC    =  0x200;  // circular buffer
const bitWord AB_NOINIT  =  0x400; // do not initialize
const bitWord AB_REVERSE =  0x800; // write out in reverse order
const bitWord AB_CONSEC  = 0x1000; // allocate consecutively with next state
				    // (of same star)
const bitWord AB_SYMMETRIC = 0x2000; // allocate in two memories
// Define attributes for code generation states
// note that attributes may combine several attribute bits.
extern const Attribute A_ROM; // a ROM state
extern const Attribute A_RAM; // a RAM state
extern const Attribute A_MEMORY; // either ROM or RAM
extern const Attribute A_CIRC; // circular buffer
extern const Attribute A_NOINIT; // no initialization
extern const Attribute A_REVERSE; // write in reverse order
extern const Attribute A_CONSEC;  // write consecutively
extern const Attribute A_SYMMETRIC;  // allocate in two memories

#include "CGStar.h"

class ProcMemory;
class StateAddrEntry;

class AsmStar : public CGStar {
public:
	// constructor
	AsmStar() : addrList(0) {}

	// destructor
	~AsmStar();

	// Clear list of state memory allocations and requests,
	// and prepare SDF properties of the star for scheduling.
	void initialize();

	// Add a state entry after memory allocation to indicate memory
	// requirements for the state.
	void addEntry(const State&,ProcMemory&,unsigned);

	// Look up an address for a State entry by name.
	// Return a pointer to the memory in which the state will be stored,
	// or NULL if no entry is found.  Set the second argument
	// equal to the address.
	ProcMemory* lookupEntry(const char*,unsigned&);

	// generate one execution of the star
	int run();

protected:
	// Get one of my ports, which is always an AsmPortHole
	AsmPortHole* asmPortWithName(const char* name) {
		return (AsmPortHole*)portWithName(name);
	}

	// Save PC, addCode, Restore PC & set interruptFlag=TRUE.
	void genInterruptCode(CodeBlock& block);

	// State or PortHole reference.
	virtual StringList expandRef(const char*);

	// State or PortHole reference with offset.
	virtual StringList expandRef(const char* name, const char* offset);

	// Look up the address of porthole or state with an offset for
	// the current access.
	StringList lookupAddress(const char* name);

	// Look up the memory of porthole or state.
	StringList lookupMem(const char* name);

	// compute the address, plus an offset, with buffer wraparound
	// for accessing an element in a porthole buffer.
	StringList lookupAddress(const char* name, const char* offset);

        /* Expand macros that are defined for this star.
           The following macros are recognized:

		$addr(name)		Address in memory.
		$addr(name,offset)	Address in memory with offset.
		$mem(name)		Memory space.
		$fullname()		Name of the star.
		$starname()		Abbreviated name of the star.

	   The follwing macros are also recognized:

		$val(name)              Value of a state.
		$size(name)             Size of a state or port.
		$ref(name)              Reference to a state or port.
		$ref(name,offset)       Reference with offset.
		$label(name)            Unique label for codeblock.
		$codeblockSymbol(name)  Another name for label.
		$starSymbol(name)       Unique label for star.

           The number, names, and meaning of
           these functions can be easily redefined in derived classes.
        */
	virtual StringList expandMacro(const char* func, const StringList& argList);

	// Reset the state entry list.
	void zapStateEntries();

private:
	// State entry list.  This stores the addresses allocated to each
	// State.
	StateAddrEntry* addrList;
};

class AsmStarPortIter : public BlockPortIter {
public:
	AsmStarPortIter(AsmStar& s) : BlockPortIter(s) {}
	AsmPortHole* next() { return (AsmPortHole*)BlockPortIter::next();}
	AsmPortHole* operator++(POSTFIX_OP) { return next();}
};

#endif
