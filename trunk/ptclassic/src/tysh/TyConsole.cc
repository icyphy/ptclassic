/*******************************************************************
Define a main window object.  This object has a Tcl interpreter
with PTcl extensions associated with it, created by the constructor.
This main window will be converted to a console window by Lib.tcl.

$Id$
Programmer: E. A. Lee

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

********************************************************************/

#include "TyConsole.h"
#include "InfString.h"
#if ITCL_MAJOR_VERSION == 2
#include <itk.h>
#endif
#include <stdio.h>

TyConsole::TyConsole(int argc, char **argv) {

  // Create an interpreter
  interp = Tcl_CreateInterp();
  if (!interp) {
    fprintf(stderr,"Failed to create Tcl interpreter!\n");
    exit(1);
  }
  ptcl = new PTcl(interp);

  // Create the main Tk window
  char *appName = "tycho";
  char *appClass = "Tycho";

  ptkw = Tk_CreateMainWindow(interp, NULL, appName, appClass);
  if (!ptkw) {
    fprintf(stderr,"Failed to create Tk main window: %s\n", interp->result);
    exit(1);
  }

  // Define Tcl and Tk extensions
  tytcl = new TyTcl(interp, ptkw);

  // Process command line arguments.
  // Main function is to just make them available in the Tcl variables "argc"
  // and "argv".  However, in addition, if the first argument does not start
  // with a "-", then strip it off and use it as the name of a script file to
  // process.
  char *fileName = NULL;
  if ((argc > 1) && (argv[1][0] != '-')) {
    fileName = argv[1];
    argc--;
    argv++;
  }

  char *args = Tcl_Merge(argc-1, argv+1);
  Tcl_SetVar(interp, "argv", args, TCL_GLOBAL_ONLY);
  ckfree(args);
  InfString buf = argc-1;
  Tcl_SetVar(interp, "argc", (char*)buf, TCL_GLOBAL_ONLY);
  Tcl_SetVar(interp, "argv0", (fileName != NULL) ? fileName : argv[0],
	     TCL_GLOBAL_ONLY);

  // Set the "tcl_interactive" variable.
  int tty = isatty(0);
  if (tty) {		// set up interrupt handler
    SimControl::catchInt();
  }
  Tcl_SetVar(interp, "tcl_interactive",
	     ((fileName == NULL) && tty) ? "1" : "0", TCL_GLOBAL_ONLY);

  // our vers of Tk_AppInit
  if (appInit(interp, ptkw) != TCL_OK) {
    fprintf(stderr,
	"Failed to initialize Tcl, [incr Tcl], and/or Tk: %s\n",
	interp->result);
    tyExit(1);
  }

  // Load the startup file Lib.tcl
  char *pt = getenv("PTOLEMY");
  InfString command = pt ? pt : "~ptolemy";
  command << "/tycho/kernel/Lib.tcl";
  if (Tcl_EvalFile(interp, (char*)command) != TCL_OK) {
    fprintf(stderr,
	"Unable to load Lib.tcl startup file: %s\n ", interp->result);
    tyExit(1);
  }

// FIXME: tcl7.5a1 does not support tcl_RcFileName, see the changes file
#if TCL_MINOR_VERSION < 5
  // Source a user-specific startup file if Tcl_AppInit specified
  // one and if the file exists.
  if (tcl_RcFileName != NULL) {
    Tcl_DString buffer;

    char *fullName = Tcl_TildeSubst(interp, tcl_RcFileName, &buffer);
    if (fullName == NULL) {
      fprintf(stderr,
	"Tilde substitution failed in ~/.tycho: %s\n", interp->result);
    } else {
      if (access(fullName, R_OK) == 0) {
	if (Tcl_EvalFile(interp, fullName) != TCL_OK) {
	  fprintf(stderr,
		"Failed to source ~/.tycho: %s\n", interp->result);
	}
      }
    }
    Tcl_DStringFree(&buffer);
  }
#endif

  // If a script file was specified then just source that file and quit.
  if (fileName != NULL) {
    if (Tcl_EvalFile(interp, fileName) != TCL_OK) {
      fprintf(stderr, "Script file fails: %s\n", interp->result);
      tyExit(1);
    } else {
      tyExit(0);
    }
  }
}

TyConsole::~TyConsole() {
  delete interp;
  delete ptcl;
  delete tytcl;
}

void TyConsole::tyExit(int code) {
  // Rather than calling exit directly, invoke the Tcl "exit" command so that
  // users can replace "exit" with some other command to do additional
  // cleanup on exit.  The Tcl_Eval call should never return.
  // In case it does, we call exit anyway.

  // If ptkVerboseErrors is set, print out errorInfo on the way out
  int ptkVerboseErrors = 0;
  Tcl_GetBoolean(interp, "ptkVerboseErrors", &ptkVerboseErrors);
  if (ptkVerboseErrors) {
    fprintf(stderr, "%s\n", Tcl_GetVar(interp, "errorInfo",TCL_GLOBAL_ONLY));
    fflush(stderr);
  }

  InfString cmd = "exit ";
  cmd << code;
  Tcl_Eval(interp, (char*)cmd);
  // In case the actual exit is deferred.
  cmd = "update";
  Tcl_Eval(interp, (char*)cmd);
  fprintf(stderr,"Tcl exit command has been redefined, and fails to exit");
  exit(1);
}

int TyConsole::appInit(Tcl_Interp *ip, Tk_Window) {
  if (Tcl_Init(ip) == TCL_ERROR)
    return TCL_ERROR;
  if (Tk_Init(ip) == TCL_ERROR)
    return TCL_ERROR;
#if TCL_MINOR_VERSION < 5
  tcl_RcFileName = "~/.tycho";
#endif  
  // Add [incr Tcl] (itcl) facilities
  if (Itcl_Init(ip) == TCL_ERROR) {
    return TCL_ERROR;
  }
#if ITCL_MAJOR_VERSION == 2
  if (Itk_Init(interp) == TCL_ERROR) {
    return TCL_ERROR;
  }
#endif
  return TCL_OK;
}

Tcl_Interp* TyConsole::tclInterp() {
  return interp;
}

Tk_Window* TyConsole::tkWindow() {
  return &ptkw;
}

PTcl* TyConsole::ptclInterp() {
  return ptcl;
}
