#include <std.h>
#include "<T>State.h"
#include "<T>ArrayState.h"
#include "Tokenizer.h"

const int MAXLEN = 2000;

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  I. Kuroda and J. T. Buck
 Date of creation: 6/2/90
 Revisions:



**************************************************************************/
/*************************************************************************

	class <T>ArrayState methods

**************************************************************************/

void <T>ArrayState  :: initialize() {

	<C> buf[MAXLEN];
	char* specialChars = "*+-/()<";
	Tokenizer lexer(initValue,specialChars);

	int i = 0;
	while(!lexer.eof() && i < MAXLEN) {
	ParseToken t =evalExpression(lexer, parent()->parent());
	if(strcmp(t.tok,"ERROR"))
	if(strcmp(t.tok,"EOF"))
	{if(!strcmp(t.tok,"<T>")) buf[i++] = t.dval;
        else if(!strcmp(t.tok,"ID")) buf[i++] = ((<T>State*)t.s)->val;
	}
			};
	nElements  = i;

	val  = new <C> [nElements];
	for(i = 0; i < nElements; i++)
	val[i] = buf[i];
}

ParseToken <T>ArrayState :: evalExpression(Tokenizer& lexer, Block*  blockIAmIn) {
	
	<C> signflag = 1;
        ParseToken t = getParseToken(lexer, blockIAmIn);

	if(!strcmp(t.tok,"EOF")) return t;
	if(!strcmp(t.tok,"OP"))
	{
	if(t.cval == '-')
		{signflag = -1;
		t = getParseToken(lexer, blockIAmIn);
		if(!strcmp(t.tok,"NULL")) {t.tok = "ERROR"; return t;}	
		}
        }
	if(!strcmp(t.tok,"<T>")) {
					t.dval = signflag * t.dval;
					return  t;
				}
        else if(!strcmp(t.tok,"ID")) {
		((<T>State*)t.s)->val = signflag * ((<T>State*)t.s)->val;
					return t;
				}
        else {t.tok = "ERROR"; return t;}
}

// make knownstate entry
static <T>ArrayState proto;
static KnownState entry(proto,"<T>Array");

	 


	 
