static const char file_id[] = "PrecisionState.cc";
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

 Programmer: J.Weiss
 Date of creation: 10/24/94 
 Revisions:

 Functions for class PrecisionState

**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include <stream.h>
#include "PrecisionState.h"
#include "KnownState.h"
#include "Tokenizer.h"
#include <ctype.h>
#include <stdlib.h>		// strtol()

inline unsigned char max(unsigned char a, unsigned char b) {return (a>b)?a:b;}

#define TOKSIZE 256

/*************************************************************************
	class Precision methods
**************************************************************************/

// constructors
Precision::Precision()
{
	length = intBits = 0;
	symbolic_length = symbolic_intBits = NULL;
}

Precision::Precision(const Fix& f,
	const char* symbolic_len, const char* symbolic_intb)
{
	length = f.len(),  intBits = f.intb();

	symbolic_length  = symbolic_len  ? savestring(symbolic_len)  : NULL;
	symbolic_intBits = symbolic_intb ? savestring(symbolic_intb) : NULL;
}

Precision::Precision(int len, int intb,
	const char* symbolic_len, const char* symbolic_intb)
{
	length = len,      intBits = intb;

	symbolic_length  = symbolic_len  ? savestring(symbolic_len)  : NULL;
	symbolic_intBits = symbolic_intb ? savestring(symbolic_intb) : NULL;
}

Precision::Precision(const Precision& p)
{
	length = p.len(),  intBits = p.intb();

	symbolic_length  = p.symbolic_len()  ? savestring(p.symbolic_len())  : NULL;
	symbolic_intBits = p.symbolic_intb() ? savestring(p.symbolic_intb()) : NULL;
}

// destructor
Precision::~Precision()
{
	if (symbolic_length)  { LOG_DEL; delete [] symbolic_length;  }
	if (symbolic_intBits) { LOG_DEL; delete [] symbolic_intBits; }
}

// arithmetic operators
Precision Precision::operator + (const Precision& p) const
{
	if (!isValid())
		return p;
   else if (!p.isValid())
		return *this;

	int intB = max(intBits,p.intBits);
	int leng = intB + max(length-intBits,p.length-p.intBits);

	StringList sym_len, sym_intB;

	// concatenate symbolic expressions
	if (symbolic_len()  || p.symbolic_len() ||
	    symbolic_intb() || p.symbolic_intb()) {

	    StringList slen,sintb, pslen,psintb;

	    // use symbolic expressions if defined, otherwise convert integer
	    // precisions into strings
	    if (symbolic_len())    slen   << symbolic_len();    else slen   << len();
	    if (symbolic_intb())   sintb  << symbolic_intb();   else sintb  << intb();

	    if (p.symbolic_len())  pslen  << p.symbolic_len();  else pslen  << p.len();
	    if (p.symbolic_intb()) psintb << p.symbolic_intb(); else psintb << p.intb();

	    // concatenate using a MAX() function call and a - operator
	
	    sym_len  << "MAX(" << slen << ',' << pslen << ')';

	    sym_intB << '(' << sym_len << '+'
		     << "MAX(" << slen  << '-' << sintb << ','
			       << pslen << '-' << psintb
		     << "))";
	}

	return Precision(leng,intB, sym_len,sym_intB);
}

Precision Precision::operator * (const Precision& p) const
{
	if (!isValid())
		return p;
   else if (!p.isValid())
		return *this;

	int intB = length + p.length -1;
	int leng = intBits+ p.intBits-1;

	StringList sym_len, sym_intB;

	// concatenate symbolic expressions
	if (symbolic_len()  || p.symbolic_len() ||
	    symbolic_intb() || p.symbolic_intb()) {

	    StringList slen,sintb, pslen,psintb;

	    // use symbolic expressions if defined, otherwise convert integer
	    // precision into strings

	    if (symbolic_len())    slen   << symbolic_len();    else slen   << len();
	    if (symbolic_intb())   sintb  << symbolic_intb();   else sintb  << intb();

	    if (p.symbolic_len())  pslen  << p.symbolic_len();  else pslen  << p.len();
	    if (p.symbolic_intb()) psintb << p.symbolic_intb(); else psintb << p.intb();

	    // concatenate using + and - operators
	
	    sym_len  << '(' << slen  << '+' << pslen  << "-1)";
	    sym_intB << '(' << sintb << '+' << psintb << "-1)";
	}

	return Precision(leng,intB, sym_len,sym_intB);
}

