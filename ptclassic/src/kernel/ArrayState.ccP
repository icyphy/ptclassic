#include <std.h>
#include "<T>State.h"
#include "<T>ArrayState.h"
#include "Tokenizer.h"

const int MAXLEN = 2000;

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
	val = new <C> [nElements = size];
	<C> * top = &(val[nElements]);
	<C> * t = val;
	while (t < top)
		*t++ = fill_value;
}

// destructor
<T>ArrayState :: ~<T>ArrayState () {delete[nElements] val;}

// assignment operator
<T>ArrayState &	<T>ArrayState :: operator = (const <T>ArrayState & v) {
	if (this != &v) {
		delete[nElements] val;
		val  = new <C> [nElements = v.nElements];
		<C>* top = &(val[nElements]);
		<C>* t = val;
		<C>* u = v.val;
		while (t < top) *t++ = *u++;
	}
	return *this;
}

// size (not inline because it's virtual)
int <T>ArrayState :: size() { return nElements;}

// type (not inline because it's virtual)
char* <T>ArrayState :: type() { return "<T>Array";}

// the value as a string

StringList <T>ArrayState :: currentValue() { 
	StringList s; 
	s =  "\n";
	for(int i = 0; i<size(); i++) {
		s += i;
		s += " ";
		s += val[i];
		s += "\n";
	} 
	return s;
}

// clone
State* <T>ArrayState :: clone() { return new <T>ArrayState;}

// Parse initValue to set value
void <T>ArrayState  :: initialize() {
// free any old memory
	delete val;
	val = 0;
	nElements = 0;

	<C> buf[MAXLEN];
	char* specialChars = "*+-/()<[]";
	Tokenizer lexer(initValue,specialChars);

	int i = 0, err = 0;
	int numRepeats;
	<C> saveValue;
	while(!lexer.eof() && i < MAXLEN && err == 0) {
		ParseToken t = evalExpression(lexer);
		if (t.tok == T_EOF) break;
		switch (t.tok) {
		case '[':
			t = getParseToken(lexer, T_Int);
			if (t.tok != T_Int) {
				parseError ("expected intval after", "[");
				err = 1;
				break;
			}
			numRepeats = t.intval - 1;
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
		case T_<T>:
			buf[i++] = t.<C>val;
			break;
		default:
			parseError ("syntax error", "");
			err = 1;
		}
		saveValue = buf[i-1];
	}
	if (!err) {
		nElements  = i;
		val  = new <C> [nElements];
		for(i = 0; i < nElements; i++)
			val[i] = buf[i];
	}
	return;
}

ParseToken <T>ArrayState :: evalExpression(Tokenizer& lexer)
{
        <C> signflag = 1;
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

// make knownstate entry
static <T>ArrayState proto;
static KnownState entry(proto,"<T>Array");

	 


	 
