static const char file_id[] = "DynamicGalaxy.cc";
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

 Programmer:  Soonhoi Ha and J. Buck
 Date of creation: 1/14/92

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "DynamicGalaxy.h"

// Destructor body (has a different name so it can be called
// in other contexts)

void DynamicGalaxy :: zero () {
	// delete component blocks
	deleteAllBlocks();
	// delete component ports and multiports.
	deleteAllGenPorts();
	// delete states.
	deleteAllStates();
}

// isa
ISA_FUNC(DynamicGalaxy,Galaxy);

// clone function: return error

Block* DynamicGalaxy::makeNew() const {
	Error::abortRun(*this, " cannot clone object of type 'DynamicGalaxy'");
	return 0;
}
