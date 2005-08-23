static const char file_id[] = "FloatState.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include <std.h>
#include <math.h>
#include <stdio.h>        // Needed for FILE below
#include "FloatState.h"
#include "Tokenizer.h"
#include "miscFuncs.h"
#include "KnownState.h"

/**************************************************************************
Version identification:
@(#)ScalarState.ccP	2.21	05/28/98

Copyright (c) 1990-1998 The Regents of the University of California.
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


 Programmer: I. Kuroda and J. T. Buck
 Date of creation: 6/15/90
 Revisions:

 While this code looks vaguely template-like, it is NOT a template.

 This file includes member functions for class FloatState, and also
 those functions of the baseclass, State, that parse Float expressions.

 IMPORTANT!!!!  If the name of this file is not ScalarState.ccP, DO NOT
 EDIT IT!  The files IntState.cc and FloatState.cc are both generated from
 a template file by means of the "genclass" program.

**************************************************************************/

// constructor
FloatState::FloatState() { val = 0;}

// return the type
const char* FloatState :: type() const { return "FLOAT";}

// the value as a string
StringList FloatState :: currentValue() const { StringList s; s = val; return s;}

// set initValue from a double (also sets current value)
void FloatState :: setInitValue(double arg) {
	StringList sl = arg;
	val = arg;
	State::setInitValue(hashstring(sl));
}

// clone
State* FloatState :: clone () const {
	LOG_NEW; FloatState *s = new FloatState;
	s->val = val;
	return s;
}

void FloatState  :: initialize() {
	const char* specialChars = "*+-/^()!";
	Tokenizer lexer(initValue(),specialChars);

	ParseToken t = evalFloatExpression(lexer);
	if (t.tok != T_ERROR && t.tok != T_EOF)
		val = t.doubleval;
	ParseToken t2 = getParseToken (lexer, T_Float);
	if (t2.tok != T_EOF)
		parseError ("extra text after valid expression");
}

// Baseclass parsing functions

// an expression is either a term or a series of terms with intervening
// '+' or '-' signs.

ParseToken State :: evalFloatExpression(Tokenizer& lexer) {
	ParseToken t1 = evalFloatTerm(lexer);
	if (t1.tok != T_Float) return t1;
	while (1) {
		ParseToken t2 = getParseToken(lexer, T_Float);
		if (t2.tok == '+' || t2.tok == '-') {
			ParseToken t3 = evalFloatTerm(lexer);
			if (t3.tok == T_EOF) {
				parseError ("unexpected end of string");
				return t3;
			}
			else if (t3.tok == T_ERROR) return t3;
			else if (t2.tok == '+')
				t1.doubleval += t3.doubleval;
			else	
				t1.doubleval -= t3.doubleval;
		}
		else {
			setPushback(t2);
			return t1;
		}
	}
} 

// a term is either a factor or a series of factors with intervening
// '*' or '/' signs.

ParseToken State :: evalFloatTerm(Tokenizer& lexer) {
	ParseToken t1 = evalFloatFactor(lexer);
	if (t1.tok != T_Float) return t1;
	while (1) {
		ParseToken t2 = getParseToken(lexer, T_Float);
		if (t2.tok == '*' || t2.tok == '/') {
			ParseToken t3 = evalFloatFactor(lexer);
			if (t3.tok == T_EOF) {
				parseError ("unexpected end of string");
				return t3;
			}
			else if (t3.tok == T_ERROR) return t3;
			else if (t2.tok == '*')
				t1.doubleval *= t3.doubleval;
			else if (t3.doubleval == 0)
				parseError ("divide by zero");
			else	
				t1.doubleval /= t3.doubleval;
		}
		else {
			setPushback(t2);
			return t1;
		}
	}
} 

// a factor is either an atom or a series of atoms with intervening
// '^' signs for exponentiation

ParseToken State :: evalFloatFactor(Tokenizer& lexer) {
	ParseToken t1 = evalFloatAtom(lexer);
	if (t1.tok != T_Float) return t1;
	while (1) {
		ParseToken t2 = getParseToken(lexer, T_Float);
		if (t2.tok == '^') {
			ParseToken t3 = evalFloatAtom(lexer);
			if (t3.tok == T_EOF) {
				parseError ("unexpected end of string");
				return t3;
			}
			else if (t3.tok == T_ERROR) return t3;
			else t1.doubleval = power(t1.doubleval,t3.doubleval);
		} else {
			setPushback(t2);
			return t1;
		}
	}
}

// an atom is any number of optional minus signs, followed
// by either a parenthesized expression or a Float value.

ParseToken State :: evalFloatAtom(Tokenizer& lexer) {
	double signflag = 1;
        ParseToken t = getParseToken(lexer, T_Float);
	while (t.tok == '-') {
		t = getParseToken(lexer, T_Float);
		signflag = -signflag;
	}
		
	switch (t.tok) {
	case EOF:
		break;
	case '(':
		// cfront wants the declaration of t1 in a block
	{
		ParseToken t1 = evalFloatExpression(lexer);
		if (t1.tok != T_Float) return t1;
		t = getParseToken(lexer, T_Float);
		if (t.tok == ')') {
			t1.doubleval *= signflag;
			return t1;
		}
		else {
			parseError ("unbalanced parentheses");
			t.tok = T_ERROR;
		}
	}
		break;
	case T_Float:
		t.doubleval = signflag * t.doubleval;
		break;
	default:
		t.tok = T_ERROR;
		parseError ("syntax error");
	}
	return t;
}

ISA_FUNC(FloatState,State);

// make knownstate entry
static FloatState proto;
static KnownState entry(proto,"FLOAT");
