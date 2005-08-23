static const char file_id[] = "InvokeInterp.cc";
/**************************************************************************
Version identification:
@(#)InvokeInterp.cc	1.2	9/27/95

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

 Programmer:  B. L. Evans
 Date of creation: 9/26/95
 Revisions:

 Code of functions declared in InvokeInterp.h

**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include <std.h>
#include <ctype.h>
#include "InvokeInterp.h"
#include "Error.h"

// METHODS

// constructor
InvokeInterp :: InvokeInterp() {}

// destructor
InvokeInterp :: ~InvokeInterp() {}

// send a string to an external interpreter for evaluation
const char*
InvokeInterp :: interpreter(const char* expression) {
    Error::error("no interpreter to evaluate '", expression, "'.");
    return NULL;
}
