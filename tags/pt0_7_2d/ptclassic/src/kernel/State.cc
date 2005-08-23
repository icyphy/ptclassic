static const char file_id[] = "State.cc";
/**************************************************************************
Version identification:
@(#)State.cc	2.47	10/10/96

Copyright (c) 1990-1997 The Regents of the University of California.
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

 Programmer:  J. T. Buck
 Original version by I. Kuroda
 Date of creation: 5/26/90
 Revisions:

 Code of functions declared in State.h

**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include <std.h>
#include <ctype.h>
#include "InvokeInterp.h"
#include "Tokenizer.h"
#include "State.h"
#include "Block.h"
#include "miscFuncs.h"
#include "Error.h"
#include "KnownState.h"
#include "PtGate.h"
#include "Scope.h"

// The TOKSIZE is the size of the token buffer which must be large
// enough to store tokens that represent numbers, pathnames, and strings.
// Pathnames in Unix are limited to 256 characters.

#define TOKSIZE 512

/*************************************************************************

	class State methods

**************************************************************************/

// small virtual functions in baseclass
void State :: setInitValue(const char* s) { myInitValue = s; }

int State :: isArray() const { return FALSE; }

State :: ~State() {}

int State :: size () const { return 1; }

// See if character is part of an identifier
inline unsigned int is_idchar(char c) {
	return isalnum(c) || c == '_';
}

// setState -- two versions
State& State :: setState(const char* stateName, 
			 Block* parent ,
			 const char* ivalue,
			 const char* desc) {
	setDescriptor(desc);
	setNameParent(stateName, parent);
	myInitValue = ivalue;
	return *this;
}


State& State :: setState(const char* stateName, 
			 Block* parent ,
			 const char* ivalue,
			 const char* desc,
			 Attribute attr) {
	attributeBits = attr.eval(AB_DEFAULT);
	return setState(stateName,parent,ivalue,desc);
}

// Attributes
// standard attributes.
extern const Attribute A_CONSTANT = {AB_CONST,0};
extern const Attribute A_SETTABLE = {AB_SETTABLE,0};
extern const Attribute A_NONCONSTANT = {0,AB_CONST};
extern const Attribute A_NONSETTABLE = {0,AB_SETTABLE};
extern const Attribute A_DYNAMIC = {AB_DYNAMIC,0};

const int MAXLEN = 20000;
const int MAXSTRINGLEN = 4096;

// We allow the filename to be inherited from a state value using the
// curly braces, e.g. < {FilterTapFile} or < {Directory}/{File}
StringList State::parseFileName(const char* fileName) {
    StringList emptyString, parsedFileName;
    char tokbuf[MAXSTRINGLEN];
    const char* specialChars = "{}";
    Tokenizer lexer(fileName,specialChars);

    // this code used to limit the number of tokens parsed to MAXLEN
    // because the code did not use StringLists
    while( !lexer.eof() ) {
	lexer >> tokbuf;
	char c = tokbuf[0];
	if (c != 0 && tokbuf[1]) c = 0;
	switch (c) {
	  case '{': {
	    // next token must be a state name followed by a closed curly brace
	    lexer >> tokbuf;
	    const State* s = lookup(tokbuf,parent()->parent());
	    if (!s) {
		Error::abortRun(*this,"undefined symbol: ",tokbuf);
		return emptyString;
	    }
	    parsedFileName << s->currentValue();
	    lexer >> tokbuf;
	    if (tokbuf[0] != '}') {
		Error::abortRun(*this,"expected closed curly brace: ",tokbuf);
		return emptyString;
	    }
	    break;
	  }
	  default: {
	    parsedFileName << tokbuf;
	  }
	}
    }
    return parsedFileName;
}

