
#include <std.h>
#include "StringState.h"

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
	val = evalExpression(initValue, parent()->parent());
}

const char* StringState :: evalExpression(const char* string, Block*  blockIAmIn) {

//	This part may  be used later for file read etc.
//
//	lexptr = string;
//	ParseToken t = getParseToken(string, blockIAmIn);
//	if(!strcmp(t.tok,"ID")) return ((StringState*)t.s)->val;
//	else return 0;
	return string;
}

// make known state entry
static StringState proto;
static KnownState entry(proto,"string");
	 
