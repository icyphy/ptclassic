/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 6/10/90

Routine to expand a pathname
warning: may return a pointer to a static buffer; a second
call may over-write this buffer.

This is almost plain C except for calling errorHandler.error

**************************************************************************/

#include <pwd.h>
#include <std.h>
#include "Output.h"
#define MAXLEN 256

extern Error errorHandler;

const char*
expandPathName(const char* name) {
	static char buf[MAXLEN];
	if (*name != '~') return name;

	const char* pslash = index (name, '/');
	if (pslash == NULL) pslash = name + strlen(name);
	passwd* pwd;
	if (pslash == name + 1) {
		pwd = getpwuid(getuid());
		if (pwd == 0) {
			errorHandler.error ("getpwuid doesn't know you!");
			exit (1);
		}
	}
	else {
		int l = pslash - name - 1;
		strncpy (buf, name + 1, l);
		buf[l] = 0;
		pwd = getpwnam(buf);
		if (pwd == 0) return name;
	}
	// Put in the home directory
	strcpy (buf, pwd->pw_dir);

	// add the rest of the name
	strcat (buf, pslash);
	return buf;
}


	
