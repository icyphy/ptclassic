/*
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

/*
File to include in generated C code to use a Tk control panel.
It assumes that the following global variables have been defined:

char* name            the name of the application 
int   numIterations   default number of iterations

Author: Edward A. Lee
Version: @(#)tkMain.c	1.34	   02/23/97
*/

#include "tk.h"
#include "tcl.h"
#define COMMANDSIZE 1024
#define REPORT_TCL_ERRORS 1

/* 
 * Include files for signal handling (a timer is used to trigger a signal
 * which in turn causes the tk event loop to be called at the next oportune
 * moment. Note, SIG_PF should be the same type as SIG_IGN and the same type
 * as the second parameter to signal().
 * FIXME: I have only included the sun4, sol2 versions.  See SimControl.cc.
 */
#include "compat.h"

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

/* Global variables used by the main program */

/*
The main Tk window for the application.
If NULL, then the application no longer exists.
*/
static Tk_Window w;

/* Tcl Interpreter for this application. */
static Tcl_Interp *interp;

static char command[COMMANDSIZE];
char *appClass = "CGC";

/* No command line arguments are used, so use default settings */
char *display = NULL;

/*
 * Forward declarations for procedures defined later in this file:
 */

static void DelayedMap _ANSI_ARGS_((ClientData clientData));
static void StructureProc
		_ANSI_ARGS_((ClientData clientData, XEvent *eventPtr));

/* Command used to initialize Tk */

static char initCmd[] = "source $env(PTOLEMY)/src/domains/cgc/tcltk/lib/ptolemy-cgc.tcl";
static int runFlag = 0;
static int dontExit = 0;
static int exitRequest = 0;

void tkSetup();
/*
 *----------------------------------------------------------------------
 * Error handler
 *----------------------------------------------------------------------
 */

void errorReport(message)
char *message;
{
    char *msg, *cmd;
    sprintf(command, "popupMessage .error { %s }", message);
    Tcl_Eval(interp, command);
#if REPORT_TCL_ERRORS
    msg = Tcl_GetVar(interp, "errorInfo", TCL_GLOBAL_ONLY);
    if (msg == NULL) {
	msg = interp->result;
    }
    /* Have to be careful here to backslash curly braces in the string */
    strcpy(command, "popupMessage .error { ");
    cmd = command;
    cmd += strlen(command);
    if (msg != NULL)
      while (*msg != '\0') {
	if (cmd - command > COMMANDSIZE - 5) break;
	if ((*msg == '{') || (*msg == '}')) *cmd++ = '\\';
	*cmd++ = *msg++;
      }
    *cmd++ = ' ';
    *cmd++ = '}';
    *cmd = '\0';

    if(Tcl_Eval(interp, command) != TCL_OK)
	fprintf(stderr, "%s\n", msg);
#endif
}

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
static void ptSafeSig ARGS(( int foo )) {}
static long signalmask = 0;
/* Don't use prototypes since the cc distributed with hppa
 * won't work with them
 */
static void ptBlockSig (SigNum) {
int Signum;
        signalmask = sigblock(sigmask(SigNum)) | sigmask(SigNum);
}
static void ptReleaseSig (SigNum) {
int Signum;
	/* remove this signal from the signal mask */
	signalmask &= ~(sigmask(SigNum));
	sigsetmask(signalmask);
}

#else
#if defined(PTSUN4)
static void ptBlockSig ARGS((int SigNum)) {};
static void ptReleaseSig ARGS((int SigNum)) {};
static void ptSafeSig ARGS((int SigNum)) {};

#else
/*default is no assignment*/
static void ptBlockSig (int SigNum) {};
static void ptReleaseSig (int SigNum) {};
static void ptSafeSig( int SigNum ) {};

#endif /* PTSUN4 */
#endif /* PTHPPA */
#endif /* PTSOL2  PTIRIX5  PTLINUX PTALPHA */

/* The following conditional is needed because cc on Sun */
/* does not understand VOLATILE. */

static VOLATILE int pollflag = 0;

static void setPollFlag() {
	pollflag = 1;
}

static int
handleExit(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
  if (dontExit == 1) {
    /* Defer exiting until dontExit is cleared */
    exitRequest = 1;
    return TCL_OK;
  }
  else {
    /* Go ahead and exit right now */
    exit(0);
  }
  /* Should never be reached, but just in case... */
  runFlag = 0;
  return TCL_OK;
}

