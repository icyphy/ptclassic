#ifndef _CompiledUniverse_h
#define _CompiledUniverse_h 1
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and J. Buck
 Date of creation: 1/13/92
 Revisions:

A CompiledUniverse is a Galaxy intended to be executed within generated
code.  Command line arguments are processed to set the number of iterations
and the universe state values.

It is derived from Galaxy rather than from Universe because it has no
scheduler.

**************************************************************************/
#ifdef __GNUG__
#pragma once
#pragma interface
#endif

#include "Galaxy.h"

class CompiledUniverse : public Galaxy {
public:
	// This method returns 0 if command line was invalid, 1 otherwise.
	// It sets Galaxy states given on the command line and updates
	// numIters if the number of iterations is given on the command
	// line.
	int parseCommandLine(int argc, char** argv, int* numIters);
};

#endif