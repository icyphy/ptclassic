/*
 * File to include in generated C code to use a Tk control panel.
 * Assumes the variable "name" is the name of the application,
 * and "numIterations" is the default number of iterations.
 *
 * Author: Edward A. Lee
 * Version: $Id$
 */

#include "tkConfig.h"
#include "tkInt.h"
#define COMMANDSIZE 512
#define REPORT_TCL_ERRORS 1

/*
 * Global variables used by the main program:
 */

static Tk_Window w;             /* The main window for the application.  If
                                 * NULL then the application no longer
                                 * exists. */
static Tcl_Interp *interp;      /* Interpreter for this application. */
static Tcl_CmdBuf buffer;       /* Used to assemble lines of terminal input
                                 * into Tcl commands. */
static char command[COMMANDSIZE];


/* No command line arguments are used, so use default settings */
char *display = NULL;

/*
 * Forward declarations for procedures defined later in this file:
 */

static void             DelayedMap _ANSI_ARGS_((ClientData clientData));
static void             StructureProc _ANSI_ARGS_((ClientData clientData,
                            XEvent *eventPtr));
/*
 * Command used to initialize tk:
 */

static char initCmd[] = "source $env(PTOLEMY)/src/domains/cgc/tcltk/lib/ptolemy-cgc.tcl";
static int runFlag = 0;

void tkSetup();

/*
 *----------------------------------------------------------------------
 * Error handler
 *----------------------------------------------------------------------
 */

void errorReport(message)
char *message;
{
    char *msg;
    sprintf(command, "popupMessage .error {%s}", message);
    Tcl_Eval(interp, command, 0, (char**)NULL);
#if REPORT_TCL_ERRORS
    msg = Tcl_GetVar(interp, "errorInfo", TCL_GLOBAL_ONLY);
    if (msg == NULL) {
        msg = interp->result;
    }
    sprintf(command, "popupMessage .error {%s}", msg);
    if(Tcl_Eval(interp, command, 0, (char **)NULL) != TCL_OK)
        fprintf(stderr, "%s\n", msg);
#endif
}


/*
 *----------------------------------------------------------------------
 * Command to start running the Ptolemy system
 *----------------------------------------------------------------------
 */