Precision  Precision::operator -  (const Precision& p) const { return *this + p; }
Precision& Precision::operator += (const Precision& p) { return *this = *this + p; }
Precision& Precision::operator -= (const Precision& p) { return *this = *this - p; }
Precision& Precision::operator *= (const Precision& p) { return *this = *this * p; }

// the division operator currently returns an arbitrary precision
// maybe it should return a maximum word length, but what about the integer part ??
Precision  Precision::operator /  (const Precision& p) const { return *this * p; }
Precision& Precision::operator /= (const Precision& p) { return *this = *this / p; }

// comparison
int Precision::operator == (const Precision& p) const
{
	if (symbolic_len() && p.symbolic_len())
		if (!!strcmp(symbolic_len(),  p.symbolic_len()))
			return FALSE;
	if (symbolic_intb() && p.symbolic_intb())
		if (!!strcmp(symbolic_intb(), p.symbolic_intb()))
			return FALSE;

	return len() == p.len() && intb() == p.intb();

}

// assignment
Precision& Precision::operator = (const Precision& p)
{
	// sanity check
	if (this == &p)  return *this;

	length = p.len(),  intBits = p.intb();

	if (symbolic_length)  { LOG_DEL; delete [] symbolic_length;  }
	symbolic_length  = p.symbolic_len()  ? savestring(p.symbolic_len())  : NULL;
	if (symbolic_intBits) { LOG_DEL; delete [] symbolic_intBits; }
	symbolic_intBits = p.symbolic_intb() ? savestring(p.symbolic_intb()) : NULL;
	return *this;
}

// macro for << operators; put symbolic expression rather than integer constant
#define PUT_SYMBOL(s, symbolic_expr,integer_expr) \
	(symbolic_expr ? (s << symbolic_expr) : (s << integer_expr))

// convert precision into string of form intb.fracb
Precision::operator StringList () const
{
	StringList l;

	PUT_SYMBOL(l, symbolic_intb(),intb());

	l << '.';

	if (symbolic_len() && symbolic_intb()) {
		PUT_SYMBOL(l, symbolic_len(), len());
		l << '-';
		PUT_SYMBOL(l, symbolic_intb(),intb());
	} else
		l << fracb();

	return (const char*)l;
}

// print precision on stream with format "intb.fracb" (including quotes)
ostream& operator << (ostream& s, const Precision& p)
{
	return s << '"' << StringList(p) << '"';
}

// append precision to StringList in form len,intb
StringList& operator << (StringList& l, const Precision& p)
{
	PUT_SYMBOL(l, p.symbolic_len(), p.len());
	l << ',';
	PUT_SYMBOL(l, p.symbolic_intb(),p.intb());
	return l;
}

#undef PUT_SYMBOL

/*************************************************************************
	class PrecisionState methods
**************************************************************************/

// setting and deleting the value string
PrecisionState :: ~PrecisionState()
{
	if (val) { LOG_DEL; delete [] val; }
}

// initialize the state from a character string
PrecisionState& PrecisionState :: operator = (const char* arg)
{
	if (val) { LOG_DEL; delete [] val; val = NULL; }

	// leave the precision undefined if the initialization string is empty
	if (arg == NULL || *arg == '\0') {
		(Precision&)*this = Precision();
		return *this;
	}

	Tokenizer lexer(arg, "");

	//(Precision&)*this = parsePrecisionString(lexer);
	//Precision foo = Precision(6,3,NULL,NULL);
	Precision foo = parsePrecisionString(lexer);
	(Precision&)*this = foo;

	//*this = foo;
	//*this = Precision( foo.len(), foo.intb(), foo.symbolic_len(), foo.symbolic_intb());

	if (!this->isValid())
		;  // parse error already reported in parser routine
	else {
		// check for extra cruft (this also eats up any pushback token)
	  	if (getParseToken(lexer).tok != T_EOF) {
			parseError ("extra text after valid expression");
			return *this;
		}

		StringList s((Precision&)*this);
		val = savestring(s);
	}
	return *this;
}

