static const char file_id[] = "<T>ArrayState.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include <std.h>
#include "<T>ArrayState.h"
#include "Tokenizer.h"
#include "KnownState.h"

#define MAXLEN 20000

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  I. Kuroda and J. T. Buck
 Date of creation: 6/2/90
 Revisions:



**************************************************************************/
/*************************************************************************

	class <T>ArrayState methods

**************************************************************************/

// constructor
<T>ArrayState :: <T>ArrayState (int size, <C>& fill_value) {
	LOG_NEW; val = new <C> [nElements = size];
	<C> * top = &(val[nElements]);
	<C> * t = val;
	while (t < top)
		*t++ = fill_value;
}

// destructor
<T>ArrayState :: ~<T>ArrayState () {LOG_DEL; delete val;}

// assignment operator
<T>ArrayState &	<T>ArrayState :: operator = (const <T>ArrayState & v) {
	if (this != &v) {
		LOG_DEL; delete val;
		LOG_NEW; val  = new <C> [nElements = v.nElements];
		<C>* top = &(val[nElements]);
		<C>* t = val;
		<C>* u = v.val;
		while (t < top) *t++ = *u++;
	}
	return *this;
}

// size (not inline because it's virtual)
int <T>ArrayState :: size() const { return nElements;}

// type (not inline because it's virtual)
const char* <T>ArrayState :: type() const { return "<T>ARRAY";}

// the value as a string

StringList <T>ArrayState :: currentValue() const { 
	StringList s; 
	for(int i = 0; i<size(); i++) {
		s += val[i];
		s += "\n";
	} 
	return s;
}

// clone
State* <T>ArrayState :: clone() const { LOG_NEW; return new <T>ArrayState;}

// Parse initValue to set value
void <T>ArrayState  :: initialize() {
// free any old memory
	LOG_DEL; delete val;
	val = 0;
	nElements = 0;

	<C> buf[MAXLEN];
	const char* specialChars = "*+-/()<[]";
	Tokenizer lexer(initValue,specialChars);

	int i = 0, err = 0;
	int numRepeats;
	int sSiz;
	<C> saveValue = 0;
	while(!lexer.eof() && i < MAXLEN && err == 0) {
		ParseToken t = evalExpression(lexer);
		if (t.tok == T_EOF) break;
		switch (t.tok) {
		case '[':
			t = evalIntExpression(lexer);
			if (t.tok != T_Int) {
				parseError ("expected integer expression after", "[");
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
				buf[i++] = saveValue;
				numRepeats--;   
			}
			t = getParseToken(lexer);
			if (t.tok != ']') {
				parseError ("expected ']'", "");
				err = 1;
				break;
			}
			break;
		case '(':
			// allow general expression inside parenths
			t = eval<T>Expression(lexer);
			if (t.tok == T_<T>) {
				buf[i++] = t.<C>val;
				t = getParseToken(lexer);
				if (t.tok != ')') {
					parseError ("expected ')'", "");
					err = 1;
				}
			}
			else {
				parseError ("syntax error after '('", "");
				err = 1;
			}
			break;
		case T_<T>:
			buf[i++] = t.<C>val;
			break;
		case T_ID:
			// got a state value.  Must have same type as me.
			if (strcmp((t.s)->type(), type()) != 0) {
				parseError ("bad state type in initValue","");
				err = 1;
				break;
			}
			sSiz = t.s->size();
			if (i + sSiz > MAXLEN) {
				parseError ("too many elements!");
				break;
			}
			const <C> *sPtr = ((const <T>ArrayState *)(t.s))->val;
			for (int j = 0; j < sSiz; j++)
				buf[i++] = *sPtr++;
			break;
		default:
			parseError ("syntax error", "");
			err = 1;
		}
		saveValue = buf[i-1];
	}
	if (!err) {
		nElements  = i;
		LOG_NEW; val  = new <C> [nElements];
		for(i = 0; i < nElements; i++)
			val[i] = buf[i];
	}
	return;
}

ParseToken <T>ArrayState :: evalExpression(Tokenizer& lexer)
{
        ParseToken t = getParseToken(lexer, T_<T>);
	if (t.tok != '-') return t;
	t = getParseToken(lexer, T_<T>);
	switch (t.tok) {
	case T_<T>:
		t.<C>val = -t.<C>val;
		break;
	default:
		t.tok = T_ERROR;
	}
	return t;
}

// resize the array state: append zeros if size increases
void <T>ArrayState :: resize (int newSize) {
	if (newSize == nElements) return;
	<C> * oldVal = val;
	int oldSize = nElements;
	LOG_NEW; val = new <C> [nElements = newSize];
	int nCopy = oldSize;
	if (newSize < nCopy) nCopy = newSize;
	for (int i = 0; i < nCopy; i++)
		val[i] = oldVal[i];
	int nZero = newSize - oldSize;
	// note: the following loop is executed zero times
	// if nZero is negative
	for (i = 0; i < nZero; i++)
		val[oldSize+i] = 0;
	LOG_DEL; delete oldVal;
}

// isA
ISA_FUNC(<T>ArrayState,State);

// make knownstate entry
static <T>ArrayState proto;
static KnownState entry(proto,"<T>ARRAY");