// We parse nested sub-expressions appearing in the expression, e.g.
// {{{FilterTapFile}/{File}}}, that might be passed off to another
// interpreter for evaluation, e.g. Tcl.  Code derived from parseFileName.
StringList State::parseNestedExpression(const char* expression) {
    StringList parsedExpression;
    char tokbuf[MAXSTRINGLEN], tmptokbuf[MAXSTRINGLEN];
    const char* curSeparator = "";
    const char* tokSeparator = " ";
    const char* specialChars = "{}";
    Tokenizer lexer(expression, specialChars);

    while( !lexer.eof() ) {
	lexer >> tokbuf;
	char c = tokbuf[0];
	if (c != 0 && tokbuf[1]) c = 0;
	switch (c) {
	  // only substitute for expressions such as {parameterName}
	  case '{': {
	    // iterate through consecutive open curly braces
	    lexer >> tokbuf;
	    while ( tokbuf[0] == '{' ) {
	      parsedExpression << curSeparator << '{';
	      curSeparator = tokSeparator;
	      lexer >> tokbuf;
	    }
	    parsedExpression << curSeparator;
	    curSeparator = tokSeparator;

	    // lookup the next token on the parameter list
	    const State* s = lookup(tokbuf,parent()->parent());
	    if (s) {
	      lexer >> tmptokbuf;
	      if ( tmptokbuf[0] == '}' ) {
		parsedExpression << s->currentValue();
	      }
	      else {
		lexer.pushBack(tmptokbuf);
		parsedExpression << '{' << curSeparator << tokbuf;
	      }
	    }
	    else {
	      parsedExpression << '{' << curSeparator << tokbuf;
	    }
	    break;
	  }
	  default: {
	    parsedExpression << curSeparator << tokbuf;
	  }
	}
	curSeparator = tokSeparator;
    }
    return parsedExpression;
}

// The next token on the lexer is treated as a filename
int State :: mergeFileContents(Tokenizer& lexer, char* token) {
	char filename[TOKSIZE];

	// disable special characters, e.g. so '/' does not screw us up.
	const char* tc = lexer.setSpecial("");
	lexer >> filename;
	StringList parsedFileName = parseFileName(filename);
	lexer.setSpecial(tc);		// re-enable special characters

	// check for an error in parsing the file name
	if (parsedFileName.length() == 0) return FALSE;

	// direct the lexer to parse the contents of the file
	if (!lexer.fromFile(parsedFileName)) {
		StringList msg;
		msg << parsedFileName << ": " << why();
		parseError("cannot open the file ", msg);
		return FALSE;
	}

	lexer >> token;		// overwrites '<' char in token buffer
	return TRUE;
}

// Send the next token on the lexer to be evaluated by an external interpreter
int State::sendToInterpreter(Tokenizer& lexer, char* token) {
	char shellCommand[TOKSIZE];

	// temporarily disable special characters because we don't
	// know what special characters the external interpreter uses
	const char* tc = lexer.setSpecial("");
	char oldchar = lexer.setQuoteChar('\"');
	lexer >> shellCommand;
	StringList parsedCommand = parseNestedExpression(shellCommand);
	lexer.setQuoteChar(oldchar);
	lexer.setSpecial(tc);		// re-enable special characters

	// check for an error in parsing the expression
	if (parsedCommand.length() == 0) return FALSE;

	// send the command to the external interpreter
	const char* result = interp.interpreter(parsedCommand);

	// check for an error from the interpreter
	if (result == 0) {
		StringList msg;
		msg << "'" << shellCommand << "'"
		    << " which was expanded into "
		    << "'" << parsedCommand << "'";
		parseError("could not evaluate ", msg);
		return FALSE;
	}

	lexer.pushBack(result);		// overwrites ! in expr
	lexer >> token;			// updates token buffer
	return TRUE;
}

