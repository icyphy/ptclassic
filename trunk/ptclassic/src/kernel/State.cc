static const char file_id[] = "State.cc";
/**************************************************************************
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
#include "Tokenizer.h"
#include "State.h"
#include "Block.h"
#include "miscFuncs.h"
#include "Error.h"
#include "KnownState.h"
#include "PtGate.h"

#define TOKSIZE 256

/*************************************************************************

	class State methods

**************************************************************************/

// small virtual functions in baseclass
void State :: setInitValue(const char* s) { myInitValue = s;}

int State :: isArray() const { return FALSE;}

State :: ~State() {}

int State :: size () const { return 1;}

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

// code duplication, once parseFileName is part of state, remove this function
const State* tempLookup (const char* name, Block* blockIAmIn) {
       while (blockIAmIn) {
		if (blockIAmIn->isItWormhole())
			blockIAmIn = blockIAmIn->parent();
		State* p = blockIAmIn->stateWithName(name);
		if (p) return p;
		blockIAmIn = blockIAmIn->parent();
	}
	// not found, consult global symbol list
	return KnownState::lookup(name);
}

// This could be moved as a member function of State so that other states
// can use it.  We allow the filename to be inherited from a state value
// using the curly braces, e.g. < {FilterTapFile} or < {Directory}/{File}
StringList parseFileName(State& state, const char* fileName) {
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
	    const State* s = tempLookup(tokbuf,state.parent()->parent());
	    if (!s) {
		Error::abortRun(state,"undefined symbol: ",tokbuf);
		return emptyString;
	    }
	    parsedFileName << s->currentValue();
	    lexer >> tokbuf;
	    if (tokbuf[0] != '}') {
		Error::abortRun(state,"expected closed curly brace: ",tokbuf);
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
StringList parseNestedExpression(State& state, const char* expression) {
    StringList parsedExpression;
    char tokbuf[MAXSTRINGLEN], tmptokbuf[MAXSTRINGLEN];
    const char* curSeparator = "";
    const char* tokSeparator = " ";
    const char* specialChars = "{}";
    Tokenizer lexer(expression,specialChars);

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
	    const State* s = tempLookup(tokbuf,state.parent()->parent());
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

// send a string to an external interpreter for evaluation
const char*
State :: externalInterpreter(const char* expression) {
    parseError("no interpreter defined for the ! operator to evaluate ",
	       expression);
    return NULL;
}

// The state tokenizer: return next token when parsing a state
// initializer string.  Handles references to files and other states.
ParseToken
State :: getParseToken(Tokenizer& lexer, int stateType) {
	char token[TOKSIZE];
	ParseToken t = pushback();	// allow for one pushback token.
	if (t.tok) {
		clearPushback();
		return t;
	}
	lexer >> token;

	// read tokens from a file name
	if (*token == '<') {
		char filename[TOKSIZE];
		// temporarily disable special characters, e.g. so '/' 
		// does not screw us up.
		const char* tc = lexer.setSpecial("");
		lexer >> filename;
		StringList parsedFileName = parseFileName(*this,filename);
		// enable special characters
		lexer.setSpecial(tc);
		// check for an error in parsing the file name
		if (parsedFileName.length() == 0) return t;
		// parse the contents of the file
		if (!lexer.fromFile(parsedFileName)) {
			StringList msg;
			msg << parsedFileName << ": " << why();
			parseError ("can't open file ", msg);
			t.tok = T_ERROR;
			return t;
		}
		else lexer >> token;
	}

	// read tokens from the output of an external interpreter
	if (*token == '!') {
		char shellCommand[TOKSIZE];
		// temporarily disable special characters because we don't
		// know what special characters the external interpreter uses
		const char* tc = lexer.setSpecial("");
		lexer >> shellCommand;
		StringList parsedCommand =
			parseNestedExpression(*this,shellCommand);
		// enable special characters
		lexer.setSpecial(tc);
		// send the command to the external interpreter
		const char* result = externalInterpreter(parsedCommand);
		// check for an error from the interpreter
		if (result == 0) {
			StringList msg;
			msg << "'" << shellCommand << "'"
			    << " which was expanded into "
			    << "'" << parsedCommand << "'";
			parseError("could not evaluate ", msg);
			t.tok = T_ERROR;
			return t;
		}
		else lexer.pushBack(result);
	}

	if (*token == 0) {
		t.tok = T_EOF;
		return t;
	}
	
// handle all special characters
	if (strchr (",[]+*-/()^", *token)) {
		t.tok = t.cval = *token;
		return t;
	}

// we stay in this while loop only if a scalar state value is substituted
// for, otherwise we return immediately.
	while (1) {
// note: we can get a minus if 2nd time in loop and the value of the
// substituted state was negative.
		if (isdigit(*token) || *token == '.' || *token == '-' )  {
// possible scientific notation if ending in e or E.  Elim + and - as
// special if so, and append to token.  Ugly hack.
			int l = strlen(token);
			if (token[l-1] == 'e' || token[l-1] == 'E') {
				const char* tc = lexer.setSpecial("*()/[],^");
				lexer >> token + l;
				lexer.setSpecial(tc);
			}
			if (stateType == T_Float) {
				t.tok = T_Float;
				t.doubleval = atof(token);
				return t;
			}
			else {
				t.tok = T_Int;
				t.intval = (int)strtol(token,0,0);
				return t;
			}
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
				strncpy (token, value, TOKSIZE-1);
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
	while (blockIAmIn) {
		if (blockIAmIn->isItWormhole())
			blockIAmIn = blockIAmIn->parent();
		State* p = blockIAmIn->stateWithName(name);
		if (p) return p;
		blockIAmIn = blockIAmIn->parent();
	}
	// not found, consult global symbol list
	return KnownState::lookup(name);
}

// put info.
StringList
State::print(int) const {
	StringList  out;
	return out << fullName() << " type: " << type()
		   << "\n initial value: " << myInitValue
		   << "\n current value: " << currentValue() << "\n";
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

