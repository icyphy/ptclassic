static const char file_id[] = "ptclAppInit.cc";
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

This file is adapted from the tclAppInit.c files in the tcl7.4p1 and
tcl7.5a2 releases.  (The original copyright notice in tclAppInit.c is
included below.)  tclAppInit.c is modified into a C++ file, and #ifdef's
are added to make it work with both tcl7.4p1 and tcl7.5a2.  We added
the [incr Tcl] (itcl) extension and the Ptolemy/PTcl facilities.

*******************************************************************/

/* 
 * tclAppInit.c --
 *
 *	Provides a default version of the main program and Tcl_AppInit
 *	procedure for Tcl applications (without Tk).
 *
 * Copyright (c) 1993 The Regents of the University of California.
 * Copyright (c) 1994-1995 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#include <tcl.h>
#ifndef PT_NO_ITCL
#include <itcl.h>
#endif
#include "Linker.h"

EXTERN int		Ptcl_Init _ANSI_ARGS_((Tcl_Interp *interp));

// We need this so we can include HOF in ptcl
Tcl_Interp *ptkInterp;

/*
 * The following variable is a special hack that is needed in order for
 * Sun shared libraries to be used for Tcl.
 */

EXTERN int matherr();
/* This line produces a warning under hppa.cfront:
 *  warning: pointer to function cast to pointer to non-function (149)
 */
int *tclDummyMathPtr = (int *) matherr;

#if TCL_MAJOR_VERSION >= 7 && TCL_MINOR_VERSION >= 5
#ifdef TCL_TEST
EXTERN int		TclTest_Init _ANSI_ARGS_((Tcl_Interp *interp));
#endif /* TCL_TEST */
#endif /* TCL_MAJOR_VERSION >= 7 && TCL_MINOR_VERSION >= 5 */

/*
 *----------------------------------------------------------------------
 *
 * main --
 *
 *	This is the main program for the application.
 *
 * Arguments:
 *	argc: Number of command-line arguments.
 *	argv: Values of command-line arguments.
 *
 * Results:
 *	None: Tcl_Main never returns here, so this procedure never
 *	returns either.
 *
 * Side effects:
 *	Whatever the application does.
 *
 *----------------------------------------------------------------------
 */

int main(int argc, char **argv)
{
    // Initialize the Ptolemy incremental linker module.
    // Note: this initialization routine must be invoked here in the
    // main() function rather than in Ptcl_Init() because it needs to
    // be given the name of the binary executable (argv[0]).
    // (Ptcl_Init() does not have access to argv.)
    Linker::init(argv[0]);

    Tcl_Main(argc, argv, Tcl_AppInit);
    return 0;			/* Needed only to prevent compiler warning. */
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_AppInit --
 *
 *	This procedure performs application-specific initialization.
 *	Most applications, especially those that incorporate additional
 *	packages, will have their own version of this procedure.
 *
 * Arguments:
 *	interp: Interpreter for application.
 *
 * Results:
 *	Returns a standard Tcl completion code, and leaves an error
 *	message in interp->result if an error occurs.
 *
 * Side effects:
 *	Depends on the startup script.
 *
 *----------------------------------------------------------------------
 */

int Tcl_AppInit(Tcl_Interp *interp)
{
    // We need this so that we can use hof inside ptcl
    ptkInterp=interp;
    if (Tcl_Init(interp) == TCL_ERROR) {
	return TCL_ERROR;
    }

#if TCL_MAJOR_VERSION >= 7 && TCL_MINOR_VERSION >= 5
#ifdef TCL_TEST
    if (TclTest_Init(interp) == TCL_ERROR) {
	return TCL_ERROR;
    }
#endif /* TCL_TEST */
#endif /* TCL_MAJOR_VERSION >= 7 && TCL_MINOR_VERSION >= 5 */

    /*
     * Call the init procedures for included packages.  Each call should
     * look like this:
     *
     * if (Mod_Init(interp) == TCL_ERROR) {
     *     return TCL_ERROR;
     * }
     *
     * where "Mod" is the name of the module.
     */

#ifndef PT_NO_ITCL
    // Add [incr Tcl] (itcl) facilities
    if (Itcl_Init(interp) == TCL_ERROR) {
	return TCL_ERROR;
    }
#endif
    // Add PTcl commands
    if (Ptcl_Init(interp) == TCL_ERROR) {
	return TCL_ERROR;
    }

    /*
     * Call Tcl_CreateCommand for application-specific commands, if
     * they weren't already created by the init procedures called above.
     */

    /*
     * Specify a user-specific startup file to invoke if the application
     * is run interactively.  Typically the startup file is "~/.apprc"
     * where "app" is the name of the application.  If this line is deleted
     * then no user-specific startup file will be run under any conditions.
     */

#if TCL_MAJOR_VERSION >= 7 && TCL_MINOR_VERSION >= 5
    Tcl_SetVar(interp, "tcl_rcFileName", "~/.ptclrc", TCL_GLOBAL_ONLY);
#else
    tcl_RcFileName = "~/.ptclrc";
#endif /* TCL_MAJOR_VERSION >= 7 && TCL_MINOR_VERSION >= 5 */
    return TCL_OK;
}
