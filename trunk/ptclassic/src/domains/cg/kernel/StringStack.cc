static const char *file_id = "StringStack.cc";
/****************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. L. Pino
 Date of creation: 7/1/92
 Revisions:


 Methods for StringStack class.
***************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "StringStack.h"
#include "miscFuncs.h"

#define SMALL_STRING 20

// Note: all components of a StringStack are in dynamic memory,
// and are deleted by the StringStack destructor

void StringStack :: push(const char* s) {
	pushTop(savestring(s));
}

void StringStack :: push(char c) {
	char buf[2];
	buf[0] = c; buf[1] = 0;
	push(buf);
}

void StringStack :: push(int i) {
	char buf[SMALL_STRING];
        sprintf(buf,"%d",i); 
	push(buf);
}

void StringStack :: push(double f) {
	char buf[SMALL_STRING];
        sprintf(buf,"%g",f);
	push(buf);
}

char* StringStack :: pop() {
	return (char*)popTop();
}

void StringStack :: deleteAllStrings() {
	char* string;
	while (depth() !=0) {
		string = pop();
		LOG_DEL; delete string;
	}
}
