static const char file_id[] = "<T>State.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include <std.h>
#include <math.h>
#include "<T>State.h"
#include "Tokenizer.h"
#include "miscFuncs.h"
#include "KnownState.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: I. Kuroda and J. T. Buck
 Date of creation: 6/15/90
 Revisions:

 This file includes member functions for class <T>State, and also
 those functions of the baseclass, State, that parse <T> expressions.

 IMPORTANT!!!!  If the name of this file is not ScalarState.ccP, DO NOT
 EDIT IT!  The files IntState.cc and FloatState.cc are both generated from
 a template file by means of the "genclass" program.

**************************************************************************/

// constructor
<T>State::<T>State() { val = 0;}

// return the type
const char* <T>State :: type() const { return "<T>";}

// the value as a string
StringList <T>State :: currentValue() const { StringList s; s = val; return s;}

// clone
State* <T>State :: clone () const { LOG_NEW; return new <T>State;}

void <T>State  :: initialize() {
	const  char* specialChars =  "*+-/^()";
	Tokenizer lexer(initValue,specialChars);

	ParseToken t = eval<T>Expression(lexer);
	if (t.tok != T_ERROR && t.tok != T_EOF)
		val = t.<C>val;
	ParseToken t2 = getParseToken (lexer, T_<T>);
	if (t2.tok != T_EOF)
		parseError ("extra text after valid expression");
}

// Baseclass parsing functions

// an expression is either a term or a series of terms with intervening
// '+' or '-' signs.

ParseToken State :: eval<T>Expression(Tokenizer& lexer) {
	ParseToken t1 = eval<T>Term(lexer);
	if (t1.tok != T_<T>) return t1;
	while (1) {
		ParseToken t2 = getParseToken(lexer, T_<T>);
		if (t2.tok == '+' || t2.tok == '-') {
			ParseToken t3 = eval<T>Term(lexer);
			if (t3.tok == T_EOF) {
				parseError ("unexpected end of string");
				return t3;
			}
			else if (t3.tok == T_ERROR) return t3;
			else if (t2.tok == '+')
				t1.<C>val += t3.<C>val;
			else	
				t1.<C>val -= t3.<C>val;
		}
		else {
			pushback = t2;
			return t1;
		}
	}
} 

// a term is either a factor or a series of factors with intervening
// '*' or '/' signs.

ParseToken State :: eval<T>Term(Tokenizer& lexer) {
	ParseToken t1 = eval<T>Factor(lexer);
	if (t1.tok != T_<T>) return t1;
	while (1) {
		ParseToken t2 = getParseToken(lexer, T_<T>);
		if (t2.tok == '*' || t2.tok == '/') {
			ParseToken t3 = eval<T>Factor(lexer);
			if (t3.tok == T_EOF) {
				parseError ("unexpected end of string");
				return t3;
			}
			else if (t3.tok == T_ERROR) return t3;
			else if (t2.tok == '*')
				t1.<C>val *= t3.<C>val;
			else if (t3.<C>val == 0)
				parseError ("divide by zero");
			else	
				t1.<C>val /= t3.<C>val;
		}
		else {
			pushback = t2;
			return t1;
		}
	}
} 

// a factor is either an atom or a series of atoms with intervening
// '^' signs for exponentiation

ParseToken State :: eval<T>Factor(Tokenizer& lexer) {
	ParseToken t1 = eval<T>Atom(lexer);
	if (t1.tok != T_<T>) return t1;
	while (1) {
		ParseToken t2 = getParseToken(lexer, T_<T>);
		if (t2.tok == '^') {
			ParseToken t3 = eval<T>Atom(lexer);
			if (t3.tok == T_EOF) {
				parseError ("unexpected end of string");
				return t3;
			}
			else if (t3.tok == T_ERROR) return t3;
			else t1.<C>val = power(t1.<C>val,t3.<C>val);
		} else {
			pushback = t2;
			return t1;
		}
	}
}

// an atom is any number of optional minus signs, followed
// by either a parenthesized expression or a <T> value.

ParseToken State :: eval<T>Atom(Tokenizer& lexer) {
	<C> signflag = 1;
        ParseToken t = getParseToken(lexer, T_<T>);
	while (t.tok == '-') {
		t = getParseToken(lexer, T_<T>);
		signflag = -signflag;
	}
		
	switch (t.tok) {
	case EOF:
		break;
	case '(':
		// cfront wants the declaration of t1 in a block
	{
		ParseToken t1 = eval<T>Expression(lexer);
		if (t1.tok != T_<T>) return t1;
		t = getParseToken(lexer, T_<T>);
		if (t.tok == ')') {
			t1.<C>val *= signflag;
			return t1;
		}
		else {
			parseError ("unbalanced parentheses");
			t.tok = T_ERROR;
		}
	}
		break;
	case T_<T>:
		t.<C>val = signflag * t.<C>val;
		break;
	default:
		t.tok = T_ERROR;
		parseError ("syntax error");
	}
	return t;
}

ISA_FUNC(<T>State,State);

// make knownstate entry
static <T>State proto;
static KnownState entry(proto,"<T>");
