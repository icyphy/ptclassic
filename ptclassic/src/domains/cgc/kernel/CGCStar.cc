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
#include "CGCTarget.h"
#include "CGCGeodesic.h"
#include "Tokenizer.h"

// The following is defined in CGCDomain.cc -- this forces that module
// to be included if any CGC stars are linked in.
extern const char CGCdomainName[];

const char* CGCStar :: domain () const { return CGCdomainName;}

// isa

ISA_FUNC(CGCStar, CGStar);

// return the actual buffer position to write to.
StringList CGCStar :: getActualRef(CGCPortHole* port, const char* offset) {
	StringList ref;
	ref << port->getGeoName();
	if (port->bufSize() > 1) {
		ref << "[(";
		if (port->staticBuf() == FALSE) {
			ref << targ()->offsetName(port);
		} else {
			ref << port->bufPos();
		}
		if (offset != 0) {
	    		ref << " - (" << offset << ")";
	    		if (port->linearBuf() == FALSE) {
	    			ref << " + " << port->bufSize() << ") % "
					<< port->bufSize();
	    		} else {
				ref << ")";
	    		}
	    		ref << ']';
		} else {
			ref << ")]";
		}
	}
    return ref;
}

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
	ref << targ()->correctName(*st);
    }
    else if ((port = (CGCPortHole*)genPortWithName(portName)) != NULL)
    {
	// for embedded porthole
	if (port->bufType() == EMBEDDED) {
		ref << "(*" << port->getGeoName() << ")";
	} else {
		ref << getActualRef(port, 0);
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
    int valOffset;
    int useState = FALSE;

    if ((state = stateWithName(offset)) != NULL)
    {
	// Note: currently only the value of a State can be used as an offset
	if (state->isA("IntState"))
	{
	    stateVal = expandVal(offset);
	    offset = stateVal;
	    valOffset = int(*(IntState*)state);
	    useState = TRUE;
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
	if (ref.length() == 0) return ref;	// error in getRef()
	if (state->size() > 1) ref << '[' << offset << ']';
    }
    else if (port = (CGCPortHole*)genPortWithName(portName))
    {
	// for embedded porthole.
	if ((port->bufSize() == 1) && (port->bufType() == EMBEDDED)) {
		ref << "(*";
		ref << port->getGeoName();
		ref << ")";
		return ref;
	}

	if (port->bufSize() > 1) {
		if ((port->staticBuf() == TRUE)&&(useState == TRUE)) {
			ref << port->getGeoName();
			int v = (port->bufPos() - valOffset +
				 	port->bufSize()) % port->bufSize();
			ref << '[' << v << ']';
		} else {
			ref << getActualRef(port, offset);
		}
	} else {
		ref << port->getGeoName();
	}
    }
    return ref;
}

void CGCStar :: initBufPointer() {
	BlockPortIter next(*this);
	CGCPortHole* p;
	while ((p = (CGCPortHole*) next++) != 0) {
		StringList out = initializeOffset(p);
		out << initializeBuffer(p);
		if (out.length() > 0) addMainInit(out);
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
			if(strcmp(name,sp->name()) == 0) return;
		referencedStates.put(*state);
	}
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

void CGCStar :: updateOffsets() {

	CGCTarget* t = targ();
	StringList code2;

	BlockPortIter next(*this);
	CGCPortHole* p;
	while ((p = (CGCPortHole*) next++) != 0) {
		int bs = p->bufSize();
		if (bs > 1 && p->staticBuf() == FALSE) {
			int nx = p->numXfer();
			if (nx == bs) continue;
			StringList pname = t->offsetName(p);
			code2 << "\t" << pname << " += ";
			code2 << nx << ";\n\tif (" << pname << " >= ";
			code2 << bs << ")\n\t\t";
			code2 << pname << " -= " << bs << ";\n";
		}
	}
	code2 << "\t}\n";
	addCode(code2);
}

// Convert from PortHole type to C data type.
const char* ctype(PortHole& p)
{
    DataType t = p.resolvedType();

    if (t == INT) return "int";
    else if (t == COMPLEX) return "complex";
    else return "double";
}

// Convert from State type to C data type.
const char* ctype(State& state)
{
    const char* t = state.type();

    if (strcmp(t, "INT") == 0 || strcmp(t, "INTARRAY") == 0)
        return "int";
    else if (strcmp(t, "COMPLEX") == 0 || strcmp(t, "COMPLEXARRAY") == 0)
        return "complex";
    else if (strcmp(t, "STRING") == 0 || strcmp(t, "STRINGARRAY") == 0)
        return "char*";
    else
        return "double";
}

	/////////////////////////////////////////////
	// porthole declarations and initializations
	/////////////////////////////////////////////

// Define variables only for each output port, except outputs of forks.
StringList CGCStar :: declarePortHole(CGCPortHole* p) {
	StringList out;
	if (p->isItOutput()) {
		emptyFlag = FALSE;
		int dimen = p->bufSize();

		out << ctype(*p);

		// if embedded properly, do not allocate the buffer.
		if (p->bufType() == EMBEDDED) {
			out << "*";	// add pointer
			dimen = 0;
		}

		out << " " << p->getGeoName();

		if(dimen > 1) {
			out << "[" << dimen << "]";
		}
		out << ";\n";
	}
	return out;
}

// Type-specific initialization for PortHole data structures.
StringList typeInit(CGCPortHole& port, int dim = 1,
	double realVal = 0.0, double imagVal = 0.0)
{
    DataType typeName = port.resolvedType();
    const char* pName = port.getGeoName();

    StringList out;
    out << pName;
    if (dim > 0) out << "[i]";

    if (typeName == COMPLEX)
    {
	out << ".real = " << realVal << ";\n\t";
	out << pName;
	if (dim > 0) out << "[i]";
	out << ".imag = " << imagVal << ";\n";
    }
    else out << " = " << realVal << ";\n";
    return out;
}

// Initialize the porthole
StringList CGCStar :: initializeBuffer(CGCPortHole* p) {
	StringList out;
	if (p->isItInput()) return out;

	out << "    /* initializeBuffer for " << p->fullName() << " */\n";
	CGCPortHole* farP = (CGCPortHole*) p->far();

	// if embedded properly, do not allocate the buffer.
	if (p->bufType() == EMBEDDED) {
		out << "    " << p->getGeoName() << " = ";

		CGCPortHole* ep;	// embedding buffer.
		int loc = 0;
		// there are some hacks to handle buffer size = 1 scalar.
		if (p->embedded()) {
			ep = ((CGCPortHole*) p->embedded())->realFarPort();
			loc = p->whereEmbedded();
			if (ep->maxBufReq() == 1) out << "&";
			out << ep->getGeoName();
		} else if (farP->embedded()) {
			ep = (CGCPortHole*) farP->embedded();
			loc = farP->whereEmbedded();
			if (ep->maxBufReq() == 1) out << "&";
			out << ep->getGeoName();
		}
		if (ep->maxBufReq() > 1)
			out << " + " << loc;
		out << ";\n";
		return out;

	} 

	// for owner buffer
	if (p->bufSize() > 1) {
		// initialize output buffer
		StringList init = typeInit(*p);

		out << "    { int i;\n    for (i = 0; i < ";
		out << p->bufSize() << "; i++) {\n\t" << init << "    }}\n";
	}

	return out;
}

	////////////////////////////////////
	// declare and initialize offset
	////////////////////////////////////

// declare offset and copy_offset for moving embedded data.
StringList CGCStar :: declareOffset(const CGCPortHole* p) {
	StringList out;
	if ((p->bufSize() > 1) && (p->staticBuf() == FALSE)) {
		emptyFlag = FALSE;
		out << "    " << "int ";
		out << targ()->offsetName(p) << ";\n";
	}
	return out;
}

StringList CGCStar :: initializeOffset(const CGCPortHole* p) {
	StringList out;
	if ((p->bufSize() > 1) && (p->staticBuf() == FALSE)) {
		out << "    /* initializeOffset for " << p->fullName()
		    << " */\n";
		out << "    " << targ()->offsetName(p);
		out << " = " << p->bufPos() << ";\n";
	}
	return out;
}

	////////////////////////////////////
	// declare and initialize states
	////////////////////////////////////

// Type-specific initialization for State data structures.
StringList typeInit(const State& state, const char* val)
{
    const char* typeName = state.type();

    StringList out;
    if ( (strcmp(typeName, "COMPLEX") == 0)
	|| (strcmp(typeName, "COMPLEXARRAY") == 0) )
    {
	const char* special = "(,)";
	Tokenizer lexer(val,special);
	char* token;

	do	// find token following '('
	{
	    lexer >> token;
	} while (*token != '(');

	lexer >> token;		// real part
	out << "{ " << token << ", ";

	do	// find token following ','
	{
	    lexer >> token;
	} while (*token != ',');

	lexer >> token;		// imaginary part
	out << token << " }";
    }
    else if ( (strcmp(typeName, "STRING") == 0)
	|| (strcmp(typeName, "STRINGARRAY") == 0) )
    {
	out << '"' << val << '"';
    }
    else	// simple type
    {
	out << val;
    }
    return out;
}

// declare and initialize state
// Note: no automatic aggregate initialization is possibile inside
// functions.

StringList CGCStar :: declareState(const State* s)
{
    StringList dec;	// declaration
    StringList code;	// initialization code

    int insideMain = targ()->makingFunc();
    StringList sname = targ()->correctName(*s);

    dec << "    " << ctype(*s) << " " << sname;
    if (s->isArray()) dec << "[" << s->size() << "]";

    if (s->isArray())	// initialize each element
    {
	/* By convention, array States separate each value by a
	   carriage return character, '\n'.  Scan through the
	   StringList returned by State::currentValue() looking for
	   these separators. (Wouldn't it be easier if each value were
	   simply in a separate string of the StringList?  Then it
	   would be possible to iterate through the StringList to get
	   these values. tmp)
	*/

	const char* special = "";
	const char* white = "\n";
	StringList val = s->currentValue();
	Tokenizer lexer(val, special, white);
	char* token;

	if (insideMain) dec << ";\n";
	else dec << " = {\n";

	lexer >> token;
	for (int i = 0; *token != '\0'; i++, lexer >> token)
	{
	    StringList init = typeInit(*s, token);
	    if (insideMain)
	    {
		code << sname << "[" << i << "] = " << init << ";\n";
	    }
	    else
	    {
		// separator before all but first
		if (i != 0) dec << ",\n";
		dec << "\t" << init;
	    }
	}
	if (!insideMain) dec << " };\n";
    }
    else	// not an array
    {
	StringList val = s->currentValue();
	StringList init = typeInit(*s, val);
	if (insideMain)
	{
	    dec << ";\n";
	    code << sname << " = " << init << ";\n";
	}
	else
	{
	    dec << " = " << init << ";\n";
	}
    }
    if (insideMain) addMainInit(code);
    emptyFlag = FALSE;
    return dec;
}


	////////////////////////////////////
	// code stream management
	////////////////////////////////////

// Add lines to be put at the beginning of the code file
int CGCStar :: addInclude(const char* decl) {
	StringList temp = "#include ";
	temp << decl << "\n";
	return addCode(temp, "include", decl);
}

