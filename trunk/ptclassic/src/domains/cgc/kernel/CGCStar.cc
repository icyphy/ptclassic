static const char file_id[] = "CGCStar.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY

 Programmer: S. Ha, E. A. Lee, T. M. Parks

 This is the baseclass for stars that generate C language code

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "CGCStar.h"
#include "CGCGeodesic.h"
#include "Tokenizer.h"
#include "ComplexState.h"
#include <ctype.h>

// The following is defined in CGCDomain.cc -- this forces that module
// to be included if any CGC stars are linked in.
extern const char CGCdomainName[];

const char* CGCStar :: domain () const { return CGCdomainName;}

// isa

ISA_FUNC(CGCStar, CGStar);

// Sanitize a string so that it is usable as a C identifier.
const char* sanitize(const char* string)
{
    const int MAX_LENGTH = 64;
    static char clean[MAX_LENGTH];
    char* c = clean;
    int i = 0;

    // Check for leading digit.
    if (isdigit(*string))
    {
	*c++ = '_';
	i++;
    }

    // Replace strange charachters.
    while (++i < MAX_LENGTH && *string)
    {
	*c++ = isalnum(*string) ? *string : '_';
	string++;
    }
    *c = 0;

    return clean;
}

// Reference to State or PortHole.
StringList CGCStar::expandRef(const char* name)
{
    StringList ref;
    CGCPortHole* port;
    State* state;
    StringList portName = expandPortName(name);

    // Expand State reference.
    if ((state = stateWithName(name)) != NULL)
    {
	registerState(state);
	ref << starSymbol.lookup(state->name());
    }

    // Expand PortHole reference.
    else if ((port = (CGCPortHole*)genPortWithName(portName)) != NULL)
    {
	ref << port->getGeoName();

	// Use array notation for large buffers and for embedded buffers
	// which are referenced through a pointer.
	if (port->bufSize() > 1 || port->bufType() == EMBEDDED)
	{
	    ref << '[';
	    if (port->staticBuf()) ref << port->bufPos();
	    else ref << sanitize(starSymbol.lookup(port->name()));
	    ref << ']';
	}
    }

    // Could not find State or PortHole with given name.
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
    State *state, *offsetState;
    StringList offsetVal;
    CGCPortHole* port;
    StringList portName = expandPortName(name);

    // Use State value as offset (if such a State exists).
    if ((offsetState = stateWithName(offset)) != NULL)
    {
	// Get State value as a string.
	if (offsetState->isA("IntState"))
	{
	    offsetVal = expandVal(offset);
	    offset = offsetVal;
	}
	else
	{
	    codeblockError(offset, " is not an IntState");
	    ref.initialize();
	    return ref;
	}
    }

    // Expand array State reference.
    if ((state = stateWithName(name)) != NULL)
    {
	if (state->isArray())
	{
	    registerState(state);
	    ref << starSymbol.lookup(state->name()) << '[' << offset << ']';
	}
	else
	{
	    codeblockError(name, " is not an array state");
	    ref.initialize();
	}
    }

    // Expand PortHole reference with offset.
    else if (port = (CGCPortHole*)genPortWithName(portName))
    {
	ref << port->getGeoName();

	// Use array notation for large buffers and for embedded buffers
	// which are referenced through a pointer.
	if (port->bufSize() > 1 || port->bufType() == EMBEDDED)
	{
	    ref << '[';

	    // generate constant for index
	    if (port->staticBuf() && (offsetState != NULL))
	    {
		int offset = *(IntState*)offsetState;
		ref << ( (port->bufPos() - offset + port->bufSize())
			 % port->bufSize() );
	    }

	    // generate expression for index
	    else
	    {
		ref << '(';
		if (port->staticBuf()) ref << port->bufPos();
		else ref << sanitize(starSymbol.lookup(port->name()));
		ref << "-(" << offset << ')';
		if (!port->linearBuf())	// use circular addressing
		{
		    ref << '+' << port->bufSize() << ")%" << port->bufSize();
		}
		else ref << ')';
	    }

	    ref << ']';
	}
    }
    return ref;
}

