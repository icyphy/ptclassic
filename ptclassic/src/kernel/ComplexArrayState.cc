#include <std.h>
#include "ComplexState.h"
#include "ComplexArrayState.h"
#include "FloatState.h"
#include "Tokenizer.h"

const int MAXLEN = 2000;

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  I. Kuroda and J. T. Buck
 Date of creation: 6/20/90
 Revisions:



**************************************************************************/
/*************************************************************************

	class ComplexArrayState methods

**************************************************************************/

const char* ComplexArrayState :: type() const { return "ComplexArray";}

ComplexArrayState :: ~ComplexArrayState () {
	// ? no Complex destructor: do we want delete [] val, or delete val?
	// g++ seems to want delete val
	delete val;
}

void ComplexArrayState  :: initialize() {

	Complex buf[MAXLEN];
	const char* specialChars = "*+-/()<,[]";
	Tokenizer lexer(initValue,specialChars);
	double realval = 0;
	double imagval = 0;
	int numRepeats;

	// ? no Complex destructor: do we want delete [] val, or delete val?
	// g++ seems to want delete val
	delete val;
	val = 0;
	nElements = 0;

	int i = 0;
	while(!lexer.eof() && i < MAXLEN) {
		ParseToken t =getParseToken(lexer);
		switch (t.tok) {
		case T_ERROR:
			return;
		case T_EOF:
			break;
		case '[':
			t = evalIntExpression(lexer);
			if (t.tok != T_Int) {
				parseError ("expected integer expression after '['");
				return;
			}
			numRepeats = t.intval - 1;
			while (numRepeats != 0) {
				buf[i++] = Complex(realval,imagval);
				numRepeats--;
			}
			t = getParseToken(lexer);
			if (t.tok != ']') {
				parseError ("expected ']'");
				return;
			}
			break;
		case '(':
			t = evalExpression(lexer);
			if (t.tok == T_ERROR || t.tok == T_EOF) return;
			realval = t.doubleval;
			t = getParseToken(lexer);
			if (t.tok != ',') {
				parseError ("expected ','");
				return;
			}
			t = evalExpression(lexer);
			if (t.tok == T_ERROR || t.tok == T_EOF) return;
			imagval = t.doubleval;
			t = getParseToken(lexer);
			if (t.tok != ')') {
				parseError ("expected ')'");
				return;
			}
			buf[i++] = Complex(realval,imagval);
			break;
		default:
			parseError ("syntax error");
			return;
		}
	}
	nElements  = i;
	val = new Complex [nElements];	
	for(i = 0; i < nElements; i++)
		val[i] = buf[i];
	return;
}

ParseToken ComplexArrayState :: evalExpression(Tokenizer& lexer) {
	double signflag = 1;
	
        ParseToken t = getParseToken(lexer, T_Float);
	while (t.tok == '-') {
		t = getParseToken(lexer, T_Float);
		signflag = -signflag;
	}
	if (t.tok == T_EOF) {
		parseError ("unexpected end of string");
		return t;
	}
	if (t.tok == T_Float) {
		t.doubleval *= signflag;
		return t;
	}
	parseError ("syntax error");
	t.tok = T_ERROR;
	return t;
}

StringList ComplexArrayState :: currentValue() {
	StringList s; 
	s =  "\n";
	for(int i = 0; i<size(); i++) 
	{
		s = i; 
		s = "	("; 
		s = val[i].real();
		s = ","; 
		s = val[i].imag(); 
		s= ")\n";
	} 
	return s;
}

// make knownstate entry
static ComplexArrayState proto;
static KnownState entry(proto,"ComplexArray");
