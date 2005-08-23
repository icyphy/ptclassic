/******************************************************************
Version identification:
@(#)CGUtilities.h	1.13	7/29/96

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

 Programmer: Jose L. Pino, J. Buck

Misc CG routines.

****************************************************************/
#ifndef _CGUtilities_h
#define _CGUtilities_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "StringList.h"

// Create a writeable directory on either a remote or local machine
int makeWriteableDirectory(const char* hname, const char* directory);

// Return a dynamically allocated string that is a lower-case version of name.
char* makeLower(const char* name);

// Run command on hostname in specified directory.
int rshSystem(const char* hostname, const char* command,
	      const char* directory = NULL);

// Write a string to a file in a specified directory on a given host.
int rcpWriteFile(const char* hname, const char* dir, const char* file,
        const char* text, int displayFlag = FALSE, int mode = -1);

// Copy a file to a specified directory on a given host.
int rcpCopyFile(const char* hname, const char* dir, const char* filePath,
	int deleteOld = TRUE, const char* newFileName = NULL);

// Copy several files to a specified directory on a given host.
int rcpCopyMultipleFiles(const char* hname, const char* dir,
	const char* fileList, int deleteOld = TRUE);

// Returns TRUE if hname is the machine Ptolemy is running on.
int onHostMachine(const char* hname);

// Sanitize a string so that it is usable as a C/C++ identifier.
const char* ptSanitize(const char* string);

// Find a local file name
StringList findLocalFileName(const char* hname, const char* dir,
                             const char* filename, int& deleteFlag);

// Delete the filename if deleteFlag is TRUE
int cleanupLocalFileName(const char* pathname, int deleteFlag);

#endif