// add a splice star to the spliceClust list.  If atEnd
// is true, append it to the end, otherwise prepend it.
void CGCStar :: addSpliceStar(CGCStar* s, int atEnd) {
	if (spliceClust.member(s)) return;
	if (atEnd) spliceClust.append(s);
	else spliceClust.prepend(s);
}

void CGCStar::registerState(State* state)
{
    // If the State is not already on the list of referenced States, add it.
    StateListIter stateIter(referencedStates);
    State* s;
    while ((s = stateIter++) != NULL)
	if (s == state) return;
    
    referencedStates.put(*state);
}

void CGCStar::initialize() {
	CGStar::initialize();
	referencedStates.initialize();
	spliceClust.initialize();
	if (!spliceClust.member(this)) spliceClust.append(this);
}

/////////////////////////////////////////////////////////////
// runIt: 
//	(1) prefix the code with a comment
//	(2) body of code
//	(3) update offset if necessary
//	(4) move output data for Spread/Collect pair
/////////////////////////////////////////////////////////////

int CGCStar :: run() {
	int status;
	if (spliceClust.size() > 1) {
		ListIter nextStar(spliceClust);
		CGCStar* s;
		while ((s = (CGCStar*) nextStar++) != 0) {
			status = s->runIt();
			if (!status) return status;
		}
	}
	else status = runIt();
	return status;
}

int CGCStar::runIt() {
	StringList code = "\t{  /* star ";
	code << fullName() << " (class " << className() << ") */\n";
	addCode(code);
	int status = CGStar::run();
	if (!status) return status;

	if (isItFork()) {
		addCode("\t}\n");
		return status;
	}

	// update the offset member
	updateOffsets();

	// data movement between shared (embedding) buffers if necessary
	// after we write to the first shared buffer (good for Spread/
	// Collect combination.)
	moveDataBetweenShared();

	return status;
}

// After firing that star, we may need to move the input data between
// shared buffers (for example, Spread/Collect) since these movements
// are not visible from the user.
// In case, embedded portholes are connected together.
void CGCStar :: moveDataBetweenShared() {
	StringList code;

	BlockPortIter next(*this);
	CGCPortHole* p;
	while ((p = (CGCPortHole*) next++) != 0) {
		// consider output only
		if (p->isItInput()) continue;

		CGCPortHole* farP = (CGCPortHole*) p->far();
		if (farP == 0) {
			if (!p->switched()) continue;
			farP = (CGCPortHole*) p->cgGeo().destPort();
		}

		if (farP->embedding() == 0) continue;

		BlockPortIter piter(*farP->parent());
		CGCPortHole* op;
		// march from output#1 ....
		while ((op = (CGCPortHole*) piter++) != 0) {
			CGCPortHole* ip = (CGCPortHole*) op->far();
			CGCPortHole* ep = (CGCPortHole*) ip->embedded();
			if (ep == 0) continue;

			// now we check whether we need to move data
			// we know that writing to the shared
			// buffer is always static and linear.
			int to = p->bufPos() - p->numXfer();
			if (to < 0) to += p->bufSize();
			int from = to - p->numXfer() + 1;

			int start = op->whereEmbedded();
			int stop = start + op->numXfer() - 1;

			if (stop < from) continue;
			if (start > to) {
				if (code.length() > 0) addCode(code);
				return;
			}

			int there;
			if (from <= start) there = ip->whereEmbedded();
			else  there = ip->whereEmbedded() + from - start;

			if (start < from) start = from;
			if (stop > to) stop = to;

			const char* farName = ep->getGeoName();

			code << "\t/* moveDataBetweenShared */\n";
			if (op->numXfer() > 1) {
				code << "\t{ int i,j;\n\t  j = " << there;
				code << ";\n\t  for(i = " << start;
				code << "; i <= " <<  stop << "; i++) {\n";
				code << "\t\t" << farName << "[j++] = ";
				code << p->getGeoName() << "[i];\n\t}\n";
			} else {
				code << "\t" << farName << '[' << there << ']';
				code << " = " << p->getGeoName();
				code << '[' << start << "];\n";
			}
		}
	}
	if (code.length() > 0) addCode(code);
}

