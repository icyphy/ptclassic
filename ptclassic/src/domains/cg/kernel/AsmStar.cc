static const char file_id[] = "AsmStar.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck

 This is the baseclass for stars that generate assembly language code.  

*******************************************************************/
#include "AsmStar.h"
#include "ProcMemory.h"
#include <ctype.h>

// Attributes for code generation, all processors.

extern const Attribute A_ROM(AB_MEMORY|AB_CONST,0);
extern const Attribute A_RAM(AB_MEMORY,AB_CONST);
extern const Attribute A_MEMORY(AB_MEMORY,0);
extern const Attribute A_CIRC(AB_CIRC,0);
extern const Attribute A_NOINIT(AB_NOINIT,0);
extern const Attribute A_REVERSE(AB_REVERSE,0);
extern const Attribute A_SHARED(AB_SHARED,0);

// Generate code
void AsmStar::fire() {
	// No need to grab data, so just go.
	go();

	// Advance the offset in the PortHoles
	advance();
}

// The default substitution character is '$'.  Some assembly
// languages may use this, so we allow a different character.
// What appears after the $ is a function name, with an argument
// that is the name of a porthole or state.
char AsmStar::substChar() const { return '$'; }

// lookup location for a symbol (a porthole or state) in a
// codeblock.
StringList
AsmStar::lookupAddress(const char* name) {
	unsigned a;
	StringList s;
	AsmPortHole* p = (AsmPortHole*)portWithName(name);
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

// lookup memory for a symbol (a porthole or state) in a
// codeblock.
StringList
AsmStar::lookupMem(const char* name) {
	unsigned a;
	StringList s;
	ProcMemory* m;
	AsmPortHole* p = (AsmPortHole*)portWithName(name);
	if (p && p->memory()) s = p->memory()->readName();
	// see if it's a state
	else if (m = lookupEntry(name,a)) {
	    s = m->readName();
	} else {
	    if (stateWithName(name))
		Error::abortRun(*this," state ",name,
			" does not have a memory assigned");
	    else Error::abortRun(*this,name," is not defined");
	    s = "";
	}
	return s;
}

// lookup value for a state
StringList
AsmStar::lookupVal(const char* name) {
	State* s;
	if ((s = stateWithName(name)) != 0) {
		StringList v = s->currentValue();
		return v;
	}
	return "";
}

// handle functions
StringList
AsmStar::processMacro(const char* func, const char* id) {
	StringList s = "ERROR";
	if ((strcmp(func,"ADDR") == 0) || (strcmp(func, "addr") == 0)) {
		s = lookupAddress(id);
	} else if ((strcmp(func,"REF") == 0) || (strcmp(func, "ref") == 0)) {
		s = lookupMem(id);
		s += ":";
		s += lookupAddress(id);
	} else if ((strcmp(func,"VAL") == 0) || (strcmp(func, "val") == 0)) {
		s = lookupVal(id);
	} else if ((strcmp(func,"MEM") == 0) || (strcmp(func, "mem") == 0)) {
		s = lookupMem(id);
	}
	return s;
}

const int MAXLINELEN = 256;
const int TOKLEN = 80;

static const char synerr[] = "Syntax error in codeblock: expecting ";
static const char syn2[] = " after macro call";
// process a CodeBlock.  This processing just substitutes for
// symbols.
void AsmStar::gencode(CodeBlock& cb) {
	const char* t = cb.getText();
// output this text
	char line[MAXLINELEN], *o = line, c;
	while ((c = *t++) != 0) {
		if (c == substChar()) {
			if (*t == substChar()) {
				*o++ = *t++;
				continue;
			}
			// get the function.
			char func[TOKLEN], *q = func;
			while (isalnum(*t)) *q++ = *t++;
			*q = 0;
			// skip any whitespace
			while (isspace(*t)) t++;
			// must be pointing at a '('
			if (*t++ != '(') {
				Error::abortRun (*this, synerr,"'('", syn2);
				return;
			}
			// get the identifier
			char id[TOKLEN], *p = id;
			while (isalnum(*t)) *p++ = *t++;
			*p = 0;
			// skip any whitespace
			while (isspace(*t)) t++;
			// must be pointing at a ')'
			if (*t++ != ')') {
				Error::abortRun (*this, synerr,"')'", syn2);
				return;
			}
			// Don't know why the following two steps can't
			// be consolidated, but if they are, the string
			// becomes null
			StringList tmp = processMacro(func,id);
			const char* value = tmp;
			if (value == 0 || *value == 0) {
				value = "ERROR";
			}
			// plug result into code.
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

// Update all PortHoles so that the offset is incremented by the
// number of samples consumed or produced.
void AsmStar::advance() {
	AsmStarPortIter nextPort(*this);
	AsmPortHole* p;
	while ((p = nextPort++) != 0)
		p->advance();
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
