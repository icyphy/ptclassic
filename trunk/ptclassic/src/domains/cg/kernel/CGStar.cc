static const char file_id[] = "CGStar.cc";

/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee

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
}

// firing CG star : generate code.
int CGStar :: fire() {
	// No need to grab data, so just use Star::fire, not SDFStar::fire.
	int status = Star::fire();
	return status;
}

// Update all PortHoles so that the offset is incremented by the
// number of samples consumed or produced.
void CGStar::advance() {
        BlockPortIter nextPort(*this);
        CGPortHole* p;
        while ((p = (CGPortHole*) nextPort++) != 0)
                p->advance();
}

const int MAXLINELEN = 256;
const int TOKLEN = 80;

// process a CodeBlock.  This processing just substitutes for
// macro calls. The processed code is added the target's code.
void CGStar::gencode(CodeBlock& cb) {
	StringList c = processCode(cb);
	addCode((const char*)c);
}

// process a CodeBlock.  This processing just substitutes for
// macro calls and returns the result as a StringList
StringList CGStar::processCode(CodeBlock& cb) {

	StringList out = "";

	// Reset the local labels
	resetCodeblockSyms();

	const char* t = cb.getText();
	// output this text
	char line[MAXLINELEN], *o = line, c;
	while ((c = *t++) != 0) {
		if (c == substChar()) {
			// two consecutive substChar values give
			// one on the output.
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
				codeblockError ("expecting '('", " after macro call");
				return "ERROR";
			}
			// get the identifier
			char id[TOKLEN], arg2[TOKLEN], *p = id;
			while (isalnum(*t)||(*t == '_') ) *p++ = *t++;
			if (*t == '#') {
				*p++ = *t++;
				if (isdigit(*t)) {
					while (isdigit(*t)) *p++ = *t++;
				}
				else {
					char portNum[TOKLEN], *n = portNum;
					while (isalnum(*t)) *n++ = *t++;
					*n = 0;
					const char *v =lookupVal(portNum);
					while (*v != 0) *p++ = *v++;
				}
			}
			*p = 0;
			// skip any whitespace
			while (isspace(*t)) t++;
			// may be a ',' for 2nd argument
			if (*t == ',') {
				t++;
				p = arg2;
				while (isalnum(*t) || *t == '#') *p++ = *t++;
				*p = 0;
				// skip any whitespace
				while (isspace(*t)) t++;
			}
			else arg2[0] = 0;
			// must be pointing at a ')'
			if (*t++ != ')') {
				codeblockError ("expecting ')'",
						"after macro call");
				return "ERROR";
			}
			// Don't know why the following two steps can't
			// be consolidated, but if they are, the string
			// becomes null
			StringList tmp = processMacro(func,id,arg2);
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
				out += line;
				o = line;
			}
		}
	}
	if (o > line) {
		*o++ = '\n';
		*o = 0;
		out += line;
	}
	return out;
}

// Output a text line of code, raw, to the target architecture
void CGStar :: addCode (const char* code) {
	targetPtr->addCode(code);
}

// The default substitution character is '$'.  Some assembly
// languages may use this, so we allow a different character.
// What appears after the $ is a function name, with an argument
// that is the name of a porthole or state.
char CGStar::substChar() const { return '$'; }

void CGStar::codeblockError (const char* p1, const char* p2) {
	Error::abortRun(*this, "Syntax error in codeblock: ", p1, p2);
}

// the following function is provided by the SunOS and Ultrix
// libs; don't know how generally it is available.
extern "C" int strcasecmp(const char* s1, const char* s2);

// handle functions
StringList
CGStar::processMacro(const char* func, const char* id, const char* arg2) {
	StringList s;
	if (strcasecmp(func,"ref2") == 0) {
		if (*arg2 == 0) codeblockError("two arguments needed for ref2");
		else {
			s = getRef2(id,arg2);
		}
	}
	// if more two-arg funcs are added, put them before here!
	else if (*arg2 != 0) {
		codeblockError(func, " is not a 2-argument macro");
	}
	else if (strcasecmp(func, "ref") == 0) {
		s = getRef(id);
	} else if (strcasecmp(func, "val") == 0) {
		s = lookupVal(id);
	} else if ((strcasecmp(func, "label") == 0) ||
		   (strcasecmp(func, "codeblockSymbol") == 0)) {
		s = codeblockSymbol.lookup(id);
	} else if (strcasecmp(func, "starSymbol") == 0) {
		s = starSymbol.lookup(id);
	} else {
		s = "ERROR: UNKNOWN MACRO ";
		s += func;
		s += "(";
		s += id;
		s += ")";
	}
	return s;
}

// lookup reference
StringList
CGStar::getRef(const char* name) {
	StringList s = readFullName();
	s += ".";
	s += name;
	return s;
}

// lookup reference with offset
StringList
CGStar::getRef2(const char* name, const char* offset) {
	StringList s = readFullName();
	s += ".";
	s += name;
	s += "[";
	s += offset;
	s += "]";
	return s;
}

// lookup value for a state
StringList
CGStar::lookupVal(const char* name) {
	State* s;
	if ((s = stateWithName(name)) != 0) {
		StringList v = s->currentValue();
		return v;
	}
	codeblockError(name, " is not defined as a state");
	return "ERROR";
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
