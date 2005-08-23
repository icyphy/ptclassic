static const char file_id[] = "subCharByString.cc";

/**************************************************************************
Version identification:
@(#)subCharByString.cc	1.1	10/30/95

Copyright (c) 1990-1996 The Regents of the University of California.
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

Programmer:  Brian L. Evans
Date of creation: 10/30/95

This file defines subCharByString, which replaces each character 'target'
in string 'str' with string 'substr'.

For example, subCharByString("/tmp/image.#", '#', "12") returns
"/tmp/image.12".

Note that the value of copystr and str are the same, but we have
to create a copy because

**************************************************************************/

#include "StringList.h"
#include "miscFuncs.h"

// subCharByString
// note that the value of copystr and str are the same at the beginning and
// end of the routine, but we have to create a copy because str is a
// const char *
char *subCharByString(const char *str, char target, const char *substr) {
	StringList newstring;
	char *copystr = savestring(str);
	char *laststrp = copystr;
	char *curstrp = copystr;

	while ( *curstrp ) {
	  if ( *curstrp == target ) {
	    *curstrp = 0;			// force string termination
	    newstring << laststrp << substr;
	    *curstrp++ = target;		// restore string's value
	    laststrp = curstrp;
	  }
	  else {
	    curstrp++;
	  }
	}

	if ( laststrp != curstrp ) {
	  newstring << laststrp;
	}
	delete [] copystr;
	return savestring(newstring);
}
