static const char file_id[] = "WriteASCIIFiles.cc";
/**************************************************************************
Version identification:
$Id$

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

 Programmer:  Edward A. Lee and Brian L. Evans
 Date of creation: 6/14/96

Routine to write an array of double precision floating-point values
to an ASCII file.

**************************************************************************/

#include <stdio.h>
#include "type.h"
#include "miscFuncs.h"
#include "WriteASCIIFiles.h"

/*
Save an array of double precision floating-point numbers to an ASCII file
the numbers are converted to text using the specified fprintf format.
If writeIndexFlag is TRUE, then the fprintf call is
fprintf(fp, format, index, value)
Otherwise, it is
fprintf(fp, format, value)
A newline is written to the file every writeNewLine lines if writeNewLine
is positive.
*/
int doubleArrayAsASCFile(const char* filename, const char* formatstr,
			 int writeIndexFlag, const double* dptr, int length,
			 int writeNewLines) {
	int validFlag = TRUE;
	if (filename && *filename) {
	    // expand environment variables in the filename
	    char* saveFileName = expandPathName(filename);
	    // open the file for writing
	    FILE* fp = fopen(saveFileName, "w");
	    validFlag = (fp != 0);
	    if (validFlag) {
		int newLineIndex = 0;
		int i = 0;
		for (i = 0; i < length; i++) {
		    // write the value to file in ASCII format
		    if (writeIndexFlag) {
			fprintf(fp, formatstr, i, *dptr);
		    }
		    else {
			fprintf(fp, formatstr, *dptr);
		    }
		    dptr++;

		    // write a newline every writeNewLine lines
		    newLineIndex++;
		    if (newLineIndex == writeNewLines) {
			newLineIndex = 0;
			fprintf(fp, "\n");
		    }
		}
		fclose(fp);
	    }
	    delete [] saveFileName;
	}
	return validFlag;
}
