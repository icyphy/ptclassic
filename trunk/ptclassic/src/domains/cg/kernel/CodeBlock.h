#ifndef _CodeBlock_h
#define _CodeBlock_h 1
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.
							COPYRIGHTENDKEY

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
