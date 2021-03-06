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
		       
 Programmer:  J. T. Buck
 Date of creation: 3/4/92

This is the main program for the ptcl interpreter.  Most of the code
here handles I/O and is copied from the "wish" Tcl/Tk interpreter by
John Ousterhout.

Eventually we will use streams instead and move this stuff into the PTcl
class.

**************************************************************************/
static const char file_id[] = "ptclMain.cc";

#include "compat.h"
#include "PTcl.h"
#include "Linker.h"
#include "SimControl.h"
#include "Error.h"
#include "StringList.h"
#include <stdio.h>

static void loadStartup(Tcl_Interp* interp);

#include <stdio.h>
#include <sys/file.h>
#include <tcl.h>
#include <itcl.h>
#include <errno.h>

#ifdef PTLINUX
#include <fpu_control.h>
#endif

#ifndef PTLINUX
/*
 * Declarations for various library procedures and variables (don't want
 * to include tclUnix.h here, because people might copy this file out of
 * the Tcl source directory to make their own modified versions).
 */

extern "C" {
extern int		access _ANSI_ARGS_((CONST char *path, int mode));
extern int		errno;
extern void		exit _ANSI_ARGS_((int status));
extern int		isatty _ANSI_ARGS_((int fd));

#ifndef PTAIX_XLC
extern char *		strcpy _ANSI_ARGS_((char *dst, CONST char *src));
#endif

};
#endif

/* Interpreter for application. */
static Tcl_Interp *interp;

/* Used to buffer incomplete commands being read from stdin. */
static Tcl_DString command;

/*
 * Name of a user-specific startup script to source if the application
 * is being run interactively (e.g. "~/.tclshrc").  Set by Tcl_AppInit.
 * NULL means don't source anything ever.
 */


#if TCL_MAJOR_VERSION >= 7 && TCL_MINOR_VERSION > 4
char *tcl_RcFileName = "~/.ptclrc";
#else
extern char *tcl_RcFileName;
#endif


/* This defines the default domain for ptcl. */
extern char DEFAULT_DOMAIN[];

/*
 *----------------------------------------------------------------------
 *
 * main --
 *
 *	This is the main program for a Tcl-based shell that reads
 *	Tcl commands from standard input.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Can be almost arbitrary, depending on what the Tcl commands do.
 *
 *----------------------------------------------------------------------
 */