/*
Procedure to set the timer.
Based on a similar procedure in SimControl.
*/
static void setPollTimer( seconds, micro_seconds )
int seconds;
int micro_seconds;
{

	/* reset the timer - this cancels any current timing in progress */
	struct itimerval i;
	i.it_interval.tv_sec = i.it_interval.tv_usec = 0;
	i.it_value.tv_sec = seconds;
	i.it_value.tv_usec = micro_seconds;

	/* Turn on the poll flag when the timer expires */
	signal(SIGALRM, (SIG_PF)setPollFlag);

	/* Make the signal safe from interrupting system calls */
	ptSafeSig(SIGALRM);

	/* Block the signal so that it will not interrupt system calls */
	ptBlockSig(SIGALRM);

	/* Turn off the poll flag until the timer fires */
	pollflag = 0;

	/* Start the timer */
	setitimer(ITIMER_REAL, &i, 0);
}

/*
Procedure that processes Tk events during a run.
Based on a similar procedure in pigi.
*/
static int runEventsOnTimer ()
{
	int sec = 0,
	    usec = 50000;	/* every 50,000 micro seconds = 20 times/sec */

	/* Process all pending events */
	while (Tk_DoOneEvent(TK_DONT_WAIT|TK_ALL_EVENTS));

	/* Reset the Timer so that it will fire again */
	setPollTimer( sec, usec);

	return 1;
}

typedef int (*SimHandlerFunction)();

static SimHandlerFunction onPoll = 0;

/*
Register a function to be called if the poll flag is set.
Returns old handler if any.
*/
SimHandlerFunction setPollAction(f)
SimHandlerFunction f;
{
	SimHandlerFunction ret;
	ret = onPoll;
	onPoll = f;
	pollflag = 1;	/* Makes sure onPoll can get called */
	return ret;
}

/*
Get the value of the pollflag.  Turn off signal blocking briefly
to allow a blocked signal to get through.
*/
static int getPollFlag() {
	ptReleaseSig(SIGALRM);
	ptBlockSig(SIGALRM);
	return pollflag;
}

/* React according to how the pollflag is set */
static void processFlags() {
	if (pollflag != 0) {
		/* check to see if a polling function is defined */
		if (onPoll) {
		    /*
		     * polling function defined.  Reset the polling
		     * flag only if the polling function has failed
		     * otherwise it is assumed that the polling function
		     * will handle resetting the flag.
		     */
		    if (!onPoll()) 
			pollflag = 0;
		} else {
		    /* There is no polling function defined.  Reset the flag. */
		     pollflag = 0;
		}
	}
}


/* Command to start running the Ptolemy system */
static int
goCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
	if(runFlag == 1) return TCL_OK;	/* ignore if already running */
	/* If paused, just reset the run flag and return */
	if(runFlag == -1) {
	    runFlag = 1;
	    return TCL_OK;
	}
	runFlag = 1;

	/* First make sure Tcl grabs the latest number */
	/* of iterations from the control panel */
	if(Tcl_Eval(interp, "updateIterations") != TCL_OK) {
	    runFlag = 0;
	    return TCL_ERROR;
	}

	/* Then convert the resulting string to an integer. */
	if(sscanf(interp->result,"%d",&numIterations) != 1) {
	    runFlag = 0;
	    errorReport("Invalid number of iterations");
	    return TCL_OK;
	}

	setPollAction(runEventsOnTimer);

	/* Temporarily disallow exiting */
	dontExit = 1;
	go(argc, argv);

	/*
	  If DISMISS was pressed, then runFlag should
	  have been set to 0, causing a break out of go(),
	  and here is where we do an exit if DISMISS was
	  in fact what brings us to this point.

	  We want to make sure that the wrapup C code is executed
	  (for instance, tearing down sockets when running with
	  VHDL, so that ptvhdlsim is exited) before we exit entirely.
	  It should work like this:  runFlag = 0 causes go() to break
	  out of its while loop and then execute the wrapup stage.
	  We need to give the C code time to exit out of go() and return
	  to this point before doing the global exit, if DISMISS was
	  in fact pressed.  Formerly, DISMISS caused an immediate exit
	  when the event was handled, which prevented wrapup C code
	  from being executed.

	  However, we also need to handle the case where DISMISS
	  wasn't pressed in the middle of a go() call, so we only
	  throttle the exit if it's during a run.  Tricky.
	  */

	/* Clear restriction on exiting */
	dontExit = 0;

	/* If an exit was requested during go(), then do it now */
	if (exitRequest == 1) handleExit();

	setPollAction(NULL);

	Tcl_Eval(interp,".go configure -relief raised");
	runFlag = 0;
	return TCL_OK;
}

