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

const int TOKSIZE = 80;

/*************************************************************************

	class State methods

**************************************************************************/

// small virtual functions in baseclass
char* State :: type () { return "STRING";}

int State :: size () { return 1;}

StringList State :: currentValue () { return initValue;}

// See if character is part of an identifier
inline unsigned int is_idchar(char c) {
        return isalnum(c) || c == '_';
}

// The state tokenizer without type specification
ParseToken
State :: getParseToken(Tokenizer& lexer, Block* blockIAmIn) {
	return getParseToken(lexer, blockIAmIn, "NONE");	};

// The state tokenizer: return next token
ParseToken
State :: getParseToken(Tokenizer& lexer, Block* blockIAmIn, char* stateType) {
        char token[TOKSIZE];
	ParseToken t;

	lexer >> token;
        if (*token == '<') {
                char filename[TOKSIZE];
                lexer >> filename;
                if (!lexer.fromFile(filename))
                        { t.tok = "ERROR";  return t;}
                else lexer >> token;
        }

        if (*token == 0) {
                t.tok = "EOF";
                return t;
        }

	if(*token  == ',') {
		t.tok = "SEPARATOR";
		t.cval = (char)*token;
		return t;
	}

	if(*token == '[' || *token == ']') {
		t.tok = "REPEATER";
		t.cval = (char)*token;
		return t;
	}

        if (*token == '+' || *token == '-' || *token == '*' ||  *token == '/'
	|| *token == '(' || *token == ')') {
                t.tok = "OP";
                t.cval = (char)*token;
		return t;
	}

        if (isdigit(*token) || *token == '.' )  {
		if (!strcmp(stateType,"FLOAT")) {
                        t.tok = "FLOAT";
                        t.doubleval = atof(token);
			return t;
                }
                else {
                        t.tok = "INT";
                        t.intval = atoi(token);
			return t;
                }
        }

        if (is_idchar(*token)) {
                State* s = lookup(token,blockIAmIn);
                // better, maybe, to return type and value of the State
		if(s){
                t.tok =  "ID"; 
		t.s =  s ;
                return t;
        	}
		else if(!strcmp(this->type(),"STRING")){
		t.tok = "STRING";
		t.sval = savestring(token);
		return t;
		}
		else{
		t.tok = "NULL";
		t.s = 0;
		return t;
		} 
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

