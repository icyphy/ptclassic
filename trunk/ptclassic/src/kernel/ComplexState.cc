
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
		if (!t.s->isA("ComplexState")) {
			parseError ("invalid state type: ", t.s->readFullName());
			return;
		}
		val = ((ComplexState*)t.s)->val;
		return;
	}
	// if it does not begin with '(', assume a purely real value.
	if (t.tok != '(') {
		pushback = t;
		t = evalFloatExpression(lexer);
		if (t.tok == T_Float) {
			realval = t.doubleval;
			imagval = 0.0;
		}
		return;
	}
	// get real , imag
	t =  evalFloatExpression(lexer);
	if (t.tok != T_Float) return;
	realval = t.doubleval;
	t = getParseToken(lexer);
	if (t.tok != ',') {
		parseError ("expected a comma");
		return;
	}
	t =  evalFloatExpression(lexer);
	if (t.tok != T_Float) return;
	imagval = t.doubleval;
	val = Complex(realval,imagval);
	t = getParseToken (lexer);
	if (t.tok != ')')
		parseError ("expected )");
	return;
}

// make knownstate entry
static ComplexState proto;
static KnownState entry(proto,"COMPLEX");

// isa
ISA_FUNC(ComplexState,State);
