/* 
   The include file for CGC-generated packages

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
#include <stdlib.h>
#include "../tytimer/tytimer.h"


/*
 * tclinterface
 *
 * The interface procedure to Tcl. The first argument is one of
 * "setup," "execute",
 * or "wrapup." If "setup," a second argument is expcted, which is the
 * name of a Tcl variable which will be updated with the value of the
 * counter each time the task executes.
 */
static int
tclinterface(ClientData dummy, Tcl_Interp *interp, int argc, char **argv) {
  /* Check number of arguments */
  if (argc < 2) {
    sprintf(interp->result, "CGC Module %s: mode not specified", moduleName );
    return TCL_ERROR;
  }
  if (argc > 3) {
    sprintf(interp->result, "Too many arguments");
    return TCL_ERROR;
  }
     
  /* Switch on the first argument */
  if ( ! strcmp(argv[1], "scriptfile") ) {
    /* Return the name of the script to create the user interface */
    interp->result = scriptFile;
    return TCL_OK;

  } else if ( ! strcmp(argv[1], "init") ) {
    /* Call the function to register callbacks */
    tychoSetup();
    return TCL_OK;

  } else if ( ! strcmp(argv[1], "setup") ) {
    /* If a counter name is supplied, remember it */
    if ( argc > 2 ) {
      strcpy(tclcountername,argv[2]);
      updatetclcounter = 1;
    } else {
      updatetclcounter = 0;
    }
    /* Call the setup function */
    setup();
    return TCL_OK;
         
  } else if ( ! strcmp(argv[1], "execute") ) {
    int timedout;
         
         /* Start the timer */
    Ty_TimerStart();
         
    /* Call the execute function, which must check Ty_TimerElapsed()
         * return when it returns 1
         */
    timedout = execute();
         
    /* If there is a tcl counter, update it with the iteration count */
    if ( updatetclcounter ) {
      char buffer[80];
      sprintf(buffer,"%d", iterationCounter);
      Tcl_SetVar(interp, tclcountername, buffer, 0);
    }
         
    /* Return "1" if the timer timed out so we know that the task
         * didn't terminate; return "0" if the iterayion count was reached
         * so the scheduler can delete the task
         */
    if ( timedout ) {
      interp->result = "1";
    } else {
      interp->result = "0";
    }
    return TCL_OK;
         
  } else if ( ! strcmp(argv[1], "wrapup") ) {
    /* Call the wrapup function */
    wrapup();
    return TCL_OK;
         
  }

  /* If we get to here, something went wrong */
  sprintf(interp->result, "Invalid mode in module %s: %s", argv[1], moduleName);
  return TCL_ERROR;
}

