static const char file_id[] = "CompiledUniverse.cc";

#ifdef __GNUG__
#pragma implementation
#endif
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee
 Date of creation: 12/14/91
 Revisions:

A CompiledUniverse is a Galaxy intended to be executed within generated
code.  Command line arguments are processed to set the number of iterations
and the universe state values.

**************************************************************************/

#include "CompiledUniverse.h"

// function to parse the command line.

int CompiledUniverse::parseCommandLine(int argc, char** argv, int* numIters) {
	int i = 1;
	while (i < argc) {
		if(stateWithName(argv[i])) {
			// argument is recognized as a state name
			char* value = (++i < argc) ? argv[i] : "";
			setState(argv[(i++)-1],value);
		} else {
			// assume that the argument is the number of iterations
			// check to be sure it's the last argument
			if((i<argc-1) || !sscanf(argv[i], "%d", numIters)) {
				BlockStateIter nextState(*this);
				State* s;
				StringList st;
				st << "Usage: " << name();
				while(s = nextState++) {
					st << " [" << s->name() << " <value>]";
				}
				st << " [iterations]";
				Error::abortRun(st);
				return FALSE;
			} else { break; }
		}
	}
	return TRUE;
}
