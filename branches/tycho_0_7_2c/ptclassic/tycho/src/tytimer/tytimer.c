/* 
   Artificial Tcl event loop using timers. Taken from Ptolemy's
   CGC/Tk library file, tkMain.c.

Authors: Edward A. Lee, John Reekie.

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
#include "tySched.h"


/***************************************************************
 * This section of code is taken from the original code used for
 * CGC/Tk system: $PTOLEMY/src/domains/cgc/tcltk/lib/tkMain.c
 */

/*
 * Include files for signal handling (a timer is used to trigger a signal
 * which in turn causes the tk event loop to be called at the next opportune
 * moment. Note, SIG_PF should be the same type as SIG_IGN and the same type
 * as the second parameter to signal().
 */

/*
 * Note, SIG_PF should be the same type as SIG_IGN and the same type
 * as the second parameter to signal()
 */
#if !defined(PTIRIX5) 

/* PTIRIX5 defines SIG_PF in <signal.h> */
#if defined(PTSUN4) || defined(PTSOL2)  || ! defined(__STDC__)

/* sun4, sol2 */
typedef void (*SIG_PF)();
#else
typedef void (*SIG_PF)(int);
#endif /* defined sun4, sol2 */
#endif /* !defined PTIRIX5 */

#include <sys/time.h>
#include <signal.h>

/*
Following is a set of procedures drawn from SimControl and ptsignals
to handle the tk event loop during a run using a timer and signals.
*/
#if defined(PTSOL2) || defined(PTIRIX5) || defined(PTLINUX) || defined(PTALPHA)
static void ptSafeSig( int SigNum ) {
  struct sigaction pt_alarm_action;
  sigaction( SigNum, NULL, &pt_alarm_action);
  pt_alarm_action.sa_flags |= SA_RESTART;
  sigaction( SigNum, &pt_alarm_action, NULL);
}
static void ptBlockSig ARGS((int SigNum)) {}
static void ptReleaseSig ARGS((int SigNum)) {}

#else 
#if defined(PTHPPA)
static void ptSafeSig(int foo) {}
static long signalmask = 0;
/* Don't use prototypes since the cc distributed with hppa
 * won't work with them
 */
static void ptBlockSig (int SigNum) {
  signalmask = sigblock(sigmask(SigNum)) | sigmask(SigNum);
}
static void ptReleaseSig (int SigNum) {
  /* remove this signal from the signal mask */
  signalmask &= ~(sigmask(SigNum));
  sigsetmask(signalmask);
}

#else
#if defined(PTSUN4)
static void ptBlockSig (int SigNum) {};
static void ptReleaseSig (int SigNum) {};
static void ptSafeSig (int SigNum) {};

#else
/*default is no assignment*/
static void ptBlockSig (int SigNum) {};
static void ptReleaseSig (int SigNum) {};
static void ptSafeSig( int SigNum ) {};

#endif /* PTSUN4 */
#endif /* PTHPPA */
#endif /* PTSOL2  PTIRIX5  PTLINUX PTALPHA */


/*
 * Ty_TimerElapsed
 *
 * This flag is set when the timer elapses
 */
static volatile int
Ty_TimerElapsed = 1;

/*
 * Ty_TickPeriod
 *
 * The period of the timer, in microseconds.
 */
static int
Ty_TimerPeriod = 20000;


/*
 * Ty_TimerDone
 *
 * Set the timer elapsed flag.
 */
static void
Ty_TimerDone() {
  Ty_TimerElapsed = 1;
}

/*
 * Ty_TimerPeriod
 *
 * Set the period of the scheduler timer.
*/
void
Ty_TimerPeriod( int ms ) {
  /* Maximum time allowed is 999 ms, minimum is 1 ms */
  if (ms > 999) {
    ms = 999;
  } else if (ms < 1) {
    ms = 1;
  }

  Ty_TimerPeriod = ms * 1000;
}

/*
 * Ty_TimerStart
 *
 * Start the scheduler timer running.
*/
void
Ty_TimerStart( ) {
  /* Reset the elapsed flag */
  Ty_TimerElapsed = 0;

  /* reset the timer - this cancels any current timing in progress */
  struct itimerval i;
  i.it_interval.tv_sec = i.it_interval.tv_usec = 0;
  i.it_value.tv_sec = 0;
  i.it_value.tv_usec = Ty_TimerPeriod;

  /* Call the handler function when the timer expires */
  signal(SIGALRM, Ty_TimerDone);

  /* Make the signal safe from interrupting system calls */
  ptSafeSig(SIGALRM);

  /* Block the signal so that it will not interrupt system calls */
  ptBlockSig(SIGALRM);

  /* Start the timer */
  setitimer(ITIMER_REAL, &i, 0);
}

