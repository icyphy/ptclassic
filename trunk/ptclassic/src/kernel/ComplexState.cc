static const char file_id[] = "ComplexState.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "ComplexState.h"
#include "KnownState.h"
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

void ComplexState :: setInitValue(const Complex& arg) {
	StringList sl;
	sl << "(" << arg.real() << "," << arg.imag() << ")";
	State::setInitValue(hashstring(sl));
	val = arg;
}

// clone
State* ComplexState :: clone () const {
	LOG_NEW; ComplexState *s = new ComplexState;
	s->val = val;
	return s;
}

void ComplexState  :: initialize() {
	const  char* specialChars =  "*+-/(),";
	Tokenizer lexer(initValue(),specialChars);

	ParseToken t =getParseToken(lexer);

	// possibility one: a galaxy ComplexState name

	if (t.tok == T_ID) {
		if (!t.s->isA("ComplexState")) {
			parseError ("invalid state type: ", t.s->fullName());
			return;
		}
		const ComplexState& cstate = *(const ComplexState*)t.s;
		val = cstate.val;
	}
	// possibility two:
	// if it does not begin with '(', assume a purely real value.
	// this may be any valid floating expression.
	else if (t.tok != '(') {
		pushback = t;
		t = evalFloatExpression(lexer);
		if (t.tok == T_Float) val = t.doubleval;
		// if wrong type, evalFloatExpression has already complained
		else return;
	}
	// possibility three: ( exp , exp )
	// where each exp is a floating expression.
	else {
		// get real , imag
		t =  evalFloatExpression(lexer);
		if (t.tok != T_Float) return;
		double realval = t.doubleval;
		t = getParseToken(lexer);
		if (t.tok != ',') {
			parseError ("expected a comma");
			return;
		}
		t =  evalFloatExpression(lexer);
		if (t.tok != T_Float) return;
		double imagval = t.doubleval;
		val = Complex(realval,imagval);
		t = getParseToken (lexer);
		if (t.tok != ')') {
			parseError ("expected )");
			return;
		}
	}
	// check for extra cruft (this also eats up any pushback token)
	ParseToken t2 = getParseToken (lexer);
	if (t2.tok != T_EOF)
		parseError ("extra text after valid expression");
	return;
}

// make knownstate entry
static ComplexState proto;
static KnownState entry(proto,"COMPLEX");

// isa
ISA_FUNC(ComplexState,State);
