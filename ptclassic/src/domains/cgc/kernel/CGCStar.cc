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
extern const char* whichType(DataType);

const char* CGCStar :: domain () const { return CGCdomainName;}

// isa

ISA_FUNC(CGCStar, CGStar);

// Reference to State or PortHole.
StringList CGCStar::expandRef(const char* name)
{
    StringList ref;
    CGCPortHole* port;
    State* st;
    StringList portName = expandPortName(name);

    if ((st = stateWithName(name)) != NULL)
    {
	registerState(name);
	ref << ((CGCTarget*)myTarget())->correctName(*st);
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
	    ref << " - (" << offset << ")";
	    if (port->staticBuf() == 0) {
	    	ref << " + " << port->maxBufReq() << ") % " 
			<< port->maxBufReq();
	    } else {
		ref << ")";
	    }
	    ref << ']';
	}
    }
    return ref;
}

void CGCStar :: initBufPointer() {
	BlockPortIter next(*this);
	CGCPortHole* p;
	while ((p = (CGCPortHole*) next++) != 0) {
		StringList out;
		out += initializeOffset(p);
		out += initializeBuffer(p);
		if (out.size()) addMainInit(out);
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
	gencode(code);
	int status = CGStar::fire();
	
	if (isItFork()) {
		gencode("\t}\n");
		return status;
	} 

	// update the offset member
	updateOffsets();

	return status;
}

void CGCStar :: updateOffsets() {

	CGCTarget* t = (CGCTarget*) myTarget();
	StringList code2;

	BlockPortIter next(*this);
	CGCPortHole* p;
	while ((p = (CGCPortHole*) next++) != 0) {
		if (p->maxBufReq() > 1) {
			if (p->numberTokens == p->maxBufReq()) continue;
			code2 += "\t";
			code2 += t->offsetName(p);
			code2 += " += ";
			code2 += p->numberTokens;
			code2 += ";\n\tif (";
			code2 += t->offsetName(p);
			code2 += " >= ";
			code2 += p->maxBufReq();
			code2 += ")\n\t\t";
			code2 += t->offsetName(p);
			code2 += " -= ";
			code2 += p->maxBufReq();
			code2 += ";\n";
		}
	}
	code2 += "\t}\n";
	gencode(code2);
}

	/////////////////////////////////////////////
	// Variable declarations and initializations
	/////////////////////////////////////////////

// Define variables only for each output port, except outputs of forks
StringList CGCStar :: declarePortHole(const CGCPortHole* p) {
	StringList out;
	if (p->isItOutput()) {
		emptyFlag = FALSE;
		int dimen = p->maxBufReq();
		out += "    ";
		out += whichType(p->plasmaType());
		out += " ";
		out += ((CGCTarget*) myTarget())->correctName(*p);
		if(dimen > 1) {
			out += "[";
			out += dimen;
			out += "]";
		}
		out += ";\n";
	}
	return out;
}

// declare offset 
StringList CGCStar :: declareOffset(const CGCPortHole* p) {
	StringList out;
	if (p->maxBufReq() > 1) {
		emptyFlag = FALSE;
		out += "    ";
		out += "int ";
		out += ((CGCTarget*) myTarget())->correctName(*p);
		out += "_ix";           // suffix to indicate the offset.
		out += ";\n";
	}
	return out;
}

// declare state
StringList CGCStar :: declareState(const State* s) {
	StringList out;
	out += "    ";
	out += whichType(s->type());
	out += " ";
	out += ((CGCTarget*) myTarget())->correctName(*s);
	if (s->size() > 1) {
		out += "[";
		out += s->size();
		out += "]";
	}
	out += ";\n";
	emptyFlag = FALSE;
	return out;
}

// Initialize the porthole
StringList CGCStar :: initializeBuffer(const CGCPortHole* p) {
	StringList out;
	if (p->maxBufReq() > 1) {
		// initialize output buffer
		if (p->isItOutput()) {
			out += "    { int i;\n";
			out += "    for (i = 0; i < ";
			out += p->maxBufReq();
			out += "; i++)\n\t";
			out += p->getGeoName();
			out += "[i] = 0;\n    }\n";
		}
	}
	return out;
}

StringList CGCStar :: initializeOffset(const CGCPortHole* p) {
	StringList out;
	if (p->maxBufReq() > 1) {
		out = ((CGCTarget*)myTarget())->offsetName(p);
		out += " = ";
		out += p->bufPos();
		out += ";\n";
	}
	return out;
}

#define SMALL_STRING 20

// Initialize the state
StringList CGCStar :: initializeState(const State* s) {
	CGCTarget* t = (CGCTarget*) myTarget();
	StringList out;
	if (s->size() > 1) {
		StringList temp = s->currentValue();
		const char* sval = temp;
		for (int i = 0; i < s->size(); i++) {
			out += "    ";
			out += t->correctName(*s);
			out += "[";
			out += i;
			out += "] = ";
			char wval[SMALL_STRING];
			char* wc = wval;
			char c = *sval++;
			while ((c != 0) && (c != '\n')) {
				*wc++ = c;
				c = *sval++;
			}
			*wc = 0;
			out += wval;
			out += ";\n";
			}
	} else {
		out += "    ";
		out += t->correctName(*s);
		out += " = ";
		out += s->currentValue();
		out += ";\n";
	}
	return out;
}