/* Command to stop running the Ptolemy system */
static int
stopCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
	runFlag = 0;
	return TCL_OK;
}


/* Command to pause running the Ptolemy system */
static int
pauseCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
	/* If stopped already, this has no effect */
	if (runFlag == 1) runFlag = -1;
	return TCL_OK;
}


/* Convenience utilities for standard Tk operations */

/*
Entries in a control panel

This function creates an entry box in a control panel and attaches
a callback function to it.  The callback function will be called whenever
the user enters a Return in the box.  The argument to the callback
function will be the entry that the user has put in the control box.
The return value of the callback function should be TCL_OK.

Arguments:
char* win	Name of window into which to put the control
char* name	Name to use for the control pane
char* desc	Description of the control
char* initValue	Initial value of the control
int* callback	Callback procedure to use on update

Make an entry in the main control panel to control a state value.
The name must be unique (e.g. derived from the star name).
The description is an arbitrary string
*/
void makeEntry (win, name, desc, initValue, callback)
    char *win, *name, *desc, *initValue;
    Tcl_CmdProc *callback;
{
    sprintf(command, "%sCallback", name);
    Tcl_CreateCommand(interp, command, callback,
		      (ClientData) 0, (void (*)()) NULL);
    sprintf(command, "makeEntry %s %s \"%s\" \"%s\" %sCallback",
	    win, name, desc, initValue, name);
    if(Tcl_Eval(interp, command) != TCL_OK) {
	errorReport("Cannot make entry box in control panel.");
    }
}

/*
Buttons in the control panel

This function creates a push button in a control panel and attaches
a callback function to it.  The callback function will be called whenever
the user pushes the button.
The return value of the callback function should be TCL_OK.

Arguments:
char* win	Name of window into which to put the control
char* name	Name to use for the control pane
char* desc	Description of the control
int* callback	Callback procedure to use on update

Make a button in the main control panel.
The name must be unique (e.g. derived from the star name).
The description is an arbitrary string
*/
void makeButton (win, name, desc, callback)
    char *win, *name, *desc;
    Tcl_CmdProc *callback;
{
    sprintf(command, "%sCallback", name);
    Tcl_CreateCommand(interp, command, callback, (ClientData) 0, NULL);
    sprintf(command, "makeButton %s %s \"%s\" %sCallback",
	win, name, desc, name);
    if(Tcl_Eval(interp, command) != TCL_OK) {
	errorReport("Cannot make button in control panel.");
    }
}

/* Get a float entry from the main control panel */

/*
Scales in the control panel

This function creates a scale (with slider) in a control panel and attaches
a callback function to it.  The callback function will be called whenever
the user moves the slider.  The argument to the callback
function will be the position of the slider, which can range from
0 to 100.  The return value of the callback function should be TCL_OK.

The scale has room to its left for displaying the value associated with
it, but it is up to the user of the scale to update this value. This
can be done using the displaySliderValue function below.

Arguments:
char* win	Name of window into which to put the control
char* name	Name to use for the control pane
char* desc	Description of the control
int position	Initial position of the slider (between 0 and 100)
int* callback	Callback procedure to use on update

Make a scale slider.
*/
void makeScale (win, name, desc, position, callback)
    char *win, *name, *desc;
    int position;
    Tcl_CmdProc *callback;
{
    /* Compute the initial slider position in the range of 0 to 100 */
    if ((position < 0) || (position > 100)) {
	errorReport("In Tk scale: position is out of range");
	return;
    }

    /* Register the callback function with Tcl */
    sprintf(command, "%sCallback", name);
    Tcl_CreateCommand (interp, command, callback,
	    (ClientData) 0, (void (*)()) NULL);

    /* Create the scale */
    sprintf(command,
	"makeScale %s %s \"%s\" %d %sCallback",
	win, name, desc, position, name);
    if(Tcl_Eval(interp, command) != TCL_OK)
	errorReport("Cannot make slider scale in control panel");
}

/*
Display a value associated with the slider position.
This must be called by the user of the slider.
Only the first 6 characters of the value will be used.
*/
void displaySliderValue (win, name, value)
    char *win, *name, *value;
{
    sprintf(command, "%s.%s.value configure -text \"%.6s \"",
	win, name, value);
    if(Tcl_Eval(interp, command) != TCL_OK)
	errorReport("Cannot update slider display");
}


