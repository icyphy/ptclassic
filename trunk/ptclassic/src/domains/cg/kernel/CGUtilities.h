/******************************************************************
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

 Programmer: Jose L. Pino, J. Buck

Misc CG routines.

****************************************************************/
#ifndef _CGUtilities_h
#define _CGUtilities_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "type.h"

char* makeLower(const char* name);

//////////////////////////////////////////////////////////////////////////
//run command on hostname in specified directory.  if directory == NULL
//then the command will be executed in the home directory.  Returns
//the status flag of the system call.

int rshSystem(const char* hostname,const char* command,const char* directory = NULL);

//////////////////////////////////////////////////////////////////////////
//write a string to a file in a specified directory on a given host.  If
//the direcory does not exit, it will be created.  The code can be
//optionally displayed.  If host is not the localhost, then this method
//will use rcp to copy it.  If mode is not null, will execute a
//chmod on the file with the given mode.  Returns TRUE if successful.

int rcpWriteFile(const char* hname,const char* dir,const char* file,
const char* text,int displayFlag = FALSE, int mode = -1);

/////////////////////////////////////////////////////////////////////////
//returns TRUE if hname is the machine Ptolemy is running on.  NULL, "\0"
//and "localhost" all return TRUE as well.
int onHostMachine(const char* hname);


#endif