void CGCStar :: updateOffsets()
{
	StringList code2;

	BlockPortIter next(*this);
	CGCPortHole* p;
	while ((p = (CGCPortHole*) next++) != 0) {
		int bs = p->bufSize();
		if (bs > 1 && p->staticBuf() == FALSE) {
			int nx = p->numXfer();
			if (nx == bs) continue;
			StringList pname = sanitize(starSymbol.lookup(p->name()));
			code2 << "\t" << pname << " += ";
			code2 << nx << ";\n\tif (" << pname << " >= ";
			code2 << bs << ")\n\t\t";
			code2 << pname << " -= " << bs << ";\n";
		}
	}
	code2 << "\t}\n";
	addCode(code2);
}

// Declare PortHole data structures.
StringList CGCStar::declarePortHoles()
{
    StringList dec;	// declarations

    CGCPortHole* port;
    BlockPortIter portIter(*this);
    while ((port = (CGCPortHole*)portIter++) != NULL)
    {
	dec << declareBuffer(port);
	dec << declareOffset(port);
    }

    return dec;
}

// Generate initialization code for PortHole data structures.
StringList CGCStar::initCodePortHoles()
{
    StringList code;	// initialization code

    CGCPortHole* port;
    BlockPortIter portIter(*this);
    while ((port = (CGCPortHole*)portIter++) != NULL)
    {
	code << initCodeBuffer(port);
	code << initCodeOffset(port);
    }

    return code;
}

// Declare State data structures.
StringList CGCStar::declareStates()
{
    StringList dec;	// declarations

    State* state;
    StateListIter stateIter(referencedStates);
    while ((state = stateIter++) != NULL)
    {
	dec << declareState(state);
    }

    return dec;
}

// Generate initialization code for State data structures.
StringList CGCStar::initCodeStates()
{
    StringList code;	// initialization code

    State* state;
    StateListIter stateIter(referencedStates);
    while ((state = stateIter++) != NULL)
    {
	code << initCodeState(state);
    }

    return code;
}

// Declare PortHole buffer.
StringList CGCStar::declareBuffer(const CGCPortHole* port)
{
    StringList dec;	// declarations

    if (!port->switched() && port->isItOutput())
    {
	StringList name = port->getGeoName();
	DataType type = port->resolvedType();

	if (type == INT) dec << "int";
	else if (type == COMPLEX) dec << "complex";
	else dec << "double";

	if (port->bufType() == EMBEDDED)
	{
	    dec << " *" << name << ";\n";        // declare a pointer
	}
	else	// not embedded
	{
	    dec << " " << name;
	    if (port->bufSize() > 1)	// declare as array
		dec << '[' << port->bufSize() << ']';
	    dec << ";\n";
	}
    }
    return dec;
}

// Generate initialization code for PortHole buffer.
StringList CGCStar::initCodeBuffer(CGCPortHole* port)
{
    StringList code;

    if (!port->switched() && port->isItOutput())
    {
	StringList name = port->getGeoName();
	if (port->bufType() == EMBEDDED)
	{
	    CGCPortHole* far = (CGCPortHole*) port->far();
	    CGCPortHole* host;		// Host buffer.
	    int loc = 0;		// Embedding location.

	    if (port->embedded() != NULL)
	    {
		host = ((CGCPortHole*) port->embedded())->realFarPort();
		loc = port->whereEmbedded();
	    }
	    else
	    {
		host = (CGCPortHole*) far->embedded();
		loc = far->whereEmbedded();
	    }

	    code << name << " = &" << host->getGeoName();
	    if (host->bufSize() > 1) code << '[' << loc << ']';
	    code << ";\n";
	}
	else	// not embedded
	{
	    DataType type = port->resolvedType();
	    int array = (port->bufSize() > 1);

	    if (array)
		code << "{int i; for(i=0;i<" << port->bufSize() << ";i++) ";
	    code << name;
	    if (array) code << "[i]";
	    if (type == INT)
	    {
		code << " = 0;";
	    }
	    else if (type == COMPLEX)
	    {
		code << ".real = " << name;
		if (array) code << "[i]";
		code << ".imag = 0.0;";
	    }
	    else	// default to double
	    {
		code << " = 0.0;";
	    }
	    if (array) code << '}';
	    code << '\n';
	}
    }
    return code;
}

