static const char file_id[] = "CGStar.cc";

/******************************************************************
Version identification:
@(#)CGStar.cc	1.57	7/30/96

Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

 Programmer:  E. A. Lee, T. M. Parks

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGStar.h"
#include "CGGeodesic.h"
#include "CGWormBase.h"
#include "CGTarget.h"
#include "StringList.h"
#include "CodeStream.h"
#include "CodeBlock.h"
#include <ctype.h>

// The following is defined in CGDomain.cc -- this forces that module
// to be included if any CG stars are linked in.
extern const char CGdomainName[];

extern const Attribute A_GLOBAL = {AB_GLOBAL,0};
extern const Attribute A_LOCAL  = {0,AB_GLOBAL};
extern const Attribute A_SHARED = {AB_SHARED,0};
extern const Attribute A_PRIVATE = {0,AB_SHARED};

/*******************************************************************

	class CGStar methods

********************************************************************/

// Constructor
CGStar :: CGStar() : dataParallel(0), profile(0), forkId(0) {
	starSymbol.initialize();
        addState(procId.setState("procId", this, "-1",
                "assigned processor id. If -1, not assigned manually."));
}

// illegal to try to execute CGStars dynamically
int CGStar :: setDynamicExecution(int dyn) {
	if (dyn) {
		Error::abortRun(*this,
			"this star cannot run under a dynamic scheduler");
		return FALSE;
	}
	else return TRUE;
}

// firing CG star : generate code.
int CGStar :: run() {
	// No need to grab data, so just use Star::run, not DataFlowStar::run.
	int status = Star::run();
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
                p->advance();
	}
}

const int MAXLINELEN = 1024;
const int TOKLEN = 1024;


// Create a new CodeStream called 'name' if it doesn't exist.  Return it's 
// pointer.
CodeStream* CGStar::newStream(const char* name) {
	CodeStream* code = cgTarget()->newStream(name);
	if (!code) {
		Error::abortRun(*this,"newStream: allocation failed ",name);
	}
	return code;
}

// Lookup a shared symbol by scope name & symbol name.
// Return NULL on error.
const char* CGStar::lookupSharedSymbol(const char* scope, const char* name)
{
    return cgTarget()->lookupSharedSymbol(scope, name);
}

// Set the Target pointer, initialize the symbols to point to the target
// and initize the pointer to the stream code.
int CGStar :: setTarget(Target* t)
{
    	if (!Star::setTarget(t)) return FALSE;
	codeblockSymbol.setSeparator(cgTarget()->separator);
	codeblockSymbol.setCounter(cgTarget()->symbolCounter());
	starSymbol.setSeparator(cgTarget()->separator);
	starSymbol.setCounter(cgTarget()->symbolCounter());
	return TRUE;
}

// Add a string to the Target code.
// Expand macros in code and name.
int CGStar::addCode (const char* string,const char* stream, const
		     char* name) {
    CodeStream* cs = getStream(stream);
    if (cs != NULL) {
	if (name != NULL) {
	    StringList temp;
	    processCode(temp,name);
	    if (!cs->sharedNames.isUnique(temp)) return FALSE;
	}
	return processCode(*cs,string);
    }
    else {
	return FALSE;
    }
}

// Add a procedure to the target procedure stream.
int CGStar::addProcedure(const char* string, const char* name) {
   CodeStream* proc = getStream(PROCEDURE);
   if (proc != NULL) {
       if (name != NULL) {
	   StringList temp;
	   processCode(temp,name);
	   if (!proc->sharedNames.isUnique(temp)) return FALSE;
       }
       return processCode(*proc,string);
   }
   else
       return FALSE;
}

// Add a comment to a target stream.
void CGStar::outputComment (const char* msg,const char* stream)
{
	CodeStream* cs = getStream(stream);
	if (cs != NULL) *cs << cgTarget()->comment(msg);
}

