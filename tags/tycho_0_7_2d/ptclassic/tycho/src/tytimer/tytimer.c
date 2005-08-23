/* 
   Support for timing tasks in Tycho. Adapted from Ptolemy's
   SimControl.cc and cgc/tcltk/lib/tkMain.c.

Authors: Edward A. Lee, John Reekie.

Version: @(#)tytimer.c	1.8 04/29/98

Copyright (c) 1997-1998 The Regents of the University of California.
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
#include "tytimer.h"

/* The section below is copied from $PTOLEMY/src/compat/ptolemy/compat.h
 * Users might not have Ptolemy, so we can't just include that here.
 * FIXME: the real fix would be to modify configure.in so that it
 * figures out the architecture.
 */
/***************************************************************************/
/* Define #defines for each Ptolemy architecture. (Alphabetical, please) */

#if defined(_AIX)
/* IBM rs6000 and powerpc running AIX */
#define PTAIX
#endif

#if defined(_AIX) && ! defined(__GNUC__)
/* AIX with IBM xlc and xlC */
#define PTAIX_XLC
#endif

#if defined(__alpha)
/* DEC Alpha */
#define PTALPHA
#endif
  
#if defined(freebsd)
/* FreeBSD */
#define PTFREEBSD
#endif

#if defined(hpux) || defined(__hpux)
/* HP PA, cfront or g++ */
#define PTHPPA
#endif

#if defined(PTHPPA) && ! defined(__GNUC__)
/* HP PA, cfront only, not g++ */
#define PTHPPA_CFRONT
#endif

#if defined(__sgi) && defined(IRIX5)
/* SGI running IRIX5.x */
#define PTIRIX5
#endif

#if defined(__sgi) && ! defined(__GNUC__)
/* SGI running IRIX5.x with native SGI CC*/
#define PTIRIX5_CFRONT
#endif

#if defined(__sgi) && ! defined(__GNUC__) && defined(IRIX6) && defined (_ABIO32)
/* SGI running IRIX6.x , native SGI CC in 32 bits mode */
#define PTIRIX5			/* SGI says it is 100% compatible with IRIX5 */
#define PTIRIX6
#define PTIRIX6_32_CFRONT
#endif

#if defined(__sgi) && ! defined(__GNUC__) && defined(IRIX6) && defined (_ABI64)
/* SGI running IRIX6.x , native SGI CC in 64 bit mode */
#define PTIRIX6
#define PTIRIX6_64_CFRONT
#endif

#if defined(linux)
#define PTLINUX
#endif

#if defined(PTLINUX) && defined(__ELF__)
/* Shorthand for Linux and ELF */
#define PTLINUX_ELF
#endif

#if defined(netbsd_i386)
#define PTNBSD_386
#endif

#if defined(sparc) && (defined(__svr4__) || defined(__SVR4))
/* Sun SPARC running Solaris2.x, SunC++ or g++ */
#ifndef SOL2
#define SOL2
#endif

#define PTSOL2
#endif

#if defined(sparc) && (defined(__svr4__) || defined(__SVR4)) && !defined(__GNUC__)
/* Sun SPARC running Solaris2.x, with something other than gcc/g++ */
#define PTSOL2_CFRONT
#endif

#if defined(sparc) && !(defined(__svr4__) || defined(PTSOL2_CFRONT))
/* Really, we mean sun4 running SunOs4.1.x, Sun C++ or g++ */
#define SUN4
#define PTSUN4
#endif

#if defined(sparc) && !defined(__svr4__) && !defined(__GNUC__)
/* Really, we mean sun4 running SunOs4.1.x with something other than gcc/g++ */
#define SUN4
#define PTSUN4_CFRONT
#endif

/* Unixware1.1
*#define PTSVR4
*/

#if defined(ultrix)
/* DEC MIPS running Ultrix4.x */
#define PTULTRIX
#define PTMIPS
#endif

