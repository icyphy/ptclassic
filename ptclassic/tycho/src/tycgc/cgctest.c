/*
@Copyright (c) 1997 The Regents of the University of California.
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
/* User:      johnr
   Date:      Fri Jan 31 15:52:44 1997
   Target:    default-CGC
   Universe:  tychotest
*/

#include <stdlib.h>
#include <stdio.h>

#include "../tytimer/tyTimer.h"

/* Name of this CGC module */
static char* moduleName = "cgctest";

/* Loop counters */
int numIterations = 30000;
int iterationCounter;

/* State variables */
static FILE* fp_1[1];
static double output_0;
static int count_3;
static double index_4;
static int sdfLoopCounter_5;

/* Tcl interface data */
static char
tclcountername[40];

static char
updatetclcounter;

/*
 * setup
 *
 * Initialize all data needed by the module
 */
void
setup() {
  /* Initialize the state variables and the loop counter */
  count_3=0;
  index_4=0.0;
  output_0 = 0.0;
  sdfLoopCounter_5=0;
  iterationCounter = 0;

  /* Initialize the random number generator */
  srand(1);
  if(!(fp_1[0] = fopen("default-CGC_temp_20","w"))) {
    fprintf(stderr,"ERROR: cannot open output file for Xgraph star.\n");
    exit(1);
  }
}

/*
 * wrapup
 *
 * Cleanup in preparation for exit.
 */
void
wrapup() {
  {
    int i;
    for (i = 0; i < 1; i++) fclose(fp_1[i]);
    system("( pxgraph -t 'Ptolemy Xgraph' -bb -tk =800x400 default-CGC_temp_20 ; /bin/rm -f default-CGC_temp_20) &");
  }
}

/*
 * execute
 *
 * Execute the program until Ty_TimerElapsed() returns 1. Return 1
 * if the task stopped because the timer elapsed; return 0 if the
 * task stopped because the iteration count was reached.
 */
int
execute() {
  while ( iterationCounter != numIterations ) {
    {
      /* star tychotest.IIDUniform1 (class CGCIIDUniform) */
      /* Generate a random number on the interval [0,1] and */
      /* map it into the interval [0.0,1.0] */
      double randomValue = 0.0;
      int randomInt = rand();
      double scale = 1.0 - (0.0);
      double center = (1.0 + (0.0))/2.0;

      /* RAND_MAX is an ANSI C standard constant */
      /* If not defined, then just use the lower 15 bits */
#ifdef RAND_MAX
      randomValue = ((double) randomInt) / ((double) RAND_MAX);
#else
      randomInt &= 0x7FFF;
      randomValue = ((double) randomInt) / 32767.0;
#endif
      output_0 = scale * (randomValue - 0.5) + center;
    }
    {
      /* star tychotest.XMgraph.input=11 (class CGCXMgraph) */
      if (++count_3 >= 0) {
	fprintf(fp_1[0],"%g %g\n", index_4,output_0);
      }	
      index_4 += 1.0;
    }
    /* If timer has expired, return */
    if ( Ty_TimerElapsed() ) {
      return 1;
    }
    iterationCounter++;
  }
  /* If we get to here, the iteration count was reached, so call
   * wrapup myself and return 0 */
  wrapup();
  return 0;
}

/*
 * tclinterface
 *
 * The interface procedure to Tcl. The first argument is one of
 * "setup," "execute",
 * or "wrapup." If "setup," a second argument is expcted, which is the
 * name of a Tcl variable which will be updated with the value of the
 * counter each time the task executes.
 */
int
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
  if ( ! strcmp(argv[1], "setup") ) {
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

  } else {
    sprintf(interp->result, "Invalid mode in module %s: %s", argv[1], moduleName);
    return TCL_ERROR;
  }
}


/*
 * Initialize the package. This adds the interface procs to Tcl.
 */
int
Cgctest_Init(Tcl_Interp *interp) {
 
  /* Adds a single interface command */
  Tcl_CreateCommand(interp, "cgctest", tclinterface,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  return TCL_OK;
}
