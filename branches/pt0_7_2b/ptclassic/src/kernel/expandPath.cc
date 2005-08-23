static const char file_id[] = "expandPath.cc";
/**************************************************************************
Version identification:
@(#)expandPath.cc	1.21 12/08/97

Copyright (c) 1990-1997 The Regents of the University of California.
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

Routine to expand a pathname.  It returns the result of savestring applied
to the expanded path name.  The savestring routine allocates a new dynamic
string via the new operator.

The prototype declaration of the expandPathName function is defined in
miscFuncs.h.

The POSIX standard requires that every login shell defines the HOME and
LOGNAME environment variables [1, p. 39].  The POSIX standard also
defines two routines for searching the password database: getpwid
and getpwnam [1, p. 147].

Reference:

1. W. Richard Stevens, {Advanced Programming in the Unix Environment},
   Addison-Wesley, Reading, MA, ISBN 0-201-56317-7, 1992.

**************************************************************************/
#ifndef PT_NO_PWD
#include <pwd.h>
#endif
#include <unistd.h>
#include "Error.h"
#include "Tokenizer.h"
#include "StringList.h"
#include "miscFuncs.h"
#define MAXSTRINGLEN 4096

#ifdef PT_NO_PWD
// We don't have pwd.h, so 
// Expand a pathname. The pathname may begin with ~, ~user, or $var where
// var is an environment variable.  The expansion of ~ and ~user is
// POSIX compliant.  The expansion of environment variables is ANSI C
// compliant.  This function returns the result of savestring applied
// to the expanded path name, which allocates a new dynamic string via
// the new operator.
char* expandPathName(const char* name) {
    // Allow file name to expand to an arbitrary length
    StringList expandedPath;
    expandedPath.initialize();

    // Parse file name using Tokenizer class
    // '#' is default Tokenizer comment char which is a valid file name char
    const char* specialChars = "~/$";	// separators
    const char* whitespace = "\r";	// allow TABS and SPACES in file name
    Tokenizer lexer(name, specialChars, whitespace);
    lexer.setCommentChar('\n');		// override default comment char '#'
    lexer.setQuoteChar('\002');		// unprintable quote character CTRL-B

    while(!lexer.eof()) {
	char tokbuf[MAXSTRINGLEN];
	lexer >> tokbuf;
	char c = tokbuf[0];
	if (c != 0 && tokbuf[1]) c = 0;
	switch (c) {
	  case '~' : {
	    // we only expand the first tilda
	    if (expandedPath.numPieces() != 0) {
		expandedPath << '~';
		break;
	    }

	    // next token might be user name or /.
	    lexer >> tokbuf;
	    if (tokbuf[0] == '/') {
//		passwd* pwd = getpwuid(getuid());
//		if (pwd == 0) {
		    Error::abortRun ("getpwuid doesn't know you!");
//		    exit (1);
//		}
//		expandedPath << pwd->pw_dir << '/';
	    }
	    else {

//		passwd* pwd = getpwnam(tokbuf);
//		if (pwd == 0)
		    expandedPath << tokbuf;
//		else
//		    expandedPath << pwd->pw_dir;
	    }
	    break;
	  }    
	  case '/' : {
	    expandedPath << '/';
	    break;
	  }
	  case '$' : {
	    // next token might be an environment variable
	    lexer >> tokbuf;
	    const char* value = getenv(tokbuf);
	    if (!value)
		expandedPath << '$' << tokbuf;
	    else
		expandedPath << value;
	    break;
	  }
	  default: {
	    expandedPath << tokbuf;
	  }
	}
    }
    return savestring(expandedPath);
}


#else /* PT_NO_PWD */

// Expand a pathname. The pathname may begin with ~, ~user, or $var where
// var is an environment variable.  The expansion of ~ and ~user is
// POSIX compliant.  The expansion of environment variables is ANSI C
// compliant.  This function returns the result of savestring applied
// to the expanded path name, which allocates a new dynamic string via
// the new operator.
char* expandPathName(const char* name) {
    // Allow file name to expand to an arbitrary length
    StringList expandedPath;
    expandedPath.initialize();

    // Parse file name using Tokenizer class
    // '#' is default Tokenizer comment char which is a valid file name char
    const char* specialChars = "~/$";	// separators
    const char* whitespace = "\r";	// allow TABS and SPACES in file name
    Tokenizer lexer(name, specialChars, whitespace);
    lexer.setCommentChar('\n');		// override default comment char '#'
    lexer.setQuoteChar('\002');		// unprintable quote character CTRL-B

    while(!lexer.eof()) {
	char tokbuf[MAXSTRINGLEN];
	lexer >> tokbuf;
	char c = tokbuf[0];
	if (c != 0 && tokbuf[1]) c = 0;
	switch (c) {
	  case '~' : {
	    // we only expand the first tilda
	    if (expandedPath.numPieces() != 0) {
		expandedPath << '~';
		break;
	    }

	    // next token might be user name or /.
	    lexer >> tokbuf;
	    if (tokbuf[0] == '/') {
		passwd* pwd = getpwuid(getuid());
		if (pwd == 0) {
		    Error::abortRun ("getpwuid doesn't know you!");
		    exit (1);
		}
		expandedPath << pwd->pw_dir << '/';
	    }
	    else {

		passwd* pwd = getpwnam(tokbuf);
		if (pwd == 0)
		    expandedPath << tokbuf;
		else
		    expandedPath << pwd->pw_dir;
	    }
	    break;
	  }    
	  case '/' : {
	    expandedPath << '/';
	    break;
	  }
	  case '$' : {
	    // next token might be an environment variable
	    lexer >> tokbuf;
	    const char* value = getenv(tokbuf);
	    if (!value)
		expandedPath << '$' << tokbuf;
	    else
		expandedPath << value;
	    break;
	  }
	  default: {
	    expandedPath << tokbuf;
	  }
	}
    }
    return savestring(expandedPath);
}
#endif /* PT_NO_PWD */
