/* 
   Example Tycho "task" written in C.

Authors: John Reekie.

Version: @(#)tyTimerTest.c	1.3 01/20/97

Copyright (c) 1990-1997 The Regents of the University of California.
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
#include "tyTest.h"


static volatile int
taskcounter = 0;

static char
tclcountername[40];

/*
 * Tytest_TimerTask
 *
 * A procedure that starts and stop a C "task" of the kind required by
 * tha Tycho scheduler. The first argument is one of "setup," "execute",
 * or "wrapup." If "setup," a second argument is expcted, which is the
 * name of a Tcl variable which will be updated with the value of the
 * counter eacj time the task executes.
 *
 * Any C module to be executed as a Tycho task (recommended for any
 * module that will take more than a few milliseconds to execute) should
 * be structured in a similar way.
 */
int
Tytest_TestTask(ClientData dummy, Tcl_Interp *interp, int argc, char **argv) {
  /* Check number of arguments */
  if (argc > 3) {
    sprintf(interp->result, "Too many arguments");
    return TCL_ERROR;
  }

  /* Switch on the first argument */
  if ( ! strcmp(argv[1], "setup") ) {
    /* Start the counter task. */
    taskcounter = 0;
    strcpy(tclcountername,argv[2]);
    return TCL_OK;

  } else if ( ! strcmp(argv[1], "execute") ) {
    char buffer[40];

    /* Start the timer */
    Ty_TimerStart();

    /* Loop while the timer is running */
    while ( ! Ty_TimerElapsed() ) {
      taskcounter++;
    }

    /* Update the Tcl variable and exit */
    sprintf(buffer,"%d", taskcounter);
    Tcl_SetVar(interp, tclcountername, buffer, 0);

    /* Return "1" so we know that the task didn't terminate */
    interp->result = "1";
    return TCL_OK;

  } else if ( ! strcmp(argv[1], "wrapup") ) {
    return TCL_OK;

  } else {
    sprintf(interp->result, "Invalid mode: %s", argv[1]);
    return TCL_ERROR;
  }
}
