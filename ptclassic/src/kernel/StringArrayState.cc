static const char file_id[] = "StringArrayState.cc";
/**************************************************************************
Version identification:
@(#)StringArrayState.cc	1.17	3/16/96

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

 Programmer:  I. Kuroda and J. T. Buck
 Date of creation: 10/29/92

This type of State is an array of strings.

**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include <std.h>
#include "StringArrayState.h"
#include "State.h"
#include "Tokenizer.h"
#include "KnownState.h"

void StringArrayState :: zap() {
	if ( val ) {
	    for (int i = 0; i < nElements; i++) {
		LOG_DEL; delete [] val[i];
	    }
	    LOG_DEL; delete [] val;
	    val = 0;
	}
	nElements = 0;
}

StringArrayState :: ~StringArrayState() {
	zap();
}

void StringArrayState :: resize(int newSize) {
    zap();
    LOG_NEW; val = new char* [nElements = newSize];
    for (int i = 0; i < newSize ; i++)
	val[i] = 0;
}

// copy constructor body
void StringArrayState :: copy(const StringArrayState & v) {
	LOG_NEW; val  = new char* [nElements = v.nElements];
	for (int i = 0; i < nElements; i++) {
		val[i] = savestring(v.val[i]);
	}
}

// assignment operator
StringArrayState &StringArrayState :: operator = (const StringArrayState & v) {
	if (this != &v) {
		zap();
		copy(v);
	}
	return *this;
}

// copy constructor
StringArrayState::StringArrayState(const StringArrayState& v) {
	copy(v);
}

// size (not inline because it's virtual)
int StringArrayState :: size() const { return nElements;}

// type (not inline because it's virtual)
const char* StringArrayState :: type() const { return "STRINGARRAY";}

// the value as a string: print one per line.

StringList StringArrayState :: currentValue() const { 
	StringList s; 
	if (val) {
		if (size() != 0) s << val[0];
		for (int i = 1; i < size(); i++) {
			s << '\n' << val[i];
		} 
	}
	return s;
}

// clone
State* StringArrayState :: clone() const
{ LOG_NEW; return new StringArrayState;}

// parameters for parsing a StringArrayState parameter expression
// -- number of string pointers to allocate statically (max. array length)
const int MAXLEN = 20000;
// -- maximum size of each string element 
const int MAXSTRINGLEN = 4096;

int StringArrayState::repeatItem(Tokenizer& lexer, char** buf, int& i) {
	char* saveValue = 0;
	if ( i > 0 ) {
		saveValue = buf[i-1];
	}
	if ( saveValue == 0 ) {
		parseError ("no string to repeat ",
			    "([ must appear after a string).");
		return FALSE;
	}
	ParseToken t = evalIntExpression(lexer);
	if (t.tok != T_Int) {
		parseError ("expected integer expression after '['");
		return FALSE;
	}
	int numRepeats = t.intval - 1;
	if ( numRepeats < 0 ) {
		parseError("cannot repeat a string a negative number of times.");
		return FALSE;
	}
	if (i + numRepeats > MAXLEN) {
		parseError ("too many strings while repeating ", saveValue);
		return FALSE;
	}
	while ( numRepeats > 0) {
		buf[i++] = savestring(saveValue);
		numRepeats--;   
	}
	t = getParseToken(lexer);
	if (t.tok != ']') {
		parseError ("expected ']'");
		return FALSE;
	}
	return TRUE;
}

int StringArrayState::expectParameterName(Tokenizer& lexer, char** buf, int& i) {
	char parameterName[MAXSTRINGLEN];
	char closeBraceBuf[MAXSTRINGLEN];

	// look for state name followed by closed curly brace
	lexer >> parameterName;
	lexer >> closeBraceBuf;
	const State* s = lookup(parameterName, parent()->parent());
	if (s == 0 || closeBraceBuf[0] != '}' || closeBraceBuf[1]) {
		lexer.pushBack(closeBraceBuf);
		lexer.pushBack(parameterName);
		return TRUE;
	}

	// By default, return TRUE
	int retval = TRUE;

	// String array state: copy element-by-element
	if (s->isA("StringArrayState")) {
		const StringArrayState *ss = (const StringArrayState*)s;
		int numstrings = ss->size();
		if (i + numstrings > MAXLEN) {
			parseError("too many strings encountered while including the StringArrayState parameter ",
			parameterName );
			retval = FALSE;
		}
		else {
			for (int j = 0; j < numstrings; j++)
				buf[i++] = savestring(ss->val[j]);
		}
	}
	else if (s->isArray()) {
		parseError("StringArrayStates cannot substitute values of type ",
			   s->type());
		retval = FALSE;
	}
	// others: one string only
	else {
		buf[i++] = s->currentValue().newCopy();
	}
	return retval;
}

// Parse initValue to set value
void StringArrayState  :: initialize() {
	// free any old memory
	zap();

	// handle empty initial value
	const char* initString = initValue();
	if (initString == 0 || *initString == 0) return;

	// initialize lexical parsing
	const char* specialChars = "<{[]}!";
	Tokenizer lexer(initString, specialChars);
	lexer.clearwhite();

	int i = 0, err = FALSE;
	char* buf[MAXLEN];
	while(!lexer.eof() && i < MAXLEN && !err) {
		char tokbuf[MAXSTRINGLEN];
		int saveToken = FALSE;
		lexer >> tokbuf;
		char c = tokbuf[0];
		if (c != 0 && tokbuf[1]) c = 0;

		switch (c) {

		    case '<':
			err = !mergeFileContents(lexer, tokbuf);
			saveToken = TRUE;
			break;

		    case '[':
			err = !repeatItem(lexer, buf, i);
			saveToken = FALSE;
			break;

		    case '{':
			err = !expectParameterName(lexer, buf, i);
			saveToken = FALSE;
			break;

		    case '!':
			err = !sendToInterpreter(lexer, tokbuf);
			saveToken = TRUE;
			break;

		    default:
			saveToken = TRUE;
			break;
		}

		if (saveToken && !err) buf[i++] = savestring(tokbuf);
		lexer.clearwhite();
	}

	// Make sure that we did not exceed the capacity of buf
	if ( !lexer.eof() && i == MAXLEN ) {
		parseError("too many strings!");
		err = TRUE;
	}

	// val was deallocated at the beginning of the routine
	if (!err) {
		nElements  = i;
		LOG_NEW; val = new char* [nElements];
		for(int j = 0; j < nElements; j++)
			val[j] = buf[j];
	}

	return;
}

int StringArrayState::member(const char* v) const {
	for (int i = 0; i < nElements; i++) {
		if (strcmp(v, val[i]) == 0) return TRUE;
	}
	return FALSE;
}

ISA_FUNC(StringArrayState,State);

int StringArrayState::isArray() const { return TRUE;}

const char* StringArrayState::className() const {
	return "StringArrayState";
}

// make knownstate entry
static StringArrayState proto;
static KnownState entry(proto,"STRINGARRAY");
