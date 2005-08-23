static const char file_id[] = "FixArrayState.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include <std.h>
#include "FixArrayState.h"
#include "Tokenizer.h"
#include "KnownState.h"
#include "PrecisionState.h"

const int MAXLEN = 20000;

/**************************************************************************
Version identification:
@(#)FixArrayState.cc	2.14   02/21/97

Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

 Programmer: A. Khazeni  
 Date of creation: 2/24/93 
 Revisions:



**************************************************************************/
/*************************************************************************

	class FixArrayState methods

**************************************************************************/

// constructor
FixArrayState :: FixArrayState ()
{
	nElements = 0; val = 0;

	overrideablePrecision = TRUE;
	symbolic_length = symbolic_intBits = NULL;
}

// alternate constructor: size
FixArrayState :: FixArrayState (int size) {
	LOG_NEW; val = new Fix [nElements = size];

	overrideablePrecision = TRUE;
	symbolic_length = symbolic_intBits = NULL;
}

// alternate constructor: size and fill value.
FixArrayState :: FixArrayState (int size, const Fix& fill_value) {
	LOG_NEW; val = new Fix [nElements = size];
	Fix * top = &(val[nElements]);
	Fix * t = val;
	while (t < top) *t++ = fill_value;

	overrideablePrecision = TRUE;
	symbolic_length = symbolic_intBits = NULL;
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

	double buf[MAXLEN];
        const char* specialChars = "*+-/()<[],!";
        Tokenizer lexer(initValue(),specialChars);

        int i = 0, err = 0;
        int numRepeats;
        int sSiz;
        double saveValue = 0.0;

	Precision prec;
	int need_precision = 0;

	// check whether value matches "(<fix-values>,<precision>)"
     {  char token[1024];
	lexer >> token;

	if (!strcmp(token, "(")) {
		SequentialList token_list = savestring(token);

		while (lexer >> token, *token) {
			token_list.prepend(savestring(token));

			if (!strcmp(token, ",")) need_precision = 1;
		}	

		ListIter titer(token_list);
		char* tp;
		int cnt = 0;

		// skip the first parenth of the initialization list
		// (value,precision) value --> value,precision) value
		// in order to discriminate from (expression) for the
		// the parser below.
		int rightParen = 0, comma = 0;
		while (cnt++, tp = (char*)titer++) {
			if (!comma || *tp != '(' || rightParen != 1)
				lexer.pushBack(tp);
			else comma = 0;
			
			if (rightParen && *tp == ',') comma = 1;
			else if (*tp == ')') rightParen++;
			else if (*tp == '(') rightParen--;

			LOG_DEL; delete [] tp;
		}
	} else
		lexer.pushBack(token);
     }

	// scan field expressions
        while (i < MAXLEN && err == 0) {
                ParseToken t = evalExpression(lexer);
                if (t.tok == T_EOF) break;
                switch (t.tok) {
                case '[':
                        t = evalIntExpression(lexer);
                        if (t.tok != T_Int) {
                                parseError (
				    "expected integer expression after '['" );
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
                                if (t.tok != ',' && t.tok != ')') {
                                        parseError ("expected ',' or ')'");
                                        err = 1;
                                }
				else if (t.tok == ',' ) {
					lexer.pushBack(",");
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
                                const Fix *sPtr =
                                        ((const FixArrayState *)(t.s))->val;
                                for (int j = 0; j < sSiz; j++)
                                        buf[i++] = double(*sPtr++);
                        }
                        break;
		case ',':
			// scan precision string in "(<values...>,<precision>)"
			if (need_precision) {
				prec = PrecisionState::parsePrecisionString(
					      lexer, name(), parent());

				if (!prec.isValid()) {
					// parser error; is already reported in
					// PrecisionState::parsePrecisionString
					err = 1;
				      break;
				}
/* The following code breaks on the alpha - gcc-2.6.3 */
#ifdef NEVER_THIS_IS_AN_ALPHA_BUG	
		   else if ((getParseToken(lexer).tok == ')') &&
			    (getParseToken(lexer).tok == T_EOF))
					break;
#else
				else {
				  // just to skip ')'	
				  t = getParseToken(lexer);
				  break;
				}
#endif
			}
                default:
                        parseError ("syntax error in FixArray state");
                        err = 1;
                }
		if(i > 0) saveValue = buf[i-1];
        }
        if (!err) {
                nElements  = i;
                LOG_NEW; val  = new Fix [nElements];
                for(i = 0; i < nElements; i++)
			val[i] = prec.isValid() ? Fix(prec,buf[i]) : Fix(buf[i]);
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

// Explicitly set the precision;
// if overrideable is FALSE the symbolic expressions may not be redefined
// in the future.
void FixArrayState::setPrecision(const Precision& p, int overrideable)
{
	// change the precision of all fix values
	Fix f(p.len(),p.intb());

	for (int i=size(); i--; ) {
	    f = val[i];
	    val[i].initialize();
	    val[i] = f;
	}

	// set the symbolic expressions for the precision
	if (overrideablePrecision) {
#if 0
	    if (symbolic_length) {
		LOG_DEL; delete [] symbolic_length;
	    }
	    if (symbolic_intBits) {
		LOG_DEL; delete [] symbolic_intBits;
	    }
#endif
	    symbolic_length  = p.symbolic_len()  ?
	      savestring(p.symbolic_len())  : (char*)NULL;
	    symbolic_intBits = p.symbolic_intb() ?
	      savestring(p.symbolic_intb()) : (char*)NULL;

	    overrideablePrecision = overrideable;
	}
}

Precision FixArrayState::precision() const
{
	return Precision(val[0].len(),val[0].intb(), symbolic_length,symbolic_intBits);
}

// make knownstate entry
static FixArrayState proto;
static KnownState entry(proto,"FIXARRAY");

// isa
ISA_FUNC(FixArrayState,State);
