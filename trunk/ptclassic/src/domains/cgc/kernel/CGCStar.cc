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
	if(!p) return CGStar::getRef(name);
	if(p->isItMulti()) {
	    Error::abortRun(*this,
	        "Accessing MultiPortHole without identifying which port.");
	    return("ERROR");
	}
	return ((CGCPortHole*)p)->getGeoName();
}

StringList CGCStar::getRef2(const char* name, const char* offset) {
	StringList out = getRef(name);
	out += "[";
	out += offset;
	out += "]";
	return out;
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

void CGCStar::start() {
	CGStar::start();
	referencedStates.initialize();
}

// fire: prefix the code with a comment

void CGCStar::fire() {
	StringList code = "/* code from star ";
	code += readFullName();
	code += " (class ";
	code += readClassName();
	code += ") */\n";
	addCode(code);
	CGStar::fire();
}