// copy from another PrecisionState
PrecisionState& PrecisionState :: operator = (const PrecisionState& p)
{
	(Precision&)*this = p;
	val = p.val ? savestring(p.val) : NULL;
	return *this;
}

#ifdef NEVER
// copy from a Precision
PrecisionState& PrecisionState :: operator = (const Precision& p)
{
  StringList sl(p);
  val = savestring(sl);
  return *this;
}
#endif
// the type
const char* PrecisionState :: type() const { return "PRECISION";}

// Return the current value
StringList PrecisionState :: currentValue() const {
	StringList res;
	if (val) res = val;
	return res;
}

// clone
State* PrecisionState :: clone () const
{
	LOG_NEW; PrecisionState *s = new PrecisionState;
	if (val)  s->val = savestring(val);
	(Precision&)*s = *this;
	return s;
}

// parse initial value
void PrecisionState :: initialize()
{
	// use overloaded operator = (const char*)
	this->operator = (initValue());
}

// static function that can be invoked from outside
// (used by class FixState and class FixArrayState)

Precision PrecisionState :: parsePrecisionString(Tokenizer& lexer, const char* nm, Block* parent)
{
	PrecisionState prec;
	prec.setNameParent(nm, parent);

	return prec.parsePrecisionString(lexer);
}

// non static function;
// parses the given string for expressions of the form "intBits.fracBits"
// or "fracBits/length",

Precision PrecisionState :: parsePrecisionString(Tokenizer& lexer)
{
        char token[TOKSIZE];
        const char* specialChars =  "*+-/(),.:";

	// replace set of special characters for tokenizer
	// make sure that the tokenizer is reset when leaving this method
	struct TokenizerControl {
	   TokenizerControl(Tokenizer& l, const char* specials) {
		specs = (lexer=&l)->setSpecial(specials);
	   }
	   ~TokenizerControl() {
		lexer->setSpecial(specs);
	   }
	   const char* specs;
	   Tokenizer* lexer;
	} c (lexer, specialChars);


	// replace the '/' in expressions of the form "fracBits/length"
	// by ':' in order to prevent the slash from being interpreted
	// as the division operator

	SequentialList token_list;

	// first check whether a "." is in the token list
	int cnt=0;
	while (lexer >> token, *token) {
		token_list.prepend(savestring(token));
		if (!strcmp(token, "."))  break;
	   else if (!strcmp(token, "/"))  cnt++;
	}

	ListIter titer(token_list);
	char* tp;

	if (!*token && (cnt == 1)) {

		// looks like a "n/m" expression,
		// replace slash by colon
		while ( (tp = (char*)titer++) ) {
			lexer.pushBack(!!strcmp("/", tp) ? tp : ":");
			LOG_DEL; delete [] tp;
		}
	} else
		while ( (tp = (char*)titer++) ) {
			lexer.pushBack(tp);
			LOG_DEL; delete [] tp;
		}


        // parse format "intBits.fracBits" or "fracBits/length"
        // where each expression may be an integer expression.

	// parse next token; allow precision state here
        ParseToken t = getParseToken(lexer, T_Precision);
	if (t.tok != T_Int) return Precision();
	
	ParseToken t1 = getParseToken(lexer);
	if ((t1.tok != '.') && (t1.tok != ':')) {
		parseError ("expected . or /");
		return Precision();
	}

	int flag = (t1.tok == '.');

	ParseToken t2 = evalIntExpression(lexer);
	if (t2.tok != T_Int) return Precision();

	int length, intBits;

	if (flag)
		length  = t.intval + t2.intval,
		intBits = t.intval;
	else
		length  = t2.intval,
		intBits = t2.intval - t.intval;

	return Precision(length,intBits);
}

