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

#include <std.h>
#include <ctype.h>
#include <string.h>
#include "Tokenizer.h"
#include "State.h"
#include "Block.h"
#include "miscFuncs.h"
#include "Output.h"

extern Error errorHandler;

const int TOKSIZE = 256;

/*************************************************************************

	class State methods

**************************************************************************/

// small virtual functions in baseclass
const char* State :: type () { return "STRING";}

int State :: size () { return 1;}

StringList State :: currentValue () { return initValue;}

// See if character is part of an identifier
inline unsigned int is_idchar(char c) {
        return isalnum(c) || c == '_';
}

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
	if(strchr (",[]+*-/()", *token)) {
		t.tok = t.cval = *token;
		return t;
	}

reparse:	
        if (isdigit(*token) || *token == '.' )  {
// possible scientific notation if ending in e or E.  Elim + and - as
// special if so, and append to token.  Ugly hack.
		int l = strlen(token);
		if (token[l-1] == 'e' || token[l-1] == 'E') {
			const char* tc = lexer.setSpecial("*()/[],");
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
                        t.intval = atoi(token);
			return t;
		}
	}
	if (is_idchar(*token)) {
                State* s = lookup(token,parent()->parent());
		if (!s) {
			parseError ("undefined symbol", token);
			t.tok = T_ERROR;
			return t;
		}
// hack -- right now we only return a state pointer for Complex
		if (strcmp(s->type(), "COMPLEX") == 0) {
			t.tok = T_ID;
			t.s = s;
			return t;
		}
		else if (s->size() == 1) {
			StringList value = s->currentValue();
			strncpy (token, value, TOKSIZE-1);
			goto reparse;
		}
		else {
			parseError ("can't use aggregate states", token);
			t.tok =  T_ERROR; 
			return t;
        	}
	}
	else {
		parseError ("unexpected token", token);
		t.tok = T_ERROR;
		return t;
	}
}

State* State :: lookup (char* name, Block* blockIAmIn) {
        while (blockIAmIn) {
                State* p = blockIAmIn->stateWithName(name);
                if (p) return p;
                blockIAmIn = blockIAmIn->parent();
        }
        return NULL;
}

// put info.
StringList
State::printVerbose() {
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

// initialize elements
void StateList::initElements() {
	State *p;
	for(int i=size(); i>0; i--)  {
		p  = (State  *)next();
		p->initialize();
	};
};

// Find a state with the given name and return pointer
State* StateList::stateWithName(const char* name) {
	State *sp;
	for (int i =  size(); i>0;i--) {
		sp = (State *) next();
		if (strcmp(name,sp->readName()) == 0)
			return  sp;
	}
	return  NULL;
}

// complain of parse error
void State::parseError (const char* text, const char* text2) {
	StringList msg = "Error initializing ";
	msg += readFullName();
	msg += ": ";
	msg += text;
	msg += " ";
	msg += text2;
	Error::abortRun (msg);
}

