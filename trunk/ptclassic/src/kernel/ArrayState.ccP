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

// constructor
<T>ArrayState :: <T>ArrayState (int size, <C>& fill_value) {
	val = new <C> [nElements = size];
	<C> * top = &(val[nElements]);
	<C> * t = val;
	while (t < top)
		*t++ = fill_value;
}

// destructor
<T>ArrayState :: ~<T>ArrayState () {delete[nElements] val;}

// assignment operator
<T>ArrayState &	<T>ArrayState :: operator = (const <T>ArrayState & v) {
	if (this != &v) {
		delete[nElements] val;
		val  = new <C> [nElements = v.nElements];
		<C>* top = &(val[nElements]);
		<C>* t = val;
		<C>* u = v.val;
		while (t < top) *t++ = *u++;
	}
	return *this;
}

// size (not inline because it's virtual)
int <T>ArrayState :: size() { return nElements;}

// type (not inline because it's virtual)
char* <T>ArrayState :: type() { return "<T>Array";}

// the value as a string

StringList <T>ArrayState :: currentValue() { 
	StringList s; 
	s =  "\n";
	for(int i = 0; i<size(); i++) {
		s += i;
		s += " ";
		s += val[i];
		s += "\n";
	} 
	return s;
}

// clone
State* <T>ArrayState :: clone() { return new <T>ArrayState;}

// Parse initValue to set value
void <T>ArrayState  :: initialize() {

	<C> buf[MAXLEN];
	char* specialChars = "*+-/()<[]";
	Tokenizer lexer(initValue,specialChars);

	int i = 0;
	int numRepeats;
	<C> saveValue;
	while(!lexer.eof() && i < MAXLEN) {
	ParseToken t =evalExpression(lexer, parent()->parent());
	if(strcmp(t.tok,"ERROR"))
	if(strcmp(t.tok,"EOF"))
        if(!strcmp(t.tok,"REPEATER")) {
                if(t.cval != '[') return;
                t = getParseToken(lexer, parent()->parent());
                if(strcmp(t.tok,"INT"))return;
                numRepeats = t.intval - 1;
                while ( numRepeats != 0) {
                        buf[i++] = saveValue;
                        numRepeats--;   
                }
                t = getParseToken(lexer,parent()->parent());
                if(strcmp(t.tok,"REPEATER")) return;
                if(t.cval != ']') return;
                continue;
        }

	{if(!strcmp(t.tok,"<T>")) buf[i++] = t.<C>val;
        else if(!strcmp(t.tok,"ID")) buf[i++] = ((<T>State*)t.s)->val;
	}
	saveValue = buf[i-1];
			};
	nElements  = i;

	val  = new <C> [nElements];
	for(i = 0; i < nElements; i++)
	val[i] = buf[i];
}

ParseToken <T>ArrayState :: evalExpression(Tokenizer& lexer, Block*  blockIAmIn) {
        <C> signflag = 1;
        ParseToken t = getParseToken(lexer, blockIAmIn,"<T>");

        if(!strcmp(t.tok,"EOF")) return t;
	if(!strcmp(t.tok,"REPEATER")) return t;
        if(!strcmp(t.tok,"OP"))
        {
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
        else if(!strcmp(t.tok,"ID")) {
                ((<T>State*)t.s)->val = signflag * ((<T>State*)t.s)->val;
                                        return t;
                                }
        else {t.tok = "ERROR"; return t;}
	
}


// make knownstate entry
static <T>ArrayState proto;
static KnownState entry(proto,"<T>Array");

	 


	 
