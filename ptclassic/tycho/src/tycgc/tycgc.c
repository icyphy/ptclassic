/* 
      Tycho's support for CGC-generated files.

Authors: John Reekie.

Version: $Id$

Copyright (c) 1997 The Regents of the University of California.
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
*/
#include "tycgc.h"


/* Tcl Interpreter for this application. */
static Tcl_Interp *interpreter;

/*
 * connectControl
 *
 * Connect to a control by name.
*/
void
connectControl (char *galaxyname, char *starname,
                char *ctrlname, Tcl_CmdProc *callback)
{
  char callbackname[100];
  char command[200];

  /* Register the callback function with Tcl */
  sprintf(callbackname, "::tycho::ptolemy.%s.%s.%s",
	  galaxyname, starname, ctrlname);
  Tcl_CreateCommand (interpreter, callbackname, callback,
		     (ClientData) 0, (void (*)()) NULL);

  /* Call Tcl to make the connection */
  sprintf(command,
	  "::tycho::ControlPanel::starConnect %s %s %s %s",
	  galaxyname, starname, ctrlname, callbackname);
  if(Tcl_Eval(interpreter, command) != TCL_OK) {
    printf("Cannot connect control %s.%s\n", starname, ctrlname);
  }
}

/*
 * Compatibility function. Can we make this bring up
 * a proper stack trace?
 */
void
errorReport(char *message)
{
   char command[1000];
   sprintf(command, "error { %s }", message);
   Tcl_Eval(interpreter, command);
}

/*
Bogus compatibility function
*/
void
displaySliderValue (char *win, char *name, char *value)
{
    ;
}

/*
 * Ty_CGC
 *
 * The Tcl interface to this package.
 */
int
Ty_CGC (ClientData dummy, Tcl_Interp *interp, int argc, char **argv) {
  /* Doesn't do anything... */
  interp->result = "Hey, don't call this!!";
  return TCL_ERROR;
}

/*
 * Tycgc_Init
 *
 * Initialize the package. This adds the interface proc to Tcl.
 */
int
Tycgc_Init(Tcl_Interp *interp) {
  interpreter = interp;

  Tcl_CreateCommand(interp, "cgc", Ty_CGC,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  return TCL_OK;
}