static int
goCmd(dummy, interp, argc, argv)
    ClientData dummy;                   /* Not used. */
    Tcl_Interp *interp;                 /* Current interpreter. */
    int argc;                           /* Number of arguments. */
    char **argv;                        /* Argument strings. */
{
	if(runFlag == 1) return TCL_OK;	/* ignore if already running */
	/* If paused, just reset the run flag and return */
	if(runFlag == -1) {
	    runFlag = 1;
	    return TCL_OK;
	}
	runFlag = 1;

	/* First make sure Tcl grabs the latest number */
	/* of iterations from the control panel        */
	if(Tcl_Eval(interp,"updateIterations", 0, (char*)NULL) != TCL_OK) {
		runFlag = 0;
		return TCL_ERROR;
	}

	/* Then convert the resulting string to an integer. */
	if(sscanf(interp->result,"%d",&numIterations) != 1) {
		runFlag = 0;
		errorReport("Invalid number of iterations");
		return TCL_OK;
	}

	go();
	Tcl_Eval(interp,".go configure -relief raised", 0, (char*)NULL);
	runFlag = 0;
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 * Command to stop running the Ptolemy system
 *----------------------------------------------------------------------
 */
static int
stopCmd(dummy, interp, argc, argv)
    ClientData dummy;                   /* Not used. */
    Tcl_Interp *interp;                 /* Current interpreter. */
    int argc;                           /* Number of arguments. */
    char **argv;                        /* Argument strings. */
{
	runFlag = 0;
	return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 * Command to pause running the Ptolemy system
 *----------------------------------------------------------------------
 */
static int
pauseCmd(dummy, interp, argc, argv)
    ClientData dummy;                   /* Not used. */
    Tcl_Interp *interp;                 /* Current interpreter. */
    int argc;                           /* Number of arguments. */
    char **argv;                        /* Argument strings. */
{
	/* If stopped already, this has no effect */
	if (runFlag == 1) runFlag = -1;
	return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 * Convenience utilities for standard Tk operations
 *----------------------------------------------------------------------
 */

/*
 *----------------------------------------------------------------------
 * Entries in a control panel
 *----------------------------------------------------------------------
 * This function creates an entry box in a control panel and attaches
 * a callback function to it.  The callback function will be called whenever
 * the user enters a Return in the box.  The argument to the callback
 * function will be the entry that the user has put in the control box.
 * The return value of the callback function should be TCL_OK.
 *
 * Arguments:
 *	char* win	Name of window into which to put the control
 *	char* name	Name to use for the control pane
 *	char* desc	Description of the control
 *	char* initValue	Initial value of the control
 *	int* callback	Callback procedure to use on update
 */

/* Make an entry in the main control panel to control a state value. */
/* The name must be unique (e.g. derived from the star name). */
/* The description is an arbitrary string */

void makeEntry (win, name, desc, initValue, callback)
    char *win, *name, *desc, *initValue;
    Tcl_CmdProc *callback;
{
    sprintf(command, "%sCallback", name);
    Tcl_CreateCommand (interp, command, callback,
	    (ClientData) 0, (void (*)()) NULL);
    sprintf(command, "makeEntry %s %s \"%s\" \"%s\" %sCallback",
		win, name, desc, initValue, name);
    if(Tcl_Eval(interp, command, 0, (char*)NULL) != TCL_OK) {
        errorReport("Cannot make entry box in control panel.");
    }
}

/*
 *----------------------------------------------------------------------
 * Buttons in the control panel
 *----------------------------------------------------------------------
 * This function creates a push button in a control panel and attaches
 * a callback function to it.  The callback function will be called whenever
 * the user pushes the button.
 * The return value of the callback function should be TCL_OK.
 *
 * Arguments:
 *	char* win	Name of window into which to put the control
 *	char* name	Name to use for the control pane
 *	char* desc	Description of the control
 *	int* callback	Callback procedure to use on update
 */

/* Make a button in the main control panel. */
/* The name must be unique (e.g. derived from the star name). */
/* The description is an arbitrary string */

void makeButton (win, name, desc, callback)
    char *win, *name, *desc;
    Tcl_CmdProc *callback;
{
    sprintf(command, "%sCallback", name);
    Tcl_CreateCommand(interp, command, callback, (ClientData) 0, NULL);
    sprintf(command, "makeButton %s %s \"%s\" %sCallback",
	win, name, desc, name);
    if(Tcl_Eval(interp, command, 0, (char*)NULL) != TCL_OK) {
        errorReport("Cannot make button in control panel.");
    }
}

/* Get a float entry from the main control panel */

/*
 *----------------------------------------------------------------------
 * Scales in the control panel
 *----------------------------------------------------------------------
 * This function creates a scale (with slider) in a control panel and attaches
 * a callback function to it.  The callback function will be called whenever
 * the user moves the slider.  The argument to the callback
 * function will be the position of the slider, which can range from
 * 0 to 100.  The return value of the callback function should be TCL_OK.
 *
 * The scale has room to its left for displaying the value associated with
 * it, but it is up to the user of the scale to update this value. This
 * can be done using the displaySliderValue function below.
 *
 * Arguments:
 *	char* win	Name of window into which to put the control
 *	char* name	Name to use for the control pane
 *	char* desc	Description of the control
 *	int position	Initial position of the slider (between 0 and 100)
 *	int* callback	Callback procedure to use on update
 */

/* Make a scale slider */

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
    if(Tcl_Eval(interp, command, 0, (char**)NULL) != TCL_OK)
        errorReport("Cannot make slider scale in control panel");
}

/*
 * Display a value associated with the slider position.
 * This must be called by the user of the slider.
 * Only the first 6 characters of the value will be used.
 */
void displaySliderValue (win, name, value)
    char *win, *name, *value;
{
    sprintf(command, ".%s.%s.value configure -text \"%.6s \"",
	win, name, value);
    if(Tcl_Eval(interp, command, 0, (char**)NULL) != TCL_OK)
        errorReport("Cannot update slider display");
}

/*
 *----------------------------------------------------------------------
 * Internal functions for bar chart
 *----------------------------------------------------------------------
 * Routine to scale the fullScale data member of the owner of a bar chart.
 */
static int
verticalScale(fullScale, interp, argc, argv)
    double* fullScale;
    Tcl_Interp *interp;
    int argc;
    char **argv;
{
    float temp;
    if(sscanf(argv[1], "%4f", &temp) != 1) {
	errorReport("Cannot reset full scale in bar chart");
	return TCL_ERROR;
    }
    *fullScale = *fullScale * temp;

    /* Return new full scale value to tcl */
    sprintf(interp->result, "%f", *fullScale);

    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 * Create a bar chart in its own window
 *----------------------------------------------------------------------
 *     name: name of the top level window
 *     desc: description to be put in the window
 *     data: an array of data to be plotted
 *     numBars: number of bars in the bar chart
 *     fullScale: pointer to value of the full scale on the bar chart
 *     id: an array to fill with item IDs
 *     geo: geometry of the overall window, in the form =AxB+C+D
 *     width: width of the bar chart itself
 *     height: height of the bar chart itself
 */

void makeBarChart (name, desc, data, numBars, fullScale, id, geo, width, height)
    char *name, *desc;
    double *data, *fullScale;
    int numBars;
    int *id;
    char *geo;
    double width, height;
{
    int i;
    int test;

    /* Register the function to reset fullScale with Tcl */
    sprintf(command, "%sverticalScale", name);
    Tcl_CreateCommand (interp, command, verticalScale,
            (ClientData) fullScale, (void (*)()) NULL);

    /* Make the bar chart */
    sprintf(command, "makeBarChart %s \"%s\" \"%s\" %d %f %f",
	name, desc, geo, numBars, width, height);
    if(Tcl_Eval(interp, command, 0, (char**)NULL) != TCL_OK)
        errorReport("Cannot make bar chart");
    test = 0;
    for (i=0;i<numBars;i++) {
        sprintf(command,
		".%s.pf.plot create rect %fc %fc %fc %fc -fill firebrick4",
		name, (i+0.1)*10/numBars, height/2, (i+0.9)*width/numBars,
		(1-data[i]/(*fullScale))*height/2);
        if(Tcl_Eval(interp, command, 0, (char**)NULL) != TCL_OK)
            test = 1;
        else
            sscanf(interp->result,"%d",&id[i]);
    }
    if (test) errorReport("Cannot make bar in bar chart");
}

/*
 *----------------------------------------------------------------------
 * Set the bars in a bar chart
 *----------------------------------------------------------------------
 *     name: name of the top level window
 *     data: an array of data to be plotted
 *     numBars: number of bars in the bar chart
 *     fullScale: pointer to value of the full scale on the bar chart
 *     id: an array of item IDs filled by makeBarChart
 */

void setBarChart (name, data, numBars, fullScale, id)
    char *name;
    double *data;
    int numBars;
    double *fullScale;
    int *id;
{
    int i;
    int test=0;
    int width, height;
    int x0, y0, x1, y1;
    Tk_Window plotwin;

    /* Get the current plot window size */
    sprintf(command, ".%s.pf.plot", name);
    plotwin = Tk_NameToWindow(interp,command,w);
    width = Tk_Width (plotwin);
    height = Tk_Height (plotwin);

    for (i=0;i<numBars;i++) {
	x0 = (i+0.1)*width/numBars;
	y0 = height/2.0;
	x1 = (i+0.9)*width/numBars;
	y1 = (1-data[i]/(*fullScale))*height/2;
        sprintf(command, ".%s.pf.plot coords %d %d %d %d %d",
		name, id[i], x0, y0, x1, y1);
        if(Tcl_Eval(interp, command, 0, (char**)NULL) != TCL_OK)
            test = 1;
    }
    if (test) {
	runFlag = 0;
	errorReport("Cannot update bar chart");
    }
}


/*
 *----------------------------------------------------------------------
 * Main function
 *----------------------------------------------------------------------
 */

main() {
    Tk_3DBorder border;
    interp = Tcl_CreateInterp();
    w = Tk_CreateMainWindow(interp, display, name);
    if (w == NULL) {
        fprintf(stderr, "%s\n", interp->result);
        exit(1);
    }
    Tk_SetClass(w, "Tk");
    Tk_CreateEventHandler(w, StructureNotifyMask, StructureProc,
            (ClientData) NULL);
    Tk_DoWhenIdle(DelayedMap, (ClientData) NULL);
    Tk_GeometryRequest(w, 200, 200);
    border = Tk_Get3DBorder(interp, w, None, "#ffe4c4");
    if (border == NULL) {
        Tcl_SetResult(interp, (char *) NULL, TCL_STATIC);
        Tk_SetWindowBackground(w, WhitePixelOfScreen(Tk_Screen(w)));
    } else {
        Tk_SetBackgroundFromBorder(w, border);
    }
    XSetForeground(Tk_Display(w), DefaultGCOfScreen(Tk_Screen(w)),
            BlackPixelOfScreen(Tk_Screen(w)));
    Tcl_CreateCommand(interp, "goCmd", goCmd, (ClientData) w,
            (void (*)()) NULL);
    Tcl_CreateCommand(interp, "stopCmd", stopCmd, (ClientData) w,
            (void (*)()) NULL);
    Tcl_CreateCommand(interp, "pauseCmd", pauseCmd, (ClientData) w,
            (void (*)()) NULL);
    sprintf(command,"set applicationName \"%s\"", name);
    if(Tcl_Eval(interp, command, 0, (char **) NULL) != TCL_OK) goto error;
    sprintf(command, "set numIterations %d", numIterations);
    if(Tcl_Eval(interp, command, 0, (char **) NULL) != TCL_OK) goto error;
    if(Tcl_Eval(interp, initCmd, 0, (char **) NULL) != TCL_OK) goto error;
    buffer = Tcl_CreateCmdBuf();
    (void) Tcl_Eval(interp, "update", 0, (char **) NULL);
    tkSetup();

    Tk_MainLoop();

    Tcl_DeleteInterp(interp);
    Tcl_DeleteCmdBuf(buffer);
    exit(0);

error:
    errorReport("Error in main loop");
    Tcl_Eval(interp, "destroy .", 0, (char **) NULL);
    exit(1);
    return 0;                   /* Needed only to prevent compiler warnings. */
}

/*
 *----------------------------------------------------------------------
 *
 * StructureProc --
 *
 *      This procedure is invoked whenever a structure-related event
 *      occurs on the main window.  If the window is deleted, the
 *      procedure modifies "w" to record that fact.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      Variable "w" may get set to NULL.
 *
 *----------------------------------------------------------------------
 */

        /* ARGSUSED */
static void
StructureProc(clientData, eventPtr)
    ClientData clientData;      /* Information about window. */
    XEvent *eventPtr;           /* Information about event. */
{
    if (eventPtr->type == DestroyNotify) {
        w = NULL;
    }
}



/*
 *----------------------------------------------------------------------
 *
 * DelayedMap --
 *
 *      This procedure is invoked by the event dispatcher once the
 *      startup script has been processed.  It waits for all other
 *      pending idle handlers to be processed (so that all the
 *      geometry information will be correct), then maps the
 *      application's main window.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      The main window gets mapped.
 *
 *----------------------------------------------------------------------
 */

        /* ARGSUSED */
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

/*
 *----------------------------------------------------------------------
 * Redfine error handler to popup messages.
 *----------------------------------------------------------------------
 */

void perror(s)
char *s;
{
    errorReport(s);
}

