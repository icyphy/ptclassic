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


/*
 * connectControl
 *
 * Connect to a control by name.
*/
void
connectControl (char *starname, char *ctrlname, Tcl_CmdProc *callback)
{	
  /* Register the callback function with Tcl */
  sprintf(command, "%s.%s.Callback", starname, ctrlname);
  Tcl_CreateCommand (interp, command, callback,
		     (ClientData) 0, (void (*)()) NULL);

  /* Call Tcl to make the connection */
  sprintf(command,
	  "::tycho::connectControl %s %s %s.%s.Callback",
	  starname, ctrlname, starname, ctrlname);
  if(Tcl_Eval(interp, command) != TCL_OK) {
    printf("Cannot connect control %s.%s\n", starname, ctrlname);
  }
}

/*
 * Ty_CGC
 *
 * The Tcl interface to this package.
 */
int
Ty_CGC (ClientData dummy, Tcl_Interp *interp, int argc, char **argv) {
  /* Doesn't do anything... */
  interp-result = "Hey, don't call this!!";
  return TCL_ERROR;
}

/*
 * Tycgc_Init
 *
 * Initialize the package. This adds the interface proc to Tcl.
 */
int
Tycgc_Init(Tcl_Interp *interp) {
  
  Tcl_CreateCommand(interp, "cgc", Ty_CGC,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  return TCL_OK;
}
