static const char file_id[] = "FixArrayState.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include <std.h>
#include "FixArrayState.h"
#include "Tokenizer.h"
#include "KnownState.h"

const int MAXLEN = 20000;

/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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

 Programmer: A. Khazeni  
 Date of creation: 2/24/93 
 Revisions:



**************************************************************************/
/*************************************************************************

	class FixArrayState methods

**************************************************************************/

// alternate constructor: size
FixArrayState :: FixArrayState (int size) {
	LOG_NEW; val = new Fix [nElements = size];
}

// alternate constructor: size and fill value.
FixArrayState :: FixArrayState (int size, const Fix& fill_value) {
	LOG_NEW; val = new Fix [nElements = size];
	Fix * top = &(val[nElements]);
	Fix * t = val;
	while (t < top) *t++ = fill_value;
}

const char* FixArrayState :: type() const { return "FIXARRAY";}

const char* FixArrayState :: className() const {
	return "FixArrayState";
}

int FixArrayState :: isArray() const { return TRUE;}

int FixArrayState :: size() const { return nElements;}

State *FixArrayState :: clone() const { LOG_NEW; return new FixArrayState;}

FixArrayState :: ~FixArrayState () {
	LOG_DEL; delete [] val;
}

void FixArrayState  :: initialize() {
// free any old memory
        LOG_DEL; delete [] val;
        val = 0;
        nElements = 0;

	Fix buf[MAXLEN];
        const char* specialChars = "*+-/()<[]";
        Tokenizer lexer(initValue(),specialChars);

        int i = 0, err = 0;
        int numRepeats;
        int sSiz;
        Fix saveValue = 0.0;
        while(!lexer.eof() && i < MAXLEN && err == 0) {
                ParseToken t = evalExpression(lexer);
                if (t.tok == T_EOF) break;
                switch (t.tok) {
                case '[':
                        t = evalIntExpression(lexer);
                        if (t.tok != T_Int) {
                                parseError ("expected integer expression after '['
");
                                err = 1;
                                break;
                        }
                        numRepeats = t.intval - 1;
                        if (i + numRepeats > MAXLEN) {
                                parseError ("too many elements!");
                                err = 1;
                                break;
                        }
                        while ( numRepeats != 0) {
                                buf[i++] = saveValue;
                                numRepeats--;
                        }
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
                                buf[i++] = Fix(t.doubleval);
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
                        buf[i++] = Fix(t.doubleval);
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
                                const Fix *sPtr =
                                        ((const FixArrayState *)(t.s))->val;
                                for (int j = 0; j < sSiz; j++)
                                        buf[i++] = *sPtr++;
                        }
                        break;
                default:
                        parseError ("syntax error in FixArray state");
                        err = 1;
                }
                saveValue = buf[i-1];
        }
        if (!err) {
                nElements  = i;
                LOG_NEW; val  = new Fix [nElements];
                for(i = 0; i < nElements; i++)
                        val[i] = buf[i];
        }
        return;
}

ParseToken FixArrayState :: evalExpression(Tokenizer& lexer) 
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

StringList FixArrayState :: currentValue() const {
        StringList s;
        for(int i = 0; i<size(); i++) {
                s += val[i].value();
                s += "\n";
        }
        return s;
}

// resize the array state: append zeros if size increases
void FixArrayState :: resize (int newSize) {
	if (newSize == nElements) return;
	Fix * oldVal = val;
	int oldSize = nElements;
	// constructor initializes elements to 0.0
	LOG_NEW; val = new Fix [nElements = newSize];
	int nCopy = oldSize;
	if (newSize < nCopy) nCopy = newSize;
	for (int i = 0; i < nCopy; i++)
		val[i] = oldVal[i];
	LOG_DEL; delete [] oldVal;
}

// make knownstate entry
static FixArrayState proto;
static KnownState entry(proto,"FIXARRAY");

// isa
ISA_FUNC(FixArrayState,State);
