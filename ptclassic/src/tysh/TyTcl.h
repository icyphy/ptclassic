/**************************************************************************
Version identification:
$Id$
		       
    Programmer:  Edward A. Lee

    This file implements in a class C++ extensions to Tcl used
    in the Tycho system.

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

**************************************************************************/

#ifndef _TyTcl_h
#define _TyTcl_h 1

/* Do the right thing for sol2 boolean defs.  compat.h must be included
 * first so sys/types.h is included correctly.
 */
#include "sol2compat.h"

#include <std.h>
#include "tcl.h"
#include "tk.h"
#include "TclObj.h"

#if defined(hppa)
/* Include math.h outside of extern "C", other wise we get errors with
   pow() g++2.5.8 */
#include <math.h>
#endif

#ifdef __GNUG__
#pragma interface
#endif

class TyTcl : public TclObj {

public:
	Tk_Window tkwin;

	TyTcl(Tcl_Interp* interp, Tk_Window tkwin);
	~TyTcl();

	// the dispatcher is called by Tcl to handle all extension
	// commands.
	static int dispatcher(ClientData,Tcl_Interp*,int,char*[]);

        // the following are the Tcl-callable functions.  Each returns TCL_OK
        // or TCL_ERROR, and may set the Tcl result to return a string
	// The comments below indicate how the function behaves in Tcl.
	// Thus, the value returned is what Tcl will see.

	// Return 1 if the font name passed as an argument is valid.
	// Return 0 otherwise.
        int ptkCheckFont (int argc,char** argv);

private:
        static TyTcl* findTyTcl(Tcl_Interp*);
        void makeEntry();
        void removeEntry();
	// function to register extensions with the Tcl interpreter
	void registerFuncs();
};

#endif		// _TyTcl_h