// Look for parameters of the form {name}
int State :: getParameterName(Tokenizer& lexer, char* token) {
	char parameterName[TOKSIZE], closeBraceBuf[TOKSIZE];

	// look for a state name followed by a closed curly brace '}'
	lexer >> parameterName;
	lexer >> closeBraceBuf;
	const State* s = lookup(parameterName, parent()->parent());
	if (s == 0 || closeBraceBuf[0] != '}' || closeBraceBuf[1]) {
		lexer.pushBack(closeBraceBuf);
		lexer.pushBack(parameterName);
	}
	else {
		StringList value = s->currentValue();
		lexer.pushBack(value);	// overwrites {name} in expr
		lexer >> token;		// update token buffer
	}
	return TRUE;
}

// The state tokenizer: return next token when parsing a state
// initializer string.  Handles references to files and other states.
ParseToken
State :: getParseToken(Tokenizer& lexer, int stateType) {
	char token[TOKSIZE];
	ParseToken t = pushback();	

	// allow one push back token in State class to support multithreading
	if (t.tok) {
		clearPushback();
		return t;
	}

	// read next token
	lexer >> token;

	// check for a null token
	if (*token == 0) {
		t.tok = T_EOF;
		t.sval = 0;
		return t;
	}

	// process single character directives
	// '<' treat next token as a file name to be read
	// '!' send next token to an external interpreter for evaluation
	// '{' look for {parameterName} and replace it with its value
	if (token[1] == 0) {
		int err = FALSE;
		switch ( *token ) {
		    case '<':
			err = !mergeFileContents(lexer, token);
			break;

		    case '!':
			if ( stateType != T_STRING ||
			     lexer.peekAtNextChar() == '\"' ) {
				err = !sendToInterpreter(lexer, token);
			}
			break;

		    case '{':
			err = !getParameterName(lexer, token);
			break;
		}

		if (err) {
			t.tok = T_ERROR;
			return t;
		}
	}

	if (stateType == T_STRING) {
		t.tok = *token;
		t.sval = savestring(token);
		return t;
	}

	// handle all special characters in non-string states
	if (strchr(",[]+*-/()^", *token)) {
		t.tok = t.cval = *token;
		return t;
	}

	// stay in this while loop only if a scalar state value is substituted
	// for, otherwise we return immediately.
	while (TRUE) {
		// note: we can get a minus if 2nd time in loop and value of
		// substituted state was negative.
		if (isdigit(*token) || *token == '.' || *token == '-' )  {
			// Handle scientific notation.
			// If '+' or '-' is a special char then the number
			// could be as many as three lexer tokens
			// (consider "1.23e-10" == "1.23e","-","10").
			// To deal with this, we check to see if the next
			// token is '+' or '-', and if so append it and the
			// following token to our work buffer.  We will push
			// back whatever isn't consumed by strtod.
			// Ugly, but it works.
			int l = strlen(token);
			if (token[l-1] == 'e' || token[l-1] == 'E') {
				lexer >> token + l;
				if ((token[l] == '+' || token[l] == '-') &&
				    token[l+1] == 0) {
					lexer >> token + (l+1);
				} else {
					// Not '+' or '-' next, so toss it
					// right back.  This is necessary
					// to avoid doing the wrong thing
					// with "0xe 22"; we do not want
					// to merge two tokens into one.
					lexer.pushBack(token + l);
					token[l] = 0;
				}
			}
			// We want to support floating notation as well as
			// C-style hex and octal integer notation.  Note that
			// we must accept float input format even for int
			// states, because substitution of a state's value
			// uses currentValue() which can produce, eg, "10.0".
			// So, we try both strtod() and strtol(), using
			// whichever one parses a longer substring.
			// A freebie side benefit is that hex/octal notation
			// works in float states.
			char *endptr, *endptrI;
			double doubleval = strtod(token, &endptr);
			long longval = strtol(token, &endptrI, 0);
			if (endptrI >= endptr) {
				// note: must believe strtol in a tie
				// if we want "077" to parse as octal.
				doubleval = double(longval);
				endptr = endptrI;
			}
			// Push back whatever wasn't consumed by strtod
			// or strtol.  This ensures that our idea of a
			// number is exactly a valid number, even though
			// our lexer is far too dumb to get it right.
			if (*endptr) {
				lexer.pushBack(endptr);
			}
			// They should've consumed something, however.
			if (endptr == token) {
				parseError ("invalid number: ", token);
				t.tok = T_ERROR;
				return t;
			}
			// Return the result in the desired format.
			if (stateType == T_Float) {
				t.tok = T_Float;
				t.doubleval = doubleval;
			}
			else {
				t.tok = T_Int;
				// make sure C rounds to nearest integer.
				if (doubleval >= 0.0)
					t.intval = int(doubleval + 0.5);
				else
					t.intval = - int(-doubleval + 0.5);
			}
			return t;
		}
		if (is_idchar(*token)) {
			const State* s = lookup(token,parent()->parent());
			if (!s) {
				parseError ("undefined symbol: ", token);
				t.tok = T_ERROR;
				return t;
			}
			if (strcmp(s->type(), "COMPLEX") == 0 ||
			    s->size() != 1) {
				t.tok = T_ID;
				t.s = s;
				return t;
			}
			// for int, float states we substitute their values
			// here and reparse (stay in while loop)
			else if (s->size() == 1) {
				StringList value = s->currentValue();
				strncpy(token, value, TOKSIZE-1);
				token[TOKSIZE-1] = 0;
				// (re-execute the loop)
			}
		}
		else {
			// if we get here, next token is bogus
			parseError ("unexpected token: ", token);
			t.tok = T_ERROR;
			return t;
		}
	}
}

