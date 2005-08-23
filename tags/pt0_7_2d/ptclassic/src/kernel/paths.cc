static const char file_id[] = "paths.cc";
/**************************************************************************
Version identification:
@(#)paths.cc	1.5	01/05/99

Copyright (c) 1990-1999 The Regents of the University of California.
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

 Programmer:  J. T. Buck
 Date of creation: 12/7/92

The pathSearch routine was formerly in Linker.cc.

**************************************************************************/

// search for file on the colon-separated directory path given by
// path.  If path is omitted or null, the environment variable PATH
// is used.
#include <std.h>
#include <unistd.h>	// access()
#include "miscFuncs.h"
#include "StringList.h"
#include "Error.h"
#include "paths.h"

#if defined(PT_NT4VC)
#define PT_FILEDELIMITER '\\'
#define PT_PATHDELIMITER ';'
#else
#define PT_FILEDELIMITER '/'
#define PT_PATHDELIMITER ':'
#endif

const char*
pathSearch (const char* name, const char* path) {
    char nameBuf[512];
    char* q = nameBuf;

    if (path == 0) path = getenv("PATH");
    if (path == 0) path = ".";
    // handle null string
    if (name == 0 || *name == 0) return 0;
    // if name begins with . or / it must be exact or end in .exe
    if (*name == '.' || *name == PT_FILEDELIMITER ) {
        if (access (name, 0) == 0) return hashstring(name);
        strcpy (q, name);
        strncat (q, ".exe", 512);
        return access(nameBuf, 0) == 0 ? hashstring(nameBuf) : 0;
    }

    // build the next candidate name
    while (*path) {
        q = nameBuf;
        while (*path && *path != PT_PATHDELIMITER) *q++ = *path++;
        *q++ = PT_FILEDELIMITER;
        strcpy (q, name);
        if (access (nameBuf, 0) == 0) return hashstring(nameBuf);
        // Look for .exe files
        strncat (q, ".exe", 512);
        if (access (nameBuf, 0) == 0) return hashstring(nameBuf);
        if (*path == PT_PATHDELIMITER) path++;
    }
    return 0;
}

// return true and produce an error msg if program is not found in the
// PATH.
int
progNotFound(const char* program,const char* extra) {

	if (!pathSearch(program)) {
		StringList msg;
		msg << "The required program \"" << program
		    << "\" is not on your path.";
		if (extra)
			msg << "\n" << extra;
		Error::abortRun(msg);
		return 1;
	}
	return 0;
}

