static const char file_id[] = "CompiledUniverse.cc";

#ifdef __GNUG__
#pragma implementation
#endif
/**************************************************************************
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