// Declare PortHole buffer index.
StringList CGCStar::declareOffset(const CGCPortHole* port)
{
    StringList dec;	// declarations

    if (!port->staticBuf())
    {
	dec << "int " << sanitize(starSymbol.lookup(port->name()))
	    << ";\n";
    }
    return dec;
}

// Generate initialization code for PortHole buffer index.
StringList CGCStar::initCodeOffset(const CGCPortHole* port)
{
    StringList code;	// initialization code

    if (!port->staticBuf())
    {
	code << sanitize(starSymbol.lookup(port->name()))
	    << " = " << port->bufPos() << ";\n";
    }
    return code;
}

// Declare State variable.
StringList CGCStar::declareState(const State* state)
{
    StringList dec;

    if (state->isA("IntState") || state->isA("IntArrayState"))
	dec << "int";
    else if (state->isA("ComplexState") || state->isA("ComplexArrayState"))
	dec << "complex";
    else if (state->isA("StringState") || state->isA("StringArrayState"))
	dec << "char*";
    else dec << "double";

    dec << " " << starSymbol.lookup(state->name());
    if (state->isArray()) dec << "[" << state->size() << "]";
    dec << ";\n";

    return dec;
}

// Generate initialization code for State variable.
StringList CGCStar::initCodeState(const State* state)
{
    StringList code;
    StringList val = state->currentValue();
    StringList name = starSymbol.lookup(state->name());

    if (state->isArray())	// Array initialization.
    {
	/* By convention, array States separate each value by a
	   carriage return character, '\n'.  Scan through the
	   StringList returned by State::currentValue() looking for
	   these separators.
	*/

	const char* special = "";
	const char* white = "\n";
	Tokenizer lexer(val, special, white);
	char token[256];

	lexer >> token;
	for (int i = 0; *token != '\0'; i++, lexer >> token)
	{
	    code << name << '[' << i << ']';
	    if (state->isA("ComplexArrayState"))
	    {
		ComplexState cxState;
		cxState.setInitValue(token);
		cxState.initialize();
		Complex x = cxState;

		code << ".real=" << x.real() << ";\n";
		code << name << '[' << i << ']';
		code << ".imag=" << x.imag() << ";\n";
	    }
	    else if (state->isA("StringArrayState"))
	    {
		code << '=' << '"' << token << '"' << ";\n";
	    }
	    else
	    {
		code << '=' << token << ";\n";
	    }
	}
    }
    else	// Scalar initialization.
    {
	code << name;
	if (state->isA("ComplexState"))
	{
	    ComplexState cxState;
	    cxState.setInitValue(val);
	    cxState.initialize();
	    Complex x = cxState;

	    code << ".real=" << x.real() << ";\n";
	    code << name;
	    code << ".imag=" << x.imag() << ";\n";
	}
	else if (state->isA("StringState"))
	{
	    code << '=' << '"' << val << '"' << ";\n";
	}
	else
	{
	    code << '=' << val << ";\n";
	}
    }
    return code;
}

// Add lines to be put at the beginning of the code file
int CGCStar :: addInclude(const char* decl) {
	StringList temp = "#include ";
	temp << decl << "\n";
	return addCode(temp, "include", decl);
}
