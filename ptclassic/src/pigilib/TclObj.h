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


#ifndef _TclObj_h
#define _TclObj_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "tcl.h"

class StringList;

class TclObj {

protected:
        // the Tcl interpreter
        Tcl_Interp* interp;

        // flag to indicate that interp is owned by me
        short myInterp;

        // return a usage error
        int usage(const char*);

        // return the passed value (with proper cleanup) as the result
        // of a Tcl Command
        int result(StringList&);
        int result(char*);
        int result(int);

        // return a "static result".  Don't give this one a stringlist!
        int staticResult(const char*);

        // append a value to the result, using Tcl_AppendElement.
        void addResult(const char*);

};

#endif          // _TclObj_h


