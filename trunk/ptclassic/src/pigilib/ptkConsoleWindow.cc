static const char file_id[] = "ptkConsoleWindow.cc";
/* 
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

 Programmer: Alan Kamas 
 Created: 5/27/93

Sets up the standard input to feed from the xterm that started up
pigi and feeds stdout to that same window.

*/

/*
 * Do the right thing for sol2 boolean defs.  compat.h must be included
 * first so sys/types.h is included correctly.
 */
#include "sol2compat.h"

#include <stdio.h>
#include <stdlib.h>	/* for exit() */
#include <unistd.h>	/* for read() */
#ifdef PTSVR4
/* ptk.h eventually includes X11/Xlib.h which gets the C++ string.h,
 * and the extern C kills it, so include it first and it protects
 * itself.
 */
#include <X11/Xlib.h>
#endif /* PTSVR4 */

extern "C" {
#include "ptk.h"
}


// The following function is taken directly from tkMain.c


/*
 *----------------------------------------------------------------------
 *
 * Prompt --
 *
 *	Issue a prompt on standard output, or invoke a script
 *	to issue the prompt.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	A prompt gets output, and a Tcl script may be evaluated
 *	in interp.
 *
 *----------------------------------------------------------------------
 */

static void
_ptkPrompt(Tcl_Interp *interp, int partial) {
    /* Tcl_Interp *interp; */		/* Interpreter to use for prompting. */
    /* int partial; */			/* Non-zero means there already
					 * exists a partial command, so use
					 * the secondary prompt. */
    char *promptCmd;
    int code;

    promptCmd = Tcl_GetVar(interp,
	partial ? "tcl_prompt2" : "tcl_prompt1", TCL_GLOBAL_ONLY);
    if (promptCmd == NULL) {
	defaultPrompt:
	if (!partial) {
	    fputs("% ", stdout);
	}
    } else {
	code = Tcl_Eval(interp, promptCmd);
	if (code != TCL_OK) {
	    Tcl_AddErrorInfo(interp,
		    "\n    (script that generates prompt)");
	    fprintf(stderr, "%s\n", interp->result);
	    goto defaultPrompt;
	}
    }
    fflush(stdout);
}

/*
 *----------------------------------------------------------------------
 *
 * StdinProc --
 *
 *	This procedure is invoked by the event dispatcher whenever
 *	standard input becomes readable.  It grabs the next line of
 *	input characters, adds them to a command being assembled, and
 *	executes the command if it's complete.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Could be almost arbitrary, depending on the command that's
 *	typed.
 *
 *----------------------------------------------------------------------
 */

static Tcl_DString _ptkStdinCmdStr;

    /* ARGSUSED */
static void
_ptkStdinProc(ClientData /*clientData*/, int /*mask*/) {
#define BUFFER_SIZE 4000
    char input[BUFFER_SIZE+1];
    static int gotPartial = 0;
    char *cmd;
    int code, count;
    int tty = 1;

    count = read(fileno(stdin), input, BUFFER_SIZE);
    if (count <= 0) {
	if (!gotPartial) {
	    if (tty) {
		Tcl_Eval(ptkInterp, "exit");
		exit(1);
	    } else {
		Tk_DeleteFileHandler(0);
	    }
	    return;
	} else {
	    count = 0;
	}
    }
    cmd = Tcl_DStringAppend(&_ptkStdinCmdStr, input, count);
    if (count != 0) {
	if ((input[count-1] != '\n') && (input[count-1] != ';')) {
	    gotPartial = 1;
	    goto prompt;
	}
	if (!Tcl_CommandComplete(cmd)) {
	    gotPartial = 1;
	    goto prompt;
	}
    }
    gotPartial = 0;

    /*
     * Disable the stdin file handler while evaluating the command;
     * otherwise if the command re-enters the event loop we might
     * process commands from stdin before the current command is
     * finished.  Among other things, this will trash the text of the
     * command being evaluated.
     */

    Tk_CreateFileHandler(0, 0, _ptkStdinProc, (ClientData) 0);
    code = Tcl_RecordAndEval(ptkInterp, cmd, 0);
    Tk_CreateFileHandler(0, TK_READABLE, _ptkStdinProc, (ClientData) 0);
    Tcl_DStringFree(&_ptkStdinCmdStr);
    if (*ptkInterp->result != 0) {
	if ((code != TCL_OK) || (tty)) {
	    printf("%s\n", ptkInterp->result);
	}
    }

    /*
     * Output a prompt.
     */

    prompt:
    if (tty) {
	_ptkPrompt(ptkInterp, gotPartial);
    }
}



void ptkConsoleWindow() {
    Tcl_DStringInit(&_ptkStdinCmdStr);
    Tk_CreateFileHandler(0, TK_READABLE, _ptkStdinProc, (ClientData) 0);
    _ptkPrompt(ptkInterp,0);
}
