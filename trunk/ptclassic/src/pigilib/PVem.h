/**************************************************************************
Version identification:
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
		       
 Programmer:  Alan Kamas
 Date of creation: October 1993

This file implements a class that adds Ptolemy-specific Vem commands to
a Tcl interpreter.  

**************************************************************************/

#ifndef _PVem_h
#define _PVem_h 1

/* Do the right thing for sol2 boolean defs.  compat.h must be included
 * first so sys/types.h is included correctly.
 */
#include "sol2compat.h"

#include <std.h>
#include "tcl.h"
#include "TclObj.h"

#if defined(hppa)
/* Include math.h outside of extern "C", other wise we get errors with
   pow() g++2.5.8 */
#include <math.h>
#endif

extern "C" {
#define Pointer screwed_Pointer
#include "oct.h"  // Oct Pointer Structure
#include "rpc.h"  // Oct Pointer Structure
#include "local.h" // Defines useful items like TRUE and FALSE
#undef Pointer
}

#ifdef __GNUG__
#pragma interface
#endif

class PVem : public TclObj {

public:
	// the active Tcl interpreter, for error reporting.
	static Tcl_Interp* activeInterp;

	PVem(Tcl_Interp* interp = 0);
	~PVem();

	// the dispatcher is called by Tcl to handle all extension
	// commands.
	static int dispatcher(ClientData,Tcl_Interp*,int,char*[]);

        // the following are the Tcl-callable functions.  Each returns TCL_OK
        // or TCL_ERROR, and may set the Tcl result to return a string
        int pvOpenWindow (int argc,char** argv);

protected:
	// helper functions, not callable directly
	void AdjustScalePan (Window window, octId facetId);

private:
        static PVem* findPVem(Tcl_Interp*);
        void makeEntry();
        void removeEntry();
	// function to register extensions with the Tcl interpreter
	void registerFuncs();

};

#endif		// _PVem_h