// redefine the state tokenizer
// Most of this method is an exact copy of State::getParseToken, except 
// that "." and ":" are now special characters. (See parsePrecisionString
// for the meaning of the colon).
// Furthermore only integer and (for compatibility with older versions)
// string parameters are recognized.  This method can be invoked with
// stateType = T_Precision, which forces references to precision parameters
// to be parsed as well.

ParseToken
PrecisionState :: getParseToken(Tokenizer& lexer, int stateType) {
        char token[TOKSIZE];
// allow for one pushback token.
	ParseToken t = pushback();
	if (t.tok) {
		clearPushback();
		return t;
	}
	lexer >> token;
        if (*token == '<') {
                char filename[TOKSIZE];
// temporarily disable special characters, so '/' (for instance)
// does not screw us up.
		const char* tc = lexer.setSpecial ("");
                lexer >> filename;
// put special characters back.
		lexer.setSpecial (tc);
                if (!lexer.fromFile(filename)) {
			StringList msg;
			msg << filename << ": " << why();
			parseError ("can't open file ", msg);
			t.tok = T_ERROR;
			return t;
		}
                else lexer >> token;
        }
	
        if (*token == 0) {
                t.tok = T_EOF;
                return t;
        }
	
// handle all special characters (note that dot and colon are included in the list)
	if(strchr (".:,[]+*-/()^", *token)) {
		t.tok = t.cval = *token;
		return t;
	}

	if (isdigit(*token))  {
		t.tok = T_Int;
		t.intval = (int)strtol(token,0,0);
		return t;
	}

	// parse identifier

	// for compatibility with older versions where string states where used
	// for precisions, process string states that are enclosed in '{'/'}'
	if (*token == '{') {
		if (token[strlen(token)-1] != '}') {
			parseError ("missing '}' in string state referencation");
			t.tok = T_ERROR;
			return t;
		}
		// discard '{}'
		token[strlen(token)-1] = '\0';

		const State* s = lookup(token+1,parent()->parent());
		if (!s) {
			parseError ("undefined symbol: ", token+1);
			t.tok = T_ERROR;
			return t;
		}
		if (!strcmp(s->type(), "STRING")) {
			StringList value = s->currentValue();
			lexer.pushBack(value);
			// recursive call; reject precision symbols
			return getParseToken(lexer, T_Int);
		} else {
			StringList msg;
			msg << "bad referencation of state " << token+1
			    << "; use '{}' to reference string states only";
			parseError (msg);
			t.tok = T_ERROR;
			return t;
		}
	}
	
   else if (isalnum(*token) || (*token == '_')) {
		const State* s = lookup(token,parent()->parent());
		if (!s) {
			parseError ("undefined symbol: ", token);
			t.tok = T_ERROR;
			return t;
		}
		// only int/string states and eventually precision states are allowed here
		if (!strcmp(s->type(), "INT") ||
		    !strcmp(s->type(), "STRING") ||
		   ((stateType == T_Precision) && !strcmp(s->type(), "PRECISION"))) {

			StringList value = s->currentValue();
			lexer.pushBack(value);
			// recursive call; reject precision symbols
			return getParseToken(lexer, T_Int);
		} else {
			StringList msg;
			msg << "bad symbol type " << s->type() << ", expected "
			    << ((stateType == T_Precision) ? "integer or precision" : "integer");
			parseError (msg);
			t.tok = T_ERROR;
			return t;
		}
	}

	// if we get here, next token is bogus
	parseError ("unexpected token: ", token);
	t.tok = T_ERROR;
	return t;
}


// make knownstate entry
static PrecisionState proto;
static KnownState entry(proto,"PRECISION");

// isa
ISA_FUNC(PrecisionState,State);
