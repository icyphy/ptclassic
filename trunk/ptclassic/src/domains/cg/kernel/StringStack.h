#ifndef _StringStack_h
#define _StringStack_h 1

/****************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. L. Pino
 Date of creation: 7/1/92
 Revisions:

A StringStack is a stack of Strings.
***************************************************************/

#include "DataStruct.h"

#ifdef __GNUG__
#pragma interface
#endif

class StringStack : public Stack {
public:
	//Constructors
	StringStack(){}

	void initialize() { deleteAllStrings(); }

	~StringStack();

	//Return the depth of the stack
	int depth() const {return size();}

	void push(const char* p);
	void push(char c);
	void push(int i);
	void push(double f);
	char* pop();
private:
	void deleteAllStrings();
};
#endif
