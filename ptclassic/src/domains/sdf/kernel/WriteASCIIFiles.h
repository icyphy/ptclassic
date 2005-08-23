/**************************************************************************
Version identification:
@(#)WriteASCIIFiles.h	1.3	03/17/97

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

 Programmer:  Brian L. Evans
 Date of creation: 6/15/96

 This header file provides prototypes for miscellaneous file operations
 only used by the SDF domain.

*************************************************************************/

#ifndef _WriteASCIIFiles_h
#define _WriteASCIIFiles_h 1

#ifdef __GNUG__
#pragma interface
#endif

// save an array of double precision floating-point numbers to an ASCII file
// the numbers are converted to text using the specified sprintf format
int doubleArrayAsASCFile(const char* filename, const char* formatstr,
			   int writeIndexFlag, const double* dptr, int length,
			   int writeNewLines);

#endif
