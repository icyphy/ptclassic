static const char file_id[] = "IntState.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include <std.h>
#include <math.h>
#include <stdio.h>        // Needed for FILE below
#include "IntState.h"
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

 This file includes member functions for class IntState, and also
 those functions of the baseclass, State, that parse Int expressions.

 IMPORTANT!!!!  If the name of this file is not ScalarState.ccP, DO NOT
 EDIT IT!  The files IntState.cc and FloatState.cc are both generated from
 a template file by means of the "genclass" program.

**************************************************************************/

// constructor
IntState::IntState() { val = 0;}

// return the type
const char* IntState :: type() const { return "INT";}

// the value as a string
StringList IntState :: currentValue() const { StringList s; s = val; return s;}

// set initValue from a int (also sets current value)
void IntState :: setInitValue(int arg) {
	StringList sl = arg;
	val = arg;
	State::setInitValue(hashstring(sl));
}

// clone
State* IntState :: clone () const {
	LOG_NEW; IntState *s = new IntState;
	s->val = val;
	return s;
}

void IntState  :: initialize() {
	const char* specialChars = "*+-/^()!";
	Tokenizer lexer(initValue(),specialChars);

	ParseToken t = evalIntExpression(lexer);
	if (t.tok != T_ERROR && t.tok != T_EOF)
		val = t.intval;
	ParseToken t2 = getParseToken (lexer, T_Int);
	if (t2.tok != T_EOF)
		parseError ("extra text after valid expression");
}

// Baseclass parsing functions

// an expression is either a term or a series of terms with intervening
// '+' or '-' signs.

ParseToken State :: evalIntExpression(Tokenizer& lexer) {
	ParseToken t1 = evalIntTerm(lexer);
	if (t1.tok != T_Int) return t1;
	while (1) {
		ParseToken t2 = getParseToken(lexer, T_Int);
		if (t2.tok == '+' || t2.tok == '-') {
			ParseToken t3 = evalIntTerm(lexer);
			if (t3.tok == T_EOF) {
				parseError ("unexpected end of string");
				return t3;
			}
			else if (t3.tok == T_ERROR) return t3;
			else if (t2.tok == '+')
				t1.intval += t3.intval;
			else	
				t1.intval -= t3.intval;
		}
		else {
			setPushback(t2);
			return t1;
		}
	}
} 

// a term is either a factor or a series of factors with intervening
// '*' or '/' signs.

ParseToken State :: evalIntTerm(Tokenizer& lexer) {
	ParseToken t1 = evalIntFactor(lexer);
	if (t1.tok != T_Int) return t1;
	while (1) {
		ParseToken t2 = getParseToken(lexer, T_Int);
		if (t2.tok == '*' || t2.tok == '/') {
			ParseToken t3 = evalIntFactor(lexer);
			if (t3.tok == T_EOF) {
				parseError ("unexpected end of string");
				return t3;
			}
			else if (t3.tok == T_ERROR) return t3;
			else if (t2.tok == '*')
				t1.intval *= t3.intval;
			else if (t3.intval == 0)
				parseError ("divide by zero");
			else	
				t1.intval /= t3.intval;
		}
		else {
			setPushback(t2);
			return t1;
		}
	}
} 

// a factor is either an atom or a series of atoms with intervening
// '^' signs for exponentiation

ParseToken State :: evalIntFactor(Tokenizer& lexer) {
	ParseToken t1 = evalIntAtom(lexer);
	if (t1.tok != T_Int) return t1;
	while (1) {
		ParseToken t2 = getParseToken(lexer, T_Int);
		if (t2.tok == '^') {
			ParseToken t3 = evalIntAtom(lexer);
			if (t3.tok == T_EOF) {
				parseError ("unexpected end of string");
				return t3;
			}
			else if (t3.tok == T_ERROR) return t3;
			else t1.intval = power(t1.intval,t3.intval);
		} else {
			setPushback(t2);
			return t1;
		}
	}
}

// an atom is any number of optional minus signs, followed
// by either a parenthesized expression or a Int value.

ParseToken State :: evalIntAtom(Tokenizer& lexer) {
	int signflag = 1;
        ParseToken t = getParseToken(lexer, T_Int);
	while (t.tok == '-') {
		t = getParseToken(lexer, T_Int);
		signflag = -signflag;
	}
		
	switch (t.tok) {
	case EOF:
		break;
	case '(':
		// cfront wants the declaration of t1 in a block
	{
		ParseToken t1 = evalIntExpression(lexer);
		if (t1.tok != T_Int) return t1;
		t = getParseToken(lexer, T_Int);
		if (t.tok == ')') {
			t1.intval *= signflag;
			return t1;
		}
		else {
			parseError ("unbalanced parentheses");
			t.tok = T_ERROR;
		}
	}
		break;
	case T_Int:
		t.intval = signflag * t.intval;
		break;
	default:
		t.tok = T_ERROR;
		parseError ("syntax error");
	}
	return t;
}

ISA_FUNC(IntState,State);

// make knownstate entry
static IntState proto;
static KnownState entry(proto,"INT");
