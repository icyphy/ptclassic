
#ifdef __GNUG__
#pragma implementation
#endif

#include <std.h>
#include "ComplexState.h"
#include "FloatState.h"
#include "Tokenizer.h"


/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: I. Kuroda and J. T. Buck
 Date of creation: 6/15/90
 Revisions:

 Functions for class ComplexState

**************************************************************************/
/*************************************************************************

	class ComplexState methods

**************************************************************************/

// the type
const char* ComplexState :: type() const { return "COMPLEX";}

// the value as a string
StringList ComplexState :: currentValue() const {
	StringList s; 
	s = "(";
	s += val.real();
	s += ",";
	s += val.imag();
	s += ")\n";
	return s;
}

// clone
State* ComplexState :: clone () const { return new ComplexState;}

void ComplexState  :: initialize() {
	const  char* specialChars =  "*+-/(),";
	Tokenizer lexer(initValue,specialChars);

	double realval;
	double imagval;
	ParseToken t =getParseToken(lexer);
	if (t.tok == T_ID) {
		val = ((ComplexState*)t.s)->val;
		return;
	}
	if (t.tok != '(') {
		parseError ("syntax error, want (1.2, 3.4) type syntax");
		return;
	}
	t =  evalExpression(lexer);
	if (t.tok != T_Float) return;
	realval = t.doubleval;
	t = getParseToken(lexer);
	if (t.tok != ',') {
		parseError ("expected a comma");
		return;
	}
	t =  evalExpression(lexer);
	if (t.tok != T_Float) return;
	imagval = t.doubleval;
	val = Complex(realval,imagval);
	t = getParseToken (lexer);
	if (t.tok != ')')
		parseError ("expected )");
	return;
	return;
}

ParseToken ComplexState :: evalExpression(Tokenizer& lexer) {
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


// make knownstate entry
static ComplexState proto;
static KnownState entry(proto,"COMPLEX");

// isa
ISA_FUNC(ComplexState,State);
