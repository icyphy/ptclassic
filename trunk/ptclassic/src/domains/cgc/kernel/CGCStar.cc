static const char file_id[] = "CGCStar.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: E. A. Lee, T. M. Parks

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

// Reference to State or PortHole.
StringList CGCStar::expandRef(const char* name)
{
    StringList ref;
    CGCPortHole* port;
    StringList portName = expandPortName(name);

    if (stateWithName(name) != NULL)
    {
	registerState(name);
	ref << ((CGCTarget*)myTarget())->correctName(*this) << '.' << name;
    }
    else if ((port = (CGCPortHole*)genPortWithName(portName)) != NULL)
    {
	ref << port->getGeoName();
	if (port->maxBufReq() > 1)
	{
	    ref << '[' << ((CGCTarget*)myTarget())->offsetName(port) << ']';
	}
    }
    else
    {
	codeblockError(name, " is not defined as a state or port");
	ref.initialize();
    }
    return ref;
}

// Reference to State or PortHole with offset.
StringList CGCStar::expandRef(const char* name, const char* offset)
{
    StringList ref;
    State* state;
    CGCPortHole* port;
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
	    ref.initialize();
	    return ref;
	}
    }

    if ((state = stateWithName(name)) != NULL)
    {
	ref = expandRef(name);
	if (ref.size() == 0) return ref;	// error in getRef()
	if (state->size() > 1) ref << '[' << offset << ']';
    }
    else if (port = (CGCPortHole*)genPortWithName(portName))
    {
	ref << port->getGeoName();
	if (port->maxBufReq() > 1)
	{
	    ref << "[(" << ((CGCTarget*)myTarget())->offsetName(port);
	    ref << " - (" << offset;
	    ref << ") + " << port->maxBufReq() << ") % " << port->maxBufReq();
	    ref << ']';
	}
    }
    return ref;
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
