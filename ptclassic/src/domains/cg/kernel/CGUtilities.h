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

#include "StringList.h"

char* makeLower(const char* name);

//run command on hostname.
int rshSystem(const char* hostname,const char* command);

#endif
