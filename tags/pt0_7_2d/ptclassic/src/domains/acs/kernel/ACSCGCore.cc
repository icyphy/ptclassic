/**********************************************************************
Copyright (c) 1998 The Regents of the University of California.
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


 Programmers:  James A. Lundblad
 Date of creation: 3/23/98
 Version: @(#)ACSCGCore.cc	1.7 09/10/99

The ACS CG Core class provides a base class for all derived CG Core category classes. Many instance variables and methods are stolen from CGCStar.


***********************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif
#include "Attribute.h"
#include "ACSCGCore.h"
#include "ACSPortHole.h"
#include "FixState.h"
#include "FixArrayState.h"
#include "Tokenizer.h"
#include "ComplexState.h"
#include "FixArrayState.h"
#include "PrecisionState.h"
#include "CGUtilities.h"
#include <ctype.h>
#include <stdio.h>              // sprintf(), sscanf()

// isA
ISA_FUNC(ACSCGCore, ACSCore);


// int isCmdArg (const State*);  // FIXME: should probably be a member function.

// This must be static because CGCStar.cc includes a similar definition,
// and statically linked binaries will fail if there are duplicate symbols
// with the same names
static inline int operator ==(bitWord b, Attribute a)
{
    return b == a.eval(b);
}


void ACSCGCore::addSpliceStar(ACSStar* s, int atEnd) {
    if (spliceClust.member(s)) return;
    if (atEnd) spliceClust.append(s);
    else spliceClust.prepend(s);
}

void ACSCGCore::initialize() {
	ACSCore::initialize();
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

int ACSCGCore :: run() {
	int status = 0;
	if (spliceClust.size() > 1) {
		ListIter nextStar(spliceClust);
		ACSStar* s;
		while ((s = (ACSStar*) nextStar++) != 0) {
			status = s->runIt();
			if (!status) return status;
		}
	}
	else status = runIt();
	return status;
}

// redefine the setTarget method in order to set the symbolic precision
// of fix states as soon as the symbol lists become fully initialized
// we cannot leave this to the precision() method of the state classes
// (as is done with portholes)  since the state classes know nothing
// about their symbolic representation

int ACSCGCore::setTarget(Target* t)
{
    // set the symbol counters of the symbol lists so that starSymbol.lookup()
    // can construct symbol names
    
    ACSCore::setTarget(t);
    
    // iterate through all states of type FixState and FixArrayState
    //  with variable precision, that is those with attribute A_VARPREC
    BlockStateIter nextState(getCorona());
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


// substitute macros for fixed point variables
StringList ACSCGCore::expandMacro(const char* func, const StringList& argList)
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

    return ACSCore::expandMacro(func, argList);
}

// Expand State value macros. If "name" state can be set via command-
// line argument, then change it to a reference and add it to the list
// of referneced states.
StringList ACSCGCore::expandVal(const char *name) {						
  StringList ref;				
  State* state;					
  
  if ((state = stateWithName(name)) != NULL) {
    if (isCmdArg(state)) {					
      registerState(state);
      ref << starSymbol.lookup(state->name());
    }							
    else {						
      ref << ACSCore::expandVal(name);		
    }
  }						       
  else {							
    codeblockError(name, " is not defined as a state");
    ref.initialize();						
  }								
  
  return ref;							
}

// Reference to State or PortHole.
// For references to states or ports of type FIX the reference is replaced
// by the triplet "length,intbits,fix_variable"
StringList ACSCGCore::expandRef(const char* name)
{
    StringList ref;
    GenericPort* genPort;
    ACSPortHole* port;
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
    else if (((genPort = genPortWithName(portName)) != NULL)
	     && genPort->isItMulti()) {
	codeblockError(name, " is a multihole name, not an actual "
		       "port name.  Ports that correspond to a "
		       "multihole need to be referenced with "
		       "<name>#<port number> syntax.");
    }
    else if (genPort) {
	port = (ACSPortHole*)genPort;
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
	    ref << portBufferIndex(port);
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
StringList ACSCGCore::expandRef(const char* name, const char* offset)
{
    StringList ref;
    State *state, *offsetState;
    StringList offsetVal;
    ACSPortHole* port;
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

	    ref << arrayStateIndexRef(state,offset);
	}
	else
	{
	    codeblockError(name, " is not an array state");
	    ref.initialize();
	}
    }

    // Expand PortHole reference with offset.
    else if ( (port = (ACSPortHole*)genPortWithName(portName)) )
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
		prec = (*port ^ offset).precision();
	    }
	    // generate expression for index
	    else
		prec = (*port ^ offset).precision();

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
			ref << portBufferIndex(port,offsetState,offset);
	}
    }
    return ref;
}

// expand $precision macro
StringList ACSCGCore::expandFixPrecisionMacro(const char* name)
{
    StringList ref;
    ACSPortHole* port;
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
    else if ((port = (ACSPortHole*)genPortWithName(portName)) != NULL)
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
StringList ACSCGCore::expandFixPrecisionMacro(const char* name, const char* offset)
{
    StringList ref;
    GenericPort* genPort;
    ACSPortHole* port;
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
    else if (((genPort = genPortWithName(portName)) != NULL)
	     && genPort->isItMulti()) {
	codeblockError(name, " is a multihole name, not an actual "
		       "port name.  Ports that correspond to a "
		       "multihole need to be referenced with "
		       "<name>#<port number> syntax.");
    }
    else if (genPort) {
	port = (ACSPortHole*)genPort;
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
		    p = (*port ^ offset).precision();
		}
		// generate expression for index
		else
		    p = (*port ^ offset).precision();

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


void ACSCGCore::registerState(State* state)
{
    // If the State is not already on the list of referenced States, add it.
    StateListIter stateIter(referencedStates);
    State* s;
    while ((s = stateIter++) != NULL)
	if (s == state) return;
    
    referencedStates.put(*state);
}

// Declare 'struct' for states settable via command-line arguments
StringList ACSCGCore::cmdargStates(Attribute a)
{
  StringList struct_mem;

  State* state;
  StateListIter stateIter(referencedStates);
  while ((state = stateIter++) != NULL)
    {
   if (state->attributes() == a)
	struct_mem << cmdargState(state);
    }

    return struct_mem;
}

// Initialize the 'struct' declared above to the default values.
StringList ACSCGCore::cmdargStatesInits(Attribute a)
{
  StringList struct_init;

  State* state;
  StateListIter stateIter(referencedStates);
  while ((state = stateIter++) != NULL)
    {
    if (state->attributes() ==  a)
	struct_init << cmdargStatesInit(state);
    }

    return struct_init;
}

// Define the function codes to set the appropriate 'struct'
// member to that specified on the command-line.
StringList ACSCGCore::setargStates(Attribute a)		
{
  StringList setarg_proc;

  State* state;
  StateListIter stateIter(referencedStates);
  while ((state = stateIter++) != NULL)
    {
     if (state->attributes() == a)
	setarg_proc << setargState(state);
    }

    return setarg_proc;
}

// Define the '-help' option by adding each settable states
// and their default values.
StringList ACSCGCore::setargStatesHelps(Attribute a)	
{
  StringList arg_help;

  State* state;
  StateListIter stateIter(referencedStates);
  while ((state = stateIter++) != NULL)
    {
     if (state->attributes() == a)
	arg_help << setargStatesHelp(state);
    }

    return arg_help;
}

// Generate initialization code for State data structures.
StringList ACSCGCore::initCodeStates(Attribute a)
{
    StringList code;	// initialization code

    State* state;
    StateListIter stateIter(referencedStates);
    while ((state = stateIter++) != NULL)
    {
	if (state->attributes() ==  a)
	    code << initCodeState(state);
    }

    return code;
}

// Declare PortHole data structures.
StringList ACSCGCore::declarePortHoles(Attribute a)
{
    StringList dec;	// declarations

    ACSPortHole* port;
    BlockPortIter portIter(getCorona());
    while ((port = (ACSPortHole*)portIter++) != NULL)
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
StringList ACSCGCore::initCodePortHoles(Attribute a)
{
    StringList code;	// initialization code

    ACSPortHole* port;
    BlockPortIter portIter(getCorona());
    while ((port = (ACSPortHole*)portIter++) != NULL)
    {
	if (port->attributes() ==  a)
	{
	    code << initCodeBuffer(port);
	    code << initCodeOffset(port);
	}
    }

    return code;
}

// Declare State data structures.
StringList ACSCGCore::declareStates(Attribute a)
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

// private function for setTarget;
// construct symbolic precision for state or port with given name
Precision ACSCGCore :: newSymbolicPrecision(int length,int intBits, const char* name)
{
    const char* label = ptSanitize(starSymbol.lookup(name));

    StringList sym_len, sym_intb;
    sym_len  << label << "p.len";
    sym_intb << label << "p.intb";

    return Precision(length,intBits, sym_len,sym_intb);
}

// After firing that star, we may need to move the input data between
// shared buffers (for example, Spread/Collect) since these movements
// are not visible from the user.
// In case, embedded portholes are connected together.
void ACSCGCore :: moveDataBetweenShared() {
	StringList code;

	BlockPortIter next(getCorona());
	ACSPortHole* p;
	while ((p = (ACSPortHole*) next++) != 0) {
		// consider output only
		if (p->isItInput()) continue;

		ACSPortHole* farP = (ACSPortHole*) p->far();
		if (farP == 0) {
			if (!p->switched()) continue;
			farP = (ACSPortHole*) p->cgGeo().destPort();
		}

		if (farP->embedding() == 0) continue;

		BlockPortIter piter(*farP->parent());
		ACSPortHole* op;
		// march from output#1 ....
		while ((op = (ACSPortHole*) piter++) != 0) {
			ACSPortHole* ip = (ACSPortHole*) op->far();
			ACSPortHole* ep = (ACSPortHole*) ip->embedded();
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

			code << addMoveDataBetweenShared(there, start, stop, farName, p->getGeoName(), op->numXfer() );
		}
	}
	if (code.length() > 0) addCode(code);
}
	
// This function checks whether "state" is to be set from a command-line
// argument. If it is, returns the name to be specified on the command-
// line. The function returns "" otherwise.

StringList ACSCGCore::cmdArg(const State* state) const {
  StringList temp;
 // Check whether the block has a NULL parent
 // This is necessary for CGDDF
  if (!state->parent()->parent()) return "";
  StringList mapsList = state->parent()->target()->\
    pragma(state->parent()->parent()->name(),
	   state->parent()->name(),
	   "state_name_mapping");
  const char* maps = (const char*)mapsList;
  maps = strstr(maps, state->name());

  if (maps) {
    while((!isspace(*maps)) && (*maps != '\0'))
      maps++;

    if (*maps != '\0')
      while((isspace(*maps)) && (*maps != '\0'))
	maps++;

    if (*maps != '\0')
      while ((!isspace(*maps)) && (*maps != '\0')) {
	temp << *maps++;
      }
  }
  return temp;
}
    
int ACSCGCore::isCmdArg (const State* state) const {
  StringList pragma;
  pragma << cmdArg(state);
  return pragma.length();
}
