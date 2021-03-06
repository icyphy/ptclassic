/*******************************************************************
SCCS version identification
$Id$

@Copyright (c) 1994-%Q% The Regents of the University of California.
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

 Programmer: Kennard White

********************************************************************/

/* Parts of this file are from tcl4.0, so:
 * Copyright (c) 1994-1995 Sun Microsystems, Inc.
 */

#ifndef lint
static char SccsId[] = "@(#)appinit.c	1.4 11/11/94";
#endif /* not lint */

#include "topFixup.h"

#include "errtrap.h"

#include "tk.h"
#include "topStd.h"
#include "topMsg.h"
#include "tkoct.h"

#include "oct.h"
#include "toct.h"

/*
 * The following variable is a special hack that is needed in order for
 * Sun shared libraries to be used for Tcl.
 */

extern int matherr();
int *tclDummyMathPtr = (int *) matherr;

/* Simple routine to return the full path name of the initialization file
 */
char *
tkvemInitFile(filename)
char *filename;
{
  char *ptolemy = getenv("PTOLEMY");
  if (ptolemy)
    sprintf(filename,"%s/lib/tcl/tkoct.tcl",ptolemy);
  else
    sprintf(filename,"/users/ptolemy/lib/tcl/tkoct.tcl");
  return filename;
}

/*
 * The following variable is a special hack that allows applications
 * to be linked using the procedure "main" from the Tk library.  The
 * variable generates a reference to "main", which causes main to
 * be brought in from the library (and all of Tk and Tcl with it).
 */

extern int main();
int *tclDummyMainPtr = (int *) main;
/*
 *----------------------------------------------------------------------
 *
 * main --
 *
 *	This is the main program for the application.
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

int
main(argc, argv)
    int argc;			/* Number of command-line arguments. */
    char **argv;		/* Values of command-line arguments. */
{
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
 * Results:
 *	Returns a standard Tcl completion code, and leaves an error
 *	message in interp->result if an error occurs.
 *
 * Side effects:
 *	Depends on the startup script.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_AppInit(interp)
    Tcl_Interp *interp;		/* Interpreter for application. */
{
    char initfilename[1024];

    Tk_Window main;

    main = Tk_MainWindow(interp);

    errProgramName("tkoct");
    errCore(1);

    if ( topMsgRegisterCmds(interp) != TCL_OK) {
	return TCL_ERROR;
    }
    if ( xpGedInit(interp) != TCL_OK) {
	return TCL_ERROR;
    }
    if ( toctInit(interp) != TCL_OK) {
	return TCL_ERROR;
    }

    if (Tcl_Init(interp) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tk_Init(interp) != TCL_OK) {
	return TCL_ERROR;
    }

    /*
     * Call Tcl_CreateCommand for application-specific commands, if
     * they weren't already created by the init procedures called above.
     */
    if ( Tcl_EvalFile(interp,tkvemInitFile(initfilename))!=TCL_OK ) {
	return TCL_ERROR;
    }

    /*
     * Specify a user-specific startup file to invoke if the application
     * is run interactively.  Typically the startup file is "~/.apprc"
     * where "app" is the name of the application.  If this line is deleted
     * then no user-specific startup file will be run under any conditions.
     */

    Tcl_SetVar(interp, "tcl_rcFileName", "~/.tkoctrc", TCL_GLOBAL_ONLY);
    return TCL_OK;
}
