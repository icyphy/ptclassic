static const char file_id[] = "expandPath.cc";
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

 Programmer:  J. T. Buck, Jose Luis Pino
 Date of creation: 6/10/90

Routine to expand a pathname
warning: may return a pointer to a static buffer; a second
call may over-write this buffer.

The pathname may begin with ~, ~user, or $var where var is an
environment variable.  Variables are expanded only at the beginning.

**************************************************************************/

#include <pwd.h>
#include "Error.h"
#include "Tokenizer.h"
#include "StringList.h"
#define MAXLEN 2000
#define MAXSTRINGLEN 4096

const char*
expandPathName(const char* name) {
    const char* buf[MAXLEN];
    char tokbuf[MAXSTRINGLEN];
    const char* specialChars = "~/$";
    Tokenizer lexer(name,specialChars);
    int i = 0;
    int err=0;
    const char* tilda = "~";
    const char* dollar = "$";
    const char* slash = "/";
    while(!lexer.eof() && i < MAXLEN && err == 0) {
	lexer >> tokbuf;
	char c = tokbuf[0];
	if (c != 0 && tokbuf[1]) c = 0;
	switch (c) {
	case '~' : {
	    // we only expand the first tilda
	    if (i != 0) {
		buf[i++] = tilda;
		break;
	    }

	    // next token might be user name or /.
	    lexer >> tokbuf;
	    passwd* pwd;
	    if (tokbuf[0] == '/') {
		pwd = getpwuid(getuid());
		if (pwd == 0) {
		    Error::abortRun ("getpwuid doesn't know you!");
		    exit (1);
		}
		buf[i++] = savestring(pwd->pw_dir);
		buf[i++] = slash;
	    }
	    else {
		pwd = getpwnam(tokbuf);
		if (pwd == 0)
		    buf[i++] = savestring(tokbuf);
		else
		    buf[i++] = savestring(pwd->pw_dir);
	    }
	    break;
	}    
	case '/' : {
	    buf[i++] = slash;
	    break;
	}
	case '$' : {
	    // next token might be an environment variable
	    lexer >> tokbuf;
	    const char* value = getenv (tokbuf);
	    if (!value) {
		buf[i++] = dollar;
		buf[i++] = savestring(tokbuf);
	    }
	    else {
		buf[i++] = savestring(value);
	    }
	    break;
	}
	default: {
	    buf[i++] = savestring(tokbuf);
	}
	}
    }
    StringList expandedPath;
    for (int j = 0 ; j < i ; j++) expandedPath << buf[j];
    return savestring(expandedPath);
}
