static const char file_id[] = "ptclAppInit.cc";
/*******************************************************************
SCCS Version identification :
@(#)ptclAppInit.cc	1.19	02/08/99

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
#ifdef PT_PTCL_WITH_TK
#include <tk.h>
#include <locale.h>
#endif
#include "Linker.h"

extern "C" int Ptcl_Init _ANSI_ARGS_((Tcl_Interp *interp));

#ifdef PT_PTCL_WITH_TK
#include "ptk.h"

Tk_Window ptkW;
#else
// We need this so we can include HOF in ptcl
Tcl_Interp *ptkInterp;
#endif

/*
 * The following variable is a special hack that is needed in order for
 * Sun shared libraries to be used for Tcl.
 */
#if !defined(PT_NT4VC)
EXTERN int matherr();

/* This line produces a warning under hppa.cfront:
 *  warning: pointer to function cast to pointer to non-function (149)
 */
int *tclDummyMathPtr = (int *) matherr;
#endif /* PT_NT4VC */

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

#ifdef PT_NT4VC

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN

static void		setargv _ANSI_ARGS_((int *argcPtr, char ***argvPtr));

#endif

#if defined(PT_NT4VC) && defined(PT_PTCL_WITH_TK)
int APIENTRY
WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
#else
int main(int argc, char **argv)
#endif
{
#ifdef PT_PTCL_WITH_TK
    TkConsoleCreate();
    
#ifdef PT_NT4VC
    // Parse command line arguments
    char **argv, *p;
    int argc;
    char buffer[MAX_PATH];

    setlocale(LC_ALL, "C");
    SetMessageQueue(64);
 
    setargv(&argc, &argv);
    GetModuleFileName(NULL, buffer, sizeof(buffer));
    argv[0] = buffer;
    for (p = buffer; *p != '\0'; p++) {
			if (*p == '\\') {
	  	  *p = '/';
			}
    }
#endif
    
#endif
 
    // Initialize the Ptolemy incremental linker module.
    // Note: this initialization routine must be invoked here in the
    // main() function rather than in Ptcl_Init() because it needs to
    // be given the name of the binary executable (argv[0]).
    // (Ptcl_Init() does not have access to argv.)
    Linker::init(argv[0]);

#ifdef PT_PTCL_WITH_TK
    Tk_Main(argc, argv, Tcl_AppInit);
#else
    Tcl_Main(argc, argv, Tcl_AppInit);
#endif
    
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

#ifdef PT_PTCL_WITH_TK
    if (Tk_Init(interp) == TCL_ERROR) {
	return TCL_ERROR;
    }
    Tcl_StaticPackage(interp, "Tk", Tk_Init, Tk_Init);
    ptkW = Tk_MainWindow(ptkInterp);

    /*
     * Initialize the console only if we are running as an interactive
     * application.
     */

    if (TkConsoleInit(interp) == TCL_ERROR) {
	return TCL_ERROR;
    }

	// Set name and class
    char *appName = "pigi";
    char *appClass = "Pigi";
    Tk_SetClass(ptkW, appClass);
    Tk_SetAppName(ptkW, appName);
#endif

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

#if (TCL_MAJOR_VERSION >= 7 && TCL_MINOR_VERSION >= 5) || (TCL_MAJOR_VERSION >=8)
    Tcl_SetVar(interp, "tcl_rcFileName", "~/.ptclrc", TCL_GLOBAL_ONLY);
#else
    tcl_RcFileName = "~/.ptclrc";
#endif /* TCL_MAJOR_VERSION >= 7 && TCL_MINOR_VERSION >= 5 */
    return TCL_OK;
}




#ifdef PT_NT4VC 

/*
 *-------------------------------------------------------------------------
 *
 * setargv --
 *
 *	Parse the Windows command line string into argc/argv.  Done here
 *	because we don't trust the builtin argument parser in crt0.  
 *	Windows applications are responsible for breaking their command
 *	line into arguments.
 *
 *	2N backslashes + quote -> N backslashes + begin quoted string
 *	2N + 1 backslashes + quote -> literal
 *	N backslashes + non-quote -> literal
 *	quote + quote in a quoted string -> single quote
 *	quote + quote not in quoted string -> empty string
 *	quote -> begin quoted string
 *
 * Results:
 *	Fills argcPtr with the number of arguments and argvPtr with the
 *	array of arguments.
 *
 * Side effects:
 *	Memory allocated.
 *
 *--------------------------------------------------------------------------
 */

static void
setargv(int *argcPtr, char ***argvPtr)
{
    char *cmdLine, *p, *arg, *argSpace;
    char **argv;
    int argc, size, inquote, copy, slashes;
    
    cmdLine = GetCommandLine();

    /*
     * Precompute an overly pessimistic guess at the number of arguments
     * in the command line by counting non-space spans.
     */

    size = 2;
    for (p = cmdLine; *p != '\0'; p++) {
	if (isspace(*p)) {
	    size++;
	    while (isspace(*p)) {
		p++;
	    }
	    if (*p == '\0') {
		break;
	    }
	}
    }
    argSpace = (char *) ckalloc((unsigned) (size * sizeof(char *) 
	    + strlen(cmdLine) + 1));
    argv = (char **) argSpace;
    argSpace += size * sizeof(char *);
    size--;

    p = cmdLine;
    for (argc = 0; argc < size; argc++) {
	argv[argc] = arg = argSpace;
	while (isspace(*p)) {
	    p++;
	}
	if (*p == '\0') {
	    break;
	}

	inquote = 0;
	slashes = 0;
	while (1) {
	    copy = 1;
	    while (*p == '\\') {
		slashes++;
		p++;
	    }
	    if (*p == '"') {
		if ((slashes & 1) == 0) {
		    copy = 0;
		    if ((inquote) && (p[1] == '"')) {
			p++;
			copy = 1;
		    } else {
			inquote = !inquote;
		    }
                }
                slashes >>= 1;
            }

            while (slashes) {
		*arg = '\\';
		arg++;
		slashes--;
	    }

	    if ((*p == '\0') || (!inquote && isspace(*p))) {
		break;
	    }
	    if (copy != 0) {
		*arg = *p;
		arg++;
	    }
	    p++;
        }
	*arg = '\0';
	argSpace = arg + 1;
    }
    argv[argc] = NULL;

    *argcPtr = argc;
    *argvPtr = argv;
}

#endif
