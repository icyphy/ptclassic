static const char file_id[] = "ComplexArrayState.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include <std.h>
#include "ComplexArrayState.h"
#include "Tokenizer.h"
#include "KnownState.h"

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

// alternate constructor: size
ComplexArrayState :: ComplexArrayState (int size) {
	LOG_NEW; val = new Complex [nElements = size];
}

// alternate constructor: size and fill value.
ComplexArrayState :: ComplexArrayState (int size, const Complex& fill_value) {
	LOG_NEW; val = new Complex [nElements = size];
	Complex * top = &(val[nElements]);
	Complex * t = val;
	while (t < top) *t++ = fill_value;
}

const char* ComplexArrayState :: type() const { return "COMPLEXARRAY";}

int ComplexArrayState :: size() const { return nElements;}

State *ComplexArrayState :: clone() const { LOG_NEW; return new ComplexArrayState;}

ComplexArrayState :: ~ComplexArrayState () {
	// ? no Complex destructor: do we want delete [] val, or delete val?
	// g++ seems to want delete val
	LOG_DEL; delete val;
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
	LOG_DEL; delete val;
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
		case T_ID:
			// got a state reference.  Must have same type as me.
			if (strcmp((t.s)->type(), type()) != 0) {
				parseError ("bad state type in initValue","");
				return;
			}
			int sSiz = t.s->size();
			if (i + sSiz > MAXLEN) {
				parseError ("too many elements!");
				break;
			}
			const Complex *sPtr = ((const ComplexArrayState *)(t.s))->val;
			for (int j = 0; j < sSiz; j++)
				buf[i++] = sPtr[j];
			break;
		default:
			parseError ("syntax error in complex expression");
			return;
		}
	}
	nElements  = i;
	LOG_NEW; val = new Complex [nElements];	
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

StringList ComplexArrayState :: currentValue() const {
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

// resize the array state: append zeros if size increases
void ComplexArrayState :: resize (int newSize) {
	if (newSize == nElements) return;
	Complex * oldVal = val;
	int oldSize = nElements;
	// constructor initializes elements to (0,0)
	LOG_NEW; val = new Complex [nElements = newSize];
	int nCopy = oldSize;
	if (newSize < nCopy) nCopy = newSize;
	for (int i = 0; i < nCopy; i++)
		val[i] = oldVal[i];
	LOG_DEL; delete oldVal;
}

// make knownstate entry
static ComplexArrayState proto;
static KnownState entry(proto,"COMPLEXARRAY");

// isa
ISA_FUNC(ComplexArrayState,State);
