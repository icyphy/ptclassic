static const char file_id[] = "FixState.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "FixState.h"
#include "PrecisionState.h"
#include "KnownState.h"
#include "Tokenizer.h"


/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

 Programmer: A. Khazeni, J. Buck 
 Date of creation: 2/24/93 
 Revisions:

 Functions for class FixState

**************************************************************************/
/*************************************************************************

	class FixState methods

**************************************************************************/

// the type
const char* FixState :: type() const { return "FIX";}

// constructor
FixState :: FixState()
{
	val = 0.0;
	overrideablePrecision = TRUE;
	symbolic_length = symbolic_intBits = NULL;
}

// the value as a string
StringList FixState :: currentValue() const {
        StringList s = val.value();
        return s;
}

void FixState :: setInitValue(const Fix& arg) {
	StringList sl = arg.value();
	val = arg;
	State::setInitValue(hashstring(sl));
}

// clone
State* FixState :: clone () const {
	LOG_NEW; FixState *s = new FixState;
	s->val = val;
	return s;
}

void FixState :: initialize() {
        const  char* specialChars =  "*+-/(),";
        Tokenizer lexer(initValue(),specialChars);

        ParseToken t =getParseToken(lexer);

        // possibility one: a galaxy FixState name

        if (t.tok == T_ID) {
                if (!t.s->isA("FixState")) {
                        parseError ("invalid state type: ", t.s->fullName());
                        return;
                }
                const FixState& fstate = *(const FixState*)t.s;
                val = fstate.val;
        }
        // possibility two:
        // if it does not begin with '(', assume default precision. 
        // this may be any valid floating expression.
        else if (t.tok != '(') {
		setPushback(t);
                t = evalFloatExpression(lexer);
                if (t.tok == T_Float) val = t.doubleval;
                // if wrong type, evalFloatExpression has already complained
                else return;
        }

        // possibility three: ( exp , exp ) or ( value , precision )
        // where each exp is a floating expression.
        else {
                t =  evalFloatExpression(lexer);
                if (t.tok != T_Float) return;
                double fixval = t.doubleval;
                t = getParseToken(lexer);
                if (t.tok != ',') {
                        parseError ("expected a comma");
                        return;
                }
		const Precision prec =
			PrecisionState::parsePrecisionString(
				lexer, name(), parent());
		if (!prec.isValid())
			return;	// parser error already reported in parsePrec.String

		val = Fix(prec, fixval);
		// might happen if requested length of precision exceeds implementation limit
		if (val.invalid()) {
			parseError ("invalid Fix precision specifier");
			return;
		}

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

// Explicitly set the precision;
// if overrideable is FALSE the symbolic expressions may not be redefined
// in the future.
void FixState::setPrecision(const Precision& p, int overrideable)
{
	// change the precision of the fix value
	Fix f(p.len(),p.intb(), val);
	val.initialize();
	val = f;

	// set the symbolic expressions for the precision
	if (overrideablePrecision) {
	    if (symbolic_length) {
		LOG_DEL; delete [] symbolic_length;
	    }
	    if (symbolic_intBits) {
		LOG_DEL; delete [] symbolic_intBits;
	    }
	    symbolic_length  = p.symbolic_len()  ? savestring(p.symbolic_len())  : (char*) NULL;
	    symbolic_intBits = p.symbolic_intb() ? savestring(p.symbolic_intb()) : (char*) NULL;

	    overrideablePrecision = overrideable;
	}
}

Precision FixState::precision() const
{
	return Precision(val.len(),val.intb(), symbolic_length,symbolic_intBits);
}


// make knownstate entry
static FixState proto;
static KnownState entry(proto,"FIX");

// isa
ISA_FUNC(FixState,State);
