static const char file_id[] = "ACSCGCCore.cc";
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


 Programmers:  J. A. Lundblad
 Date of creation: 3/29/98
 Version: @(#)ACSCGCCore.cc	1.8 09/21/99

***********************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "ACSCGCCore.h"
#include "ACSKnownCategory.h"
#include "Tokenizer.h"
#include "ComplexState.h"
#include "FixState.h"
#include "FixArrayState.h"
#include "CGUtilities.h"
#include <ctype.h>
#include <stdio.h>              // sprintf(), sscanf()

// isA
ISA_FUNC(ACSCGCCore, ACSCGCore);


// Generate initialization code for State variable.

    // initializer classes for method ACSCGCCore::initCodeState() [JW 1994]
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


// Adds each settable state and their default values to the
// code segment that generates the help message.
StringList ACSCGCCore::setargStatesHelp(const State* state)
{
  StringList arg_help;
  if (isCmdArg(state))
    arg_help << "\t" << cmdArg(state) << "\\tdefault : "
	     << state->currentValue() << "\\\n\\n";
  return arg_help;
}

StringList ACSCGCCore::initCodeState(const State* state)
{
    StringList code;
    StringList val;
    
    StringList name = starSymbol.lookup(state->name());
    
    // If "state" is to be setted using command-line arguments,
    // initialize it from the 'struct' member.
    if (isCmdArg(state)) {	
      val = "arg_store.";		
      val << cmdArg(state);		
    }					
    else				
      val = state->currentValue();	

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

	    // the FixInitializer::declare method generates code that uses
	    // the FIX_DoubleAssign routine that is defined by CGCrtlib.h
	    addFixedPointSupport();

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

	    // the fixed-point routines FIX_SetToZero and FIX_DoubleAssign
	    // used below are defined by CGCrtlib.h
	    addFixedPointSupport();

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

StringList ACSCGCCore::addMoveDataBetweenShared( int thereEmbedded, int startEmbedded, int stopEmbedded, const char* farName, const char* geoName, int numXfer ) {
			StringList code;
			code << "\t/* moveDataBetweenShared */\n";
			if (numXfer > 1) {
				code << "\t{ int i,j;\n\t  j = " << thereEmbedded;
				code << ";\n\t  for(i = " << startEmbedded;
				code << "; i <= " <<  stopEmbedded << "; i++) {\n";
				code << "\t\t" << farName << "[j++] = ";
				code << geoName << "[i];\n\t}\n";
			} else {
				code << "\t" << farName << '[' << thereEmbedded << ']';
				code << " = " << geoName;
				code << '[' << startEmbedded << "];\n";
			}
			return code;
}

// Initialize the 'struct' member as declared above.
StringList ACSCGCCore::cmdargStatesInit(const State* state)
{
  StringList struct_init;
  if (isCmdArg(state))
    struct_init << state->currentValue() << ", ";
  return struct_init;
}

// Define the function codes to set the appropriate 'struct'
// member to that specified on the command-line.
StringList ACSCGCCore::setargStates(Attribute a)		
{
  StringList setarg_proc;

  State* state;
  StateListIter stateIter(getReferencedStates());
  while ((state = stateIter++) != NULL)
    {
    //  if (state->attributes() == a)
	setarg_proc << setargState(state);
    }

    return setarg_proc;
}

int ACSCGCCore::runIt() {
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

// Generate initialization code for PortHole buffer index.
StringList ACSCGCCore::initCodeOffset(const ACSPortHole* port)
{
    StringList code;	// initialization code

    if (!port->staticBuf())
    {
	code << ptSanitize(starSymbol.lookup(port->name()))
	    << " = " << port->bufPos() << ";\n";
    }
    return code;
}

StringList ACSCGCCore::arrayStateIndexRef(const State* state, const char* offset)
{
	StringList index;

	index << starSymbol.lookup(state->name()) << '[' << offset << ']';
	return index;
}

// Declare State variable.
StringList ACSCGCCore::declareState(const State* state)
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

    const char* name = ptSanitize(starSymbol.lookup(state->name()));
    dec << " " << name;
    if (state->isArray()) dec << "[" << state->size() << "]";

    // for fix variables with variable precision declare a fix_prec variable
    // by adding a lower 'p' to the state name
    if ((state->isA("FixState") || state->isA("FixArrayState")) &&
	(state->attributes() & AB_VARPREC))
 	dec << ";  fix_prec " << name << "p";
    // Strings cant just be declared, they have to be initialized as well
    if (state->isA("StringState"))
      dec << '=' << '"' << state->currentValue() << '"';

    dec << ";\n";
    
    return dec;
}

// Declare PortHole buffer.
StringList ACSCGCCore::declareBuffer(const ACSPortHole* port)
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
		label = ((const ACSPortHole*)port->realFarPort())->getGeoName();

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

// Generate code segment to set the 'struct' member using
// command-line specified values.
StringList ACSCGCCore::setargState(const State* state)	
{
  StringList setarg_proc;
  if (isCmdArg(state)) {
    StringList temp;
    temp << cmdArg(state);
    setarg_proc << "\t\t\tif(!strcmp(arg[i], \"-" << temp << "\")) {\n";
    setarg_proc << "\t\t\t\tif(arg[i+1])\n";
    setarg_proc << "\t\t\t\t\targ_store." << temp << "=";
    if(state->isA("IntState"))
      setarg_proc << "atoi";
    else setarg_proc << "atof";
    setarg_proc << "(arg[i+1]);\n\t\t\t\tcontinue;\n\t\t\t}\n";
  }

  return setarg_proc;
}

// Declare PortHole buffer index.
StringList ACSCGCCore::declareOffset(const ACSPortHole* port)
{
    StringList dec;	// declarations

    if (!port->staticBuf())
    {
	dec << "int " << ptSanitize(starSymbol.lookup(port->name()))
	    << ";\n";
    }
    return dec;
}


StringList ACSCGCCore::portBufferIndex( const ACSPortHole* port, const State* offsetState, const char* offset )
{
StringList index;
	    index << '[';

	    // generate constant for index
	    if (port->staticBuf() && (offsetState != NULL))
	    {
		int offset = *(IntState*)offsetState;
		index << ( (port->bufPos() - offset + port->bufSize())
			 % port->bufSize() );
	    }
	    // generate expression for index
	    else
	    {
		if (!port->linearBuf()) // use circular addressing
			index << "CGC_MOD(";
		else
			index << '(';
		if (port->staticBuf()) index << port->bufPos();
		else index << ptSanitize(starSymbol.lookup(port->name()));
		index << "-(" << offset << ')';
		if (!port->linearBuf())	// use circular addressing
		{
		    index << '+' << port->bufSize() << "," << port->bufSize() << ")";
		}
		else index << ')';
	    }

	    index << ']';
		return index;
}

StringList ACSCGCCore::portBufferIndex( const ACSPortHole* port )
{
		StringList index;
	    index << '[';
	    if (port->staticBuf()) index << port->bufPos();
	    else index << ptSanitize(starSymbol.lookup(port->name()));
	    index << ']';
		return index;
}

// Generate initialization code for PortHole buffer.
StringList ACSCGCCore::initCodeBuffer(ACSPortHole* port)
{
    StringList code;

    if (!port->switched() && port->isItOutput())
    {
	StringList name = port->getGeoName();
	if (port->bufType() == EMBEDDED)
	{
	    ACSPortHole* far = (ACSPortHole*) port->far();
	    ACSPortHole* host;		// Host buffer.
	    int loc = 0;		// Embedding location.

	    if (port->embedded() != NULL)
	    {
		host = ((ACSPortHole*) port->embedded())->realFarPort();
		loc = port->whereEmbedded();
	    }
	    else
	    {
		host = (ACSPortHole*) far->embedded();
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
		    label = ((ACSPortHole*)port->realFarPort())->getGeoName();

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
		    label = ((ACSPortHole*)port->realFarPort())->getGeoName();

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

// Declare the state variable as a 'struct' member.
StringList ACSCGCCore::cmdargState(const State* state)
{
  StringList struct_mem;
  if (isCmdArg(state)) {
    if (state->isA("IntState"))
      struct_mem << "\tint";
    else struct_mem << "\tdouble";

    struct_mem << " " << cmdArg(state);
    struct_mem << ";\n";
  }

  return struct_mem;
}

void ACSCGCCore :: updateOffsets()
{
	StringList code;

	BlockPortIter next(getCorona());
	ACSPortHole* p;
	while ((p = (ACSPortHole*) next++) != 0) {
		int bs = p->bufSize();

		if (bs > 1 && p->staticBuf() == FALSE) {
		    int nx = p->numXfer();
		    if (nx == bs) continue;

		    StringList pname = ptSanitize(starSymbol.lookup(p->name()));
		    const char* geoname = NULL;

		    // determine label of fix_prec array for ports of type FIX
		    if (p->attributes() & AB_VARPREC)
			geoname = p->getGeoName();
	       else if (p->realFarPort()->attributes() & AB_VARPREC)
			geoname = ((ACSPortHole*)p->realFarPort())->getGeoName();

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
				" = "  << SYMBOLIC_EXPR("") << ";\n\t}\n";
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

// Pull in a supporting C module from a library
void ACSCGCCore::addModuleFromLibrary(
	const char* module, const char* subdirectory,
	const char* libraryName) {
    StringList cFile, cPath, includeDir, includeFileQuoted, includePath,
	       includeDirSpec, libSpec, path, ptolemyLibDirSpec;
    char* ptolemy = "$PTOLEMY";
    char* ptarch = "$PTARCH";
    cFile << module << ".c";
    includeFileQuoted << "\"" << module << ".h\"";
    includeDir << ptolemy << "/" << subdirectory;
    includeDirSpec << "-I" << includeDir;
    ptolemyLibDirSpec << "-L" << ptolemy << "/lib." << ptarch;
    libSpec << "-l" << libraryName;
    path << ptolemy << "/" << subdirectory << "/";
    includePath << path << module << ".h";
    cPath << path << cFile;

    addInclude(includeFileQuoted);
    addCompileOption(includeDirSpec);
    addLocalLinkOption(ptolemyLibDirSpec);
    addLocalLinkOption(libSpec);
    addRemoteFile(cPath, TRUE);
    addRemoteFile(includePath, FALSE);
}



// Pull in a supporting C module from a library
void ACSCGCCore::addFixedPointSupport() {
    addModuleFromLibrary("CGCrtlib", "src/domains/cgc/rtlib", "CGCrtlib");
}

// Add lines to be put at the beginning of the code file
int ACSCGCCore :: addInclude(const char* decl) {
    StringList temp = "#include ";
    temp << decl << "\n";
    return addCode(temp, "include", decl);
}

// Add options to be used when compiling a C program
// We don't run process code on this - it would add a \n
int ACSCGCCore::addCompileOption(const char* decl) {
    CodeStream* compileOptions;
    if ((compileOptions = getStream("compileOptions")) == 0) return FALSE;
    StringList temp;
    temp << " " << decl << " ";
    return compileOptions->put(temp,decl);
}

// Add options to be used when linking a C program
// We don't run process code on this - it would add a \n
int ACSCGCCore::addLinkOption(const char* decl) {
    CodeStream* linkOptions;
    if ((linkOptions = getStream("linkOptions")) == 0) return FALSE;
    StringList temp;
    temp << " " << decl << " ";
    return linkOptions->put(temp,decl);
}

// Add options to be used when linking a C program on the local machine.
// We don't run process code on this - it would add a \n
int ACSCGCCore::addLocalLinkOption(const char* decl) {
    CodeStream* linkOptions;
    if ((linkOptions = getStream("localLinkOptions")) == 0) return FALSE;
    StringList temp;
    temp << " " << decl << " ";
    return linkOptions->put(temp,decl);
}

// Add a file that will be copied over to a remote machine
int ACSCGCCore::addRemoteFile(const char* filename, int CcodeFlag) {
    CodeStream* remoteFiles = getStream("remoteFiles");
    StringList temp = filename;
    temp << " ";
    int retval = FALSE;
    if (remoteFiles) {
	retval = remoteFiles->put(temp, filename);
        if (retval && CcodeFlag) {
	    CodeStream* remoteCFiles = getStream("remoteCFiles");
	    if (remoteCFiles) retval = remoteCFiles->put(temp, filename);
	}
    }
    return retval;
}

