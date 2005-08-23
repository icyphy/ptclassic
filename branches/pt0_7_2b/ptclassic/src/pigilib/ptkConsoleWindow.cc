static const char file_id[] = "ptkConsoleWindow.cc";

/* 
SCCS Version identification :
@(#)ptkConsoleWindow.cc	1.16	08/30/99

Copyright (c) 1990-1996 The Regents of the University of California.
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

 Programmers: Alan Kamas, Christopher Hylands
 Created: 5/27/93

Sets up the standard input to feed from the xterm that started up
pigi and feeds stdout to that same window.

*/

// Do the right thing for sol2 boolean defs.  compat.h must be included
// first so sys/types.h is included correctly.
#include "sol2compat.h"

// Include standard include files to prevent conflict with
// the type definition Pointer used by "rpc.h". BLE
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
// We must include ptk.h last because ptk.h includes tk.h which 
// eventually includes X11/X.h, which on Solaris2.4 
// there is a #define Complex 0, which causes no end of trouble.
#include "ptk.h"
}

// The following function is taken directly from tkMain.c
// We split up Tcl_Main () so that we can run the Ptolemy event loop
// and the tcl event loop (I think).

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
_ptkPrompt(Tcl_Interp *interp, int gotPartial) {
  /* Tcl_Interp *interp; */		/* Interpreter to use for prompting. */
  /* int gotPartial; */			/* Non-zero means there already
					 * exists a partial command, so use
					 * the secondary prompt. */
  char *promptCmd;
  int code;
  Tcl_Channel inChannel, outChannel, errChannel;

  inChannel = Tcl_GetStdChannel(TCL_STDIN);
  outChannel = Tcl_GetStdChannel(TCL_STDOUT);
  promptCmd = Tcl_GetVar(interp,
          (char *) (gotPartial ? "tcl_prompt2" : "tcl_prompt1"),
          TCL_GLOBAL_ONLY);
  if (promptCmd == NULL) {
defaultPrompt:
    if (!gotPartial && outChannel) {
      Tcl_Write(outChannel, "% ", 2);
    }
  } else {
    code = Tcl_Eval(interp, promptCmd);
    inChannel = Tcl_GetStdChannel(TCL_STDIN);
    outChannel = Tcl_GetStdChannel(TCL_STDOUT);
    errChannel = Tcl_GetStdChannel(TCL_STDERR);
    if (code != TCL_OK) {
      if (errChannel) {
	Tcl_Write(errChannel, interp->result, -1);
	Tcl_Write(errChannel, "\n", 1);
      }
      Tcl_AddErrorInfo(interp,
		       "\n    (script that generates prompt)");
      goto defaultPrompt;
    }
  }
  if (outChannel) {
    Tcl_Flush(outChannel);
  }
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

static Tcl_DString command;	/* Used to buffer incomplete commands being
				 * read from stdin. */

    /* ARGSUSED */
static void
_ptkStdinProc(ClientData /*clientData*/, int /*mask*/)
{
#define BUFFER_SIZE 4000
  char buffer[BUFFER_SIZE+1], *cmd/*, *args, *fileName*/;
  int code, gotPartial, tty, length;
  int exitCode = 0;
  Tcl_Channel inChannel, outChannel, errChannel;

  /*
   * Process commands from stdin until there's an end-of-file.  Note
   * that we need to fetch the standard channels again after every
   * eval, since they may have been changed.
   */

  gotPartial = 0;
  inChannel = Tcl_GetStdChannel(TCL_STDIN);
  outChannel = Tcl_GetStdChannel(TCL_STDOUT);
  tty = isatty(0);
  
  if (!inChannel) {
    goto done;
  }
  length = Tcl_Gets(inChannel, &command);
  if (length < 0) {
    goto done;
  }
  if ((length == 0) && Tcl_Eof(inChannel) && (!gotPartial)) {
    goto done;
  }

  /*
   * Add the newline removed by Tcl_Gets back to the string.
   */

  (void) Tcl_DStringAppend(&command, "\n", -1);
  cmd = Tcl_DStringValue(&command);
  if (!Tcl_CommandComplete(cmd)) {
    gotPartial = 1;
    goto prompt;
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
  inChannel = Tcl_GetStdChannel(TCL_STDIN);
  outChannel = Tcl_GetStdChannel(TCL_STDOUT);
  errChannel = Tcl_GetStdChannel(TCL_STDERR);
  Tcl_DStringFree(&command);
  if (code != TCL_OK) {
    if (errChannel) {
      Tcl_Write(errChannel, ptkInterp->result, -1);
      Tcl_Write(errChannel, "\n", 1);
    }
  } else if (tty && (*ptkInterp->result != 0)) {
    if (outChannel) {
      Tcl_Write(outChannel, ptkInterp->result, -1);
      Tcl_Write(outChannel, "\n", 1);
    }
  }


  /*
   * Output a prompt.
   */
  
prompt:
  if (tty) {
    _ptkPrompt(ptkInterp, gotPartial);
  }
  return;

  /*
   * Rather than calling exit, invoke the "exit" command so that
   * users can replace "exit" with some other command to do additional
   * cleanup on exit.  The Tcl_Eval call should never return.
   */
done:
  sprintf(buffer, "exit %d", exitCode);
  Tcl_Eval(ptkInterp, buffer);
}



void ptkConsoleWindow() {
  Tcl_DStringInit(&command);
#if TCL_MAJOR_VERSION < 8
  Tcl_CreateFileHandler(Tcl_GetFile((0),TCL_UNIX_FD),
			TK_READABLE, _ptkStdinProc, (ClientData) 0);
#else
  Tcl_CreateChannelHandler(Tcl_GetStdChannel(TCL_STDIN),
          TCL_READABLE, _ptkStdinProc, (ClientData) 0);
#endif
  _ptkPrompt(ptkInterp,0);
}
