
#include <std.h>
#include "ComplexState.h"
#include "FloatState.h"
#include "Tokenizer.h"


/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: I. Kuroda and J. T. Buck
 Date of creation: 6/15/89
 Revisions:

 Functions for class ComplexState

**************************************************************************/
/*************************************************************************

	class ComplexState methods

**************************************************************************/

void ComplexState  :: initialize() {
	const  char* specialChars =  "*+-/(),";
	Tokenizer lexer(initValue,specialChars);

	double realval;
	double imagval;
	ParseToken t =getParseToken(lexer, parent()->parent());

        if(!strcmp(t.tok,"ID")) {
			val = ((ComplexState*)t.s)->val;
			return;
			}

	if(strcmp(t.tok,"OP")) return;
	if(t.cval != '(') return;
	t =  evalExpression(lexer, parent()->parent());
	if(!strcmp(t.tok,"ERROR")) return;
	if(!strcmp(t.tok,"EOF")) return;
	{if(!strcmp(t.tok,"FLOAT")) realval = t.doubleval;
        else if(!strcmp(t.tok,"ID")) val = *(double*)((FloatState*)t.s);
	}
	t = getParseToken(lexer,  parent()->parent());
	if(strcmp(t.tok,"SEPARATOR")) return;
	if(t.cval != ',') return;
	t =  evalExpression(lexer, parent()->parent());
	if(!strcmp(t.tok,"ERROR")) return;
	if(!strcmp(t.tok,"EOF")) return;
	{if(!strcmp(t.tok,"FLOAT")) imagval = t.doubleval;
        else if(!strcmp(t.tok,"ID")) val = *(double*)((FloatState*)t.s);
	}
	t =getParseToken(lexer, parent()->parent());
	if(strcmp(t.tok,"OP")) return;
	if(t.cval != ')') return;
	val = Complex(realval,imagval);
	return;
}

ParseToken ComplexState :: evalExpression(Tokenizer& lexer, Block*  blockIAmIn) {
	
	double signflag = 1;
        ParseToken t = getParseToken(lexer, blockIAmIn, "FLOAT");

	if(!strcmp(t.tok,"EOF")) return t;
	if(!strcmp(t.tok,"OP"))
	{
	if(t.cval == '-')
		{signflag = -1;
		t = getParseToken(lexer, blockIAmIn, "FLOAT");
		if(!strcmp(t.tok,"NULL")) {t.tok = "ERROR"; return t;}	
		}
        }
	if(!strcmp(t.tok,"FLOAT")) {
					t.doubleval = signflag * t.doubleval;
					return  t;
				}
        else if(!strcmp(t.tok,"ID")) {
					t.doubleval = signflag * (*(double*)((FloatState*)t.s));
					return t;
				}
        else {t.tok = "ERROR"; return t;}
}


// make knownstate entry
static ComplexState proto;
static KnownState entry(proto,"COMPLEX");