int
main(int argc, char **argv) {
    char buffer[1000], *cmd, *args, *fileName;
    int code, gotPartial, tty;
    int exitCode = 0;

#ifdef PTLINUX
    // Fix for DECalendarQueue SIGFPE under linux.
    __setfpucw(_FPU_DEFAULT | _FPU_MASK_IM);
#endif

    /*
     * Name of a user-specific startup script to source if the application
     * is being run interactively (e.g. "~/.tclshrc").  Set by Tcl_AppInit.
     * NULL means don't source anything ever.
     */
    tcl_RcFileName = "~/.ptclrc";

    interp = Tcl_CreateInterp();
    Linker::init(argv[0]);	// initialize incremental link module
    PTcl ptcl(interp);

    /*
     * Make command-line arguments available in the Tcl variables "argc"
     * and "argv".  If the first argument doesn't start with a "-" then
     * strip it off and use it as the name of a script file to process.
     */

    fileName = NULL;
    if ((argc > 1) && (argv[1][0] != '-')) {
	fileName = argv[1];
	argc--;
	argv++;
    }
    args = Tcl_Merge(argc-1, argv+1);
    Tcl_SetVar(interp, "argv", args, TCL_GLOBAL_ONLY);
    ckfree(args);
    sprintf(buffer, "%d", argc-1);
    Tcl_SetVar(interp, "argc", buffer, TCL_GLOBAL_ONLY);
    Tcl_SetVar(interp, "argv0", (fileName != NULL) ? fileName : argv[0],
	    TCL_GLOBAL_ONLY);

    /*
     * Set the "tcl_interactive" variable.
     */

    tty = isatty(0);
    if (tty) {		// set up interrupt handler
	    SimControl::catchInt();
    }
    Tcl_SetVar(interp, "tcl_interactive",
	    ((fileName == NULL) && tty) ? "1" : "0", TCL_GLOBAL_ONLY);

    /*
     * Invoke application-specific initialization.
     */

    if (Tcl_AppInit(interp) != TCL_OK) {
	fprintf(stderr, "Tcl_AppInit failed: %s\n", interp->result);
    }

    // Add [incr Tcl] (itcl) facilities
    if (Itcl_Init(interp) == TCL_ERROR) {
      return TCL_ERROR;
    }

    /*
     * Always load the PTcl startup file
     */
    loadStartup(interp);

    /*
     * If a script file was specified then just source that file
     * and quit.
     */

    if (fileName != NULL) {
	code = Tcl_EvalFile(interp, fileName);
	if (code != TCL_OK) {
	    fprintf(stderr, "%s\n", interp->result);
	    exitCode = 1;
	}
	/* Cannot use a goto and label here, cfront1.0 fails to compile with:
	 *  sorry, not implemented: label in block with destructors
	 */  
	sprintf(buffer, "exit %d", exitCode);
	Tcl_Eval(interp, buffer);
	return 1;
    }

    /*
     * Process commands from stdin until there's an end-of-file.
     */

    gotPartial = 0;
    Tcl_DStringInit(&command);
    while (1) {
	clearerr(stdin);
	if (tty) {
	    char *promptCmd;

	    promptCmd = Tcl_GetVar(interp,
		gotPartial ? "tcl_prompt2" : "tcl_prompt1", TCL_GLOBAL_ONLY);
	    if (promptCmd == NULL) {
		if (!gotPartial) {
		    fputs("% ", stdout);
		}
	    } else {
		code = Tcl_Eval(interp, promptCmd);
		if (code != TCL_OK) {
		    fprintf(stderr, "%s\n", interp->result);
		    Tcl_AddErrorInfo(interp,
			    "\n    (script that generates prompt)");
		    if (!gotPartial) {
		      fputs("% ", stdout);
		    }
		}
	    }
	    fflush(stdout);
	}
	if (fgets(buffer, 1000, stdin) == NULL) {
	    if (ferror(stdin)) {
		if (errno == EINTR) {
#if TCL_MINOR_VERSION < 5
		    if (tcl_AsyncReady) {
			(void) Tcl_AsyncInvoke((Tcl_Interp *) NULL, 0);
		    }
#else
		    if ( Tcl_AsyncReady() ) {
			(void) Tcl_AsyncInvoke((Tcl_Interp *) NULL, 0);
		    }
#endif
		    clearerr(stdin);
		} else {
		    sprintf(buffer, "exit %d", exitCode);
 		    Tcl_Eval(interp, buffer);
		    return 1;
		}
	    } else {
		if (!gotPartial) {
		    sprintf(buffer, "exit %d", exitCode);
 		    Tcl_Eval(interp, buffer);
		    return 1;
		}
	    }
	    buffer[0] = 0;
	}
	cmd = Tcl_DStringAppend(&command, buffer, -1);
	if ((buffer[0] != 0) && !Tcl_CommandComplete(cmd)) {
	    gotPartial = 1;
	    continue;
	}

	gotPartial = 0;
	code = Tcl_RecordAndEval(interp, cmd, 0);
	Tcl_DStringFree(&command);
	if (code != TCL_OK) {
	    fprintf(stderr, "%s\n", interp->result);
	} else if (tty && (*interp->result != 0)) {
	    printf("%s\n", interp->result);
	}
    }

    /*
     * Rather than calling exit, invoke the "exit" command so that
     * users can replace "exit" with some other command to do additional
     * cleanup on exit.  The Tcl_Eval call should never return.
     */

    /* Cannot use a goto and label here, cfront1.0 fails to compile with:
     *  sorry, not implemented: label in block with destructors
     */  
    sprintf(buffer, "exit %d", exitCode);
    Tcl_Eval(interp, buffer);
    return 1;
}

// the following stub is here until we support the Gantt chart
// under the interpreter.

extern "C" int displayGanttChart(const char* file) {
	StringList cmd;
	cmd << "gantt " << file;
	system(cmd);
	return 0;
}

// this file loads in the startup PTcl code.  This will
// in term load the startup tcl code.

static void loadStartup(Tcl_Interp* interp) {
	char *pt = getenv("PTOLEMY");
	int newmemory = FALSE;
	if (!pt) {
	    pt = expandPathName("~ptolemy");
	    newmemory = TRUE;
	}
	StringList startup = pt;
	startup << "/lib/tcl/ptcl.tcl";
	if (Tcl_EvalFile(interp, startup.chars()) != TCL_OK) {
	    fprintf(stderr, "ptcl: error in startup file '%s'.\n",
		    interp->result);
	}
	if ( newmemory ) {
	    delete [] pt;
	}
}