const State* State :: lookup (const char* name, Block* blockIAmIn) {
    if (parent()->scope()) {
	State* p = parent()->scope()->lookup(name);
	if (p) return p;
    }	
    else {
	while (blockIAmIn) {
	    if (blockIAmIn->isItWormhole())
		blockIAmIn = blockIAmIn->parent();
	    State* p = blockIAmIn->stateWithName(name);
	    if (p) return p;
	    blockIAmIn = blockIAmIn->parent();
	}
    }
    // not found, consult global symbol list
    return KnownState::lookup(name);
}

// put info.
StringList State::print(int verbose) const {
    StringList  out;

    out << "\t" << name() << " type: " << type();
    // Print out the attributes.  One problem here is that code gen
    // domains add more attributes, which we fail to print out here.
    if (!(attributes() & AB_CONST))
	    out << " A_CONSTANT";
    if (!(attributes() & AB_SETTABLE))
	    out << " A_NONSETTABLE";
    if (!(attributes() & AB_DYNAMIC))
	    out << " A_DYNAMIC";
    out << "\n";
    if (verbose) {
        out << "\t initial value: " << myInitValue
	    << "\n\t current value: " << currentValue() << "\n";
    }
    return out;
};

// change current value.  We use the initialization parser to do the job.
void
State::setCurrentValue(const char* newval) {
	const char* save = myInitValue;
	myInitValue = newval;
	initialize();
	myInitValue = save;
}

// methods for class StateList

// complain of parse error
void State::parseError (const char* text, const char* text2) {
	Error::abortRun (*this, "Parameter initialization error: ", text,
			 text2);
}

// isa
ISA_FUNC(State,NamedObj);

// power function for use in int state expressions
int power(int base,int exp) {
	int r = 1;
	while (exp > 0) { r *= base; exp--;}
	return r;
}

// The following functions arrange for exclusive access to the pushback
// token in case we are multi-threading.

ParseToken State :: pushbackToken;

// mutex for protecting access to pushback
static KeptGate gate;

// functions for safely accessing the pushback token.
void State :: setPushback(const ParseToken& t) {
	CriticalSection region(gate);
	pushbackToken = t;
}

void State :: clearPushback() {
	CriticalSection region(gate);
	pushbackToken.tok = 0;
}

ParseToken State::pushback() {
	CriticalSection region(gate);
	ParseToken t = pushbackToken;
	return t;
}

