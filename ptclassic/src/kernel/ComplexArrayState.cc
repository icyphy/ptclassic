#include <std.h>
#include "ComplexState.h"
#include "ComplexArrayState.h"
#include "FloatState.h"
#include "Tokenizer.h"

const int MAXLEN = 2000;

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  I. Kuroda and J. T. Buck
 Date of creation: 6/20/90
 Revisions:



**************************************************************************/
/*************************************************************************

	class ComplexArrayState methods

**************************************************************************/

void ComplexArrayState  :: initialize() {

	Complex buf[MAXLEN];
	char* specialChars = "*+-/()<,[]";
	Tokenizer lexer(initValue,specialChars);
	double realval;
	double imagval;
	int numRepeats;
	int i = 0;
	while(!lexer.eof() && i < MAXLEN) {
	
	ParseToken t =getParseToken(lexer, parent()->parent());
	if(!strcmp(t.tok,"ERROR")) break;
        if(!strcmp(t.tok,"EOF")) break;
	if(!strcmp(t.tok,"REPEATER")) {
		if(t.cval != '[') return;
		t = getParseToken(lexer, parent()->parent());
		if(strcmp(t.tok,"INT"))return;
		numRepeats = t.intval - 1;
		while ( numRepeats != 0) {
			buf[i++] = Complex(realval,imagval);
			numRepeats--;
		}
		t = getParseToken(lexer,parent()->parent());
		if(strcmp(t.tok,"REPEATER")) return;
		if(t.cval != ']') return;
		continue;
	}
        if(strcmp(t.tok,"OP")) return;
        if(t.cval != '(') return;
        t =  evalExpression(lexer, parent()->parent());
        if(!strcmp(t.tok,"ERROR")) return;
        if(!strcmp(t.tok,"EOF")) return;
        {if(!strcmp(t.tok,"FLOAT")) realval = t.doubleval;
        else if(!strcmp(t.tok,"ID")) realval = *(double*)((FloatState*)t.s);
        }
	t = getParseToken(lexer,  parent()->parent());
        if(strcmp(t.tok,"SEPARATOR")) return;
        if(t.cval != ',') return;

        t =  evalExpression(lexer, parent()->parent());
        if(!strcmp(t.tok,"ERROR")) return;
        if(!strcmp(t.tok,"EOF")) return;
        {if(!strcmp(t.tok,"FLOAT")) imagval = t.doubleval;
        else if(!strcmp(t.tok,"ID")) imagval = *(double*)((FloatState*)t.s);
        }
        t =getParseToken(lexer, parent()->parent());
        if(strcmp(t.tok,"OP")) return;
        if(t.cval != ')') return;
        buf[i++] = Complex(realval,imagval);

			};
	nElements  = i;

	val = new Complex [nElements];	
	for(i = 0; i < nElements; i++)
	val[i] = buf[i];
}

ParseToken ComplexArrayState :: evalExpression(Tokenizer& lexer, Block*  blockIAmIn) {
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
static ComplexArrayState proto;
static KnownState entry(proto,"ComplexArray");
