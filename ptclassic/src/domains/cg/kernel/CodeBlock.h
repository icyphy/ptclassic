#ifndef _Code_h
#define _Code_h 1
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

// This class should be replaced with a derived class for each specific
// code generator.  The constructor XXXCodeBlock, where XXX is the domain
// name, will normally process the input code in some way.

class CodeBlock {
public:
	// Constructor for the default case just stores the code
	CodeBlock(const char* b) { text = b; }
	// Is the following needed also?
	void setText(char* line) {text = line;}
	const char* getText() {return text;}
	void printCode ();
private:
	const char* text;
};

#endif
