// This header file provides prototypes for miscellaneous
// library functions that are not members.
// $Id$
// J. T. Buck

#ifndef _miscFuncs_h
#define _miscFuncs_h 1
#include <std.h>

const char* expandPathName(const char*);

inline char* savestring (const char* txt) {
	return strcpy (new char[strlen(txt)+1], txt);
}
#endif
