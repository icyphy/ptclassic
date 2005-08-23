/* 
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
*/
static const char file_id[] = "miscImpl.cc";
// miscellaneous functions.
// Under g++, inline implementationss for Attribute and the various
// ConstIters classes go here, using the #pragma mechanism.
// @(#)miscFuncs.cc	1.10 02/04/99

#ifdef __GNUG__
#pragma implementation
#pragma implementation "Attribute.h"
#pragma implementation "ConstIters.h"

#include "Attribute.h"
#include "ConstIters.h"
#endif

#include <stdio.h>		// Pick up sprintf under sol2 Sun CC
#include <unistd.h>		// getpid()
#include "miscFuncs.h"
#ifdef PT_NT4VC
#include <process.h>		// Need getpid()
#endif


// This function returns a new, unique temporary file name.
// It lives on the heap and may be deleted when no longer needed.
char* tempFileName() {
	int pid = (int) getpid();
	static int count = 1;
	LOG_NEW; char* buf = new char[255];
#ifdef PT_NT4VC
	sprintf (buf, "%s\\pt%04x.%04d", getenv("TEMP"), pid, count);
#else
	sprintf (buf, "/tmp/pt%04x.%04d", pid, count);
#endif
	count++;
	return buf;
}