/* Main function */
int main(int argc, char *argv[]) {
    Tk_3DBorder border;
    interp = Tcl_CreateInterp();

    /* Initialize the Tk window */
#if TK_MAJOR_VERSION <= 4 && TK_MINOR_VERSION < 1
    w = Tk_CreateMainWindow(interp, display, name, appClass);
#else
    if (Tcl_Init(interp) == TCL_ERROR) {
	fprintf(stderr,
		"%s: Error while trying to initialize tcl: %s\n",
		argv[0], interp->result);
	exit(1);
      }
    if (Tk_Init(interp) == TCL_ERROR)  {
	fprintf(stderr,
		"%s: Error while trying to get main window: %s\n",
		argv[0], interp->result);
	exit(1);
    }

    Tcl_StaticPackage(interp, "Tk", Tk_Init, (Tcl_PackageInitProc *) NULL);
    w = Tk_MainWindow(interp);
#endif /* TK_MAJOR_VERSION <= 4 && TK_MINOR_VERSION < 1 */
    if (w == NULL) {
	fprintf(stderr,
		"%s: Error while trying to get main window: %s\n",
		argv[0], interp->result);
	exit(1);
    }
    Tk_SetClass(w, "CGC");
    Tk_CreateEventHandler(w, StructureNotifyMask, StructureProc,
			  (ClientData) NULL);
    Tk_DoWhenIdle(DelayedMap, (ClientData) NULL);
    Tk_GeometryRequest(w, 200, 200);
#if TK_MAJOR_VERSION < 4
    border = Tk_Get3DBorder(interp, w, None, "#ffe4c4");
#else
    border = Tk_Get3DBorder(interp, w, "#ffe4c4");
#endif
    if (border == NULL) {
	Tcl_SetResult(interp, (char *) NULL, TCL_STATIC);
	Tk_SetWindowBackground(w, WhitePixelOfScreen(Tk_Screen(w)));
    } else {
	Tk_SetBackgroundFromBorder(w, border);
    }
    XSetForeground(Tk_Display(w), DefaultGCOfScreen(Tk_Screen(w)),
		   BlackPixelOfScreen(Tk_Screen(w)));
    Tcl_CreateCommand(interp, "handleExit", handleExit, (ClientData) w,
		      (void (*)()) NULL);
    Tcl_CreateCommand(interp, "goCmd", goCmd, (ClientData) w,
		      (void (*)()) NULL);
    Tcl_CreateCommand(interp, "stopCmd", stopCmd, (ClientData) w,
		      (void (*)()) NULL);
    Tcl_CreateCommand(interp, "pauseCmd", pauseCmd, (ClientData) w,
		      (void (*)()) NULL);
    sprintf(command,"set applicationName \"%s\"", name);
    if(Tcl_Eval(interp, command) != TCL_OK) goto error;
    sprintf(command, "set numIterations %d", numIterations);
    if(Tcl_Eval(interp, command) != TCL_OK) goto error;
    if(Tcl_Eval(interp, initCmd) != TCL_OK) goto error;
    (void) Tcl_Eval(interp, "update");
    tkSetup();

    Tk_MainLoop();

    Tcl_DeleteInterp(interp);
    exit(0);

error:
    errorReport("Error in main loop");
    Tcl_Eval(interp, "destroy .");
    exit(1);
    return 0;		/* Needed only to prevent compiler warnings. */
}

/*
StructureProc --

This procedure is invoked whenever a structure-related event
occurs on the main window.  If the window is deleted, the
procedure modifies "w" to record that fact.

Results:
    None.

Side effects:
    Variable "w" may get set to NULL.

Arguments:
    ClientData clientData       Information about window.
    XEvent *eventPtr            Information about event.
*/
static void
StructureProc(clientData, eventPtr)
    ClientData clientData;		/* Information about window. */
    XEvent *eventPtr;			/* Information about event. */
{
    if (eventPtr->type == DestroyNotify) {
	w = NULL;
    }
}


/*
DelayedMap --

This procedure is invoked by the event dispatcher once the
startup script has been processed.  It waits for all other
pending idle handlers to be processed (so that all the
geometry information will be correct), then maps the
application's main window.

Results:
    None.

Side effects:
    The main window gets mapped.
*/
static void
DelayedMap(clientData)
    ClientData clientData;      /* Not used. */
{

    while (Tk_DoOneEvent(TK_IDLE_EVENTS) != 0) {
	/* Empty loop body. */
    }
    if (w == NULL) {
	return;
    }
    Tk_MapWindow(w);
}

/* Redfine the error handler to popup messages.  */
/* Removed due to Ultrix incompatibility
void perror(s)
char *s;
{
    errorReport(s);
}
*/