/*
 * Ty_TimerStop
 *
 * Stop the timer.
 */
void
Ty_TimerStop()
{
  /* reset the timer - this cancels any current timing in progress */
  struct itimerval i;
  i.it_interval.tv_sec = i.it_interval.tv_usec = 0;
  i.it_value.tv_sec = i.it_value.tv_usec = 0;

  /* Stop it */
  setitimer(ITIMER_REAL, &i, 0);

  /* Let a pending signal get through */
  ptReleaseSig(SIGALRM);
  ptBlockSig(SIGALRM);

  /* Reset the timer elapsed flag */
  Ty_TimerElapsed = 0;
}

/*
 * Ty_TimerElapsed
 *
 * Test if the timer has elapsed. This call can be made multiple
 * times; the elapsed flag is not reset until the timer is started
 * again. Note that this call must be made in order to actually
 * allow the signal to get through.
 */
int
Ty_TimerElapsed() {
  /*
   * Turn off signal blocking briefly to allow a blocked signal
   * to get through and set Ty_TimerElapsed.
   */
  ptReleaseSig(SIGALRM);
  ptBlockSig(SIGALRM);

  /* Return the flag */
  return Ty_TimerElapsed;
}

/*
 * Ty_DoAllEvents
 *
 * Process all pending Tk events. This only does something if the timer
 * has gone off and there are pending Tk events. After processing
 * all events, reset the timer if the event loop is still active.
 */
void
Ty_DoAllEvents() {
  /*
   * Turn off signal blocking briefly to allow a blocked signal
   * to get through and set Ty__TimerElapsed.
   */
  ptReleaseSig(SIGALRM);
  ptBlockSig(SIGALRM);

  /* Process events only if the timer has elapsed */
  if (Ty_TimerElapsed) {

    /* Process all pending Tk events */
    while (Tcl_DoOneEvent(TK_DONT_WAIT | TK_ALL_EVENTS));

    /* Reset the timer elapsed flag */
    tyevent_TimerElapsed = 0;

    /* Restart the timer */
    Ty_TimerStart();
  }
}


/*
 * Ty_Scheduler
 *
 * The Tcl interface to the functions in this package.
 */
int
Ty_Scheduler (ClientData dummy, Tcl_Interp *interp, int argc, char **argv) {

  /* Check number of arguments */
  if (argc < 3) {
    sprintf(interp->result, "Expected arguments: mode ?value?");
    return TCL_ERROR;
  }
  
  /* Switch on the first argument */
  if ( ! strcmp(argv[1], "start") ) {
    /* Start the timer */
    int periodcopy;
    int period;

    /* If the timer is already running, generate an error */
    if ( ! Ty_TimerElapsed ) {
      interp->result = "Timer is already running";
      return TCL_ERROR;
    }

    /* If the period is given, use it just this once. */
    if (argc >= 3) {
      if ( sscanf(argv[2], "%d", &period) != 1 ) {
	sprintf(interp->result, "Integer period expected: %s", argv[2]);
	return TCL_ERROR;
      }
      periodcopy = Ty_TimerPeriod;
      Ty_TimerPeriod = period;
      Ty_TimerStart();
      Ty_TimerPeriod = periodcopy;
    } else {
      Ty_TimerStart();
    }
    return TCL_OK;

  } else if ( ! strcmp(argv[1], "stop") ) {
    /* Stop the timer */
    if ( ! Ty_TimerElapsed ) {
      Ty_TimerStop();
    }
    return TCL_OK;

  } else if ( ! strcmp(argv[1], "elapsed") ) {
    /* Test if the timer has elapsed */
    interp->result = Ty_TimerElapsed() ? 1 : 0;
    return TCL_OK;

  } else if ( ! strcmp(argv[1], "period") ) {
    /* Get or set the timer period */
    if (argc > 2) {
      int period;
      if ( sscanf(argv[2], "%d", &period) != 1 ) {
	sprintf(interp->result, "Integer period expected: %s", argv[2]);
	return TCL_ERROR;
      }
      Ty_TimerPeriod(period);
    }
    sprintf(interp->result, "%d", Ty_TimerPeriod / 1000);
    return TCL_OK;

  } else {
    sprintf(interp->result, "Invalid mode: %s", argv[1]);
    return TCL_ERROR;
  }
}


/*
 * Tysched_Init
 *
 * Initialize the package. This adds the interface proc to Tcl.
 */
int
Tyevent_Init(Tcl_Interp *interp) {
  
  Tcl_CreateCommand(interp, "scheduler", Ty_Scheduler,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  return TCL_OK;
}


