static const char file_id[] = "CGStar.cc";

/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee, T. M. Parks

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGStar.h"
#include "CGWormhole.h"
#include "CGTarget.h"
#include <ctype.h>

/*******************************************************************

	class CGStar methods

********************************************************************/

//Constructor
CGStar :: CGStar() {
	starSymbol.initialize();
        addState(procId.setState("procId", this, "-1",
                "assigned processor id. If -1, not assigned manually."));
}

// firing CG star : generate code.
int CGStar :: fire() {
	// No need to grab data, so just use Star::fire, not SDFStar::fire.
	int status = Star::fire();
	// Advance the offset in the PortHoles
	advance();
	return status;
}

// Update all PortHoles so that the offset is incremented by the
// number of samples consumed or produced.
void CGStar::advance() {
        BlockPortIter nextPort(*this);
        CGPortHole* p;
        while ((p = (CGPortHole*) nextPort++) != 0) {
		// temporary do not consider WORMHOLE case.
		if (p->far()->parent()->isItWormhole() == FALSE)
                	p->advance();
	}
}

const int MAXLINELEN = 256;
const int TOKLEN = 80;

// Add processed code to the Target.
void CGStar::gencode(const char* code)
{
    StringList temp = processCode(code);
    addCode(temp);
}

// Add processed code to the Target.
void CGStar::gencode(CodeBlock& block)
{
    StringList temp = processCode(block);
    addCode(temp);
}

// Add a string to the Target code.
void CGStar::addCode (const char* code)
{
	myTarget()->addCode(code);
}

// Process a CodeBlock, expanding macros.
// Return empty StringList on error.
StringList CGStar::processCode(CodeBlock& block)
{
    const char* text = block.getText();
    return processCode(text);
}

StringList CGStar::processCode(const char* text)
{
    StringList out;
    char line[MAXLINELEN], *o = line, c;
    const char* t = text;

    // Reset the local labels
    resetCodeblockSyms();

    // scan until end of text
    while ((c = *t++) != 0)
    {
	if (c == substChar())	// parse macro
	{
	    // two consecutive substChar values give one on the output
	    if (*t == substChar())
	    {
		*o++ = *t++;
		continue;
	    }

	    // get the function name
	    char func[TOKLEN], *q = func;
	    while (isalnum(*t)) *q++ = *t++;
	    *q = 0;

	    // skip any whitespace
	    while (isspace(*t)) t++;

	    // must be pointing at a '('
	    if (*t++ != '(')
	    {
		codeblockError ("expecting '('", " after macro call");
		out.initialize();
		return out;
	    }

	    // skip any white space
	    while (isspace(*t)) t++;

	    // build argument list
	    StringList argList;
	    while (*t != ')')
	    {
		char arg[TOKLEN], *p = arg;
		arg[0] = 0;

		// skip any leading white space
		while (isspace(*t)) t++;

		// copy argument
		while (*t != 0 && *t != ',' && *t != ')') *p++ = *t++;

		if (p == arg) // null arguments are not allowed
		{
		    codeblockError ("null argument");
		    out.initialize();
		    return out;
		}

		// back up to remove any trailing white space
		while (isspace(*--p));

		// terminate the string
		*++p = 0;

		argList << arg;

		// skip over separator
		if (*t == ',')
		{
		    t++;
		    if (*t == ')')	// final null argument
		    {
		        codeblockError ("null argument");
			out.initialize();
		        return out;
		    }
		}
	    }
	    t++;	// skip ')' at end of argument list

	    // Must save temporary value returned by expandMacro.
	    StringList temp = expandMacro(func, argList);
	    const char* macro = temp;
	    if (macro != NULL)
	    {
		while (*macro != 0) *o++ = *macro++;
	    }
	}

	else	// not a macro call
	{
	    *o++ = c;
	    if (c == '\n')
	    {
		*o = 0;
		out << line;
		o = line;
	    }
	}
    }	// reached end of text

    if (o > line)
    {
	*o++ = '\n';
	*o = 0;
	out << line;
    }
    return out;
}

// The default substitution character is '$'.  Some assembly
// languages may use this, so we allow a different character.
// What appears after the $ is a function name, with an argument
// that is the name of a porthole or state.
char CGStar::substChar() const { return '$'; }

