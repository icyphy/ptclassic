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
#include "State.h"
#include "Block.h"

/*************************************************************************

	class State methods

**************************************************************************/

// See if character is part of an identifier
inline unsigned int is_idchar(char c) {
        return isalnum(c) || c == '_';
}

// The state tokenizer: return next token
ParseToken
State :: getParseToken(const char* string, Block* blockIAmIn) {
	int c;
	int namelen;
	const char *tokstart;
	ParseToken   t;
 retry:
	
	tokstart = lexptr;
	c  = *tokstart;
	switch (c) {
	case 0:			// End of string
		t.tok = "NULL";
		t.cval = 0;
		return t;

	case ' ':		// White-space, skip it
	case '\t':
	case '\n':
		lexptr++;
		goto retry;

	case '/':		// one-character token
	case '+':
	case '-':
	case '*':
	case '(':
	case ')':
		lexptr++;
		t.tok = "OP";
		t.cval = c;
		return t;

	}

	if  (c >= '0'  && c<= '9') { // digit
		int floatflag = 0;
		namelen = 0;
		c = tokstart[namelen];
		while(is_idchar(c) || c == '.')
		{ 
			if(c == '.') floatflag =  1;
			namelen++;
			c = tokstart[namelen];
		};	

		if(floatflag)
		{ 	t.dval = atof(tokstart);
			t.tok = "FLOAT"; }
		else
		{ 	t.ival = atoi(tokstart);
			t.tok = "INT"; }
		lexptr +=  namelen;
		return t;
	}

	namelen=0;		// otherwise: assume ID
	c = tokstart[namelen];
	while(is_idchar(c))
	{
		namelen++;
		c = tokstart[namelen];
	};


	char yytext[namelen+1];
	for(int i = 0 ; i < namelen + 1 ; i++) {
		yytext[i] = tokstart[i];
	};

	State* s = lookup(yytext, blockIAmIn);
	if(s == 0) 
	{
	t.tok = "NULL";
	t.s = 0;	
	return t;
	};

	t.tok  = "ID";
	t.s  = s;

	lexptr  += namelen;
	return t;
};

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
	out += ", initial value: ";
	out += initValue;
	out += ", current value: ";
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

