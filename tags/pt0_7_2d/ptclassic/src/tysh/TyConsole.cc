/*******************************************************************
Define a main window object.  This object has a Tcl interpreter
with PTcl extensions associated with it, created by the constructor.

@(#)TyConsole.cc	1.19	08/29/99
Programmer: E. A. Lee

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

********************************************************************/

#include "TyConsole.h"
#include "InfString.h"
#if ITCL_MAJOR_VERSION == 2
#include <itk.h>
#endif
#include <unistd.h>             // getpid()
#include <stdio.h>

static void SetVersionInfo (Tcl_Interp *ptkInterp, char *Filename)
{
    InfString pid = (int)getpid();
    Tcl_VarEval(ptkInterp, "set TychoVersionInfo {", gVersion, \
	    " (proc. id. ", (char*)pid, ")}", (char *) NULL);
    Tcl_VarEval(ptkInterp, "set TychoBinaryInfo {", Filename, "}", \
	    (char *) NULL);
}

TyConsole::TyConsole(int argc, char **argv) {

  // Create an interpreter
  tyInterp = Tcl_CreateInterp();

  if (!tyInterp) {
    fprintf(stderr,"Failed to create Tcl interpreter!\n");
    exit(1);
  }
  ptcl = new PTcl(tyInterp);

  // our vers of Tk_AppInit
  if (appInit(tyInterp) != TCL_OK) {
    fprintf(stderr,
	"Failed to initialize Tcl, [incr Tcl], and/or Tk: %s\n",
	tyInterp->result);
    tyExit(1);
  }

  // Create the main Tk window
  char *appName = "tycho";
  char *appClass = "Tycho";

#if TK_MAJOR_VERSION <= 4 && TK_MINOR_VERSION < 1
  ptkw = Tk_CreateMainWindow(tyInterp, NULL, appName, appClass);
#else
  ptkw = Tk_MainWindow(tyInterp);
#endif
  if (!ptkw) {
    fprintf(stderr,"Failed to create Tk main window: %s\n", tyInterp->result);
    exit(1);
  }

#if TK_MAJOR_VERSION <= 4 && TK_MINOR_VERSION < 1
#else
  Tk_SetClass(ptkw, appClass);
  Tk_SetAppName(ptkw, appName);
#endif /* TK_MAJOR_VERSION <= 4 && TK_MINOR_VERSION < 1 */

  // Define Tcl and Tk extensions
  tytcl = new TyTcl(tyInterp, ptkw);

  // Command line arguments are made available in the Tcl variables "argc"
  // and "argv".
  InfString argcstring = argc-1;
  Tcl_SetVar(tyInterp, "argc", (char*)argcstring, TCL_GLOBAL_ONLY);
  char *args = Tcl_Merge(argc-1, argv+1);
  Tcl_SetVar(tyInterp, "argv", args, TCL_GLOBAL_ONLY);
  ckfree(args);

  // Set the "tcl_interactive" variable.
  int tty = isatty(0);
  if (tty) {		// set up interrupt handler
    // SimControl::catchInt(); 
    // I removed this to allow for killing of Tycho with ^C. - jking -
  }
  Tcl_SetVar(tyInterp, "tcl_interactive",
	     (char *) ((tty) ? "1" : "0"), TCL_GLOBAL_ONLY);

  // Set version/binary info variables
  SetVersionInfo(tyInterp, argv[0]);

  // Load the startup file Tycho.tcl
  char *ty = getenv("TYCHO");
  char *pt = getenv("PTOLEMY");
  InfString command;
  if (ty) {
    command << ty << "/kernel/Tycho.tcl";
  } else {
    if (pt) {
      command << pt << "/tycho/kernel/Tycho.tcl";
    } else {
      command << "~ptolemy/tycho/kernel/Tycho.tcl";
    }
  }	
  if (Tcl_EvalFile(tyInterp, (char*)command) != TCL_OK) {
    fprintf(stderr,
	"Unable to load Tycho.tcl startup file: %s\n ", tyInterp->result);
    tyExit(1);
  }
}

TyConsole::~TyConsole() {
  delete tyInterp;
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
  Tcl_GetBoolean(tyInterp, "ptkVerboseErrors", &ptkVerboseErrors);
  if (ptkVerboseErrors) {
    fprintf(stderr, "%s\n", Tcl_GetVar(tyInterp, "errorInfo",TCL_GLOBAL_ONLY));
    fflush(stderr);
  }

  InfString cmd = "exit ";
  cmd << code;
  Tcl_Eval(tyInterp, (char*)cmd);
  // In case the actual exit is deferred.
  cmd = "update";
  Tcl_Eval(tyInterp, (char*)cmd);
  fprintf(stderr,"Tcl exit command has been redefined, and fails to exit");
  exit(1);
}

int TyConsole::appInit(Tcl_Interp *ip) {
  if (Tcl_Init(ip) == TCL_ERROR)
    return TCL_ERROR;
  if (Tk_Init(ip) == TCL_ERROR)
    return TCL_ERROR;
  // Add [incr Tcl] (itcl) facilities
  if (Itcl_Init(ip) == TCL_ERROR) {
    return TCL_ERROR;
  }
#if ITCL_MAJOR_VERSION == 2
  if (Itk_Init(ip) == TCL_ERROR) {
    return TCL_ERROR;
  }
#endif
#if PTUSE_EXPECT
  // Add expect facilities
  if (Exp_Init(ip) == TCL_ERROR) {
    return TCL_ERROR;
  }
#endif 
  return TCL_OK;
}

Tcl_Interp* TyConsole::tclInterp() {
  return tyInterp;
}

Tk_Window* TyConsole::tkWindow() {
  return &ptkw;
}

PTcl* TyConsole::ptclInterp() {
  return ptcl;
}
