static const char file_id[] = "expandPath.cc";
/**************************************************************************
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

 Programmer:  J. T. Buck
 Date of creation: 6/10/90

Routine to expand a pathname
warning: may return a pointer to a static buffer; a second
call may over-write this buffer.

The pathname may begin with ~, ~user, or $var where var is an
environment variable.  Variables are expanded only at the beginning.

**************************************************************************/

#include <pwd.h>
#include <std.h>
#include "Error.h"
#define MAXLEN 256

const char*
expandPathName(const char* name) {
	static char buf[MAXLEN];
	const char* value;

	if (*name != '~' && *name != '$') return name;

	// find first / after the env-var or the username

	const char* pslash = strchr (name, '/');
	if (pslash == NULL) pslash = name + strlen(name);

	// copy the username or variable name into buf.

	int l = pslash - name - 1;
	strncpy (buf, name + 1, l);
	buf[l] = 0;

	// if an environment variable, look up the value.
	if (*name == '$') {
		value = getenv (buf);
		if (!value) return name;
	}
	else {
		passwd* pwd;
		if (pslash == name + 1) {
			pwd = getpwuid(getuid());
			if (pwd == 0) {
				Error::abortRun ("getpwuid doesn't know you!");
				exit (1);
			}
		}
		else {
			pwd = getpwnam(buf);
			if (pwd == 0) return name;
		}
		value = pwd->pw_dir;
	}
	// Put in the home directory or value of variable
	strcpy (buf, value);

	// add the rest of the name
	strcat (buf, pslash);
	return buf;
}


	
