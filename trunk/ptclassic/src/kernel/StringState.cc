static const char file_id[] = "StringState.cc";

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

 Programmer:  I. Kuroda and J. Buck
 Date of creation: 5/26/90
 Revisions:
	Support value substitution in StringStates, with the syntax
	state foo = {foo}

	State names within braces are replaced by their values.

 Functions for class StringState

**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include <std.h>
#include "miscFuncs.h"
#include "StringState.h"
#include "Tokenizer.h"

/*************************************************************************

	class StringState methods

**************************************************************************/

StringState :: StringState() : val(0) {}

const char* StringState :: className() const { return "StringState"; }

const char* StringState :: type() const { return "STRING"; }

void StringState :: initialize() {
	// Delete the previously parsed value (set to zero in case of error)
	delete [] val;
	val = 0;

	// Make sure that the initial string is not null;
	const char* initString = initValue();
	if (initString == 0 || *initString == 0) return;

	// Parse the initial string
	// -- Substitute parameters that fall in between curly braces {}
	// -- Zero out the white space characters so string info is unaltered
	const char* specialChars = "!{}";
	const char* whiteSpace = "";
	Tokenizer lexer(initString, specialChars, whiteSpace);
	StringList parsedString;
	int saveString = FALSE;
	while (TRUE) {
        	ParseToken t = getParseToken(lexer, T_STRING);
		if (t.tok == T_EOF || t.tok == T_ERROR) break;
		saveString = TRUE;
		parsedString << t.sval;
		delete [] t.sval;
	}
	if (saveString) val = savestring(parsedString);
}

StringState& StringState :: operator=(const char* newStr) {
	LOG_DEL; delete [] val;
	val = savestring(newStr);
	return *this;
}

StringState& StringState :: operator=(StringList& newStrList) {
	// Strictly speaking this is not required, but I dont
	// really trust two much automatic type conversion.
	// Note that we might modify newStrList in the call below
	const char* newStr = newStrList;
	return (*this = newStr);
}

StringState& StringState :: operator=(const State& newState) {
	// Strictly speaking this is not required, but I dont
	// really trust two much automatic type conversion.
	StringList newStrList = newState.currentValue();
	return (*this = newStrList);
}

StringList StringState :: currentValue() const {
	StringList res;
	if (val) res = val;
	return res;
}

State* StringState :: clone() const {
	LOG_NEW; return new StringState;
}

StringState :: ~StringState() {
	LOG_DEL; delete [] val;
}

// make known state entry
static StringState proto;
static KnownState entry(proto,"STRING");

ISA_FUNC(StringState,State);
