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

unsigned int is_idchar(char c) {
        return isalnum(c) || c == '_';
}

ParseToken State :: getParseToken(const char* string, Block* blockIAmIn) {
register	int c;
register	int namelen;
register	const char *tokstart;
	ParseToken   t;
	retry:

	tokstart = lexptr;
	c  = *tokstart;
	switch (c) {
	case 0:
		{ t.tok = 0;
		return t;
		}

	case ' ':
	case '\t':
	case '\n':
		lexptr++;
		goto retry;
	case '/':
	case '+':
	case '-':
	case '*':
	case '(':
	case ')':
		{ lexptr++;
		  t.tok = (char*)c;
		return t;
		}
	}

	if  (c >= '0'  && c<= '9') {
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

	namelen=0;
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

// The following function is an error catcher -- it is called if
// a state in the known list hasn't redefined the clone
// method.
State* State::clone() {
        extern Error errorHandler;
        StringList msg = "The state \"";
        msg += readName();
        msg += "\" doesn't implement the \"clone\" method!\n";
        errorHandler.error(msg);
        return 0;
}

// put info.
State::operator StringList() {
                StringList  out;
                out = "         State: ";
                out += readFullName();
                out += initValue;
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
        };

