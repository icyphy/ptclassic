/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck

 This is the baseclass for stars that generate assembly language code.  

*******************************************************************/
#include "AsmStar.h"
#include <ctype.h>

// Attributes for code generation, all processors.

extern const Attribute A_ROM(AB_MEMORY|AB_CONST,0);
extern const Attribute A_RAM(AB_MEMORY,AB_CONST);
extern const Attribute A_CIRC(AB_CIRC,0);
extern const Attribute A_NOINIT(AB_NOINIT,0);
extern const Attribute A_REVERSE(AB_REVERSE,0);
extern const Attribute A_SHARED(AB_SHARED,0);

// The default substitution character is '$'.  Some assembly
// languages may use this, so we allow a different character.
char AsmStar::substChar() const { return '$'; }

// lookup value for a symbol (a porthole or state) in a
// codeblock.
const char*
AsmStar::lookupSymbol(const char* name) {
	unsigned a;
	AsmPortHole* p = (AsmPortHole*)portWithName(name);
	if (p) a = p->addr();
	// see if it's a state
	else if (!lookupEntry(name,a)) return 0;
	return format(a);
}

// format the symbol (it is virtual and can be overriden).
const char*
AsmStar::format(unsigned addr) {
	static char buf[10];
	sprintf (buf, "%ud", addr);
	return buf;
}

const int MAXLINELEN = 256;
const int TOKLEN = 80;

// process a CodeBlock.  This processing just substitutes for
// symbols.
void AsmStar::gencode(CodeBlock& cb) {
	const char* t = cb.getText();
// output this text
	char line[MAXLINELEN], *o = line, c;
	while ((c = *t++) != 0) {
		if (c == substChar()) {
			char id[TOKLEN], *q = id;
			while (isalnum(*t)) *q++ = *t++;
			*q = 0;
			const char* value = lookupSymbol(id);
			if (value == 0) {
				Error::abortRun (*this,"Undefined symbol '",
						 id, "' found in codeblock");
				return;
			}
			while (*value) *o++ = *value++;
		}
		else {
			*o++ = c;
			if (c == '\n') {
				*o = 0;
				addCode(line);
				o = line;
			}
		}
	}
	if (o > line) {
		*o++ = '\n';
		*o = 0;
		addCode(line);
	}
}

// postprocessing of code.  Baseclass just does same as for CGStar::addCode.
void AsmStar :: addCode(char* line) {
	CGStar::addCode(line);
}

// data structure used to keep track of memory allocation for states.
class StateAddrEntry {
	friend class AsmStar;
	const char* state;
	ProcMemory& mem;
	unsigned addr;
	StateAddrEntry* link;
	StateAddrEntry(const char* s, ProcMemory& m, unsigned a,
		       StateAddrEntry* l = 0)
		: state(s), mem(m), addr(a), link(l) {}
};

// add a new state to the table.
void AsmStar::addEntry(State& s,ProcMemory& m, unsigned a) {
	addrList = new StateAddrEntry(s.readName(),m,a,addrList);
}

// look up a state in the table.
ProcMemory* AsmStar :: lookupEntry(const char* s, unsigned& a) {
	StateAddrEntry* p = addrList;
	while (p && strcmp(p->state,s) != 0) p = p->link;
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
		delete p;
	}
}

// destructor
AsmStar::~AsmStar() {
	zapStateEntries();
}

// redo the list whenever remaking the scheduler.
void AsmStar::prepareForScheduling() {
	zapStateEntries();
}

// fire: we execute the star, then advance all the portholes.
void AsmStar::fire() {
	go();
// advance all the porthole pointers
	AsmStarPortIter nextp(*this);
	AsmPortHole *p;
	while ((p = nextp++) != 0) p->advance();
}
