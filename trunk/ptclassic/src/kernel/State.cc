static const char file_id[] = "State.cc";
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  I. Kuroda and J. T. Buck
 Date of creation: 5/26/90
 Revisions:

 Code of functions declared in State.h

**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include <std.h>
#include <ctype.h>
#include "Tokenizer.h"
#include "State.h"
#include "Block.h"
#include "miscFuncs.h"
#include "Error.h"
#include "KnownState.h"

#define TOKSIZE 256

/*************************************************************************

	class State methods

**************************************************************************/

// small virtual functions in baseclass
const char* State :: type () const { return "STRING";}

State :: ~State() {}

const char* State :: readClassName() const {return "State";}

int State :: size () const { return 1;}

StringList State :: currentValue () const { return initValue;}

// See if character is part of an identifier
inline unsigned int is_idchar(char c) {
        return isalnum(c) || c == '_';
}

// setState -- two versions
State& State :: setState(const char* stateName, 
			 Block* parent ,
			 const char* ivalue,
			 const char* desc) {
	descriptor = desc;
	setNameParent(stateName, parent);
	initValue = ivalue;
	return *this;
}


State& State :: setState(const char* stateName, 
			 Block* parent ,
			 const char* ivalue,
			 const char* desc,
			 Attribute attr) {
	attributeBits = attr.eval(AB_DEFAULT);
	return setState(stateName,parent,ivalue,desc);
}

// Attributes
// standard attributes.
extern const Attribute A_CONSTANT = {AB_CONST,0};
extern const Attribute A_SETTABLE = {AB_SETTABLE,0};
extern const Attribute A_NONCONSTANT = {0,AB_CONST};
extern const Attribute A_NONSETTABLE = {0,AB_SETTABLE};
extern const Attribute A_DYNAMIC = {AB_DYNAMIC,0};

ParseToken State :: pushback;

// The state tokenizer: return next token when parsing a state
// initializer string.  Handles references to files and other states.
ParseToken
State :: getParseToken(Tokenizer& lexer, int stateType) {
        char token[TOKSIZE];
	ParseToken t;

// allow for one pushback token.
	if (pushback.tok) {
		t = pushback;
		pushback.tok = 0;
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
			parseError ("can't open file ", filename);
			t.tok = T_ERROR;
			return t;
		}
                else lexer >> token;
        }
	
        if (*token == 0) {
                t.tok = T_EOF;
                return t;
        }
	
// handle all special characters
	if(strchr (",[]+*-/()^", *token)) {
		t.tok = t.cval = *token;
		return t;
	}

// we stay in this while loop only if a scalar state value is substituted
// for, otherwise we return immediately.
	while (1) {
// note: we can get a minus if 2nd time in loop and the value of the
// substituted state was negative.
		if (isdigit(*token) || *token == '.' || *token == '-' )  {
// possible scientific notation if ending in e or E.  Elim + and - as
// special if so, and append to token.  Ugly hack.
			int l = strlen(token);
			if (token[l-1] == 'e' || token[l-1] == 'E') {
				const char* tc = lexer.setSpecial("*()/[],^");
				lexer >> token + l;
				lexer.setSpecial(tc);
			}
			if (stateType == T_Float) {
				t.tok = T_Float;
				t.doubleval = atof(token);
				return t;
			}
			else {
				t.tok = T_Int;
				t.intval = strtol(token,0,0);
				return t;
			}
		}
		if (is_idchar(*token)) {
			const State* s = lookup(token,parent()->parent());
			if (!s) {
				parseError ("undefined symbol", token);
				t.tok = T_ERROR;
				return t;
			}
			if (strcmp(s->type(), "COMPLEX") == 0 ||
			    s->size() != 1) {
				t.tok = T_ID;
				t.s = s;
				return t;
			}
			// for int, float states we substitute their values
			// here and reparse (stay in while loop)
			else if (s->size() == 1) {
				StringList value = s->currentValue();
				strncpy (token, value, TOKSIZE-1);
				// (re-execute the loop)
			}
		}
		else {
			// if we get here, next token is bogus
			parseError ("unexpected token", token);
			t.tok = T_ERROR;
			return t;
		}
	}
}

const State* State :: lookup (const char* name, Block* blockIAmIn) {
        while (blockIAmIn) {
		if (blockIAmIn->isItWormhole())
			blockIAmIn = blockIAmIn->parent();
                State* p = blockIAmIn->stateWithName(name);
                if (p) return p;
                blockIAmIn = blockIAmIn->parent();
        }
	// not found, consult global symbol list
        return KnownState::lookup(name);
}

// put info.
StringList
State::printVerbose() const {
	StringList  out;
	out = readFullName();
	out += " type: ";
	out += type();
	out += "\n initial value: ";
	out += initValue;
	out += "\n current value: ";
	out += currentValue();
	out += "\n";
        return out;
};

// change current value.  We use the initialization parser to do the job.
void
State::setCurrentValue(const char* newval) {
	const char* save = initValue;
	initValue = newval;
	initialize();
	initValue = save;
}

// methods for class StateList

// initialize elements
void StateList::initElements() {
	State *p;
	StateListIter next(*this);
	for(int i=size(); i>0; i--)  {
		p  = next++;
		p->initialize();
	};
};

// Find a state with the given name and return pointer
State* StateList::stateWithName(const char* name) {
	State *sp;
	StateListIter next(*this);
	for (int i =  size(); i>0;i--) {
		sp = next++;
		if (strcmp(name,sp->readName()) == 0)
			return  sp;
	}
	return NULL;
}

// complain of parse error
void State::parseError (const char* text, const char* text2) {
	Error::abortRun (*this, "Parameter initialization error: ", text,
			 text2);
}

// isa
ISA_FUNC(State,NamedObj);

// power function for use in int state expressions
int power(int base,int exp) {
	int r = 1;
	while (exp > 0) { r *= base; exp--;}
	return r;
}
