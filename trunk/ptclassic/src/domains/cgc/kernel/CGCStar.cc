static const char file_id[] = "CGCStar.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: E. A. Lee

 This is the baseclass for stars that generate C language code

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "CGCStar.h"
#include "CGCTarget.h"

// The following is defined in CGCDomain.cc -- this forces that module
// to be included if any CGC stars are linked in.
extern const char CGCdomainName[];

const char* CGCStar :: domain () const { return CGCdomainName;}

// isa

ISA_FUNC(CGCStar, CGStar);

StringList CGCStar::getRef(const char* name) {
	registerState(name);
	GenericPort *p = genPortWithName(name);
	if(!p) { 
		StringList s = ((CGCTarget*) myTarget())->correctName(*this);
		s += ".";
		s += name;
		return s;
	}
		
	if(p->isItMulti()) {
		Error::abortRun(*this,
		"Accessing MultiPortHole without identifying which port.");
		return("ERROR");
	}

	// if necessary, we add offset to this.
	CGCPortHole* cp = (CGCPortHole*) p;
	StringList out = cp->getGeoName();
	if (cp->maxBufReq() > 1) {
		out += "[";
		out += ((CGCTarget*)myTarget())->offsetName(cp);
		out += "]";
	}
	
	return out;
}

StringList CGCStar::getRef2(const char* name, const char* offset) {
	StringList out;
	registerState(name);
	GenericPort *p = genPortWithName(name);
	State* s = stateWithName(offset);
	int offVal = 0;
	if (s) {
		if (!s->isA("IntState")) {
                        codeblockError(offset, " is not the name of an IntState"
);
		}
		IntState* is = (IntState*) s;
		offVal = *is;
	}

	if(!p) {
		out += CGStar::getRef(name);
		out += "[";
		if (s) out += offVal;
		else out += offset;
	} else {
		// if necessary, we add offset to this.
		CGCPortHole* cp = (CGCPortHole*) p;
		out += cp->getGeoName();
		if (cp->maxBufReq() > 1) {
			out += "[(";
			out += ((CGCTarget*)myTarget())->offsetName(cp);
			out += " - (";
			if (s) out += offVal;
			else out += offset;
			out += ") + ";
			out += cp->maxBufReq();
			out += ") % ";
			out += cp->maxBufReq();
		} else {
			return out;
		}
	}

	out += "]";
	return out;
}

void CGCStar :: initBufPointer() {
	BlockPortIter next(*this);
	CGCPortHole* p;
	while ((p = (CGCPortHole*) next++) != 0) {
		if (p->maxBufReq() > 1) {
			StringList out = ((CGCTarget*)myTarget())->offsetName(p);
			out += " = ";
			out += p->bufPos();
			out += ";\n";

			// initialize output buffer
			if (p->isItOutput()) {
				out += "    { int i;\n";
				out += "    for (i = 0; i < ";
				out += p->maxBufReq();
				out += "; i++)\n\t";
				out += p->getGeoName();
				out += "[i] = 0;\n    }\n";
			}
			addMainInit(out);
		}
	}
}
	
void CGCStar::registerState(const char* name) {
	State* state;
	if(state = stateWithName(name)) {
		// If the state is not already on the list of referenced
		// states, add it.
		StateListIter nextState(referencedStates);
		const State* sp;
		while ((sp = nextState++) != 0)
			if(strcmp(name,sp->readName()) == 0) return;
		referencedStates.put(*state);
	}
}

void CGCStar::addInclude(const char* decl) {
	((CGCTarget*)myTarget())->addInclude(decl);
}

void CGCStar::addDeclaration(const char* decl) {
	((CGCTarget*)myTarget())->addDeclaration(decl);
}

void CGCStar::addGlobal(const char* decl) {
	((CGCTarget*)myTarget())->addGlobal(decl);
}

void CGCStar::addMainInit(const char* decl) {
	((CGCTarget*)myTarget())->addMainInit(decl);
}

void CGCStar::start() {
	CGStar::start();
	referencedStates.initialize();
}

// fire: prefix the code with a comment

int CGCStar::fire() {
	StringList code = "\t{  /* star ";
	code += readFullName();
	code += " (class ";
	code += readClassName();
	code += ") */\n";
	addCode(code);
	int status = CGStar::fire();
	
	StringList code2;

	if (amIFork()) {
		code2 += "\t}\n";
		addCode(code2);
		return status;
	} 

	// update the offset member
	CGCTarget* t = (CGCTarget*) myTarget();

	BlockPortIter next(*this);
	CGCPortHole* p;
	while ((p = (CGCPortHole*) next++) != 0) {
		if (p->maxBufReq() > 1) {
			if (p->numberTokens == p->maxBufReq()) continue;
			code2 += "\t";
			code2 += t->offsetName(p);
			code2 += " = (";
			code2 += t->offsetName(p);
			code2 += " + ";
			code2 += p->numberTokens;
			code2 += ") % ";
			code2 += p->maxBufReq();
			code2 += ";\n";
		}
	}
	code2 += "\t}\n";
	addCode(code2);
	return status;
}
