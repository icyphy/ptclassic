/* User:      johnr
   Date:      Fri Jan 31 15:52:44 1997
   Target:    default-CGC
   Universe:  tychotest
*/

#include <stdlib.h>
#include <stdio.h>

#include "../tytimer/tyTimer.h"

extern main ARGS((int argc, char *argv[]));

/* State variables */
static FILE* fp_1[1];
static double output_0;
static int count_3;
static double index_4;
static count_3;
static index_4;
static output_0;
static int sdfLoopCounter_5;

/* main function */
int main(int argc, char *argv[]) {
  {
    int sdfLoopCounter_5;
    for (sdfLoopCounter_5 = 0; sdfLoopCounter_5 < 100000; sdfLoopCounter_5++) {
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
    }	
  } /* end repeat, depth 0*/

  return 1;
}


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

  /* Initialize the random number generator */
  srand(1);
  if(!(fp_1[0] = fopen("default-CGC_temp_20","w"))) {
    fprintf(stderr,"ERROR: cannot open output file for Xgraph star.\n");
    exit(1);
  }
}

/*
 * execute
 *
 * Execute the program until Ty_TimerElapsed() returns 1.
 */
void
execute() {
  for ( ; sdfLoopCounter_5 < 100000; sdfLoopCounter_5++) {
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
    /* If time has expired, return */
    if ( Ty_TimerElapsed() ) {
      return;
    }
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
  if (argc > 3) {
    sprintf(interp->result, "Too many arguments");
    return TCL_ERROR;
  }

  /* Switch on the first argument */
  if ( ! strcmp(argv[1], "setup") ) {
    /* Call the setup function */
    setup();
    return TCL_OK;

  } else if ( ! strcmp(argv[1], "execute") ) {
    /* Start the timer */
    Ty_TimerStart();
    
    /* Call the execute function, which must check Ty_TimerElapsed()
     * return when it returns 1
     */
    execute();

    /* Return "1" so we know that the task didn't terminate */
    interp->result = "1";
    return TCL_OK;

  } else if ( ! strcmp(argv[1], "wrapup") ) {
    /* Call the wrapup function */
    wrapup();
    return TCL_OK;

  } else {
    sprintf(interp->result, "Invalid mode: %s", argv[1]);
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
