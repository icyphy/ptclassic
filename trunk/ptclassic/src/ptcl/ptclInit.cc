static const char file_id[] = "ptclInit.cc";
/*******************************************************************
SCCS Version identification :
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

 Programmer: Wan-Teh Chang
 Date of creation: 10/31/95

Defines the Ptcl_Init() function.  The PTcl commands can be added
to a Tcl interpreter by invoking the Ptcl_Init() function on it.

*******************************************************************/

#include "compat.h"
#include "PTcl.h"
#include "SimControl.h"
#include "StringList.h"
#include <stdio.h>

#ifdef PTLINUX
#include <fpu_control.h>
#include <unistd.h>	// pick up isatty()
#endif

#ifndef PTLINUX
/*
 * Declarations for various library procedures and variables.
 */
extern "C" {
extern int		isatty _ANSI_ARGS_((int fd));
};
#endif


// Load in the PTcl startup file ptcl.tcl.
static int loadStartup(Tcl_Interp* interp) {
    char *pt = getenv("PTOLEMY");
    StringList startup = pt ? pt : "~ptolemy";
    startup << "/lib/tcl/ptcl.tcl";
    if (Tcl_EvalFile(interp, startup.chars()) != TCL_OK) {
	fprintf(stderr, "ptcl: error in sourcing startup file '%s'.\n",
                interp->result);
	return TCL_ERROR;
    } else
	return TCL_OK;
}

// Delete a PTcl object.
static void PTclDeleteProc(ClientData clientData, Tcl_Interp */* interp */)
{
    PTcl *ptcl = (PTcl *) clientData;
    delete ptcl;
}

/*
 *----------------------------------------------------------------------
 *
 * Ptcl_Init --
 *
 *	This procedure is typically invoked by Tcl_AppInit procedures
 *	to add the PTcl commands to a Tcl interpreter and perform
 *	additional initialization, such as sourcing the "ptcl.tcl" script.
 *
 * Arguments:
 *	interp:	Interpreter to initialize.
 *
 * Results:
 *	Returns a standard Tcl completion code and sets interp->result
 *	if there is an error.
 *
 * Side effects:
 *	Depends on what's in the ptcl.tcl script.
 *
 *----------------------------------------------------------------------
 */

extern "C" int Ptcl_Init(Tcl_Interp *interp)
{

#ifdef PTLINUX
    // Fix for DECalendarQueue SIGFPE under linux.
    __setfpucw(_FPU_DEFAULT | _FPU_MASK_IM);
#endif

    PTcl *ptcl = new PTcl(interp);
    Tcl_CallWhenDeleted(interp, PTclDeleteProc, (ClientData) ptcl);

    if (isatty(0)) {		// set up interrupt handler
	SimControl::catchInt();
    }

    // Load the PTcl startup file ptcl.tcl
    return loadStartup(interp);
}
