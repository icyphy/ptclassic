/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 6/10/90

 This header file provides prototypes for miscellaneous
 library functions that are not members, together with some
 commonly used library functions.

*************************************************************************/

#ifndef _miscFuncs_h
#define _miscFuncs_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include <std.h>
#include "logNew.h"
#include <string.h>

extern "C" double pow(double,double);

// return an expanded version of the filename argument, which may
// start with "~", "~user", or "$var".  The returned value points
// to static storage and is wiped out by the next call.
const char* expandPathName(const char*);

// return a new, unique temporary file name.  It lives on the heap
// and may be deleted when no longer needed.
char* tempFileName();

inline char* savestring (const char* txt) {
	INC_LOG_NEW; return strcpy (new char[strlen(txt)+1], txt);
}

// Save the string in a systemwide hash table, if it is not there
// already, and return a pointer to the saved copy.  Subsequent
// hashstrings with the same string return the same address.

const char* hashstring(const char*);

// I would prefer to make this int pow(int,int), but g++ gets confused
// because the other function is extern "C".

int power(int base,int exp);
inline double power(double base,double exp) { return pow(base,exp);}

#endif
