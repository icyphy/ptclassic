#ifndef _CompiledUniverse_h
#define _CompiledUniverse_h 1
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
