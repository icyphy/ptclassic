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

// Define attributes for code generation states
const bitWord AB_MEMORY = 0x20;	// state is in memory
const bitWord AB_CIRC   = 0x40;	// circular buffer
const bitWord AB_NOINIT = 0x80;	// don't care about initial value (RAM state)
const bitWord AB_REVERSE = 0x100;// write out in reverse order
const bitWord AB_CONSEC = 0x200; // allocate consecutively with next state
				 // (of same star)
const bitWord AB_SHARED = 0x400; // allocate in two memories

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

	// scheduler support
	void prepareForScheduling();
	void fire();

	// postprocessing for a line of code after symbol substitution
	// (baseclass func just does target->addCode).
	virtual void addCode(char*);

	// add state entry after memory allocation
	void addEntry(const State&,ProcMemory&,unsigned);

	// initial code (should this be rooted in CGStar?)
	virtual void initCode();
protected:
	// turn a codeblock into code
	void gencode(CodeBlock&);

	// look up a state entry by name
	ProcMemory* lookupEntry(const char*,unsigned&);

	// format an address for assembly language code
	virtual const char* format(unsigned addr);

	// character that introduces a symbol in a code block
	virtual char substChar() const;

	// look up value of state
	StringList AsmStar::lookupVal(const char* name);

	// look up address of porthole or state
	StringList AsmStar::lookupAddress(const char* name,int update);

	// process macros that are defined for this star
	virtual StringList processMacro(const char* func, const char* id);

	// reset the state entry list
	void zapStateEntries();
private:
	// state entry list.
	StateAddrEntry* addrList;
};

class AsmStarPortIter : public BlockPortIter {
public:
	AsmStarPortIter(AsmStar& s) : BlockPortIter(s) {}
	AsmPortHole* next() { return (AsmPortHole*)BlockPortIter::next();}
	AsmPortHole* operator++() { return next();}
};

#endif
