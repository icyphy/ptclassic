/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Pino, J. Buck

Misc CG routines.

****************************************************************/
#ifndef _CGUtilities_h
#define _CGUtilities_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "StringList.h"

char* makeLower(const char* name);

//run command on hostname in specified directory.  if directory == NULL
//then the command will be executed in the home directory
int rshSystem(const char* hostname,const char* command,const char* directory = NULL);

#endif
