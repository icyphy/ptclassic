/**************************************************************************
Top-level program for Tycho, a visual interface for Ptolemy.

$Id$
Programmer:  E. A. Lee

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
		       

This file is based on the correspoding file for ptcl, written by Joe
Buck, but with the addition of Tk and itcl.
The "itcl" extension by Michael Mclennan of AT&T Bell
Laboratories are also pulled in. 

**************************************************************************/
static const char file_id[] = "TyMain.cc";

#include "PTcl.h"
#include "Linker.h"
#include "SimControl.h"
#include "Error.h"
#include "SetSigHandle.h"
#include "InfString.h"
#include "TyConsole.h"
#include <stdio.h>
#include <sys/file.h>
#include <tcl.h>
#include <errno.h>
#include "ptk.h"

/*
 * Global variables
 */
Tcl_Interp *ptkInterp;
Tk_Window dummy;
Tk_Window &ptkW = dummy;
extern char *gVersion;

/*
 * Declarations for various library procedures and variables (don't want
 * to include tclUnix.h here, because people might copy this file out of
 * the Tcl source directory to make their own modified versions).
 */

extern "C" {
extern int		access _ANSI_ARGS_((CONST char *path, int mode));
extern int		errno;
extern int		isatty _ANSI_ARGS_((int fd));
extern char *		strcpy _ANSI_ARGS_((char *dst, CONST char *src));
};

char *tyFilename = NULL;

/* This defines the default domain for Tycho */

extern char DEFAULT_DOMAIN[];

static void PrintVersion ()
{
    InfString pid = (int)getpid();
    Tcl_VarEval(ptkInterp, "ptkStartupMessage {", gVersion, "(proc. id. ",
		(char*)pid, ")} {", tyFilename, "}", NULL);
}

static void PrintSigErrorMessage ()
{
    Tcl_VarEval(ptkInterp, "TyGrabMessage .message -text {Unable to set the \
signal handlers, the program will continue but if a serious error occurs it \
may not be handled appropriately.}", NULL);
}

/*
 *----------------------------------------------------------------------
 *
 * main -- open a console window and start the tk event loop.
 *
 *----------------------------------------------------------------------
 */

int
main(int argc, char **argv) {

    tyFilename = argv[0];

    // The following creates a Tcl interpreter, adds the ptcl and itcl 
    // extensions, and creates a top-level Tk window.
    TyConsole console(argc, argv);

    // Set the global variables that indicate the current interpreter
    // and window.
    ptkInterp = console.tclInterp();
    ptkW = *(console.tkWindow());

    // Initialize the incremental linking module
    Linker::init(argv[0]);

    PrintVersion();

    if (setSignalHandlers())
        PrintSigErrorMessage();

    Tk_MainLoop();
    console.tyExit(0);
}

