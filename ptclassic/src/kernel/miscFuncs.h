/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 6/10/90

 This header file provides prototypes for miscellaneous
 library functions that are not members.

*************************************************************************/

#ifndef _miscFuncs_h
#define _miscFuncs_h 1
#include <std.h>

const char* expandPathName(const char*);

inline char* savestring (const char* txt) {
	return strcpy (new char[strlen(txt)+1], txt);
}
#endif
