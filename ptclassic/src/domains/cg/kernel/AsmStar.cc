static const char file_id[] = "AsmStar.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck, J. Pino

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

// lookup location for a symbol (a porthole or state) in a
// codeblock.
StringList
AsmStar::lookupAddress(const char* name) {
	unsigned a;
	StringList s;
	AsmPortHole* p = asmPortWithName(name);
	if (p) s = p->location();
	// see if it's a state
	else if (lookupEntry(name,a)) {
	    s = int(a);
	} else {
	    if (stateWithName(name))
		Error::abortRun(*this," state ",name,
			" does not have a memory address");
	    else Error::abortRun(*this,name," is not defined");
	    s = "";
	}
	return s;
}

void AsmStar::genInterruptCode(CodeBlock& cb) {
	AsmTarget* asmTargetPtr = (AsmTarget*)targetPtr;
	asmTargetPtr->saveProgramCounter();
	gencode(cb);
	asmTargetPtr->restoreProgramCounter();
	asmTargetPtr->interruptFlag = TRUE;
}
	
unsigned
AsmStar::addrWithOffset (const char* name, const char* offset) {
	int off;
	AsmPortHole* p = asmPortWithName(name);
	if (!p) {
		codeblockError(name, " is not a porthole name");
		return 0;
	}
	if (isdigit(*offset)) off = atoi(offset);
	else {
		State* s = stateWithName(offset);
		if (!s || !s->isA("IntState")) {
			codeblockError(offset, " is not the name of an IntState");
			return 0;
		}
		IntState* is = (IntState*)s;
		off = *is;
	}
	// compute offset within buffer, circularly
	off += p->bufPos();
	int sz = p->bufSize();
	while (off >= sz) off -= sz;
	while (off < 0) off += sz;
	// add the base
	off += p->baseAddr();
	return off;
}

// lookup size (of buffer or state) for symbol in a codeblock
int
AsmStar::lookupSize(const char* name) {
	AsmPortHole* p = asmPortWithName(name);
	if (p) return p->bufSize();
	State* s = stateWithName(name);
	if (s) return s->size();
	codeblockError(name, " is not defined");
	return 0;
}

// lookup memory for a symbol (a porthole or state) in a
// codeblock.
StringList
AsmStar::lookupMem(const char* name) {
	unsigned a;
	StringList s;
	ProcMemory* m;
	AsmPortHole* p = asmPortWithName(name);
	if (p && p->memory()) s = p->memory()->readName();
	// see if it's a state
	else if (m = lookupEntry(name,a)) {
	    s = m->readName();
	} else {
	    if (stateWithName(name))
		    codeblockError(name, " is not assigned to memory");
	    else codeblockError(name, " is not defined");
	    s = "ERROR";
	}
	return s;
}

// lookup value for a state
StringList
AsmStar::lookupVal(const char* name) {
	State* s;
	if ((s = stateWithName(name)) != 0) {
		// UGLY!  FixState has a special hack!
		if (s->isA("FixState"))
			return printFixValue(*(FixState*)s);
		else {
			StringList v = s->currentValue();
			return v;
		}
	}
	codeblockError(name, " is not defined as a state");
	return "ERROR";
}

StringList AsmStar::printFixValue(double val) const {
	StringList out = val;
	if (strchr(out,'.') == 0)
		out += ".0";
	return out;
}

// the following function is provided by the SunOS and Ultrix
// libs; don't know how generally it is available.
extern "C" int strcasecmp(const char* s1, const char* s2);

// handle functions
StringList
AsmStar::processMacro(const char* func, const char* id, const char* arg2) {
	StringList s;
	if (strcasecmp(func,"ref2") == 0) {
		if (*arg2 == 0) codeblockError("two arguments needed for ref2");
		else {
			s = lookupMem(id);
			s += ":";
			s += (int)addrWithOffset(id, arg2);
		}
	}
	else if (strcasecmp(func,"addr2") == 0) {
		if (*arg2 == 0)
			codeblockError("two arguments needed for addr2");
		else s = (int)addrWithOffset(id, arg2);
	}
	// if more two-arg funcs are added, put them before here!
	else if (*arg2 != 0) {
		codeblockError(func, " is not a 2-argument macro");
	}
	else if (strcasecmp(func,"addr") == 0) {
		s = lookupAddress(id);
	} else if (strcasecmp(func, "ref") == 0) {
		s = lookupMem(id);
		s += ":";
		s += lookupAddress(id);
	} else if (strcasecmp(func, "val") == 0) {
		s = lookupVal(id);
	} else if (strcasecmp(func, "mem") == 0) {
		s = lookupMem(id);
	} else if (strcasecmp(func, "fullname") == 0) {
		s = readFullName();
	} else if ((strcasecmp(func, "label") == 0) ||
		   (strcasecmp(func, "codeblockSymbol") == 0)) {
		s = codeblockSymbol.lookup(id);
	} else if (strcasecmp(func, "starSymbol") == 0) {
		s = starSymbol.lookup(id);
	} else if (strcasecmp(func, "size") == 0) {
		s = lookupSize(id);
	} else {
		s = "ERROR: UNKNOWN MACRO ";
		s += func;
		s += "(";
		s += id;
		s += ")";
	}
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
	LOG_NEW; addrList = new StateAddrEntry(s.readName(),m,a,addrList);
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
void AsmStar::prepareForScheduling() {
	zapStateEntries();
	CGStar::prepareForScheduling();
}

