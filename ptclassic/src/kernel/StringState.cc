
#include <std.h>
#include "StringState.h"

const int TOKSIZE = 80;

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  I. Kuroda
 Date of creation: 5/26/90
 Revisions:

 Functions for class StringState

**************************************************************************/


/*************************************************************************

	class StringState methods

**************************************************************************/


void StringState  :: initialize() {
	const char* specialChars = "*+-/()";
	Tokenizer lexer(initValue,specialChars);

	val = evalExpression(lexer, parent()->parent());
}

const char* StringState :: evalExpression(Tokenizer& lexer, Block*  blockIAmIn) {

	ParseToken t = getParseToken(lexer, blockIAmIn);
	if(!strcmp(t.tok,"ID")) return ((StringState*)t.s)->val;
	else if (!strcmp(t.tok,"STRING")) return t.sval;
	else return 0;

}

// make known state entry
static StringState proto;
static KnownState entry(proto,"string");
	 