// Process a CodeBlock, expanding macros.
// Return empty StringList on error.
int CGStar::processCode(StringList& list, CodeBlock& block) {
    const char* text = block.getText();
    return processCode(list,text);
}

// Parse text expanding macros
int CGStar::processCode(StringList& list, const char* text)
{
    if (text == NULL) return TRUE;
    // Reset the local labels
    resetCodeblockSyms();

    // scan until end of text
    while (*text != 0)
    {
	// parse eventually nested macro invocations
	if (!processMacro(list,text)) return FALSE;
    }
    return TRUE;
}

// Parse macro including macro header and arguments;
// update text pointer t to point after the end of the argument list.
int CGStar::processMacro(StringList& list, const char*& t) {

    // copy any character before the macro header unchanged
    while (*t != '\0') {

	if (*t == substChar())	// parse macro
	{
	    // two consecutive substChar values give one on the output
	    if (t++, *t == substChar())
	    {
		t++;
		list << substChar();
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
		return FALSE;
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
		int level=1;
		while (*t != '\0') {

		    if ((level == 1) && (*t == ',' || *t == ')'))
			break;   // reached end of argument

	       else if (*t == substChar()) {
			if (t[1] == substChar())
			    t++; // skip this character, copy next
			else {
			    // nested macro invocation
			    StringList list;
			    if (!processMacro(list,t)) return FALSE;
			    const char* s = list;
			    while (*s)  *p++ = *s++;
			}
		     } else {
			 // adapt nesting level
			if (*t == '(')  level++;
		   else if (*t == ')')  level--;

			*p++ = *t++;
		     }
		}

		if (p == arg) // null arguments are not allowed
		{
		    codeblockError ("null argument");
		    return FALSE;
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
			return FALSE;
		    }
		}
	    }
	    t++;	// skip ')' at end of argument list

	    list << expandMacro(func, argList);
	    return TRUE;
	}
	else	// not a macro call
	{
	    const char* start = t;
	    int length = 0;
	    while (*t != '\0' && *t != substChar()) {
		length++;
		t++;
	    }
	    if (*t == '\0')
		list << start;
	    else {
		char *temp = new char[length+1];
		temp[length] = '\0';
		strncpy(temp,start,length);
	    	list << temp;
		delete [] temp; 
	    }
	}
    }
    if (list.length())
    {
	const char* string = list.tail();
	if(string[strlen(string)-1] != '\n') list << '\n';
    }
    return TRUE;
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
	const char* arg1 = arg++;
	const char* arg2 = arg++;

	// ref2 provided for backward compatibility
	if (matchMacro(func, argList, "ref2", 2)) s = expandRef(arg1, arg2);
	else if (matchMacro(func, argList, "ref", 2)) s = expandRef(arg1, arg2);
	else if (matchMacro(func, argList, "ref", 1)) s = expandRef(arg1);
	else if (matchMacro(func, argList, "val", 1)) s = expandVal(arg1);
	else if (matchMacro(func, argList, "size", 1)) s = expandSize(arg1);
	else if (matchMacro(func, argList, "label", 1)) s = codeblockSymbol.lookup(arg1);
	else if (matchMacro(func, argList, "codeblockSymbol", 1)) s = codeblockSymbol.lookup(arg1);
	else if (matchMacro(func, argList, "starSymbol", 1)) s = starSymbol.lookup(arg1);
	else if (matchMacro(func, argList, "sharedSymbol", 2)) s = lookupSharedSymbol(arg1, arg2);
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
    GenericPort* port = genPortWithName(portName);
    if (state != NULL) size << state->size();
    else if (port != NULL) {
	if (!port->isItMulti())	
	    size << ((CGPortHole*)port)->bufSize();
	else
	    size << ((MultiPortHole*)port)->numberPorts();
    }
	
    else codeblockError(name, " is not defined as a state or port");
    return size;
}

