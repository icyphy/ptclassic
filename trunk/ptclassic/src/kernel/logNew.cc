static const char file_id[] = "logNew.cc";
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
 Date of creation: 1/7/92

This is a logging memory allocator.  If the environment variable
MEMORYLOG is set, a log of memory allocation and freeing is produced.
For maximum usefulness, the macros LOG_NEW, LOG_DEL, INC_LOG_NEW,
and INC_LOG_DEL should be used to provide source lines where the
calls to new and delete occurred.

**************************************************************************/

// if the symbol MEMORYLOG is not defined, this whole source unit is
// "commented out".

#ifdef MEMORYLOG

#include "logNew.h"
#include <stdio.h>
#include <sys/types.h>
#include <std.h>

static const char* ccfile = 0;
static const char* hfile = 0;
static int sLine = 0;
static int mode = 0;
static FILE* logfd = 0;
static int firstCall = 1;

#define M_NEW 1
#define M_DEL 2
#define M_INEW 3
#define M_IDEL 4

// mark for a call to new
void logNew(const char* f, int l) {
	ccfile = f;
	hfile = 0;
	sLine = l;
	mode = M_NEW;
}

// mark for a call to delete
void logDel(const char* f, int l) {
	ccfile = f;
	hfile = 0;
	sLine = l;
	mode = M_DEL;
}

// mark for a call to new from an .h file
void incLogNew(const char* ccf, const char* incf, int l) {
	ccfile = ccf;
	hfile = incf;
	sLine = l;
	mode = M_INEW;
}

// mark for a call to delete from an .h file
void incLogDel(const char* ccf, const char* incf, int l) {
	ccfile = ccf;
	hfile = incf;
	sLine = l;
	mode = M_IDEL;
}


static void openLog () {
	firstCall = 0;
	const char* logFileName = getenv ("MEMORYLOG");
	if (!logFileName) return;
	if ((logfd = fopen (logFileName, "w")) == 0) {
		fprintf (stderr, "Can't open memory log file ");
		perror (logFileName);
		return;
	}
	// buffer it by lines.
	setvbuf(logfd, NULL, _IOLBF, 0);
}

static const char noMemMsg[] =
"FATAL: operator new failed: virtual memory exhausted\n";

// operator new -- allocate memory.  Bomb on malloc failure.
void* operator new(size_t sz) {
	if (firstCall) openLog();
	void* p = malloc(sz);
	if (p == 0) {
		// use write rather than fprintf because fprintf may
		// try doing a malloc.
		write(2, noMemMsg, sizeof(noMemMsg)-1);
		exit(1);
	}
	if (logfd) {
		if (mode == 0) {
			ccfile = "Unknown";
			sLine = 0;
		}
		if (mode < M_INEW) hfile = "Top";

		fprintf (logfd, "new %s %s %d %ld %lx\n",
			 ccfile, hfile, sLine, sz, p);
		mode = 0;
	}
	return p;
}

// operator delete -- free memory
void operator delete(void *p) {
	if (p == 0) return;
	if (logfd) {
		if (mode == 0) {
			ccfile = "Unknown";
			sLine = 0;
		}
		if (mode < M_INEW) hfile = "Top";
		fprintf (logfd, "del %s %s %d %lx\n",
			 ccfile, hfile, sLine, p);
		mode = 0;
	}
	// cast to char* needed for Sun port of cfront.  Gack.
	free((char *)p);
}

#endif