/* End of text copied from compat.h */

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
static void ptBlockSig (int SigNum) {}
static void ptReleaseSig (int SigNum) {}

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
 * timerElapsed
 *
 * This flag is set when the timer elapses
 */
static volatile int
timerElapsed = 1;

/*
 * timerSeconds, timerMicroSeconds
 *
 * The period of the timer, in seconds and microseconds.
 */
static int
timerSeconds = 0;

static int
timerMicroSeconds = 20000;


/*
 * Ty_TimerDone
 *
 * Set the timer elapsed flag.
 */
static void
Ty_TimerDone() {
  timerElapsed = 1;
}

/*
 * Ty_TimerPeriod
 *
 * Set the period of the scheduler timer.
*/
void
Ty_TimerPeriod( int period ) {
  /* Minimum time is 1 ms, no maximum */
  if (period < 1) {
    period = 1;
  }

  period = period * 1000;
  timerSeconds = period / 1000000;
  timerMicroSeconds = period % 1000000;
}

/*
 * Ty_TimerStart
 *
 * Start the scheduler timer running.
*/
void
Ty_TimerStart( ) {
  struct itimerval i;

  /* Reset the elapsed flag */
  timerElapsed = 0;

  /* reset the timer - this cancels any current timing in progress */
  i.it_interval.tv_sec = i.it_interval.tv_usec = 0;
  i.it_value.tv_sec = timerSeconds;
  i.it_value.tv_usec = timerMicroSeconds;

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

  /* Set the timer elapsed flag */
  timerElapsed = 1;
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
  return timerElapsed;
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
  if (timerElapsed) {

    /* Process all pending Tk events */
    while (Tcl_DoOneEvent(TK_DONT_WAIT | TK_ALL_EVENTS));

    /* Reset the timer elapsed flag */
    timerElapsed = 0;

    /* Restart the timer */
    Ty_TimerStart();
  }
}


/*
 * Ty_Timer
 *
 * The Tcl interface to the functions in this package.
 */
int
Ty_Timer (ClientData dummy, Tcl_Interp *interp, int argc, char **argv) {

  /* Check number of arguments */
  if (argc < 2) {
    sprintf(interp->result, "Expected arguments: mode ?value?");
    return TCL_ERROR;
  }
  
  /* Switch on the first argument */
  if ( ! strcmp(argv[1], "start") ) {
    /* Start the timer */
    int tempsecs, tempusecs;
    int period;

    /* If the timer is already running, generate an error */
    if ( ! timerElapsed ) {
      interp->result = "Timer is already running";
      return TCL_ERROR;
    }

    /* If the period is given, use it just this once. */
    if (argc >= 3) {
      if ( sscanf(argv[2], "%d", &period) != 1 ) {
	sprintf(interp->result, "Integer period expected: %s", argv[2]);
	return TCL_ERROR;
      }
      tempsecs = timerSeconds;
      tempusecs = timerMicroSeconds;
      Ty_TimerPeriod(period);
      Ty_TimerStart();
      timerSeconds = tempsecs;
      timerMicroSeconds = tempusecs;
    } else {
      Ty_TimerStart();
    }
    return TCL_OK;

  } else if ( ! strcmp(argv[1], "stop") ) {
    /* Stop the timer */
    if ( ! timerElapsed ) {
      Ty_TimerStop();
    }
    return TCL_OK;

  } else if ( ! strcmp(argv[1], "elapsed") ) {
    /* Test if the timer has elapsed */
    interp->result = timerElapsed ? "1" : "0";
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
    sprintf(interp->result, "%d", timerSeconds*1000 + timerMicroSeconds/1000);
    return TCL_OK;

  } else {
    sprintf(interp->result, "Invalid mode: %s", argv[1]);
    return TCL_ERROR;
  }
}


/*
 * Tytimer_Init
 *
 * Initialize the package. This adds the interface proc to Tcl.
 */
int
Tytimer_Init(Tcl_Interp *interp) {
  
  Tcl_CreateCommand(interp, "timer", Ty_Timer,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  return TCL_OK;
}


