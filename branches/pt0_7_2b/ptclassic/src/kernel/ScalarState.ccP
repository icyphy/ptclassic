
#include <std.h>
#include "<T>State.h"
#include "Tokenizer.h"


/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: I. Kuroda and J. T. Buck
 Date of creation: 6/15/89
 Revisions:

 Functions for class <T>State

**************************************************************************/
/*************************************************************************

	class <T>State methods

**************************************************************************/

void <T>State  :: initialize() {
	const  char* specialChars =  "*+-/()";
	Tokenizer lexer(initValue,specialChars);

	ParseToken t =evalExpression(lexer, parent()->parent());
	if(strcmp(t.tok,"ERROR"))
	if(strcmp(t.tok,"EOF"))
	{if(!strcmp(t.tok,"<T>")) val = t.<C>val;
        else if(!strcmp(t.tok,"ID")) val = ((<T>State*)t.s)->val;
	}
}

ParseToken <T>State :: evalExpression(Tokenizer& lexer, Block*  blockIAmIn) {
	
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
					t.<C>val = signflag * t.<C>val;
					return  t;
				}
        else if(!strcmp(t.tok,"ID")) {
		((<T>State*)t.s)->val = signflag * ((<T>State*)t.s)->val;
					return t;
				}
        else {t.tok = "ERROR"; return t;}
}


// make knownstate entry
static <T>State proto;
static KnownState entry(proto,"<C>");

	 


	 
