static const char file_id[] = "CompiledUniverse.cc";
#ifndef _CompiledUniverse_cc
#define _CompiledUniverse_cc 1

#ifdef __GNUG__
#pragma once
#endif
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee
 Date of creation: 12/14/91
 Revisions:

A CompiledUniverse is a Galaxy intended to be executed within generated
code.  Command line arguments are processed to set the number of iterations
and the universe state values.

**************************************************************************/

#include "Galaxy.h"

class CompiledUniverse : public Galaxy {
public:
	// This method returns 0 if command line was invalid, 1 otherwise.
	// It sets Galaxy states given on the command line and updates
	// numIters if the number of iterations is given on the command
	// line.
	int parseCommandLine(int argc, char** argv, int* numIters);
};

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
			StateListIter nextState(states);
			State* s;
			StringList st = "Usage: ";
			st += readName();
			while(s = nextState++) {
			    st += " [";
			    st += s->readName();
			    st += " value]";
			}
			st += " [iterations]";
			Error::abortRun(st);
		} else { break; }
	    }
	}
}

#include "isa.h"

#endif
