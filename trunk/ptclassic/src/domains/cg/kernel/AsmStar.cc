static const char file_id[] = "AsmStar.cc";
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

 Programmer: J. Buck, J. Pino, T. M. Parks

 This is the baseclass for stars that generate assembly language code.  

*******************************************************************/
#include "AsmStar.h"
#include "AsmTarget.h"
#include "ProcMemory.h"
#include "IntState.h"
#include "FixState.h"
#include <ctype.h>
#include "miscFuncs.h"

// Attributes for code generation, all processors.

extern const Attribute A_ROM = {AB_MEMORY|AB_CONST,0};
extern const Attribute A_RAM = {AB_MEMORY,AB_CONST};
extern const Attribute A_MEMORY = {AB_MEMORY,0};
extern const Attribute A_CIRC = {AB_CIRC,0};
extern const Attribute A_NOINIT = {AB_NOINIT,0};
extern const Attribute A_REVERSE = {AB_REVERSE,0};
extern const Attribute A_CONSEC = {AB_CONSEC,0};
extern const Attribute A_SYMMETRIC = {AB_SYMMETRIC,0};

void AsmStar::genInterruptCode(CodeBlock& cb) {
	AsmTarget* asmTargetPtr = (AsmTarget*)targetPtr;
	asmTargetPtr->saveProgramCounter();
	addCode(cb);
	asmTargetPtr->restoreProgramCounter();
	asmTargetPtr->interruptFlag = TRUE;
}

// State or PortHole reference.
StringList AsmStar::expandRef(const char* name)
{
    return expandRef(name, "0");
}

// State or PortHole reference with offset.
StringList AsmStar::expandRef(const char* name, const char* offset)
{
    StringList ref;
    ref << lookupMem(name) << ':' << lookupAddress(name, offset);
    return ref;
}

// lookup location for a symbol (a porthole or state) in a
// codeblock.
StringList AsmStar::lookupAddress(const char* name)
{
    return lookupAddress(name, "0");
}

// State or PortHole address with offset.
// Return empty StringList on error.
StringList AsmStar::lookupAddress(const char* name, const char* offset)
{
    StringList addr;
    State* state;
    AsmPortHole* port;
    StringList stateVal;
    StringList portName = expandPortName(name);

    if ((state = stateWithName(offset)) != NULL)
    {
        // Note: currently only the value of a State can be used as an offset
        if (state->isA("IntState"))
        {
            stateVal = expandVal(offset);
            offset = stateVal;
        }
        else
        {
            codeblockError(offset, " is not an IntState");
            addr.initialize();
            return addr;
        }
    }

    int off = atoi(offset);

    if ((state = stateWithName(name)) != NULL)
    {
	unsigned a;
	if (lookupEntry(name, a) != NULL)
	{
	    a += off;
	    addr = a;
	}
	else
	{
	    codeblockError(name, " does not have a memory address");
	    addr.initialize();
	}
    }
    else if (port = (AsmPortHole*)genPortWithName(portName))
    {
	// compute offset within buffer, circularly
	off += port->bufPos();
	off %= port->bufSize();

	int a = port->baseAddr() + off;
	addr = a;
    }
    else
    {
	codeblockError(name, " is not defined as a state or port");
	addr.initialize();
    }

    return addr;
}

// lookup memory for a symbol (a porthole or state) in a
// codeblock.
StringList AsmStar::lookupMem(const char* name)
{
    StringList mem;
    AsmPortHole* port;
    StringList portName = expandPortName(name);

    if (stateWithName(name) != NULL)
    {
	unsigned a = 0;
	ProcMemory* m;
	if ((m = lookupEntry(name, a)) != NULL)
	{
	    mem = m->name();
	}
	else
	{
	    codeblockError(name, " is not assigned to memory");
	    mem.initialize();
	}
    }
    else if ((port = (AsmPortHole*)genPortWithName(portName)) != NULL)
    {
	if (port->memory() != NULL)
	    mem = port->memory()->name();
	else
	{
	    codeblockError(name, " is not assigned to memory");
	    mem.initialize();
	}
    }
    else
    {
	codeblockError(name, " is not defined as a state or port");
	mem.initialize();
    }
    return mem;
}

// Expand Macros.
StringList AsmStar::expandMacro(const char* func, const StringList& argList)
{
	StringListIter arg(argList);
	StringList s;

	if (matchMacro(func, argList, "addr2", 2)) s = lookupAddress(arg++, arg++);
	else if (matchMacro(func, argList, "addr", 2)) s = lookupAddress(arg++, arg++);
	else if (matchMacro(func, argList, "addr", 1)) s = lookupAddress(arg++);
	else if (matchMacro(func, argList, "mem", 1)) s = lookupMem(arg++);
	else if (matchMacro(func, argList, "fullname", 0)) s = fullName();
	else if (matchMacro(func, argList, "starname", 0)) s = name();
	else s = CGStar::expandMacro(func, argList);

	return s;
}

// data structure used to keep track of memory allocation for states.
class StateAddrEntry {
	friend class AsmStar;
	const char* stateName;
	ProcMemory& mem;
	unsigned addr;
	StateAddrEntry* link;
	StateAddrEntry(const char* s, ProcMemory& m, unsigned a,
		       StateAddrEntry* l = 0)
		: stateName(s), mem(m), addr(a), link(l) {}
};

// add a new state to the table.
void AsmStar::addEntry(const State& s,ProcMemory& m, unsigned a) {
	LOG_NEW; addrList = new StateAddrEntry(s.name(),m,a,addrList);
}

// look up a state in the table.
ProcMemory* AsmStar :: lookupEntry(const char* s, unsigned& a) {
	StateAddrEntry* p = addrList;
	while (p && strcmp(p->stateName,s) != 0) p = p->link;
	if (p) {
		a = p->addr;
		return &p->mem;
	}
	return 0;
}

// zero out all the state addresses.
void AsmStar :: zapStateEntries() {
	StateAddrEntry* p;
	while (addrList) {
		p = addrList;
		addrList = p->link;
		LOG_DEL; delete p;
	}
}

// destructor
AsmStar::~AsmStar() {
	zapStateEntries();
}

// redo the list whenever remaking the scheduler.
void AsmStar::initialize() {
	zapStateEntries();
	CGStar::initialize();
}


// run: prefix the code with a comment

int AsmStar::run() {
	StringList code = "code from star ";
	code += fullName();
	code += " (class ";
	code += className();
	code += ")\n";
	outputComment(code);
	return CGStar::run();
}

