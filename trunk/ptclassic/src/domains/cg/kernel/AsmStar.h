#ifndef _AsmStar_h
#define _AsmStar_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck & J. Pino

 This is the baseclass for stars that generate assembly language code.  

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "Attribute.h"
#include "AsmConnect.h"
// FixSample is included so AsmStars can use FIX-type ports
#include "FixSample.h"

// Define attribute bit masks for code generation states
const bitWord AB_MEMORY = 0x20;	// state is in memory
const bitWord AB_CIRC   = 0x40;	// circular buffer
const bitWord AB_NOINIT = 0x80;	// don't care about initial value (RAM state)
const bitWord AB_REVERSE = 0x100;// write out in reverse order
const bitWord AB_CONSEC = 0x200; // allocate consecutively with next state
				 // (of same star)
const bitWord AB_SYMMETRIC = 0x400; // allocate in two memories

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
	void prepareForScheduling();

	// If postprocessing for a line of code after symbol substitution
	// is needed, the function addCode(char*) should be redefined.

	// Add a state entry after memory allocation to indicate memory
	// requirements for the state.
	void addEntry(const State&,ProcMemory&,unsigned);

	// Look up an address for a State entry by name.
	// Return a pointer to the memory in which the state will be stored,
	// or NULL if no entry is found.  Set the second argument
	// equal to the address.
	ProcMemory* lookupEntry(const char*,unsigned&);

	int fire();

protected:
	// Get one of my ports, which is always an AsmPortHole
	AsmPortHole* asmPortWithName(const char* name) {
		return (AsmPortHole*)portWithName(name);
	}

	// Save PC, gencode, Restore PC & set interruptFlag=TRUE.
	void genInterruptCode(CodeBlock& block);

	// Look up the value of state and return it as a StringList.
	// A zero-length StringList is returned if there is no such State.
	StringList lookupVal(const char* name);

	// Look up the address of porthole or state with an offset for
	// the current access.
	StringList lookupAddress(const char* name);

	// Look up the memory of porthole or state.
	StringList lookupMem(const char* name);

	// Look up the size of a porthole or state
	int lookupSize(const char* name);

	// compute the address, plus an offset, with buffer wraparound
	// for accessing an element in a porthole buffer.
	unsigned addrWithOffset(const char*, const char*);

	// Process the macros that are defined for this star.
	// These are found by gencode.  In this class, the following
	// functions are handled:
	//	$addr(name)	Address in memory
	//	$addr2(name,o)	Address in memory with an offset o
	//	$mem(name)	Name of the memory containing variable "name"
	//	$val(name)	Value of state "name"
	//	$ref(name)	$mem(name):$addr(name)
	//	$ref2(name,o)	$mem(name):(address with offset)
	// 	$label(name)	unique label for (codeblock,name) pair.
	//			If the label does not exist, a new, unique
	//			label is created.
	//	$fullname()	Name of the star
	//	$size(name)	Size of memory allocated to "name"
	// The number, names, and meaning of
	// these functions can be easily redefined in derived classes.
	StringList processMacro(const char* func, const char* id,
					const char* arg2);

	// Reset the state entry list.
	void zapStateEntries();

	// add runCmds or miscCmds to the target streams.  cmd2 is
	// supported to allow for individual command separators.
	void addRunCmd(const char*,const char* cmd2=NULL);
	void addMiscCmd(const char*,const char* cmd2=NULL);
	void genRunCmd(CodeBlock&);
	void genMiscCmd(CodeBlock&);

private:
	// State entry list.  This stores the addresses allocated to each
	// State.
	StateAddrEntry* addrList;
};

class AsmStarPortIter : public BlockPortIter {
public:
	AsmStarPortIter(AsmStar& s) : BlockPortIter(s) {}
	AsmPortHole* next() { return (AsmPortHole*)BlockPortIter::next();}
	AsmPortHole* operator++() { return next();}
};

#endif
