#ifndef _AsmStar_h
#define _AsmStar_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck

 This is the baseclass for stars that generate assembly language code.  

*******************************************************************/

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
const bitWord AB_SHARED = 0x400; // allocate in two memories

// Define attributes for code generation states
// note that attributes may combine several attribute bits.
extern const Attribute A_ROM; // a ROM state
extern const Attribute A_RAM; // a RAM state
extern const Attribute A_MEMORY; // either ROM or RAM
extern const Attribute A_CIRC; // circular buffer
extern const Attribute A_NOINIT; // no initialization
extern const Attribute A_REVERSE; // write in reverse order
extern const Attribute A_CONSEC;  // write consecutively
extern const Attribute A_SHARED;  // allocate in two memories

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

	// Generate code.  No data is grabbed or put onto geodesics.
	void fire();

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

protected:
	// Given a CodeBlock, generate output code and add it to the
	// code of the star's target.  Macros referencing states and
	// inputs and outputs are processed here.
	void gencode(CodeBlock&);

	// Return the special character that introduces a macro
	// in a code block.  This character is used by gencode() to
	// parse the CodeBlock.  In this base class, the character is '$'.
	virtual char substChar() const;

	// Look up the value of state and return it as a StringList.
	// A zero-length StringList is returned if there is no such State.
	StringList AsmStar::lookupVal(const char* name);

	// Look up the address of porthole or state with an offset for
	// the current access.
	StringList AsmStar::lookupAddress(const char* name);

	// Look up the memory of porthole or state.
	StringList AsmStar::lookupMem(const char* name);

	// Process the macros that are defined for this star.
	// These are found by gencode.  In this base class, the following
	// functions are handled: $addr(name), $mem(name),
	// $val(name), and $ref(name).  The $ref(name) is equivalent
	// to $mem(name):$addr(name).  The $val(name) refers to the value
	// of a State.  The number, names, and meaning of
	// these functions can be easily redefined in derived classes.
	virtual StringList processMacro(const char* func, const char* id);

	// Update all PortHoles so that the offset is incremented by the
	// number of samples consumed or produced.
	void advance();

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
	AsmPortHole* operator++() { return next();}
};

#endif
