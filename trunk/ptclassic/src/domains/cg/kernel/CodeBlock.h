#ifndef _CodeBlock_h
#define _CodeBlock_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "DataStruct.h"
#include "type.h"

	////////////////////////////////////
	// class CodeBlock
	////////////////////////////////////
// NOTE:  When text is sent to a CodeBlock, the text pointer is stored.
// Make sure the text that the pointer points to lives as long as the
// codeblock is to be used.
class CodeBlock {
public:
	// Constructor for the default case just stores the code
	CodeBlock(const char* b) { text = b; }
	// Is the following needed also?
	void setText(char* line) {text = line;}
	const char* getText() {return text;}
	void printCode ();
	operator const char*() {return text;}
private:
	const char* text;
};

#endif
