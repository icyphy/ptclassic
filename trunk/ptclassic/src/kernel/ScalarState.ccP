
#include <std.h>
#include "<T>State.h"
#include "Tokenizer.h"


/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: I. Kuroda and J. T. Buck
 Date of creation: 6/15/89
 Revisions:

 Functions for class <T>State

**************************************************************************/
/*************************************************************************

	class <T>State methods

**************************************************************************/

// return the type
char* <T>State :: type() { return "<T>";}

// the value as a string
StringList <T>State :: currentValue() { StringList s; s = val; return s;}

// clone
State* <T>State :: clone () { return new <T>State;}

void <T>State  :: initialize() {
	const  char* specialChars =  "*+-/()";
	Tokenizer lexer(initValue,specialChars);

	ParseToken t = evalExpression(lexer);
	if (t.tok != T_ERROR && t.tok != T_EOF)
		val = t.<C>val;
	ParseToken t2 = getParseToken (lexer, T_<T>);
	if (t2.tok != T_EOF)
		parseError ("extra text after valid expression");
}

ParseToken <T>State :: evalExpression(Tokenizer& lexer) {
	ParseToken t1 = evalTerm(lexer);
	if (t1.tok != T_<T>) return;
	while (1) {
		ParseToken t2 = getParseToken(lexer, T_<T>);
		if (t2.tok == '+' || t2.tok == '-') {
			ParseToken t3 = evalTerm(lexer);
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

ParseToken <T>State :: evalTerm(Tokenizer& lexer) {
	ParseToken t1 = evalFactor(lexer);
	if (t1.tok != T_<T>) return;
	while (1) {
		ParseToken t2 = getParseToken(lexer, T_<T>);
		if (t2.tok == '*' || t2.tok == '/') {
			ParseToken t3 = evalFactor(lexer);
			if (t3.tok == T_EOF) {
				parseError ("unexpected end of string");
				return t3;
			}
			else if (t3.tok == T_ERROR) return t3;
			else if (t2.tok == '*')
				t1.<C>val *= t3.<C>val;
			else	
				t1.<C>val /= t3.<C>val;
		}
		else {
			pushback = t2;
			return t1;
		}
	}
} 


ParseToken <T>State :: evalFactor(Tokenizer& lexer) {
	
	<C> signflag = 1;
        ParseToken t = getParseToken(lexer, T_<T>);
	while (t.tok == '-') {
		t = getParseToken(lexer, T_<T>);
		signflag = -signflag;
	}
		
	switch (t.tok) {
	case EOF:
		return t;
	case '(':
		ParseToken t1 = evalExpression(lexer);
		if (t1.tok != T_<T>) return t1;
		t = getParseToken(lexer, T_<T>);
		if (t.tok == ')') {
			t1.<C>val *= signflag;
			return t1;
		}
		else {
			parseError ("unbalanced parentheses");
			t.tok = T_ERROR;
			return t;
		}
	case T_<T>:
		t.<C>val = signflag * t.<C>val;
		return  t;
	default:
		t.tok = T_ERROR;
		parseError ("syntax error");
	}
}


// make knownstate entry
static <T>State proto;
static KnownState entry(proto,"<C>");

	 


	 
