static const char file_id[] = "StringArrayState.cc";
/**************************************************************************
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

 Programmer:  I. Kuroda and J. T. Buck
 Date of creation: 10/29/92

This type of State is an array of strings.

**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include <std.h>
#include "StringArrayState.h"
#include "Tokenizer.h"
#include "KnownState.h"

void StringArrayState :: zap() {
	for (int i = 0; i < nElements; i++) {
		LOG_DEL; delete [] val[i];
	}
	LOG_DEL; delete [] val;
	val = 0;
	nElements = 0;
}

StringArrayState :: ~StringArrayState() {
	zap();
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
	for(int i = 0; i<size(); i++) {
		s << val[i] << "\n";
	} 
	return s;
}

// clone
State* StringArrayState :: clone() const
{ LOG_NEW; return new StringArrayState;}

const int MAXLEN = 20000;
const int MAXSTRINGLEN = 4096;

// Parse initValue to set value
void StringArrayState  :: initialize() {
// free any old memory
	zap();
	val = 0;
	nElements = 0;

	char* buf[MAXLEN];
	char tokbuf[MAXSTRINGLEN];
	const char* specialChars = "{[]}";
	Tokenizer lexer(initValue(),specialChars);

	int i = 0, err = 0;
	int numRepeats;
	char* saveValue = 0;
	ParseToken t;
	while(!lexer.eof() && i < MAXLEN && err == 0) {
		lexer >> tokbuf;
		char c = tokbuf[0];
		if (tokbuf[1]) c = 0;
		switch (c) {
		case '[':
			t = evalIntExpression(lexer);
			if (t.tok != T_Int) {
				parseError ("expected integer expression after '['");
				err = 1;
				break;
			}
			numRepeats = t.intval - 1;
			if (i + numRepeats > MAXLEN) {
				parseError ("too many elements!");
				err = 1;
				break;
			}
			while ( numRepeats != 0) {
				buf[i++] = savestring(saveValue);
				numRepeats--;   
			}
			t = getParseToken(lexer);
			if (t.tok != ']') {
				parseError ("expected ']'");
				err = 1;
				break;
			}
			break;
		case '{':
			{
			// next token must be a state name.
			lexer >> tokbuf;
			const State* s = lookup(tokbuf,parent()->parent());
			if (!s) {
				parseError ("undefined symbol: ",tokbuf);
				err = 1;
				break;
			}
			lexer >> tokbuf;
			if (tokbuf[0] != '}') {
				parseError ("expected '}'");
				err = 1;
				break;
			}
			// stringarraystate: copy element-by-element
			if (s->isA("StringArrayState")) {
				const StringArrayState *ss = 
					(const StringArrayState*)s;
				for (int j = 0; j < ss->size(); j++)
					buf[i++] = savestring(ss->val[j]);
			}
			else if (s->isArray()) {
				parseError(
		   "StringArrayStates cannot substitute values of type ",
					   s->type());
				err = 1;
				break;
			}
			// others: one string only
			else {
				buf[i++] = s->currentValue().newCopy();
			}
			}
			break;
		default:
			buf[i++] = savestring(tokbuf);
		}
		saveValue = buf[i-1];
	}
	if (!err) {
		nElements  = i;
		LOG_NEW; val = new char* [nElements];
		for(i = 0; i < nElements; i++)
			val[i] = buf[i];
	}
	return;
}

int StringArrayState::member(const char* v) const {
	for (int i = 0; i < nElements; i++) {
		if (strcmp(v,val[i]) == 0) return TRUE;
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
