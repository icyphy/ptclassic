
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

ParseToken pt;

void <T>State  :: initialize() {
	const  char* specialChars =  "*+-/()";
	Tokenizer lexer(initValue,specialChars);

	ParseToken t =evalExpression(lexer, parent()->parent());
	if(strcmp(t.tok,"ERROR"))
	if(strcmp(t.tok,"EOF"))
	val = t.<C>val;
}

ParseToken <T>State :: evalExpression(Tokenizer& lexer, Block*  blockIAmIn) {
	ParseToken t1 = evalTerm(lexer, blockIAmIn);
	if(!strcmp(pt.tok,"EOF")) return t1;
	ParseToken t2 = pt;
	while((t2.cval == '+')||(t2.cval == '-')){
	ParseToken t3 = evalTerm(lexer, blockIAmIn);

        if(t2.cval == '+') t1.<C>val += t3.<C>val;
        else              t1.<C>val -= t3.<C>val;
	if(!strcmp(pt.tok,"EOF")) return t1;	
	t2 = pt;}
	pt = t2;
	return t1;
} 

ParseToken <T>State :: evalTerm(Tokenizer& lexer, Block*  blockIAmIn) {
	ParseToken t1 = evalFactor(lexer, blockIAmIn);
	ParseToken t2 = getParseToken(lexer, blockIAmIn, "<T>");
	ParseToken t3;
	while(((t2.cval == '*')||(t2.cval == '/'))&& 
	strcmp((t3 = evalFactor(lexer, blockIAmIn)).tok,"EOF"))
	{
 
	if(t2.cval == '*') t1.<C>val *= t3.<C>val;
	else		  t1.<C>val /= t3.<C>val;
	
	t2 = getParseToken(lexer, blockIAmIn, "<T>"); }
	pt = t2; 
	return t1;
} 

ParseToken <T>State :: evalFactor(Tokenizer& lexer, Block*  blockIAmIn) {
	
	<C> signflag = 1;
        ParseToken t = getParseToken(lexer, blockIAmIn,"<T>");

	if(!strcmp(t.tok,"EOF")) return t;
	if(!strcmp(t.tok,"OP"))
	{
	if(t.cval == '(') {
	ParseToken t1 = evalExpression(lexer, blockIAmIn);	
	return t1;}

	if(t.cval == '-')
		{signflag = -1;
		t = getParseToken(lexer, blockIAmIn,"<T>");
		if(!strcmp(t.tok,"NULL")) {t.tok = "ERROR"; return t;}	
		}
        }
	if(!strcmp(t.tok,"<T>")) {
					t.<C>val = signflag * t.<C>val;
					return  t;
				}
        else if(!strcmp(t.tok,"ID")) {	t.tok = "<T>";
					t.<C>val = signflag * ((<T>State*)t.s)->val;
					return t;
				}
        else {t.tok = "ERROR"; return t;}
}


// make knownstate entry
static <T>State proto;
static KnownState entry(proto,"<C>");

	 


	 
