static const char file_id[] = "CGCStar.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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
#include "FixState.h"
#include "FixArrayState.h"
#include "PrecisionState.h"
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

    // Replace strange characters.
    while (++i < MAX_LENGTH && *string)
    {
	*c++ = isalnum(*string) ? *string : '_';
	string++;
    }
    *c = 0;

    return clean;
}

// substitute macros for fixed point variables
StringList CGCStar::expandMacro(const char* func, const StringList& argList)
{
    // $precision(fix_variable)
    if (matchMacro(func, argList, "precision",1)) {
	StringListIter arg(argList);
	return expandFixPrecisionMacro(arg++);
    }
    // $precision(fix_variable,offset)
    else if (matchMacro(func, argList, "precision",2)) {
	StringListIter arg(argList);
	const char* arg0 = arg++;
	return expandFixPrecisionMacro(arg0,arg++);
    }

    return CGStar::expandMacro(func, argList);
}

// Reference to State or PortHole.
// For references to states or ports of type FIX the reference is replaced
// by the triplet "length,intbits,fix_variable"
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

	// pass precision for fix types
        if (state->isA("FixState") || state->isA("FixArrayState"))
	{
	    Precision prec;

	    // precision() should be a virtual function here
	    if (state->isArray())
		 prec = ((FixArrayState*)state)->precision();
	    else prec = ((FixState*)state)->precision();

	    // note that the << operator below automatically appends a symbolic
	    // expression for states with attribute A_VARPREC
	    if (prec.isValid())
	        ref << prec << ',';
	    else {
		codeblockError("unspecified or invalid precision for state ", name);
		return ref;
	    }
	}
	ref << starSymbol.lookup(state->name());
    }

    // Expand PortHole reference.
    else if ((port = (CGCPortHole*)genPortWithName(portName)) != NULL)
    {
	DataType type = port->resolvedType();

	// pass precision for fix ports
	if (type == FIX)
	{
	    Precision prec = port->precision();

	    // note that the << operator below automatically appends a symbolic
	    // expression for ports with attribute A_VARPREC
	    if (prec.isValid())
	        ref << prec << ',';
	    else {
		codeblockError("unspecified or invalid precision for port ", name);
		return StringList();
	    }
	}

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
// For references to states or ports of type FIX the reference is replaced
// by the triplet "length,intbits,fix_variable"
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

	    // pass precision for fix types
	    if (state->isA("FixArrayState"))
	    {
		Precision prec = ((FixArrayState*)state)->precision();

		// note that the << operator below automatically appends a symbolic
		// expression for states with attribute A_VARPREC
		if (prec.isValid())
		    ref << prec << ',';
		else {
		    codeblockError("unspecified or invalid precision for state ", name);
		    return ref;
		}
	    }

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
	DataType type = port->resolvedType();

	// pass precision for fix ports
	if (type == FIX)
	{
	    Precision prec;

	    // generate constant for index
	    if (port->staticBuf() && (offsetState != NULL))
	    {
		int offset = *(IntState*)offsetState;
		prec = (*port % offset).precision();
	    }
	    // generate expression for index
	    else
		prec = (*port % offset).precision();

	    // note that the << operator below automatically appends a symbolic
	    // expression for ports with attribute A_VARPREC

	    if (prec.isValid())
	        ref << prec << ',';
	    else {
		codeblockError("unspecified or invalid precision for port ", name);
		return ref;
	    }
	}

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

// expand $precision macro
StringList CGCStar::expandFixPrecisionMacro(const char* name)
{
    StringList ref;
    CGCPortHole* port;
    State* state;
    StringList portName = expandPortName(name);

    // Expand State reference.
    if ((state = stateWithName(name)) != NULL)
    {
        if (state->isA("FixState") || state->isA("FixArrayState"))
	{
	    // Return a reference to the associated fix_prec variable
	    if (state->attributes() & AB_VARPREC)
	    {
		registerState(state);
		ref << starSymbol.lookup(state->name()) << "p";
	    }
	    else
		codeblockError("$precision macro references fix state",
			" with attribute A_CONSTPREC");
	}
	else
	    codeblockError("$precision requested for a non-fix type");
    }

    // Expand PortHole reference.
    else if ((port = (CGCPortHole*)genPortWithName(portName)) != NULL)
    {
	if (port->resolvedType() == FIX)
	{
	    // Return a reference to the associated fix_prec variable
	    if (port->attributes() & AB_VARPREC) {

		// use the symbolic expressions returned from the port's
		// precision method to derive a reference to a fix_prec
		// structure;
		// since we do not want to construct an expession for the
		// reference here, we simply strip the ".len" tag from
		// the symbolic length

		const char* tp = (const char *)NULL;  char* bp;
		Precision p = port->precision();
		const char* sym_len = p.symbolic_len();

		if ((sym_len != NULL) &&
		    ((tp = strrchr(sym_len,'.')) != NULL)) {

		    LOG_NEW; bp = new char[tp-sym_len+1];
		    strncpy(bp,sym_len,tp-sym_len) [tp-sym_len] = '\0';
		    ref << bp;
		    LOG_DEL; delete [] bp;
		}
		else
		    codeblockError(
			"internal error while expanding $precision macro");
	    }
	    else
		codeblockError("$precision macro references fix port",
			" with attribute A_CONSTPREC");
	}
	else
	    codeblockError("$precision requested for a non-fix type");
    }

    // Could not find State or PortHole with given name.
    else
    {
	codeblockError(name, " is not defined as a state or port");
    }
    return ref;
}

// expand $precision macro with offset specification
StringList CGCStar::expandFixPrecisionMacro(const char* name, const char* offset)
{
    StringList ref;
    CGCPortHole* port;
    State *state, *offsetState;
    StringList offsetVal;
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

    // Expand State reference.
    if ((state = stateWithName(name)) != NULL)
    {
	if (state->isA("FixArrayState"))
	{
	    // Return a reference to the associated fix_prec variable
	    if (state->attributes() & AB_VARPREC)
	    {
		registerState(state);
		ref << starSymbol.lookup(state->name()) << "p";
	    }
	    else
		codeblockError("$precision macro references fix state",
			" with attribute A_CONSTPREC");
	}
	else
	{
	    codeblockError(name, " is not an fix array state");
	    ref.initialize();
	}
    }

    // Expand PortHole reference.
    else if ((port = (CGCPortHole*)genPortWithName(portName)) != NULL)
    {
	if (port->resolvedType() == FIX)
	{
	    // return a reference to the associated fix_prec variable
	    if (port->attributes() & AB_VARPREC) {

		// use the symbolic expressions returned from the port's
		// precision method to derive a reference to a fix_prec
		// structure;
		// since we do not want to construct an expession for the
		// reference here, we simply strip the ".len" tag from
		// the symbolic length

		Precision p;

		// generate constant for index
		if (port->staticBuf() && (offsetState != NULL))
		{
		    int offset = *(IntState*)offsetState;
		    p = (*port % offset).precision();
		}
		// generate expression for index
		else
		    p = (*port % offset).precision();

		const char* tp = (const char *)NULL;  char* bp;
		const char* sym_len = p.symbolic_len();

		if ((sym_len != NULL) &&
		    ((tp = strrchr(sym_len,'.')) != NULL)) {

		    LOG_NEW; bp = new char[tp-sym_len+1];
		    strncpy(bp,sym_len,tp-sym_len) [tp-sym_len] = '\0';
		    ref << bp;
		    LOG_DEL; delete [] bp;
		}
		else
		    codeblockError(
			"internal error while expanding $precision macro");
	    }
	    else
		codeblockError("$precision macro references fix port",
			" with attribute A_CONSTPREC");
	}
	else
	    codeblockError("$precision requested for a non-fix type");
    }

    // Could not find State or PortHole with given name.
    else
    {
	codeblockError(name, " is not defined as a state or port");
    }
    return ref;
}

// redefine the setTarget method in order to set the symbolic precision
// of fix states as soon as the symbol lists become fully initialized
// we cannot leave this to the precision() method of the state classes
// (as is done with portholes)  since the state classes know nothing
// about their symbolic representation

int CGCStar::setTarget(Target* t)
{
    // set the symbol counters of the symbol lists so that starSymbol.lookup()
    // can construct symbol names
    CGStar::setTarget(t);
    
    // iterate through all states of type FixState and FixArrayState
    //  with variable precision, that is those with attribute A_VARPREC
    BlockStateIter nextState(*this);
    State* state;

    while ((state = nextState++) != NULL) {

	if (state->attributes() & AB_VARPREC) {

	    if (state->isA("FixState")) {
		FixState* fstate = (FixState*)state;
		const Precision p = fstate->precision();

		fstate->setPrecision(
			newSymbolicPrecision(p.len(),p.intb(), state->name()),
			/*overrideable =*/ FALSE);
	    }

       else if (state->isA("FixArrayState")) {
		FixArrayState* fstate = (FixArrayState*)state;
		const Precision p = fstate->precision();

		fstate->setPrecision(
			newSymbolicPrecision(p.len(),p.intb(), state->name()),
			/*overrideable =*/ FALSE);
	    }
	}
    }
    return 1;
}

// private function for setTarget;
// construct symbolic precision for state or port with given name
Precision CGCStar :: newSymbolicPrecision(int length,int intBits, const char* name)
{
    const char* label = sanitize(starSymbol.lookup(name));

    StringList sym_len, sym_intb;
    sym_len  << label << "p.len";
    sym_intb << label << "p.intb";

    return Precision(length,intBits, sym_len,sym_intb);
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
	int status = 0;
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
	StringList code;

	BlockPortIter next(*this);
	CGCPortHole* p;
	while ((p = (CGCPortHole*) next++) != 0) {
		int bs = p->bufSize();

		if (bs > 1 && p->staticBuf() == FALSE) {
		    int nx = p->numXfer();
		    if (nx == bs) continue;

		    StringList pname = sanitize(starSymbol.lookup(p->name()));
		    const char* geoname = NULL;

		    // determine label of fix_prec array for ports of type FIX
		    if (p->attributes() & AB_VARPREC)
			geoname = p->getGeoName();
	       else if (p->realFarPort()->attributes() & AB_VARPREC)
			geoname = ((CGCPortHole*)p->realFarPort())->getGeoName();

		    // add the number of tokens transferred per execution;
		    // substract buffer size if offset overruns buffer end

		    if ((p->resolvedType() != FIX) ||
			(geoname == 0))

			code << '\t' << pname << " += " << nx << ";\n"
			    "\tif (" << pname << " >= " << bs << ")\n"
			    "\t\t" << pname << " -= " << bs << ";\n";
		    else
		    {
			StringList label = geoname;
			label << 'p';

			// for fix ports with variable precision we must
			// assign the precision of the buffer entries of
			// the current run to those of the next;
			// this code avoids generating a for loop if there
			// is only a single entry to transfer

			if (nx == 1)
			    code << '\t' << pname << "++;\n"
				"\tif (" << pname << " >= " << bs << ") {\n"
				"\t\t" << pname << " -= " << bs << ";\n"
				"\t\t" << label << '[' << pname << "] = "
				       << label << '[' << pname << '+' << bs-nx << "];\n"
				"\t} else\n"
				"\t\t" << label << '[' << pname << "] = "
				       << label << '[' << pname << '-' << nx << "];\n";
			else {

			    // macro to create a reference into the precision
			    // array substracting the buffer size if the offset
			    // becomes too large
# define SYMBOLIC_EXPR(token) \
	label << "[i+" << pname << token << " - " \
	     "((i+" << pname << token << " >= " << bs << ") ? " << bs << " : 0)]"

			    code << "\t{ int i;\n"
				"\tfor (i=0;i<" << nx << ";i++)\n"
			    	"\t\t" << SYMBOLIC_EXPR('+' << nx) <<
				" = "  << SYMBOLIC_EXPR("") <<
			    	"\n\t}\n";
# undef SYMBOLIC_EXPR
			    code << '\t' << pname << " += " << nx << ";\n"
				"\tif (" << pname << " >= " << bs << ")\n"
				"\t\t" << pname << " -= " << bs << ";\n";
			}
		    }
		}
	}
	code << "\t}\n";
	addCode(code);
}

// Compare.
int operator ==(bitWord b, Attribute a)
{
    return b == a.eval(b);
}

// Declare PortHole data structures.
StringList CGCStar::declarePortHoles(Attribute a)
{
    StringList dec;	// declarations

    CGCPortHole* port;
    BlockPortIter portIter(*this);
    while ((port = (CGCPortHole*)portIter++) != NULL)
    {
	if (port->attributes() == a)
	{
	    dec << declareBuffer(port);
	    dec << declareOffset(port);
	}
    }

    return dec;
}

// Generate initialization code for PortHole data structures.
StringList CGCStar::initCodePortHoles(Attribute a)
{
    StringList code;	// initialization code

    CGCPortHole* port;
    BlockPortIter portIter(*this);
    while ((port = (CGCPortHole*)portIter++) != NULL)
    {
	if (port->attributes() == a)
	{
	    code << initCodeBuffer(port);
	    code << initCodeOffset(port);
	}
    }

    return code;
}

// Declare State data structures.
StringList CGCStar::declareStates(Attribute a)
{
    StringList dec;	// declarations

    State* state;
    StateListIter stateIter(referencedStates);
    while ((state = stateIter++) != NULL)
    {
	if (state->attributes() == a)
	    dec << declareState(state);
    }

    return dec;
}

// Generate initialization code for State data structures.
StringList CGCStar::initCodeStates(Attribute a)
{
    StringList code;	// initialization code

    State* state;
    StateListIter stateIter(referencedStates);
    while ((state = stateIter++) != NULL)
    {
	if (state->attributes() == a)
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
	else if (type == FIX) dec << "fix";
	else dec << "double";

	if (port->bufType() == EMBEDDED)
	{
	    dec << " *" << name;        // declare a pointer
	}
	else	// not embedded
	{
	    dec << " " << name;
	    if (port->bufSize() > 1)	// declare as array
		dec << '[' << port->bufSize() << ']';
	}

	// For fix types with variable precision declare an array of precision
	// variables by adding a lower 'p' to the geodesic name;
	// note that in contrast to the implementation of fix arrays, one prec-
	// ision variable is declared for each element of the buffer;  this
	// makes sure that the correct precision is used for older entries if
	// the precision changes.

	if (type == FIX) {
	    const char* label = NULL;

	    if (port->attributes() & AB_VARPREC)
		label = name;
       else if (port->realFarPort()->attributes() & AB_VARPREC)
		label = ((const CGCPortHole*)port->realFarPort())->getGeoName();

	    if (label) {
		dec << ";  fix_prec ";

		if (port->bufType() == EMBEDDED)
		{
		    dec << '*' << label << 'p'; // declare a pointer
		}
		else	// not embedded
		{
		    dec << label << 'p';
		    if (port->bufSize() > 1)	// declare as array
			dec << '[' << port->bufSize() << ']';
		}
	    }
	}
		
	dec << ";\n";
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

	    if (port->resolvedType() == FIX)
	    {
		// initialize pointer into fix_prec array for ports
		// with attribute A_VARPREC
		const char* label = NULL;

		if (port->attributes() & AB_VARPREC)
		    label = name;
	   else if (port->realFarPort()->attributes() & AB_VARPREC)
		    label = ((CGCPortHole*)port->realFarPort())->getGeoName();

		if (label) {
		    code << label << "p = &" << host->getGeoName() << 'p';
		    if (host->bufSize() > 1) code << '[' << loc << ']';
		    code << ";\n";
		}
	    }
	}
	else	// not embedded
	{
	    DataType type = port->resolvedType();
	    int array = (port->bufSize() > 1);

	    if (type == FIX)
	    {
		// assign fixed-point precision to the associated
		// fix_prec structure
		const char* label = NULL;

		if (port->attributes() & AB_VARPREC)
		    label = name;
	   else if (port->realFarPort()->attributes() & AB_VARPREC)
		    label = ((CGCPortHole*)port->realFarPort())->getGeoName();

		if (label) {
		    Precision p = port->precision();

		    // initialize array of precision variables;
		    // this is only necessary if the user references buffer
		    // entries for which a bit value is not yet assigned
		    if (array) {
			if (p.len() == 0 && p.intb() == 0)
			    code << "memset(" << label << "p,0, sizeof(" << label << "p));\n";
			else
			    code << "{int i; for(i=0;i<" << port->bufSize() << ";i++) "
				 << label << "p[i].len  = " << p.len()  << ", "
				 << label << "p[i].intb = " << p.intb() << "; }\n";
		    }
		    // otherwise initialize single fix_prec structure
		    else
			code << label << "p.len = "  << p.len()  << ", "
			     << label << "p.intb = " << p.intb() << ";\n";
		}

		// clear bit representation once for all elements in buffer
		code << "memset(" << name << ",0, sizeof(" << name << "));";
	    }
	    else
	    {
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
	    }
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
    else if (state->isA("FixState") || state->isA("FixArrayState"))
	dec << "fix";
    else if (state->isA("PrecisionState"))
	dec << "fix_prec";
    else dec << "double";

    const char* name = sanitize(starSymbol.lookup(state->name()));
    dec << " " << name;
    if (state->isArray()) dec << "[" << state->size() << "]";

    // for fix variables with variable precision declare a fix_prec variable
    // by adding a lower 'p' to the state name
    if ((state->isA("FixState") || state->isA("FixArrayState")) &&
	(state->attributes() & AB_VARPREC))
 	dec << ";  fix_prec " << name << "p";
    dec << ";\n";

    return dec;
}

// Generate initialization code for State variable.

    // initializer classes for method CGCStar::initCodeState() [JW 1994]
    // these classes try to minimize the code for array initializations by
    // initializing adjacent array elements with the same value in a loop

    class StateInitializer {
      protected:
	int first, last;
	char cur_value[256];
	StringList& code;
	const char* name;

      public:
	// constructor with StringList "code_stream" to append to
	// and the variable name of the array
	StateInitializer(StringList& code_stream, const char* array_name) : code(code_stream)
	{
		name  = array_name;
		first = last = 0;  *cur_value = '\0';
	}

	// initialize the next element with the given value
	void addInitialization(const char* value)
	{
	    if (strcmp(cur_value, value)) {
		flush();
		first = last;
		strcpy(cur_value, value);
	    }
	    last++;
	}

	// add the initialization for the array elements with indices
	// "first..last-1" to the code stream
	virtual void flush()
	{
	    if (first < last) {
		if (last - first <= 3) {
		    char sym[16];
		    for (int i=first; i<last; i++) {
			sprintf(sym, "%d", i);
			declare(sym, cur_value);
			code << '\n';
		    }
		} else {
		    code << "{int i; for(i=" << first << ";i<" << last << ";i++) ";
		    declare("i", cur_value);
		    code << "}\n";
		}
	    }
	    first = last+1;
	}

      protected:
	// do the actual initialization;
	// the base class initializes integer or float values
	virtual void declare(const char* index, const char* value)
	{
	    code << name << '[' << index << ']'
		 << '=' << value << ";";
	}
    };

    class StringInitializer : public StateInitializer {
      public:
	StringInitializer(StringList& code, const char* var) :
		StateInitializer(code,var) {}

      protected:
    	/*virtual*/ void declare(const char* index, const char* value)
	{
	    code << name << '[' << index << ']'
		 << "=\"" << value << "\";";
	}
    };

    class ComplexInitializer : public StateInitializer {
      public:
	ComplexInitializer(StringList& code, const char* var) :
		StateInitializer(code,var) {}

      protected:
    	/*virtual*/ void declare(const char* index, const char* value)
	{
	    ComplexState cxState;
	    cxState.setInitValue(value);
	    cxState.initialize();
	    Complex x = cxState;

	    code << name << '[' << index << ']'
		 << ".real=" << x.real() << "; ";
	    code << name << '[' << index << ']'
		 << ".imag=" << x.imag() << ";";
	}
    };

    class FixInitializer : public StateInitializer {
	const Precision& precision;
      public:
	FixInitializer(StringList& code, const char* var, const Precision& p) :
		StateInitializer(code,var), precision(p) { }

	/*virtual*/ void flush()
	{
	    // if the value of the fix is zero, we can use a single memset
	    // invocation instead of generating a loop
	    if (first < last) {
		double value;
		if (sscanf(cur_value, "%lf", &value), value == 0.0) {
		    code << "memset(" << name << '[' << first << "],0, ";
		    if (last-first != 1)
			 code << (last-first) << '*';
		    code << "sizeof(fix));\n";
		    first = last+1;
		  return;
		}
	    }
	    StateInitializer::flush();
	}

    	/*virtual*/ void declare(const char* index, const char* value)
	{
	    code << "FIX_DoubleAssign("
		 << precision.len() << ',' << precision.intb() << ','
		 << name << '[' << index << "], "  << value << ");";
	}
    };


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

	// fix arrays are different
	if (!state->isA("FixArrayState")) {
	    const char* special = "";
	    const char* white = "\n";
	    Tokenizer lexer(val, special, white);
	    char token[256];

	    if (state->isA("ComplexArrayState")) {
		ComplexInitializer initializer(code,name);
		while (lexer >> token, *token != '\0')
		    initializer.addInitialization(token);
		initializer.flush();
	    }
       else if (state->isA("StringArrayState")) {
		StringInitializer initializer(code,name);
		while (lexer >> token, *token != '\0')
		    initializer.addInitialization(token);
		initializer.flush();
	    }
	    else {
		StateInitializer initializer(code,name);
		while (lexer >> token, *token != '\0')
		    initializer.addInitialization(token);
		initializer.flush();
	    }
	} else {
	    // initialization of fix arrays;
	    // the format of the initialization string is "(values, precision)"
	    // therefore we cannot use the above scheme
	
	    // here casting is safe for we only need a non const reference
	    // for the bracket operator of class FixArrayState
	    FixArrayState& fa_state = *(FixArrayState*)state;
	    const Precision p = fa_state.precision();

	    int size = fa_state.size();

	    // set fixed point precision;
	    // it is assumed that it is the same for every element in the array
	    if (state->attributes() & AB_VARPREC)
		code << name << "p.len = "  << p.len()  << ", "
		     << name << "p.intb = " << p.intb() << ";\n";
	
	    // initialize bit arrays
	    FixInitializer initializer(code,name,p);

	    for (int i = 0; i < size; i++) {
		StringList value = (double)fa_state[i];
		initializer.addInitialization(value);
	    }
	    initializer.flush();
	}
    }
    else	// Scalar initialization.
    {
	if (state->isA("ComplexState"))
	{
	    ComplexState cxState;
	    cxState.setInitValue(val);
	    cxState.initialize();
	    Complex x = cxState;

	    code << name;
	    code << ".real=" << x.real() << ";\n";
	    code << name;
	    code << ".imag=" << x.imag() << ";\n";
	}
	else if (state->isA("StringState"))
	{
	    code << name << '=' << '"' << val << '"' << ";\n";
	}
	else if (state->isA("FixState"))
	{
	    const FixState& f_state = *(const FixState*)state;
	    const Precision p = f_state.precision();

	    // initialize associated precision variable
	    if (state->attributes() & AB_VARPREC)
		code << name << "p.len = "  << p.len()  << ", "
		     << name << "p.intb = " << p.intb() << ";\n";

	    if (f_state.asDouble() == 0.0)
		code << "FIX_SetToZero(" 
		     << p.len() << ',' << p.intb() << ", " << name << ");\n";
	    else
		code << "FIX_DoubleAssign("
		     << p.len() << ',' << p.intb() << ", " << name << ','
		     << f_state.asDouble() << ");\n";
	}
	else
	{
	    code << name << '=' << val << ";\n";
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
