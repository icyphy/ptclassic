/*******************************************************************
SCCS version identification
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

 Programmer: Alan Kamas (based on work by Joe Buck)

********************************************************************/


// This the base class for the objects that load C++ functions
// into the tcl interpreter for ptolemy applications.
// Much of this code is based on Joe Buck's PTcl object 
// - Alan Kamas 10/93

// Note that there is still some code duplication between the
// tcl objects, as the findTclObj, MakeEntry, removeEntry,
// and dispatcther functions were all written with only
// one such object in mind.

static const char file_id[] = "TclObj.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include "TclObj.h"

#include <stdio.h>
#include <string.h>

#include "StringList.h"


//
// Return functions for TclObjs:
//

// Return a "usage" error
int TclObj::usage(const char* msg) {
        strcpy(interp->result,"wrong # args: should be \"");
        strcat(interp->result,msg);
        strcat(interp->result,"\"");
        return TCL_ERROR;
}

// Return a static result.  Typical usage: return staticResult("foo")
int TclObj::staticResult( const char* value) {
        Tcl_SetResult(interp, (char*)value, TCL_STATIC);
        return TCL_OK;
}

// Return a StringList result.
// We arrange for Tcl to copy the value.
int TclObj::result(StringList& value) {
        const char* str = value;
        // VOLATILE will tell Tcl to copy the value, so it is safe
        // if the StringList is deleted soon.
        Tcl_SetResult(interp, (char*)str,TCL_VOLATILE);
        return TCL_OK;
}

// Return a String result.
// We arrange for Tcl to copy the value.
int TclObj::result(char* value) {
        Tcl_SetResult(interp, value,TCL_VOLATILE);
        return TCL_OK;
}

// Return an Integer result.
// We arrange for Tcl to copy the value.
int TclObj::result(int value) {
        char str[64];
        sprintf( str, "%d", value);
        Tcl_SetResult(interp, str,TCL_VOLATILE);
        return TCL_OK;
}

void TclObj::addResult(const char* value) {
        // cast-away-const needed to interface with Tcl.
        Tcl_AppendElement(interp,(char*)value);
}