// max Comm. time.
int CGStar :: maxComm() {

	int max = 0;	// maximum traffic with anscestors.

	BlockPortIter next(*this);
	for(int i = numberPorts(); i > 0; i--) {
		PortHole* p = next++;
		if (p->isItInput() && max < p->numXfer())
				max = p->numXfer();
	}

	// processor 1 to 2, "max" sample, both sending plus receiving.
	return target()->commTime(1,2,max,2);
}

// deferrable: never defer a fork; always defer a non-fork star that
// feeds into a fork.
// this prevents the resulting fork buffer from being larger than
// necessary; new tokens are not added until they must be.

// If Forks are cascaded, wait until all cascaded forks are fired.
// A hack to make all cascade forks fired before any other stars.
static SequentialList cascadeForks;

int CGStar :: deferrable() {
	if (isItFork()) {
		if (cascadeForks.remove(this) || (cascadeForks.size() == 0)) {
			CGStarPortIter nextp(*this);
			CGPortHole *p;
			while ((p = nextp++) != 0) {
				if (p->isItOutput() && (!p->atBoundary())) {
				    CGStar* s = (CGStar*) p->far()->parent();
				    if (s->isItFork()) {
					cascadeForks.append(s);
				    }
				}
			}
			return FALSE;
		} 
		
	} 
	if (cascadeForks.size() > 0) return TRUE;

	CGStarPortIter nextp(*this);
	CGPortHole *p;
	while ((p = nextp++) != 0) {
		if (p->isItOutput() && p->cgGeo().forkType() == F_SRC)
			return TRUE;
	}
	return DataFlowStar::deferrable();
}

// Code to shift delays from a fork's input port to its output port,
// and to make it match the rate of its input.

void CGStar :: forkInit(CGPortHole& input,MultiCGPort& output) {
	// make relation of forkSrc, forkDest between input/output portholes
	// only once.
	MPHIter nextp(output);
	CGPortHole* p;
	if (isItFork() == FALSE) {
		cascadeForks.initialize();
		isaFork();
		while ((p = (CGPortHole*)nextp++) != 0) {
			p->setForkSource(&input);
			p->setRelation(DF_SAME,&input);
		}
	}
	isaFork();
	int n = input.far()->numXfer();
	input.setSDFParams(n,n-1);
	output.setSDFParams(n,n-1);
	int srcDelay = input.numInitDelays();
	if (srcDelay > 0) {
		// move delays from input to outputs
		PortHole* f = input.far();
		input.disconnect();
		f->connect(input,0);
		nextp.reset();
		while ((p = (CGPortHole*)nextp++) != 0) {
			int n = p->numInitDelays();
			f = p->far();
			p->disconnect();
			p->connect(*f,n+srcDelay);
		}
		// re-initialize all neighbors.
		input.far()->parent()->initialize();
		nextp.reset();
		while ((p = (CGPortHole*)nextp++) != 0) {
			p->far()->parent()->initialize();
		}
	}
}

// similar to the above, but for only one output.  Useful in implementing
// a star as an "identity".
void CGStar :: forkInit(CGPortHole& input,CGPortHole& output) {
	if (isItFork() == FALSE) {
		cascadeForks.initialize();
		output.setForkSource(&input);
		isaFork();
	}
	int n = input.far()->numXfer();
	input.setSDFParams(n,n-1);
	output.setSDFParams(n,n-1);
	output.setRelation(DF_SAME,&input);
	int srcDelay = input.numInitDelays();
	if (srcDelay > 0) {
		// move delays from input to outputs
		int n = output.numInitDelays();
		PortHole* f = input.far();
		input.disconnect();
		f->connect(input,0);
		f = output.far();
		output.disconnect();
		output.connect(*f,n+srcDelay);
		// re-initialize all neighbors.
		input.far()->parent()->initialize();
		output.far()->parent()->initialize();
	}
}

// return the profile
Profile* CGStar :: getProfile(int) { return profile; }

const char* CGStar :: readTypeName() { return "unspecfied"; }

const char* CGStar :: domain () const { return CGdomainName;}

ISA_FUNC(CGStar, DynDFStar);


