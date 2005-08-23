static const char file_id[] = "ptclInit.cc";
/*******************************************************************
SCCS Version identification :
@(#)ptclInit.cc	1.7	10/28/99

Copyright (c) 1990-1999 The Regents of the University of California.
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

#if defined(PTLINUX) && defined(__i386__)
    // Fix for DECalendarQueue SIGFPE under x86 linux.
    // Note by W. Reimer: Glibc 2.1 does not support __setfpucw() any
    // longer. Instead there is a macro _FPU_SETCW. Actually, the whole
    // fix is not required any longer because in the default FPU control
    // word of libc 5.3.12 and newer (glibc) the interrupt mask bit for
    // invalid operation (_FPU_MASK_IM) is already set.
#if (_FPU_DEFAULT & _FPU_MASK_IM) == 0
#ifdef _FPU_SETCW
    { fpu_control_t cw = (_FPU_DEFAULT | _FPU_MASK_IM); _FPU_SETCW(cw); }
#else
    __setfpucw(_FPU_DEFAULT | _FPU_MASK_IM);
#endif /* _FPU_SETCW */
#endif /* (_FPU_DEFAULT & _FPU_MASK_IM) == 0 */
#endif /* defined(PTLINUX) && defined(__i386__) */

    PTcl *ptcl = new PTcl(interp);
    Tcl_CallWhenDeleted(interp, PTclDeleteProc, (ClientData) ptcl);

    if (isatty(0)) {		// set up interrupt handler
	SimControl::catchInt();
    }

    // Load the PTcl startup file ptcl.tcl
    return loadStartup(interp);
}
