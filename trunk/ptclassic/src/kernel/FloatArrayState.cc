static const char file_id[] = "FloatArrayState.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include <std.h>
#include "FloatArrayState.h"
#include "Tokenizer.h"
#include "KnownState.h"

#define MAXLEN 20000

/**************************************************************************
Version identification:
@(#)ArrayState.ccP	2.21	12/08/97

Copyright (c) 1990-1994 The Regents of the University of California.
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


 Programmer:  I. Kuroda and J. T. Buck
 Date of creation: 6/2/90
 Revisions:

 Methods for ArrayState classes: expanded for FloatArrayState

 Important!  This is not a "real" template file, but the fake kind
 of template provided by libg++'s "genclass".

**************************************************************************/
/*************************************************************************

	class FloatArrayState methods

**************************************************************************/

// constructor
FloatArrayState :: FloatArrayState (int size, double& fill_value) {
	LOG_NEW; val = new double [nElements = size];
	double * top = &(val[nElements]);
	double * t = val;
	while (t < top)
		*t++ = fill_value;
}

// destructor
FloatArrayState :: ~FloatArrayState () {LOG_DEL; delete [] val;}

// assignment operator
FloatArrayState &	FloatArrayState :: operator = (const FloatArrayState & v) {
	if (this != &v) {
		LOG_DEL; delete [] val;
		LOG_NEW; val  = new double [nElements = v.nElements];
		double* top = &(val[nElements]);
		double* t = val;
		double* u = v.val;
		while (t < top) *t++ = *u++;
	}
	return *this;
}

// size (not inline because it's virtual)
int FloatArrayState :: size() const { return nElements;}

// type (not inline because it's virtual)
const char* FloatArrayState :: type() const { return "FLOATARRAY";}

// the value as a string

StringList FloatArrayState :: currentValue() const { 
	StringList s; 
	for(int i = 0; i<size(); i++) {
		s += val[i];
		s += "\n";
	} 
	return s;
}

// clone
State* FloatArrayState :: clone() const { LOG_NEW; return new FloatArrayState;}

// Parse initValue to set value
void FloatArrayState  :: initialize() {
// free any old memory
	LOG_DEL; delete [] val;
	val = 0;
	nElements = 0;

	double buf[MAXLEN];
	const char* specialChars = "*+-/()<[]!";
	Tokenizer lexer(initValue(),specialChars);

	int i = 0, err = 0;
	int numRepeats;
	int sSiz;
	double saveValue = 0;
	while(!lexer.eof() && i < MAXLEN && err == 0) {
		ParseToken t = evalExpression(lexer);
		if (t.tok == T_EOF) break;
		switch (t.tok) {
		case '[':
			t = evalIntExpression(lexer);
			if (t.tok != T_Int) {
				parseError ("expected integer expression after '['");
				err = 1;
				break;
			}
			numRepeats = t.intval - 1;
			if (i + numRepeats > MAXLEN) {
				parseError ("too many elements!");
				err = 1;
				break;
			}
			while ( numRepeats > 0) {
				buf[i++] = saveValue;
				numRepeats--;   
			}
                        if (t.intval == 0) i--;
			t = getParseToken(lexer);
			if (t.tok != ']') {
				parseError ("expected ']'");
				err = 1;
				break;
			}
			break;
		case '(':
			// allow general expression inside parenths
			t = evalFloatExpression(lexer);
			if (t.tok == T_Float) {
				buf[i++] = t.doubleval;
				t = getParseToken(lexer);
				if (t.tok != ')') {
					parseError ("expected ')'");
					err = 1;
				}
			}
			else {
				parseError ("syntax error after '('");
				err = 1;
			}
			break;
		case T_Float:
			buf[i++] = t.doubleval;
			break;
		case T_ID:
			// got a state value.  Must have same type as me.
			if (strcmp((t.s)->type(), type()) != 0) {
				parseError ("bad state type in initValue: ",
					    t.s->type());
				err = 1;
				break;
			}
			sSiz = t.s->size();
			if (i + sSiz > MAXLEN) {
				parseError ("too many elements!");
			}
			else {	
				const double *sPtr =
					((const FloatArrayState *)(t.s))->val;
				for (int j = 0; j < sSiz; j++)
					buf[i++] = *sPtr++;
			}
			break;
		default:
			parseError ("syntax error");
			err = 1;
		}
		if (i > 0) saveValue = buf[i-1];
	}
	if (!err) {
		nElements  = i;
		LOG_NEW; val  = new double [nElements];
		for(i = 0; i < nElements; i++)
			val[i] = buf[i];
	}
	return;
}

ParseToken FloatArrayState :: evalExpression(Tokenizer& lexer)
{
        ParseToken t = getParseToken(lexer, T_Float);
	if (t.tok != '-') return t;
	t = getParseToken(lexer, T_Float);
	switch (t.tok) {
	case T_Float:
		t.doubleval = -t.doubleval;
		break;
	default:
		t.tok = T_ERROR;
	}
	return t;
}

// resize the array state: append zeros if size increases
void FloatArrayState :: resize (int newSize) {
	int i;
	if (newSize == nElements) return;
	double * oldVal = val;
	int oldSize = nElements;
	LOG_NEW; val = new double [nElements = newSize];
	int nCopy = oldSize;
	if (newSize < nCopy) nCopy = newSize;
	for (i = 0; i < nCopy; i++)
		val[i] = oldVal[i];
	int nZero = newSize - oldSize;
	// note: the following loop is executed zero times
	// if nZero is negative
	for (i = 0; i < nZero; i++)
		val[oldSize+i] = 0;
	LOG_DEL; delete [] oldVal;
}

// isA
ISA_FUNC(FloatArrayState,State);

// className
const char* FloatArrayState :: className() const {return "FloatArrayState";}

// i am an array
int FloatArrayState :: isArray() const { return TRUE;}

// make knownstate entry
static FloatArrayState proto;
static KnownState entry(proto,"FLOATARRAY");