// Produce an error message.
void CGStar::codeblockError (const char* p1, const char* p2) {
	Error::abortRun(*this, "Syntax error in codeblock: ", p1, p2);
}

// Produce an error message.
// argList must be passed by reference so that the StringList is not consolidated.
void CGStar::macroError(const char* func, const StringList& argList)
{
    StringList msg;
    StringListIter nextArg(argList);
    const char* arg;

    msg << '(';
    while ((arg = nextArg++) != NULL) msg << arg;
    msg << ')';

    arg = msg;

    Error::abortRun(*this, "Unknown macro: ", func, arg);
}

// Expand macros.
// Return empty StringList on error.
// argList must be passed by reference so that the StringList is not consolidated.
StringList CGStar::expandMacro(const char* func, const StringList& argList)
{
	StringList s;
	StringListIter arg(argList);

	// ref2 provided for backward compatibility
	if (matchMacro(func, argList, "ref2", 2)) s = expandRef(arg++, arg++);
	else if (matchMacro(func, argList, "ref", 2)) s = expandRef(arg++, arg++);
	else if (matchMacro(func, argList, "ref", 1)) s = expandRef(arg++);
	else if (matchMacro(func, argList, "val", 1)) s = expandVal(arg++);
	else if (matchMacro(func, argList, "size", 1)) s = expandSize(arg++);
	else if (matchMacro(func, argList, "label", 1)) s = codeblockSymbol.lookup(arg++);
	else if (matchMacro(func, argList, "codeblockSymbol", 1)) s = codeblockSymbol.lookup(arg++);
	else if (matchMacro(func, argList, "starSymbol", 1)) s = starSymbol.lookup(arg++);
	else macroError(func, argList);

	return s;
}

// Evaluate State used as MultiPortHole index.
// Return unmodified copy of argument if no MultiPortHole reference is found.
StringList CGStar::expandPortName(const char* string)
{
    StringList port;
    const char* s = string;
    char temp[TOKLEN], *t = temp;

    while (*s != 0 && *s != '#') *t++ = *s++;
    if (*s == 0) port << string;
    else	// found '#'
    {
	*t++ = *s++;
	if (isdigit(*s))	// explicit port number
	    port << string;
	else			// lookup State value
	{
	    *t = 0;
	    port << temp << expandVal(s);
	}
    }
    return port;
}

// State or PortHole reference.
// Return empty StringList on error.
StringList CGStar::expandRef(const char* name)
{
    StringList ref;
    StringList argList = name;
    macroError("ref", argList);
    return ref;
}

// State or PortHole reference with offset.
// Return empty StringList on error.
StringList CGStar::expandRef(const char* name, const char* offset)
{
    StringList ref;
    StringList argList = name;
    argList << offset;
    macroError("ref", argList);
    return ref;
}

// State value.
// Return empty StringList on error.
StringList CGStar::expandVal(const char* name)
{
    StringList val;
    State* state = stateWithName(name);
    if (state != NULL) val = state->currentValue();
    else codeblockError(name, " is not defined as a state");
    return val;
}

// Size of State or PortHole.
StringList CGStar::expandSize(const char* name)
{
    StringList size;
    StringList portName = expandPortName(name);
    State* state = stateWithName(name);
    CGPortHole* port = (CGPortHole*)genPortWithName(portName);
    if (state != NULL) size << state->size();
    else if (port != NULL) size << port->bufSize();
    else codeblockError(name, " is not defined as a state or port");
    return size;
}

// max Comm. time.
int CGStar :: maxComm() {

	int max = 0;	// maximum traffic with anscestors.

	BlockPortIter next(*this);
	for(int i = numberPorts(); i > 0; i--) {
		PortHole* p = next++;
		if (p->isItInput() && max < p->numberTokens)
				max = p->numberTokens;
	}

	// processor 1 to 2, "max" sample, both sending plus receiving.
	return targetPtr->commTime(1,2,max,2);
}

// return NULL
CGWormhole* CGStar :: myWormhole() { return NULL; }

// The following is defined in CGDomain.cc -- this forces that module
// to be included if any CG stars are linked in.
extern const char CGdomainName[];

const char* CGStar :: domain () const { return CGdomainName;}

// isa

ISA_FUNC(CGStar, SDFStar);
