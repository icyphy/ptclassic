static const char file_id[] = "pt_fstream.cc";

/*
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
*/

// Programmer:  J. Buck, Kennard
// $Id$

// This defines derived types of ifstream and ofstream that do the following:

// 1. expand the file name using expandFileName
// 2. report file-open errors using Ptolemy's error-reporting functions.
// The system error value (from "errno") is reported as part of the error
// message and Error::abortRun is called on an open failure.
//
// Otherwise they are the same as their baseclasses.


#include "pt_fstream.h"
#include "miscFuncs.h"
#include "Error.h"
#include <errno.h>
#include "StringList.h"

extern int sys_nerr;
extern char *sys_errlist[];
extern int errno;

// lastName is used to remember the filename for the reporting of errors.
static const char* lastName = 0;

inline const char* expand(const char * name) {
	return lastName = expandPathName(name);
}

// report the reason why the open failed
static void reportError(const char* op) {
	const char * reason = "Unknown error";
	if (errno >= 0 && errno < sys_nerr)
		reason = sys_errlist[errno];
	StringList msg = "Can't open ";
	msg << lastName << " for " << op << ": " << reason;
	Error::abortRun(msg);
}

// check for special file names
// This uses the hardcoded descriptors 0,1,2.  These are appropriate
// for UNIX only.
static int check_special(const char *name, int *pNobufB) {
	int	fd = -1, bufB = TRUE;

// see if we begin with "<c" or "<std".  If so, skip prefix, else
// exit immediately.
	if (*name != '<') return fd;
	if (name[1] == 'c') name += 2;
	else if (strncmp(name+1,"std", 3) == 0) name += 4;
	else return -1;

// look for remainder of standard names.
	if (strcmp(name,"in>") == 0)
		fd = 0;
	else if (strcmp(name,"out>") == 0)
	{
		fd = 1;
		bufB = FALSE;
	}
	else if ( strcmp(name,"err>")==0)
	{
		fd = 2;
		bufB = FALSE;
	}
	else if ( strcmp(name,"log>")==0)
		fd = 2;
	if ( pNobufB )
		*pNobufB = ! bufB;
	return fd;
}

pt_ifstream::pt_ifstream(const char *name,int mode, int prot) {
	open(name,mode,prot);
}

// open file, treating some names as special.

void pt_ifstream::open(const char* name, int mode, int prot) {
	int nobufB;
	int fd = check_special(name, &nobufB);
	if (fd == 0) rdbuf()->attach(fd);
	else if (fd > 0) {
		Error::abortRun("Can't open ", name,
				" for reading: write-only file descriptor");
		return;
	}
	else ifstream::open(expand(name),mode,prot);
	if (!*this) reportError("reading");
	if (nobufB) setf(unitbuf);
}

pt_ofstream::pt_ofstream(const char *name,int mode, int prot) {
	open(name,mode,prot);
}

void pt_ofstream::open(const char* name, int mode, int prot) {
	int nobufB;
	int fd = check_special(name, &nobufB);
	if (fd == 0) {
		Error::abortRun("Can't open ", name,
				" for writing: can't write to standard input");
		return;
	}
	else if (fd > 0) rdbuf()->attach(fd);
	else ofstream::open(expand(name),mode,prot);
	if (!*this) reportError("writing");
	if (nobufB) setf(unitbuf);
}
