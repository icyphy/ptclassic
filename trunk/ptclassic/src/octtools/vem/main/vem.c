#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/* Copyright (c) 1990-1993 The Regents of the University of California.
 * All rights reserved.
 * 
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the above
 * copyright notice and the following two paragraphs appear in all copies
 * of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
 * THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
 * CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.
 * 
 */
/*
 * VEM Primary Startup
 *
 * David Harrison
 * University of California,  Berkeley
 * 1986, 1989
 *
 * This file contains the primary loop for VEM.  It can be run in the background
 * if the user wishes.
 *
 * After starting up,  the program will prompt for a console window.
 * The console window is opened using the standard X interface for
 * opening windows.  This window will contain various messages and
 * the echo of the current argument list.  After starting,  the
 * console will display a prompt and wait for input.
 *
 * Commands to VEM are specified in post-fix form.  First,  the user
 * builds and argument list and then he selects a command.  In this
 * version of VEM,  commands can be selected by single keystrokes
 * or by a pop-up menu.  Pressing the middle button in a graphics
 * window causes the VEM menu to appear.
 *
 * Right now,  there are three kinds of input to VEM:  points, boxes
 * and lines.  Points are entered by pressing and releasing the left
 * button of the mouse.  Boxes are entered by pressing, dragging, and
 * releasing the left button.  Text is entered by simply typing the
 * text enclosed in double quotes. The command reference manual can be 
 * found in the VEM man page (vem(1)).
 *		    
 * This release of VEM includes the RPC package developed jointly by
 * Rick Spickelmier and David Harrison.  It provides a means of
 * forking applications from VEM which may call upon OCT and selected
 * VEM routines.
 */


#include "port.h"
#include <X11/Xlib.h>		/* C language interface to X */
#include <X11/Xutil.h>		/* X utilities               */
#include <X11/Xresource.h>	/* X Resource Database       */
#include "xpa.h"		/* X Menu facility           */
#include "xvals.h"		/* Critical X values         */
#include "general.h"		/* General VEM definitions   */
#include "message.h"		/* Message writing system    */
#include "bindings.h"		/* Command bindings          */
#include "list.h"		/* List handling package     */
#include "oct.h"		/* OCT database definitions  */
#include "windows.h"		/* Graphics window management*/
#include "argsel.h"		/* Argument selection        */
#include "attributes.h"		/* Attribute handler         */
#include "buffer.h"		/* Buffer management         */
#include "defaults.h"		/* Default handling          */
#include "commands.h"		/* Command argument defns    */
#include "version.h"		/* Version string management */
#include "tap.h"		/* Technology access package */
#include "objsel.h"		/* Object selction           */
#include "options.h"		/* Option parsing            */
#include "oh.h"			/* Oct Utilities             */
#include "fc.h"			/* File completion           */
#include "errtrap.h"		/* Error handling            */
#include "errors.h"		/* VEM Specific error routines */
#include "cursors.h"		/* Cursor management         */
#include "browser.h"		/* Cell browser              */
#include "vemUtil.h"		/* General vem utilities     */
#include "vemDM.h"		/* Dialog interface          */
#include "drs.h"		/* Undo command              */

/*
 * Toolkit definitions
 */
#include <X11/Shell.h>		/* Top level shell           */
#include <X11/StringDefs.h>	/* For various Xt            */
#include <X11/Xaw/Simple.h>	/* For XtNcursor             */
#include "ts.h"			/* For console window        */

/* Maximum number of points in one argument */
#define MAXINPUTPNTS	20

/* Main display */
static Display *vemDisp;	/* Set in main() */
static XtAppContext vemContext;	/* Set in main() */

/* The console window */
#define CONS_BUF_SIZE	8192
Widget consWidget;
Window consWin;

/* Selection attribute for arguments */
static atrHandle satr;

/* Characters for command line processing */

#define NULL_CHAR	'\0'
#define CARRIAGE_RETURN	'\015'
#define BACKSPACE	'\010'
#define TAB		'\t'
#define CONTROL_C	'\003'
#define CONTROL_U	'\025'
#define CONTROL_W	'\027'	
#define CONTROL_X	'\030'
#define DELETE		'\177'

#define SPACE		' '
#define LAST_CHAR	'~'


/*
 * Command Line Argument parsing
 *
 * The command line argument parser produces a lsList of the following
 * structures.  This list is processed just before the main loop
 * begins.
 */

typedef struct command_args {
    STR facet_spec;		/* What facet to open       */
    STR geo_spec;		/* Geometry specification   */
    STR rpc_spec;		/* RPC Application to start */
} CommandArgs;

/*
 * Below is the argument specifications for vem (argument parsing
 * is done by the options package).
 */

optionStruct optionList[] = {
    { "F", 	"cell[:view[:facet]]",	"Facet to display in window" 	},
    { "G",	"WxH+X+Y",		"Size and position of window"	},
    { "R",	"[host,]path",		"Remote application to start"	},
    { OPT_OARG,	"host:display",		"X display to use (defaults to"	},
    { OPT_CONT,	0,			" value of DISPLAY environment" },
    { OPT_CONT, 0,			" variable)"			},
    { OPT_DESC,	0,
	"VEM is an interactive program for viewing and editing VLSI and schematic", },
    { OPT_CONT, 0,
	" cells stored using the OCT data manager.  The -F, -G, and -R flags" },
    { OPT_CONT, 0,
	" are considered triplets that specify the initial cell, location," },
    { OPT_CONT, 0,
	" size, and remote application of a window.  There is no limit to" },
    { OPT_CONT, 0,
	" the number of triplets specified.  The location and size of the" },
    { OPT_CONT, 0,
	" console window can be specified by giving an initial -g option" },
    { OPT_CONT, 0,
	" without specifying a facet with the -f option." },
    { 0, 0, 0 }
};


/*
 * The following structure defines the current input context
 * for VEM.  This structure is used to allow a user to push
 * and pop input contexts at will.
 */

/* Input states */

#define TEXT_STATE	'"'	/* Character for text input          */
#define COMMAND_STATE	'\015'	/* Character for command termination */
#define ENTER_COMMAND	':'	/* Character to input command name   */

#define MAX_TYPE_IN_COMMAND	100
#define MAXARGSIZE 256

typedef struct vem_incontext_defn {
    int state;		/* Current input state (see above) */

    /* Current prompt string */
    char *prompt;

    /* Current type-in state */
    int typeInLength;		/* Length of current type-in       */
    char typeInCommand[MAX_TYPE_IN_COMMAND];
				/* Space for current type-in        */

    /*
     * The following space is used by echoNew and echoUpdate.  One pointer
     * points to one echoList the other points to the other echo list.
     * echoUpdate swaps the pointers.
     */
    char *newList;
    char *oldList;
    char echoList1[MAXARGSIZE];
    char echoList2[MAXARGSIZE];

    /* The primary VEM command argument list */
    lsList commandArgs;
    
    /*
     * The following function is called after all arguments have been
     * parsed and a command is to be invoked.  Normally,  this is set
     * to vemInvokeCommand().  However,  help functions of one kind or
     * another may set it to something else.  The function has the form:
     *   int func(spot, binding, options)
     *   vemPoint *spot;
     *   bdBinding *binding;
     *   int options;
     * where `spot' is where the command was invoked,  `tbl' is the
     * binding table for `binding',  and options are one of the following:
     *   TYPE_OUT	Should type out the command's name.
     *   NO_OUTPUT	Don't produce any console output
     * Other values may be defined later.  The routine should return
     * zero normally and one to terminate the current event loop.
     * The argument list passed to the command is given by cur_con.commandArgs.
     */
    int (*invocation)();
} vem_incontext;

/* Current context - initialized in main() */
static vem_incontext cur_con;

/* Stack of saved contexts */
#define MAX_CONTEXTS	4
static int con_index = 0;	/* Next spot to save context */
static vem_incontext saved_contexts[MAX_CONTEXTS];

/* Environment for error continuation */
static jmp_buf err_state;



/* This prevents calling main again */
static int noMain = 1;


/* Special flag indicating command invocation should echo command name */
#define TYPE_OUT	1
#define NO_OUTPUT	2

/* Cursor used while VEM is busy */

static Cursor grabCursor;	/* Grabbing cursor       */

#ifdef MF
char	octToolname[30] = { 'V', 'E', 'M', '\0' };
int	octLogFlag = 1;
#endif /* MF */

/*
 * No include externs -- can't check arguments either -- most from rpc
 */

extern void		rpcInit();
extern int vemServer();		/* From rpc */
extern vemStatus RPCExecApplication();
extern vemStatus RPCUserFunction();
extern vemStatus delOneItemCmd();


/*
 * Forward declarations (more for lint than anything else).
 */

static lsList parse_args();
static CommandArgs *new_triplet();
static int null_triplet();
static void start_arg_windows();
static vemStatus findArg();
static vemOneArg *newArg();
static void delArg();
static void removeArg();
static int vemInvokeCommand();
static void vemConsInput();
static void vemConsWrite();
static void vemIDT();
static vemStatus echoNew();
static vemStatus echoUpdate();
static vemStatus echoRedraw();
static int doKeyInput();
static vemStatus doNormButton();
static int doMenu();
static vemStatus doAltButton();
static int handleTextArg();
static int handleTypeInCmd();
static int handleSingleKey();
static int movePoint();
static int moveObjs();
static int vemDoInvokeCommand();

vemStatus vemPrompt();
int vemSyncEvents();



#define MAX_ARGS	20

int
main(argc, argv)
int argc;
char *argv[];
/*
 * Starts up the interface,  initializes many of the packages,
 * asks for a console window,  and enters the primary loop.
 * Logging is done in the file vem.log.
 */
{
    Widget top_level;		/* Top level widget      */
    Arg tk_list[MAX_ARGS];	/* Toolkit arguments     */
    int arg_count;		/* Toolkit index         */
#ifdef NO_RPC    
    int stopFlag;		/* Flag controlling main loop */
    XEvent theEvent;		/* Driving event         */
#endif
    STR bindFile;		/* Bindings file         */
    STR techRoot;		/* Root for technology   */
    char *disp_name;		/* Display name          */
    lsList arg_list;		/* Command line arguments */
    CommandArgs *first_arg;	/* First command line arg */
    STR tmpDefault;
    unsigned int pln, cls;
    int color_flag, browser_flag;
    int vemHandler();
    int vemXError();
    char *mktemp();
    char *log_file_name;
    char log_file_space[VEMMAXSTR*3];
    vemStatus log_file_stat;
    char mktempbuf[50];
    char prog_name[256];
    char *spot, *defver;

    VEM_CKRVAL(noMain, "Stack corruption", 5);
    noMain = 0;

    /* Test if OCTTOOLS is set. If it is not, no sense in continuing. */
    (void)util_file_expand( "$OCTTOOLS" );

    /* Basic default error handler */
    errProgramName( "vem" );
    errPushHandler(vemDefaultTrapHandler);

    /* Start up the toolkit */
    XtSetErrorHandler(vemXtError);
    XtToolkitInitialize();
    vemContext = XtCreateApplicationContext();
    xv_set_disp(vemDisp = XtOpenDisplay(vemContext, (String) 0,
					(String) 0, "Vem",
					(XrmOptionDescRec *) 0,
					0, &argc, argv));
    if (vemDisp == NIL(Display)) {
	(void) fprintf(stderr, "Can't open display.\n");
	optUsage();
    }
    top_level = XtAppCreateShell(argv[0], "Vem",
				 applicationShellWidgetClass,
				 vemDisp, (Arg *) 0, 0);

    xv_set_scrn(DefaultScreen(xv_disp()));
    xv_set_topw(top_level);
    dds_partial_init(vemContext, top_level);
    dmCompatInit(vemHandleEvent);

    /* Synchronous */
    if ( (tmpDefault = XGetDefault(vemDisp, argv[0], "debug")) ) {
	XSynchronize(vemDisp, atoi(tmpDefault));
    }

    XSetErrorHandler(vemXError);

    /*
     * There is a chicken and egg problem here.  The default for colors
     * and planes must be retrieved before initializing defaults.  Of
     * course,  if the defaults aren't initialized,  you can't get the
     * defaults for colors and planes.  The solution:  a quick precursor
     * look at Xdefaults:
     */
    if ( (tmpDefault = XGetDefault(vemDisp, argv[0], "planes")) ) {
	pln = atoi(tmpDefault);
    } else {
	pln = 1;
    }
    if ( (tmpDefault = XGetDefault(vemDisp, argv[0], "colors")) ) {
	cls = atoi(tmpDefault);
    } else {
	cls = 25;
    }
    if ( (tmpDefault = XGetDefault(vemDisp, argv[0], "logfile")) ) {
	STRMOVE(log_file_space, util_tilde_expand(tmpDefault));
	log_file_name = (char *) log_file_space;
    } else {
	STRMOVE(mktempbuf, "/tmp/vem.log.XXXXXX");
	log_file_name = mktemp(mktempbuf);
    }
    log_file_stat = vemMsgFile(log_file_name);

    color_flag = atrInit(vemDisp, pln, cls);
    STRMOVE(prog_name, argv[0]);
    if ( (spot = strrchr(prog_name, '/')) ) {
	spot++;
    } else {
	spot = prog_name;
    }
    dfInit(vemDisp, spot);

    /* VEM argument parsing */
    arg_list = parse_args(argc, argv, &disp_name);
    arg_count = 0;
    XtSetArg(tk_list[arg_count], XtNallowShellResize, True);	arg_count++;
    XtSetArg(tk_list[arg_count], XtNinput, True);		arg_count++;
    (void) sprintf(errMsgArea, "VEM version %s", vemVerString());
    XtSetArg(tk_list[arg_count], XtNtitle, errMsgArea);		arg_count++;
    if (lsFirstItem(arg_list, (lsGeneric *) &first_arg, LS_NH) == LS_OK) {
	if (!first_arg->facet_spec) {
	    XtSetArg(tk_list[arg_count], XtNgeometry, first_arg->geo_spec);
	    arg_count++;
	    (void) lsDelBegin(arg_list, (lsGeneric *) 0);
	}
    }
    XtSetValues(top_level, tk_list, arg_count);

    dfGetString("defaultsVersion", &defver);
    if (strncmp(defver, vemVerString(), strlen(defver)) != 0) {
	char buffer[1024];
	(void)sprintf( buffer, "The application defaults for vem %s (default %s) have not\n\
been installed correctly.  Please see your system adminstrator.\n", vemVerString(), defver );
	vemMsg(MSG_A, buffer );
	exit(1);
    }
    dfGetString("technology", &techRoot);
    tapRootDirectory(techRoot);
    vemIDT(vemDisp, color_flag);

    dfDump(MSG_L);

    dfGetAttr("select", &satr);

    /* Get console window going */
    arg_count = 0;
    XtSetArg(tk_list[arg_count], XtNcursor, vemCursor(CONSOLE_CURSOR)); arg_count++;
    XtSetArg(tk_list[arg_count], XtNwidth, 500);			arg_count++;
    XtSetArg(tk_list[arg_count], XtNheight, 150);			arg_count++;
    consWidget = tsCreate(vemContext, "console", top_level, CONS_BUF_SIZE,
			  vemConsInput, (caddr_t) 0, tk_list, arg_count);

    XtRealizeWidget(top_level);
    (void) vemSyncEvents();
    consWin = XtWindow(consWidget);

    vemMsgSetCons((Pointer) consWidget, vemConsWrite);
    (void) sprintf(errMsgArea, "This is VEM version %s\n", vemVerString());
    vemMessage(errMsgArea, MSG_DISP);
    (void) vemSyncEvents();
    if (log_file_stat == VEM_OK) {
	(void) sprintf(errMsgArea, "Log file is %s\n", log_file_name);
	vemMessage(errMsgArea, MSG_DISP);
    } else {
	vemMsg(MSG_A, "Can't open error logging file `%s'\nMessages logged to stderr\n",
	       log_file_name);
    }
    (void) vemSyncEvents();

    grabCursor = vemCursor(GRAB_CURSOR);
    octBegin();

    /* Command Bindings */
    dfGetString("bindings", &bindFile);
    if (bdInit(bindFile) != VEM_OK) {
	(void) fprintf(stderr, "Can't read command bindings from file '%s'\n",
		bindFile);
	exit(1);
    }

    /* Start initial browser if requested */
    dfGetInt("initialBrowser", &browser_flag);
    if (browser_flag) {
	vemBusy();
	vemMsg(MSG_C, "Starting browser...");
	vemBrowser(consWin, ".");
	vemMsg(MSG_C, "Done\n");
    }
    /* Initialize original context */
    cur_con.state = 0;
    cur_con.prompt = "vem> ";
    cur_con.typeInLength = 0;
    cur_con.commandArgs = lsCreate();
    cur_con.invocation = vemInvokeCommand;

#ifndef NO_RPC
    /* start up rpc */
    rpcInit();
#endif

    /* Start up window creation */
    start_arg_windows(arg_list);

    /* Primary command loop */
    vemMessage(cur_con.prompt, MSG_NOLOG|MSG_DISP);
    echoNew();

    /* Start up the remote procedure call system */
#ifdef NO_RPC
    stopFlag = 0;
    while (!stopFlag) {
	XPeekEvent(vemDisp, &theEvent);
	stopFlag = vemProcessXEvent( );
    }
#else
    vemServer();
#endif
    octEnd();
#ifdef MF
    m_flush();
#endif    
    return(0);
}


static lsList parse_args(argc, argv, disp_name)
int argc;			/* Number of arguments   */
char *argv[];			/* Argument list         */
char **disp_name;		/* Returned display name */
/*
 * This routine uses the options package to parse the argument
 * list to VEM.  This argument list consists of triplets: -F,
 * -G, and -R.  The -F flag is required and starts a new triplet.
 * The -G and -R flags after the -F flag are optional.  The -F flag
 * can be omitted from the first triplet in which case the -G or
 * -R flag apply to the console window.  The routine returns
 * all triplets in a lsList.  The optional display name is
 * put in `disp_name'.
 */
{
    lsList triplets;
    CommandArgs *this_triplet;
    int option_char, i;
    char *p;
    vemStatus stat;

    *disp_name = "";
    triplets = lsCreate();
    this_triplet = new_triplet((char *) 0, (char *) 0, (char *) 0);
    while ((option_char = optGetOption(argc, argv)) != EOF) {
	switch (option_char) {
	case 'F':
	    if (null_triplet(this_triplet)) {
		/* Use this one */
		this_triplet->facet_spec = util_strsav(optarg);
	    } else {
		/* Push on old, make a new one */
		lsNewEnd(triplets, (lsGeneric) this_triplet, LS_NH);
		this_triplet = new_triplet(util_strsav(optarg), (char *) 0, (char *) 0);
	    }
	    break;
	case 'G':
	    /* Set geometry specification */
	    this_triplet->geo_spec = util_strsav(optarg);
	    break;
	case 'R':
	    /* Set remote specification */
	    this_triplet->rpc_spec = util_strsav(optarg);
	    break;
	default:
	    optUsage();
	}
    }
    if (!null_triplet(this_triplet)) {
	/* Put last one on list */
	lsNewEnd(triplets, (lsGeneric) this_triplet, LS_NH);
    }

    /* Handle remaining arguments as attribute assignments */
    for (i = optind;  i < argc;  i++) {
	if ( (p = strchr(argv[i], '=')) ) {
	    char *name, *value;

	    /* Attribute assignment */
	    *p = '\0';
	    name = argv[i];
	    value = p+1;
	    stat = dfStore(name, value);
	    if (stat == VEM_CANTFIND) {
		fprintf(stderr, "%s: warning: there is no default named `%s'\n",
			argv[0], name);
	    } else if (stat != VEM_OK) {
		fprintf(stderr, "%s: warning: can't assign value of `%s' to `%s'\n\n",
			argv[0], value, name);
	    }
	} else {
	    fprintf(stderr, "%s: Unknown argument `%s'\n", argv[0], argv[i]);
	    optUsage();
	}
    }

    return triplets;
}

static CommandArgs *new_triplet(fct_spec, geo_spec, rpc_spec)
STR fct_spec;			/* Facet specification    */
STR geo_spec;			/* Geometry specification */
STR rpc_spec;			/* Remote specification   */
/*
 * Allocates and fills in a new triplet structure.
 */
{
    CommandArgs *result;

    result = VEMALLOC(CommandArgs);
    result->facet_spec = fct_spec;
    result->geo_spec = geo_spec;
    result->rpc_spec = rpc_spec;
    return result;
}

static int null_triplet(triplet)
CommandArgs *triplet;		/* Triplet to examine */
/*
 * If all fields in the triplet are zero,  this routine
 * returns a non-zero return value.
 */
{
    return (!triplet->facet_spec) && (!triplet->geo_spec) &&
      (!triplet->rpc_spec);
}


static void start_arg_windows(win_list)
lsList win_list;		/* Window specification triplets */
/*
 * This routine opens windows as specified by the triplets read
 * from `triplets'.  See vuRPCWindow for details on the opening
 * procedure.
 */
{
    lsGen gen;
    CommandArgs *triplet;
    Window new_win;
    octObject fct;
    char prompt[VEMMAXSTR*4];

    ohUnpackDefaults(&fct, "a", ":physical:contents");
    if ( (gen = lsStart(win_list)) ) {
	while (lsNext(gen, (lsGeneric *) &triplet, LS_NH) == LS_OK) {
	    vemBusy();
	    ohUnpackFacetName(&fct, triplet->facet_spec);
	    vemMsg(MSG_C, "Reading cell %s:%s:%s...",
		   fct.contents.facet.cell,
		   fct.contents.facet.view,
		   fct.contents.facet.facet);
	    (void) sprintf(prompt, "Window viewing cell %s:%s:%s",
		    fct.contents.facet.cell,
		    fct.contents.facet.view,
		    fct.contents.facet.facet);
	    if (vuRPCWindow(&new_win, prompt, &fct, triplet->rpc_spec,
			    triplet->geo_spec) != VEM_OK) {
		vemMsg(MSG_A, "Could not open new window\n");
	    }
	    vemMsg(MSG_C, "done\n");
	}
	lsFinish(gen);
    }
    vemNotBusy();
}


/* 
 * Local event queue
 *
 * The following queue of local events is used to support interruptable
 * redisplay.  It is a circular fifo queue.  The top pointer always
 * points to an empty space.
 */

#define MAX_Q_EVENTS	2000
static XEvent vem_local_queue[MAX_Q_EVENTS];
static int vem_top_event = 0;
static int vem_bot_event = 0;

#define VEM_QUEUE_SIZE	\
(vem_top_event-vem_bot_event + (vem_top_event<vem_bot_event ? MAX_Q_EVENTS : 0))

#define VEM_INCR_BOT \
if ((++vem_bot_event) >= MAX_Q_EVENTS) vem_bot_event = 0

#define VEM_INCR_TOP \
if ((++vem_top_event) >= MAX_Q_EVENTS) vem_top_event = 0; \
VEM_CKRVAL(vem_top_event != vem_bot_event, "Local queue overflow", VEM_CORRUPT);

#define VEM_DECR_TOP \
if ((--vem_top_event) < 0) vem_top_event = MAX_Q_EVENTS-1;

static int vemCheckKey(evt, key)
XEvent *evt;
int key;
/* Returns 1 if the keypressed event `evt' is `key' */
{
    int nbytes;
    char str[10];

    nbytes = XLookupString(&(evt->xkey), str, 10, (KeySym) 0,
			   (XComposeStatus *) 0);
    return (nbytes == 1) && (str[0] == key);
}

#define VEM_IS_INTR(evt) \
((((evt)->type == KeyPress) && vemCheckKey(evt, CONTROL_C)) || ((evt)->type == UnmapNotify))

int vemPending()
/*
 * Returns the number of events outstanding on both VEMs local queue
 * and on the X queue.
 */
{
#ifdef OLD
    return VEM_QUEUE_SIZE + XPending(vemDisp);
#else
    return VEM_QUEUE_SIZE + (XtAppPending(vemContext) ? 1 : 0);
#endif
}

static void vemNextEvent(evt)
XEvent *evt;			/* X Event (RETURN) */
/*
 * This is a replacement function for XNextEvent which first
 * returns events from the local queue.
 */
{
    while (VEM_QUEUE_SIZE) {
	*evt = vem_local_queue[vem_bot_event];
	VEM_INCR_BOT;
	if (!VEM_IS_INTR(evt)) return;
    }
#ifdef OLD
    XNextEvent(vemDisp, evt);
#else
    XtAppNextEvent(vemContext, evt);
#endif
}

vemStatus vemIntrCheck(event)
register XEvent *event;		/* X Event (RETURN) */
/*
 * This routine scans the current X queue for interrupt events.
 * Interrupt events are KeyPressed events which are translated
 * into ^C or iconification of a VEM window.  In either case,
 * the routine will return VEM_INTERRUPT and return the event.
 * If it sees events which are not interrupt events,  it will
 * queue them on VEM's local queue.  It tries to reduce mouse-moved
 * events to prevent queue overflow.
 *
 * I can't use XtAppPending() here because it flushes the output.
 */
{
    while (XEventsQueued(vemDisp, QueuedAfterReading) > 0) {
	XNextEvent(vemDisp, event);
	if (VEM_IS_INTR(event)) {
	    return VEM_INTERRUPT;
	} else {
	    if (VEM_QUEUE_SIZE) {
		if (event->type == MotionNotify && 
		    (vem_local_queue[vem_top_event-1].type == MotionNotify)) {
		    VEM_DECR_TOP;
		}
	    }
	    vem_local_queue[vem_top_event] = *event;
	    VEM_INCR_TOP;
	}
    }
    return VEM_OK;
}



#define MAXKEYS	10

int vemHandleEvent(evt)
XEvent *evt;
/*
 * Handles a general X event.
 */
{
    int stopFlag = 0;
    int nbytes, i;
    char keystr[MAXKEYS];

    if (XtDispatchEvent(evt)) {
	return 0;
    }
    if (wnFilter(evt) == VEM_OK) {
	return 0;
    }

    /* Must be a normal VEM event */
    switch (evt->type) {
    case KeyPress:
	/* Interpret keys typed */
	nbytes = XLookupString(&(evt->xkey), keystr, MAXKEYS,
			       (KeySym) 0, (XComposeStatus *) 0);
	for (i = 0;  i < nbytes;  i++) {
	    if ( (stopFlag = doKeyInput(evt->xkey.window,
				      evt->xkey.x, evt->xkey.y,
				      keystr[i])) ) {
		break;
	    }
	}
	break;
    case ButtonPress:
	switch (evt->xbutton.button) {
	case Button1:
	    atrSetXor(satr);
	    if (doNormButton(&(evt->xbutton)) == VEM_FALSE) XBell(vemDisp, 0);
	    break;
	case Button2:
	    stopFlag = doMenu(&(evt->xbutton),
			      (int) (evt->xbutton.state & (ShiftMask|LockMask)));
	    break;
	case Button3:
	    atrSetXor(satr);
	    if (doAltButton(&(evt->xbutton)) == VEM_FALSE) XBell(vemDisp, 0);
	    break;
	default:
	    XBell(vemDisp, 0);
	    break;
	}
	break;
    case MappingNotify:
	/* This event is sent regardless of interest - might be handled
	 * by the toolkit.
	 */
	vemMsg(MSG_A, "Change in modifer, keyboard, or pointer mapping.\n");
	if (evt->xmapping.request == MappingKeyboard) {
	    XRefreshKeyboardMapping(&(evt->xmapping));
	}
	break;
    default:
#ifdef DEBUG
	(void) sprintf(errMsgArea, "Unknown window event type: 0x%x\n",
		evt->type);
	vemMessage(errMsgArea, 0);
#endif
	break;
    }
    return stopFlag;
}


int vemProcessXEvent()
/*
 * This routine checks the length of the X queue and processes any
 * outstanding X input events.  This includes calling the appropriate
 * routines which provide argument list management,  handling exposure
 * events on the console and graphics windows,  and changing the border
 * color when the mouse cursor moves between VEM graphics windows.
 */
{
    XEvent theEvent;		/* Driving X event       */
    int stopFlag = 0;		/* VEM should stop       */

    /*
     * This loop handles all of the X requests and then flushes
     * all graphics window updates using wnFlush.
     */
    if (!setjmp(err_state)) {
	vemPushErrContext(err_state);
    }
    while ((!stopFlag) && (vemPending() > 0)) {
	vemNextEvent(&theEvent);
	stopFlag = vemHandleEvent(&theEvent);
    }
    /* Flush all of the queued updates to the display */
    if ( stopFlag ) {
	wnQuickFlush();
    } else {
	wnFlush();
    }
    /* Turn off stopWatch cursor */
    vemNotBusy();
    /* Pop off errtrap handler */
    vemPopErrContext();
    /* Return flag indicating whether VEM should stop */
    return stopFlag;
}


int vemSyncEvents()
/*
 * This routine makes sure VEM is up to date as far as events
 * are concerned before going into some other event loop.
 */
{
    XSync(vemDisp, 0);
    if (vemProcessXEvent()) {
	exit(0);
    }
    return VEM_OK;
}



static int doKeyInput(win, x, y, key)
Window win;			/* Window it was typed into */
int x, y;			/* Location in window       */
int key;			/* ASCII character typed    */
/*
 * Interprets the character typed.  There are two states in keyboard
 * input:  command and text.  Command mode looks up the character
 * and calls a command with the command argument list from the current
 * input context structure.  In text mode, the characters typed are 
 * added to the argument list as a unit until the closing delimiter 
 * is seen or a point is entered.  `cur_con.state' is set to the closing 
 * delimeter if we are in the text state, zero if we are not in the 
 * text state.
 */
{
    if (cur_con.state == TEXT_STATE) {
	/* Text argument input */
	if (handleTextArg(key)) return 1;
    } else if (cur_con.state == COMMAND_STATE) {
	/* Type-in command name (with completion) */
	if (handleTypeInCmd(win, x, y, key)) return 1;
    } else {
	/* Single character command invocation */
	if (handleSingleKey(win, x, y, key)) return 1;
    }
    return 0;
}



/*
 * Console window callback
 */

/*ARGSUSED*/
static void vemConsInput(w, client_data, call_data)
Widget w;			/* Console window widget   */
caddr_t client_data;		/* Static data from call   */
caddr_t call_data;		/* Dynamic data from call  */
/*
 * This routine handles typed input into the console window.  The
 * call data is actually a pointer to (ts_call_data) which indicates
 * what characters where typed.  This is passed on to doKeyInput.
 */
{
    ts_call_data *data = (ts_call_data *) call_data;
    int i;

    for (i = 0;  i < data->len;  i++) {
	if (doKeyInput((Window) 0, 0, 0, data->buf[i])) exit(0);
    }
}



static int handleTextArg(inKey)
int inKey;			/* Input key             */
/*
 * This routine interprets a character assuming VEM is in the text
 * argument input state (double quote input).
 */
{
    vemOneArg *currentArg;
    STR comp_str;
    char **completions;
    int start, span, comp_count;

    lsLastItem(cur_con.commandArgs, (lsGeneric *) &currentArg, LS_NH);
    /* This had better be text! */
    if (currentArg->argType != VEM_TEXT_ARG) {
	vemMessage("\nYou are not specifing a text argument\n", MSG_DISP);
	cur_con.state = 0;
	return 0;
    }
    if (inKey == cur_con.state) {
	/* Terminate text input */
	cur_con.state = 0;
	currentArg->argData.vemTextArg[currentArg->length] = NULL_CHAR;
	echoUpdate(cur_con.commandArgs);
    } else if ((inKey == BACKSPACE) || (inKey == DELETE)) {
	/* Delete a character */
	if (currentArg->length > 0) {
	    currentArg->length -= 1;
	    echoUpdate(cur_con.commandArgs);
	} else {
	    /* Break out of string */
	    lsDelEnd(cur_con.commandArgs, (lsGeneric *) &currentArg);
	    VEMFREE(currentArg);
	    cur_con.state = 0;
	    echoUpdate(cur_con.commandArgs);
	}
    } else if (inKey == CONTROL_W) {
	/* Delete a word */
	if (currentArg->length > 0) {
	    /* Delete word - eliminate space, alphanum */
	    while ((currentArg->length > 0) &&
		   isspace(currentArg->argData.vemTextArg[currentArg->length-1]))
	      (currentArg->length)--;
	    if (currentArg->length > 0) (currentArg->length)--;
	    while ((currentArg->length > 0) &&
		   (currentArg->argData.vemTextArg[currentArg->length-1] != '/') &&
		   isalnum(currentArg->argData.vemTextArg[currentArg->length-1]))
	      (currentArg->length)--;
	    echoUpdate(cur_con.commandArgs);
	} else {
	    /* Break out of string */
	    lsDelEnd(cur_con.commandArgs, (lsGeneric *) &currentArg);
	    VEMFREE(currentArg);
	    cur_con.state = 0;
	    echoUpdate(cur_con.commandArgs);
	}
    } else if ((inKey == CONTROL_U) || (inKey == CONTROL_X)) {
	/* Delete all input */
	if (currentArg->length > 0) {
	    currentArg->length = 0;
	    echoUpdate(cur_con.commandArgs);
	} else {
	    /* Break out of string */
	    lsDelEnd(cur_con.commandArgs, (lsGeneric *) &currentArg);
	    VEMFREE(currentArg);
	    cur_con.state = 0;
	    echoUpdate(cur_con.commandArgs);
	}
    } else if (inKey == TAB) {
	/* Attempt to use file name completion */
	currentArg->argData.vemTextArg[currentArg->length] = '\0';
	comp_str = strrchr(currentArg->argData.vemTextArg, SPACE);
	if (comp_str == (char *) 0) {
	    comp_str = currentArg->argData.vemTextArg;
	} else {
	    comp_str++;
	}
	completions = fc_complete(comp_str, &start, &span);
	comp_count = 0;
	while (completions && completions[comp_count]) comp_count++;
	if (comp_count > 0) {
	    /* Move significant characters */
	    STRNMOVE(currentArg->argData.vemTextArg + currentArg->length,
		     completions[0] + start, span);
	    currentArg->length += span;
	    if (comp_count == 1) echoUpdate(cur_con.commandArgs);
	    if (comp_count > 1) {
		/* Multiple completions -- write out completions */
		vemMsg(MSG_C, "\n");
		comp_count = 0;
		while (completions && completions[comp_count]) {
		    if (comp_count) {
			vemMsg(MSG_C, " %s", completions[comp_count]);
		    } else {
			vemMsg(MSG_C, "%s", completions[comp_count]);
		    }
		    comp_count++;
		}
		vemMsg(MSG_C, "\n%s", cur_con.prompt);
		echoRedraw(cur_con.commandArgs);
	    }
	} else {
	    /* No completions -- beep */
	    XBell(vemDisp, 0);
	}
    } else if ((inKey >= SPACE) && (inKey <= LAST_CHAR)) {
	/* Normal addition of character */
	if (currentArg->length < VEMMAXSTR) {
	    currentArg->argData.vemTextArg[currentArg->length] = inKey;
	    currentArg->length += 1;
	    echoUpdate(cur_con.commandArgs);
	} else {
	    /* No more text */
	    XBell(vemDisp, 0);
	}
    }
    return 0;
}


static int handleTypeInCmd(win, x, y, inKey)
Window win;			/* Current window     */
int x, y;			/* Position in window */
char inKey;			/* Input key          */
/*
 * This routine handles input keystrokes when the user is typing
 * in a command alias name.  The routine supports all the basic
 * editing primitives and command completion.
 */
{
    vemPoint curSpot;
    static char completions[4096];
    int numComplete, curSpotFlag, prevLength;
    bdTable mainTbl, altTbl;
    bdBinding command;

    if (inKey == cur_con.state) {
	/* Terminate command input - try to invoke command */
	/* Figure out the spot and binding tables */
	if (wnPoint(win, x, y, &(curSpot.theFct), &(curSpot.thePoint),
		    VEM_NOSNAP) != VEM_OK)
	  {
	      /* No current spot */
	      curSpotFlag = 0;
	      bdGetTable("standard", &mainTbl);
	      altTbl = (bdTable) 0;
	  } else {
	      /* Current spot */
	      curSpotFlag = 1;
	      curSpot.theWin = win;
	      curSpot.x = x;
	      curSpot.y = y;
	      bufBinding(curSpot.theFct, &mainTbl);
	      wnGetInfo(win, (wnOpts *) 0, (octId *) 0,
			(octId *) 0, &altTbl, (struct octBox *) 0);
	  }
	/* Then,  attempt to find alias for command */
	prevLength = cur_con.typeInLength;
	cur_con.typeInCommand[cur_con.typeInLength] = NULL_CHAR;
	if (bdName(mainTbl, altTbl, cur_con.typeInCommand, &numComplete,
		   completions, &command) == VEM_CANTFIND)
	  {
	      /* Totally in error */
	      XBell(vemDisp, 0);
	      return 0;
	  }
	else
	  {
	      /* Possible command completions */
	      cur_con.typeInLength = STRLEN(cur_con.typeInCommand);
	      if (command.bindType) {
		  /* If there is a binding,  use it */
		  echoUpdate(cur_con.commandArgs);
		  if (curSpotFlag)
		    return (*cur_con.invocation)(&curSpot, &command, 0);
		  else
		    return (*cur_con.invocation)((vemPoint *) 0, &command, 0);
	      } else if (cur_con.typeInLength != prevLength) {
		  /* Else if length has changed,  echo update */
		  echoUpdate(cur_con.commandArgs);
		  return 0;
	      } else {
		  /* Else write out completion string and ARG_RESP */
		  (void) sprintf(errMsgArea, "\n%s\n%s", completions,
			  cur_con.prompt);
		  vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
		  echoRedraw(cur_con.commandArgs);
		  return 0;
	      }
	  }
    } else if ((inKey == BACKSPACE) || (inKey == DELETE)) {
	/* Delete a character */
	if (cur_con.typeInLength > 0) {
	    cur_con.typeInLength -= 1;
	    cur_con.typeInCommand[cur_con.typeInLength] = NULL_CHAR;
	    echoUpdate(cur_con.commandArgs);
	} else {
	    /* Break out of command input */
	    cur_con.state = 0;
	    echoUpdate(cur_con.commandArgs);
	}
    } else if (inKey == CONTROL_W) {
	/* Delete a word */
	if (cur_con.typeInLength > 0) {
	    /* Delete word - eliminate space, alphanum, and more space */
	    while ((cur_con.typeInLength > 0) &&
		   isspace(cur_con.typeInCommand[cur_con.typeInLength-1]))
	      cur_con.typeInLength--;
	    if (cur_con.typeInLength > 0) cur_con.typeInLength--;
	    while ((cur_con.typeInLength > 0) &&
		   (cur_con.typeInCommand[cur_con.typeInLength-1] != '-') &&
		   isalnum(cur_con.typeInCommand[cur_con.typeInLength-1]))
	      cur_con.typeInLength--;
	    cur_con.typeInCommand[cur_con.typeInLength] = NULL_CHAR;
	    echoUpdate(cur_con.commandArgs);
	} else {
	    /* Break out of command */
	    cur_con.state = 0;
	    echoUpdate(cur_con.commandArgs);
	}
    } else if ((inKey == CONTROL_U) || (inKey == CONTROL_X)) {
	/* Delete all input */
	if (cur_con.typeInLength > 0) {
	    cur_con.typeInLength = 0;
	    echoUpdate(cur_con.commandArgs);
	} else {
	    /* Break out of string */
	    cur_con.state = 0;
	    echoUpdate(cur_con.commandArgs);
	}
    } else if (inKey == TAB) {
	/* Attempt to complete input */
	/* Figure out the binding tables */
	if (wnPoint(win, x, y, &(curSpot.theFct), &(curSpot.thePoint),
		    VEM_NOSNAP) != VEM_OK)
	  {
	      /* No current spot */
	      curSpotFlag = 0;
	      bdGetTable("standard", &mainTbl);
	      altTbl = (bdTable) 0;
	  } else {
	      /* Current spot */
	      bufBinding(curSpot.theFct, &mainTbl);
	      wnGetInfo(win, (wnOpts *) 0, (octId *) 0, (octId *) 0,
			&altTbl, (struct octBox *) 0);
	  }
	/* Then,  attempt to find alias for command */
	prevLength = cur_con.typeInLength;
	cur_con.typeInCommand[cur_con.typeInLength] = NULL_CHAR;
	if (bdName(mainTbl, altTbl, cur_con.typeInCommand, &numComplete,
		   completions, &command) == VEM_CANTFIND)
	  {
	      /* Totally in error */
	      XBell(vemDisp, 0);
	      return 0;
	  }
	else
	  {
	      /* Possible command completions */
	      cur_con.typeInLength = STRLEN(cur_con.typeInCommand);
	      if (cur_con.typeInLength != prevLength) {
		  /* Else if length has changed,  echo update */
		  echoUpdate(cur_con.commandArgs);
		  return 0;
	      } else {
		  /* Else write out completion string and ARG_RESP */
		  (void) sprintf(errMsgArea, "\n%s\n%s", completions,
			  cur_con.prompt);
		  vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
		  echoRedraw(cur_con.commandArgs);
		  return 0;
	      }
	  }
    } else if ((inKey >= SPACE) && (inKey <= LAST_CHAR)) {
	/* Normal addition of character */
	if (cur_con.typeInLength < MAX_TYPE_IN_COMMAND) {
	    cur_con.typeInCommand[cur_con.typeInLength] = inKey;
	    cur_con.typeInLength++;
	    echoUpdate(cur_con.commandArgs);
	} else {
	    /* Too many characters */
	    XBell(vemDisp, 0);
	}
    }
    return 0;
}


static int handleSingleKey(win, x, y, inKey)
Window win;			/* Current window        */
int x, y;			/* Position in window    */
char inKey;			/* Input key             */
/*
 * This routine handles character input while edting the primary
 * argument list.  Most of the time,  this is interpreting the
 * keystroke as a keystroke command.  However,  it may also
 * put VEM in a text input or command input state.
 */
{
    vemOneArg *currentArg;
    vemPoint curSpot;
    bdTable mainTbl, altTbl;
    bdBinding command;
    int curSpotFlag;

    if (inKey == TEXT_STATE) {
	/* Go into text input mode */
	cur_con.state = TEXT_STATE;
	currentArg = VEMALLOC(vemOneArg);
	currentArg->argType = VEM_TEXT_ARG;
	currentArg->length = 0;
	currentArg->argData.vemTextArg = VEMARRAYALLOC(char, VEMMAXSTR);
	if ( cur_con.commandArgs ) {
	    lsNewEnd(cur_con.commandArgs, (lsGeneric) currentArg, LS_NH);
	    echoUpdate(cur_con.commandArgs);
	} else {
	    fprintf( stderr, "Hey, you started typing too soon!\n" );
	}
	return 0;
    } else if (inKey == ENTER_COMMAND) {
	/* Go into type-in command mode */
	cur_con.state = COMMAND_STATE;
	cur_con.typeInLength = 0;
	cur_con.typeInCommand[0] = NULL_CHAR;
	echoUpdate(cur_con.commandArgs);
	return 0;
    } else {
	/* Find curSpot and main and alternate binding tables */
	if (wnPoint(win, x, y, &(curSpot.theFct), &(curSpot.thePoint),
		    VEM_NOSNAP) != VEM_OK)
	  {
	      /* No current spot */
	      curSpotFlag = 0;
	      bdGetTable("standard", &mainTbl);
	      altTbl = (bdTable) 0;
	  } else {
	      	  /* Current spot */
	      curSpotFlag = 1;
	      curSpot.theWin = win;
	      curSpot.x = x;
	      curSpot.y = y;
	      bufBinding(curSpot.theFct, &mainTbl);
	      wnGetInfo(win, (wnOpts *) 0, (octId *) 0, (octId *) 0,
			&altTbl, (struct octBox *) 0);
	  }
	/* 
	 * Interpret as a command - try to find binding.
	 * Check main table first,  then alternate table.
	 */
	if ((bdKey(mainTbl, inKey, &command) != VEM_OK) &&
	    (bdKey(altTbl, inKey, &command) != VEM_OK))
	  {
	      /* Could output message and ARG_RESP */
	      XBell(vemDisp, 0);
	      (void) sprintf(errMsgArea, "\nNo function binding for '%c'\n", inKey);
	      vemMessage(errMsgArea, 0);
	      return 0;
	  }
	else
	  {
	      /* Invoke command with command type out */
	      if (curSpotFlag)
		return (*cur_con.invocation)(&curSpot, &command, TYPE_OUT);
	      else
		return (*cur_con.invocation)((vemPoint *) 0, &command, TYPE_OUT);
	  }
    }
}



/* Low-level detail on initial state of button handler */

#define NEWARG	0x01		/* If on,  new argument                 */
#define P2B	0x02		/* If on,  point to box else line sweep */

/* High level state bits */

#define SWEEP	0x04		/* Sweeping has been detected   */
#define STOPBIT	0x08		/* End local event loop         */
#define CANCEL	0x10		/* Cancel tracing action        */
#define DELARG	0x20		/* Argument deleted             */

static vemStatus doNormButton(buttonEvent)
XButtonEvent *buttonEvent; 	/* Button event itself     */
/*
 * Handles normal (unshifted) left-button pressed events.  The
 * window must be a graphics window.  If it is,  the left button
 * is automatically grabbed.  A point is deposited at the location immediately
 * to give feed-back.  If the point is over some other component of
 * the arg-list,  this point is analyzed.  If its another point,
 * it will be removed from its current argument and made into a line
 * which can be swept out.  If its part of a line,  a new point in
 * that line will be added after the point and it can be swept
 * out freely.  If the initial button push is not over a point or
 * line,  a point is appended onto the end of the arg and it can
 * be upgraded to a box by dragging.  Dragging is measured in terms
 * of distance traveled.  This parameter can be controlled by
 * the default `argument.pixels'.
 */
{
    Window root, child;
    wnOpts theOpts;
    vemOneArg *overArg, *curArg = (vemOneArg *)NULL;
    vemOneArg *lastArg = (vemOneArg *)NULL;
    int curIdx = 0, root_x, root_y;
    unsigned int r_mask;
    XEvent localEvent;
    vemPoint startPoint, curPoint, newPoint;
    int stateWord = 0, idx, diffPixels, dragPixels;

    /* First,  make sure the state is appropriate */
    if ((cur_con.state == TEXT_STATE) || (cur_con.state == COMMAND_STATE)) {
	/* Take it out of text state */
	cur_con.state = 0;
	cur_con.typeInLength = 0;
	cur_con.typeInCommand[0] = NULL_CHAR;
	echoUpdate(cur_con.commandArgs);
    }
    dfGetInt("argument.pixels", &dragPixels);
    wnGetInfo(buttonEvent->window, &theOpts, (octId *) 0,
	      (octId *) 0, (bdTable *) 0, (struct octBox *) 0);

    /* Change current point into a real point */
    startPoint.theWin = buttonEvent->window;
    startPoint.x = buttonEvent->x;
    startPoint.y = buttonEvent->y;
    if (wnPoint(startPoint.theWin, startPoint.x, startPoint.y,
		&(startPoint.theFct), &(startPoint.thePoint),
		VEM_SETPNT|VEM_DPNT) != VEM_OK)
      {
	  XUngrabPointer(vemDisp, buttonEvent->time);
	  return VEM_FALSE;
      }

    /* See if its on top of a point or line */
    if (findArg(&startPoint, dragPixels, &overArg, &idx) == VEM_OK) {
	switch (overArg->argType) {
	case VEM_POINT_ARG:
	    if (overArg->length == 1) {
		/* Its the only point - change type to line */
		argType(overArg, VEM_LINE_ARG);
		argAddPoint(overArg, &(overArg->argData.points[0]));
		curArg = overArg;
		curIdx = 1;
		stateWord = SWEEP;
	    } else {
		/* Its among other points - make new line arg */
		curArg = newArg(VEM_LINE_ARG);
		argAddPoint(curArg, &(overArg->argData.points[idx]));
		argAddPoint(curArg, &(overArg->argData.points[idx]));
		/* Delete old point */
		argDelPoint(overArg, idx);
		curIdx = 1;
		stateWord = SWEEP|NEWARG;
	    }
	    break;
	case VEM_LINE_ARG:
	    /* Insert point after current location */
	    argInsPoint(overArg, idx, &(overArg->argData.points[idx]));
	    curArg = overArg;
	    curIdx = idx+1;
	    stateWord = SWEEP;
	    break;
	default:
	    /* No modes needed */
	    stateWord = P2B;
	    break;
	}
    } else {
	stateWord = P2B;
    }

    if (stateWord & P2B) {
	lastArg = (vemOneArg *) 0;
	if ((lsLastItem(cur_con.commandArgs, (lsGeneric *) &lastArg, LS_NH) == LS_OK) &&
	    (lastArg->argType == VEM_POINT_ARG))
	  {
	      /* Add new point to end of argument */
	      argAddPoint(lastArg, &startPoint);
	      curArg = lastArg;
	      curIdx = curArg->length-1;
	  } else {
	      /* Create new point argument        */
	      curArg = newArg(VEM_POINT_ARG);
	      argAddPoint(curArg, &startPoint);
	      curIdx = 0;
	      stateWord |= NEWARG;
	  }
    }

    /* Enter event loop */
    newPoint = curPoint = startPoint;
    while (!(stateWord & STOPBIT)) {
	vemNextEvent(&localEvent);
	switch (localEvent.type) {
	case ButtonRelease:
	    /* If its the left button we get out */
	    if (localEvent.xbutton.button == Button1) {
		stateWord |= STOPBIT;
	    }
	    break;
	case ButtonPress:
	    /* It has to be some other button.  Turn on cancel */
	    stateWord |= CANCEL;
	    break;
	case MotionNotify:
	    /* Do an update,  compute points coordinates */
	    /* If they are different from current points */
	    /* Update box.  If not in box mode,  put in  */
	    /* box mode. */
	    XQueryPointer(vemDisp, localEvent.xany.window,
			  &root, &child, &root_x, &root_y,
			  &newPoint.x, &newPoint.y, &r_mask);
	    if (wnPoint(localEvent.xany.window, newPoint.x, newPoint.y,
			&newPoint.theFct, &newPoint.thePoint, VEM_DPNT)!=VEM_OK)
	      continue;
	    if (newPoint.theFct != startPoint.theFct)
	      continue;

	    /* See if we are looking for sweep */
	    if (stateWord & SWEEP) {
		/* Already sweeping - just move the point if different */
		if ((newPoint.thePoint.x != curPoint.thePoint.x) ||
		    (newPoint.thePoint.y != curPoint.thePoint.y))
		  {
		      argChgPoint(curArg, curIdx, &newPoint);
		      curPoint.thePoint  = newPoint.thePoint;
		  }
	    } else {
		/* Not sweeping yet */
		diffPixels = VEMMAX(VEMABS(newPoint.x-startPoint.x),
				    VEMABS(newPoint.y-startPoint.y));
		if (diffPixels > dragPixels) {
		    /* Its sweeping now - we know its P2B */
		    if (stateWord & NEWARG) {
			/* See if lastArg was boxes */
			if (lastArg && (lastArg->argType == VEM_BOX_ARG)) {
			    /* Add onto end */
			    delArg(curArg);
			    curArg = lastArg;
			    argAddPoint(curArg, &startPoint);
			    argAddPoint(curArg, &newPoint);
			    curIdx = (lastArg->length*2)-1;
			    stateWord &= (~NEWARG);
			} else {
			    /* Change into boxes */
			    argType(curArg, VEM_BOX_ARG);
			    argAddPoint(curArg, &newPoint);
			    curIdx = 1;
			}
		    } else {
			/* Create new box argument */
			argDelPoint(curArg, curIdx);
			curArg = newArg(VEM_BOX_ARG);
			argAddPoint(curArg, &startPoint);
			argAddPoint(curArg, &newPoint);
			stateWord |= NEWARG;
			curIdx = 1;
		    }
		    stateWord |= SWEEP;
		}
	    }
	    break;
	default:
	    /* Send event to general dispatch */
	    if (vemHandleEvent(&localEvent)) exit(0);
	    break;
	}
    }
    XUngrabPointer(vemDisp, CurrentTime);

    if (stateWord & CANCEL) {
	/* Abort changes */
	if (stateWord & NEWARG) {
	    delArg(curArg);
	} else {
	    argDelPoint(curArg, curIdx);
	}
    } else {
	/* Normal end */
	if (curIdx > 0) {
	    /* See if same as previous point */
	    if ((curArg->argData.points[curIdx-1].theFct ==
		 curArg->argData.points[curIdx].theFct) &&
		(curArg->argData.points[curIdx].thePoint.x ==
		 curArg->argData.points[curIdx-1].thePoint.x) &&
		(curArg->argData.points[curIdx].thePoint.y ==
		 curArg->argData.points[curIdx-1].thePoint.y))
	      {
		  /* Its on top -- delete it */
		  argDelPoint(curArg, curIdx);
		  if (curArg->length == 0) {
		      removeArg(curArg);
		      stateWord |= DELARG;
		  } else if (curArg->length == 1) {
		      if ((curArg->argType == VEM_LINE_ARG)) {
			  argType(curArg, VEM_POINT_ARG);
		      }
		  }
	      }
	}
	if (!(stateWord & DELARG) && (stateWord & NEWARG) && (curArg->length > 0)) {
	    /* Add argument onto the end of the list */
	    lsNewEnd(cur_con.commandArgs, (lsGeneric) curArg, LS_NH);
	}
    }
    echoUpdate(cur_con.commandArgs);
    return VEM_OK;
}

static void removeArg(oneArg)
vemOneArg *oneArg;
/*
 * Removes the specified argument from the current command
 * arguments.
 */
{
    lsGen theGen;
    vemOneArg *theArg;

    theGen = lsStart(cur_con.commandArgs);
    while (lsNext(theGen, (lsGeneric *) &theArg, LS_NH) == LS_OK) {
	if (theArg == oneArg) {
	    lsDelBefore(theGen, (lsGeneric *) &theArg);
	    break;
	}
    }
    lsFinish(theGen);
    vuOffSelect(oneArg);
}



/*ARGSUSED*/
static vemStatus doAltButton(buttonEvent)
XButtonEvent *buttonEvent; 	/* Button event itself     */
/*
 * Handles alternate (right button) pressed events.  This button
 * generally modifies the argument under the cursor.  If it
 * is a point of a point, line, or box argument,  it picks it
 * up and allows the user to move it elsewhere.  If it is
 * an object set,  the set is interactively moved elsewhere.
 */
{
    vemOneArg *overArg;
    vemPoint startPoint, origPoint;
    struct octTransform origTran;
    int dragPixels, idx;

    /* Make sure we are in the right state */
    if ((cur_con.state == TEXT_STATE) || (cur_con.state == COMMAND_STATE)) {
	/* Take it out of that state */
	cur_con.state = 0;
	cur_con.typeInLength = 0;
	cur_con.typeInCommand[0] = NULL_CHAR;
	echoUpdate(cur_con.commandArgs);
    }

    /* Change current point into a real point */
    startPoint.theWin = buttonEvent->window;
    startPoint.x = buttonEvent->x;
    startPoint.y = buttonEvent->y;
    if (wnPoint(startPoint.theWin, startPoint.x, startPoint.y,
		&(startPoint.theFct), &(startPoint.thePoint),
		VEM_SETPNT|VEM_DPNT) != VEM_OK)
      {
	  XUngrabPointer(vemDisp, buttonEvent->time);
	  return VEM_FALSE;
      }

    /* See if its on top of a normal arg */
    dfGetInt("argument.pixels", &dragPixels);
    if (findArg(&startPoint, dragPixels, &overArg, &idx) == VEM_OK) {
	origPoint = overArg->argData.points[idx];
	/* Drag this point around */
	if (!movePoint(&startPoint, overArg, idx)) {
	    /* Abort - move it back to its original position */
	    argChgPoint(overArg, idx, &origPoint);
	}
    } else if ((lsFirstItem(cur_con.commandArgs,
			    (lsGeneric *) &overArg, LS_NH) == LS_OK) &&
	       (overArg->argType == VEM_OBJ_ARG))
      {
	  /* Drag the objects around */
	  origTran = overArg->argData.vemObjArg.tran;
	  if (!moveObjs(&startPoint, overArg)) {
	      /* Abort - move back to original position */
	      objTran(&(overArg->argData.vemObjArg), &origTran);
	  }
      }
    XUngrabPointer(vemDisp, CurrentTime);
    return VEM_OK;
}


static int movePoint(startPoint, modarg, modidx)
vemPoint *startPoint;		/* Starting snap point */
vemOneArg *modarg;		/* Modified argument   */
int modidx;			/* Point to modify     */
/*
 * This routine interactively drags a point of an argument as the
 * mouse moves.  The point dragged is the `idx'th point of `modarg'.
 * The point is originally moved to `startPoint'.
 */
{
    vemPoint curPoint, newPoint;
    Window root, child;
    XEvent localEvent;
    int root_x, root_y;
    unsigned int r_mask;
    int stop_flag = 0;
    int cancel_flag = 0;

    /* Begin by snapping to `startPoint' */
    argChgPoint(modarg, modidx, startPoint);
    curPoint = newPoint = *startPoint;
    while (!stop_flag) {
	vemNextEvent(&localEvent);
	switch (localEvent.type) {
	case ButtonRelease:
	    /* If right button,  we get out */
	    if (localEvent.xbutton.button == Button3) {
		stop_flag = 1;
	    }
	    break;
	case ButtonPress:
	    /* Some other button is pushed - cancel stretch */
	    cancel_flag = stop_flag = 1;
	    break;
	case MotionNotify:
	    /* Move mouse to new location */
	    XQueryPointer(vemDisp, localEvent.xany.window,
			  &root, &child, &root_x, &root_y,
			  &newPoint.x, &newPoint.y, &r_mask);
	    if (wnPoint(localEvent.xany.window, newPoint.x, newPoint.y,
			&newPoint.theFct, &newPoint.thePoint, VEM_DPNT) != VEM_OK)
	      continue;
	    if (newPoint.theFct != startPoint->theFct)
	      continue;

	    if ((newPoint.thePoint.x != curPoint.thePoint.x) ||
		(newPoint.thePoint.y != curPoint.thePoint.y))
	      {
		  argChgPoint(modarg, modidx, &newPoint);
		  curPoint.thePoint  = newPoint.thePoint;
	      }
	    break;
	default:
	    /* Send it to general dispatch */
	    if (vemHandleEvent(&localEvent)) exit(0);
	    break;
	}
    }
    return !cancel_flag;
}


static int moveObjs(startPoint, modarg)
vemPoint *startPoint;		/* Starting snap point */
vemOneArg *modarg;		/* Argument to modify  */
/*
 * This routine interactively drags the objects in `modarg'
 * around as the mouse moves.  The original reference point
 * is given by `startPoint'.
 */
{
    XEvent localEvent;
    Window root, child;
    vemPoint curPoint, newPoint;
    int root_x, root_y;
    unsigned int r_mask;
    int stop_flag = 0, cancel_flag = 0, man_line_p = 0;
    octCoord off_x, off_y, man_x, man_y;
    wnOpts opts;

    if (wnGetInfo(startPoint->theWin, &opts, (octId *) 0, (octId *) 0,
		  (bdTable *) 0, (struct octBox *) 0) == VEM_OK) {
	man_line_p = (opts.disp_options & VEM_MANLINE) != 0;
    }
    curPoint = newPoint = *startPoint;
    while (!stop_flag) {
	vemNextEvent(&localEvent);
	switch (localEvent.type) {
	case ButtonRelease:
	    /* If right button,  we get out */
	    if (localEvent.xbutton.button == Button3) {
		stop_flag = 1;
	    }
	    break;
	case ButtonPress:
	    /* Some other button is pushed - cancel stretch */
	    cancel_flag = stop_flag = 1;
	    break;
	case MotionNotify:
	    /* Move mouse to new location */
	    XQueryPointer(vemDisp, localEvent.xany.window,
			  &root, &child, &root_x, &root_y,
			  &newPoint.x, &newPoint.y, &r_mask);
	    if (wnPoint(localEvent.xany.window, newPoint.x, newPoint.y,
			&newPoint.theFct, &newPoint.thePoint, VEM_DPNT) != VEM_OK)
	      continue;
	    if (newPoint.theFct != startPoint->theFct)
	      continue;

	    if (man_line_p) {
		man_x = newPoint.thePoint.x - startPoint->thePoint.x;
		man_y = newPoint.thePoint.y - startPoint->thePoint.y;
		if (VEMABS(man_x) > VEMABS(man_y)) {
		    newPoint.thePoint.y = startPoint->thePoint.y;
		} else {
		    newPoint.thePoint.x = startPoint->thePoint.x;
		}
	    }

	    off_x = newPoint.thePoint.x - curPoint.thePoint.x;
	    off_y = newPoint.thePoint.y - curPoint.thePoint.y;
	    if (off_x || off_y) {
		objMove(&(modarg->argData.vemObjArg), off_x, off_y);
		curPoint.thePoint = newPoint.thePoint;
	    }
	    break;
	default:
	    /* General dispatch */
	    if (vemHandleEvent(&localEvent)) exit(0);
	    break;
	}
    }
    return !cancel_flag;
}


static vemStatus findArg(pnt, tol, returnArg, returnIdx)
vemPoint *pnt;			/* Point to check          */
int tol;			/* Tolerance distance      */
vemOneArg **returnArg;		/* Argument (null if none) */
int *returnIdx;			/* Index of point          */
/*
 * This function takes a point and finds the first argument
 * which has that point as part of its input.  The nature
 * of input arguments limits the argument types of this
 * routine to point, line, or box (strings and objects are
 * not supported).  If there is no corresponding argument, 
 * 'returnArg' will be null.  A point is considered close
 * enough if it is within `tol' pixels from the argument point.
 */
{
    lsGen theGen;
    vemOneArg *theArg;
    vemPoint *allpnts;
    struct octPoint altPoint;
    int idx, twin;
    octCoord toct;
    long d, dist = VEMMAXINT;

    theGen = lsStart(cur_con.commandArgs);
    *returnArg = (vemOneArg *) 0;
    *returnIdx = 0;
    while (lsNext(theGen, (lsGeneric *) &theArg, LS_NH) == LS_OK) {
	allpnts = theArg->argData.points;
	switch (theArg->argType) {
	case VEM_BOX_ARG:
	    /* check each box */
	    for (idx = 0;  idx < theArg->length;  idx++) {
		/* Direct comparison */
		if ((allpnts[idx*2].theFct == pnt->theFct) &&
		    (wnEq(pnt->theWin, &(allpnts[idx*2].thePoint),
			  &(pnt->thePoint), tol, &d) == VEM_OK) &&
		    (d <= dist))
		  {
		      /* Found one */
		      *returnArg = theArg;
		      *returnIdx = idx*2;
		      dist = d;
		  }
		if ((allpnts[idx*2+1].theFct == pnt->theFct) &&
		    (wnEq(pnt->theWin, &(allpnts[idx*2+1].thePoint),
			  &(pnt->thePoint), tol, &d) == VEM_OK) &&
		    (d <= dist))
		  {
		      /* Found one */
		      *returnArg = theArg;
		      *returnIdx = idx*2+1;
		      dist = d;
		  }
		/* Alternate point comparison */
		altPoint.x = allpnts[idx*2+1].thePoint.x;
		altPoint.y = allpnts[idx*2].thePoint.y;
		if ((allpnts[idx*2].theFct == pnt->theFct) &&
		    (wnEq(pnt->theWin, &altPoint, &(pnt->thePoint),
			  tol, &d) == VEM_OK) &&
		    (d <= dist))
		  {
		      /* Fix up the box */
		      twin = allpnts[idx*2].x;
		      toct = allpnts[idx*2].thePoint.x;
		      allpnts[idx*2].x = allpnts[idx*2+1].x;
		      allpnts[idx*2].thePoint.x =
			allpnts[idx*2+1].thePoint.x;
		      allpnts[idx*2+1].x = twin;
		      allpnts[idx*2+1].thePoint.x = toct;
		      /* Found one */
		      *returnArg = theArg;
		      *returnIdx = idx*2;
		      dist = d;
		  }
		altPoint.x = allpnts[idx*2].thePoint.x;
		altPoint.y = allpnts[idx*2+1].thePoint.y;
		if ((allpnts[idx*2].theFct == pnt->theFct) &&
		    (wnEq(pnt->theWin, &altPoint, &(pnt->thePoint),
			  tol, &d) == VEM_OK) &&
		    (d < dist))
		  {
		      /* Fix up the box */
		      twin = allpnts[idx*2].x;
		      toct = allpnts[idx*2].thePoint.x;
		      allpnts[idx*2].y = allpnts[idx*2+1].x;
		      allpnts[idx*2].thePoint.x =
			allpnts[idx*2+1].thePoint.x;
		      allpnts[idx*2+1].x = twin;
		      allpnts[idx*2+1].thePoint.x = toct;
		      /* Found one */
		      *returnArg = theArg;
		      *returnIdx = idx*2+1;
		      dist = d;
		  }
	    }
	    break;
	case VEM_POINT_ARG:
	case VEM_LINE_ARG:
	    /* Look through the points */
	    for (idx = 0;  idx < theArg->length;  idx++) {
		/* Point equal? */
		if ((allpnts[idx].theFct == pnt->theFct) &&
		    (wnEq(pnt->theWin, &(allpnts[idx].thePoint),
			  &(pnt->thePoint), tol, &d) == VEM_OK) &&
		    (d <= dist))
		  {
		      /* We found one */
		      *returnArg = theArg;
		      *returnIdx = idx;
		      dist = d;
		  }
	    }
	    break;
	default:
	    break;
	}
    }
    lsFinish(theGen);
    if (*returnArg) {
	return VEM_OK;
    } else {
	return VEM_CANTFIND;
    }
}


static vemOneArg *newArg(type)
int type;			/* Type of new argument */
/*
 * This routine allocates a brand new argument and sets its
 * type to `type'.  It initializes all of the fields correctly
 * and informs the argsel module of its existance.
 */
{
    vemOneArg *retArg;

    retArg = VEMALLOC(vemOneArg);
    retArg->argType = type;
    retArg->length = 0;
    retArg->alloc_size = 0;
    retArg->argData.points = (vemPoint *) 0;
    argOn(retArg);
    return retArg;
}

static void delArg(arg)
vemOneArg *arg;
/*
 * Deletes an existing argument,  freeing all its resources.
 * Any screen dispaly of the argument is turned off.
 */
{
    argOff(arg);
    if (arg->argData.points) {
	VEMFREE(arg->argData.points);
    }
    VEMFREE(arg);
}


static int vemMenuEvents(evt)
XEvent *evt;			/* Incoming Event */
/*
 * This routine handles events while a moded menu is posted.  Button
 * events are handled specially:  press events are eaten and a release 
 * causes the menu package to return.
 */
{
    if (evt->xany.type == ButtonPress) {
	/* Eat press events */
	return 0;
    } else if (evt->xany.type == ButtonRelease) {
	/* Terminate the menu */
	return 1;
    } else {
	/* Send it to the handler */
	return vemHandleEvent(evt);
    }
    /*NOTREACHED*/
}



/* Waiting states */
#define U_DOMENU	1
#define U_LASTCMD	2

static int doMenu(buttonEvent, useAlternate)
XButtonEvent *buttonEvent;	/* Button event itself       */
int useAlternate;		/* Alternate menu flag       */
/*
 * Opens up the main menu for VEM and allows the user to choose
 * a command.  If the user makes a valid choice,  the command
 * is executed.  Otherwise,  nothing happens.  Returns 1 if
 * the main loop should stop.  If 'useAlternate' is non-zero,
 * it will use the alternate menu.
 */
{
    vemPoint curSpot;
    bdTable bindings;
    bdBinding command;
    xpa_menu theMenu;
    XEvent localEvent;
    int *prevDepth, **prevSelect;
    int depth, *select;
    int curSpotFlag, retCode, state;
    static int delay = -1;
    STR commandName;

    /* Event loop for release */
    if (delay < 1) {
	dfGetInt("menu.delay", &delay);
    }
    state = 0;
    do {
	vemNextEvent(&localEvent);
	switch (localEvent.type) {
	case ButtonRelease:
	    if (localEvent.xbutton.time - buttonEvent->time > delay) {
		/* We wants a menu */
		state = U_DOMENU;
	    } else {
		/* He wants last command */
		state = U_LASTCMD;
	    }
	    break;
	case ButtonPress:
	    /* He wants to cancel */
	    return 0;
	default:
	    /* Some other event - let vemHandler handle it */
	    vemHandleEvent(&localEvent);
	    break;
	}
    } while (!state);

    /* First,  make sure the state is appropriate */
    if ((cur_con.state == TEXT_STATE) || (cur_con.state == COMMAND_STATE)) {
	/* Take it out of text state */
	cur_con.state = 0;
	cur_con.typeInLength = 0;
	cur_con.typeInCommand[0] = NULL_CHAR;
	echoUpdate(cur_con.commandArgs);
    }

    if (wnPoint(buttonEvent->window, buttonEvent->x, buttonEvent->y,
		&(curSpot.theFct), &(curSpot.thePoint),
		VEM_NOSNAP) != VEM_OK)
      {
	  /* No current spot */
	  curSpotFlag = 0;
	  bdGetTable("standard", &bindings);
      } else {
	  /* Current spot */
	  curSpotFlag = 1;
	  curSpot.theWin = buttonEvent->window;
	  curSpot.x = buttonEvent->x;
	  curSpot.y = buttonEvent->y;
	  if (useAlternate) {
	      wnGetInfo(buttonEvent->window, (wnOpts *) 0, (octId *) 0,
			(octId *) 0, &bindings, (struct octBox *) 0);
	  } else {
	      bufBinding(curSpot.theFct, &bindings);
	  }
      }
    /* Post the menu */
    if (bdGetMenu(vemDisp, bindings, &theMenu, &prevDepth, &prevSelect) != VEM_OK) {
	XBell(vemDisp, 0);
	return 0;
    }
    retCode = 1;
    if (state == U_DOMENU) {
	depth = xpa_moded(theMenu, buttonEvent->x_root, buttonEvent->y_root,
			  *prevDepth, *prevSelect, XPA_PRESS,
			  vemMenuEvents, &select);
	if (depth > 0) {
	    /* Selection made XXX should free old ones */
	    *prevDepth = depth;
	    *prevSelect = select;
	    retCode = 1;
	} else {
	    /* No selection made */
	    retCode = 0;
	}
    }
    if (retCode) {
	/* Carry out command */
	if (bdMenu(bindings, *prevDepth, *prevSelect, &commandName,
		   &command) != VEM_OK)
	  {
	      if (state == U_LASTCMD) {
		  vemMessage("No last command defined (did you use the menu)?\n",
			     MSG_DISP|MSG_NOLOG);
	      } else {
		  XBell(vemDisp, 0);
		  vemMessage("Can't find the code for that command\n", 0);
	      }
	      vemPrompt();
	      retCode = 0;
	  } else {
	      if (curSpotFlag) {
		  return (*cur_con.invocation)(&curSpot, &command, TYPE_OUT);
	      } else {
		  return (*cur_con.invocation)((vemPoint *) 0,
					       &command, TYPE_OUT);
	      }
	  }
    }
    return retCode;
}

		

static int vemInvokeCommand(curSpot, binding, typeFlag)
vemPoint *curSpot;		/* Current spot        */
bdBinding *binding;		/* Binding for command */
int typeFlag;			/* TYPE_OUT, NO_OUTPUT, or nothing */
/*
 * Sets up error handling for vemDoInvokeCommand.
 */
{
    int ret_value;
    jmp_buf env;
    
    if (setjmp(env)) {
	/* Wipes out the argument list */
	vuWipeList(cur_con.commandArgs);
	echoNew();
	cur_con.state = 0;
	cur_con.typeInLength = 0;
	cur_con.typeInCommand[cur_con.typeInLength] = NULL_CHAR;
	vemMessage(cur_con.prompt, MSG_NOLOG|MSG_DISP);
	ret_value = 0;
    } else {
	vemPushErrContext(env);
	ret_value = vemDoInvokeCommand(curSpot, binding, typeFlag);
    }
    vemPopErrContext();
    return ret_value;
}


static int vemDoInvokeCommand(curSpot, binding, typeFlag)
vemPoint *curSpot;		/* Current spot        */
bdBinding *binding;		/* Binding for command */
int typeFlag;			/* TYPE_OUT, NO_OUTPUT, or nothing */
/*
 * This command attempts to call a VEM command (either tightly
 * bound or remote).  If the TYPE_OUT flag is set,  the command
 * outputs its name (with a colon in front).  If NO_OUTPUT is
 * set,  no console messages of any kind are produced.  The
 * argument list for the command is assumed to be in cur_con.commandArgs.
 */ 
{
    vemStatus funcReturn;
    wnOpts opts;
    extern vemStatus undoCmd();

#ifdef MF
    if (binding->id) m_command(binding->id);
#endif
    if (!(typeFlag & NO_OUTPUT)) {
	/* Output command name if key or menu binding */
	if (typeFlag & TYPE_OUT) {
	    /* Put in mode as if it were in type-in mode */
	    STRMOVE(cur_con.typeInCommand, binding->dispName);
	    cur_con.typeInLength = STRLEN(cur_con.typeInCommand);
	    cur_con.state = COMMAND_STATE;
	}
	STRCONCAT(cur_con.typeInCommand, "\n");
	cur_con.typeInLength += 1;
	echoUpdate(cur_con.commandArgs);
	XFlush(vemDisp);
	vemBusy();
    }
    switch (binding->bindType) {
    case TIGHT_COMMAND:
	/* Tightly bound command */

	/* XXX for UNDO - RLS */
	if (curSpot != 0) drsInsertUndoMarker(curSpot->theFct);

	funcReturn = (*(binding->bindValue.assocFunc))(curSpot, cur_con.commandArgs);
	if (binding->bindValue.assocFunc != undoCmd) {
	    drsResetLastCommand();
        } else {
	    drsLastCommandWasUndo();
	}

	if (typeFlag & NO_OUTPUT) {
	    if (funcReturn == VEM_ABORT) return 1;
	    else return 0;
	}
	switch (funcReturn) {
	case VEM_ABORT:
	    return 1;
	case VEM_ARGRESP:
	    cur_con.state = 0;
	    cur_con.typeInLength = 0;
	    cur_con.typeInCommand[cur_con.typeInLength] = NULL_CHAR;
	    vemPrompt();
	    return 0;
	case VEM_ARGMOD:
	    /* Wipe out type-in command */
	    cur_con.typeInLength = 0;
	    cur_con.typeInCommand[cur_con.typeInLength] = NULL_CHAR;
	    cur_con.state = 0;
	    echoUpdate(cur_con.commandArgs);
	    return 0;
	case VEM_ARGBREAK:
	    /* Breaks back into text mode */
	    cur_con.state = TEXT_STATE;
	    echoUpdate(cur_con.commandArgs);
	    return 0;
	default:
	    /* Wipes out the argument list */
	    vuWipeList(cur_con.commandArgs);
	    echoNew();
	    cur_con.state = 0;
	    cur_con.typeInLength = 0;
	    cur_con.typeInCommand[cur_con.typeInLength] = NULL_CHAR;
	    vemMessage(cur_con.prompt, MSG_NOLOG|MSG_DISP);
	    return 0;
	}
    case INVOKE_REMOTE:
	/* Start new remote application */
	wnGetInfo(curSpot->theWin, &opts, (octId *) 0,
		  (octId *) 0, (bdTable *) 0, (struct octBox *) 0);
	if (curSpot == (vemPoint *) 0) {
	    vemMessage("Cannot invoke application on console window\n",
		       MSG_NOLOG|MSG_DISP);
	} else if (opts.disp_options & VEM_REMOTERUN) {
	    vemMessage("Application running in this window\n",
		       MSG_NOLOG|MSG_DISP);
	} else {
	    char name[1204], *prune;

	    RPCExecApplication(binding->bindValue.theApp.hostName,
			       binding->bindValue.theApp.pathName,
			       curSpot, cur_con.commandArgs, (long) 0);
	    opts.disp_options |= VEM_REMOTERUN;
	    wnSetInfo(curSpot->theWin, &opts, (bdTable *) 0);
	    
	    if ((prune = strrchr(binding->bindValue.theApp.pathName, '/')) == NULL) {
		(void) sprintf(name, "%s@%s",
			       binding->bindValue.theApp.pathName,
			       binding->bindValue.theApp.hostName);
	    } else {
		(void) sprintf(name, "%s@%s", ++prune,
			       binding->bindValue.theApp.hostName);
	    }
	    wnSetRPCName(curSpot->theWin, name);

	}
	/* Wipes out the argument list */
	if (!(typeFlag & NO_OUTPUT)) {
	    vuWipeList(cur_con.commandArgs);
	    echoNew();
	    cur_con.state = 0;
	    cur_con.typeInLength = 0;
	    cur_con.typeInCommand[cur_con.typeInLength] = NULL_CHAR;
	    vemMessage(cur_con.prompt, MSG_NOLOG|MSG_DISP);
	}
	break;
    case REMOTE_COMMAND:
	/* Invoke remote command */
	RPCUserFunction(binding->bindValue.theFun.appNum,
			(long) binding->bindValue.theFun.appIndex,
			curSpot, cur_con.commandArgs, (long) 0);
	if (!(typeFlag & NO_OUTPUT)) {
	    /* Wipes out the argument list */
	    vuWipeList(cur_con.commandArgs);
	    echoNew();
	    cur_con.state = 0;
	    cur_con.typeInLength = 0;
	    cur_con.typeInCommand[cur_con.typeInLength] = NULL_CHAR;
	    vemMessage(cur_con.prompt, MSG_NOLOG|MSG_DISP);
	}
	break;
    }
    return 0;
}



/*
 * Routines for supporting stack of input contexts
 */

static int pushContext()
/*
 * This routine pushes the current context onto the context stack.
 * It does not change any of the values of the current context.  If
 * sucessful,  it returns the push-level of the stack (a non-zero
 * number).  If there isn't room on the stack,  it returns 0.
 */
{
    if (con_index < 4) {
	/* There is room */
	saved_contexts[con_index] = cur_con;
	return ++con_index;
    } else {
	return 0;
    }
}

static int popContext(do_it)
int do_it;
/*
 * This routine pops off the top_level context and overwrites the current
 * one.  It DOES NOT free the current context.  If successful,  it will
 * return the push-level before the pop (a non-zero number).  If there
 * aren't any more contexts to pop,  it will return 0.  The routine
 * won't actually do the pop unless `do_it' is one.
 */
{
    if (con_index > 0) {
	/* There are things to pop off */
	if (do_it) {
	    cur_con = saved_contexts[--con_index];
	    return con_index+1;
	} else {
	    return con_index;
	}
    } else {
	return 0;
    }
}



/*ARGSUSED*/
vemStatus redrawCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This command redraws the window containing 'spot'.  Normally,  this
 * command redraws the whole window.  However,  if the last argument is 
 * a box,  only that region of the window is redrawn.  This particularly
 * useful if the window is deactivated.  Note that without a box,  this
 * command makes deactivated windows active.
 */
{
    vemOneArg *lastArg;
    int lx, ly, hx, hy;

    if (spot == (vemPoint *) 0) {
	/* Redraw console - not supported at the moment */
    } else {
	if ((lsLastItem(cmdList, (lsGeneric *) &lastArg, LS_NH) == LS_OK) &&
	    (lastArg->argType == VEM_BOX_ARG))
	  {
	      /* Selective redraw */
	      if (lastArg->argData.points[lastArg->length*2-2].x <
		  lastArg->argData.points[lastArg->length*2-1].x) {
		  lx = lastArg->argData.points[lastArg->length*2-2].x;
		  hx = lastArg->argData.points[lastArg->length*2-1].x;
	      } else {
		  lx = lastArg->argData.points[lastArg->length*2-1].x;
		  hx = lastArg->argData.points[lastArg->length*2-2].x;
	      }
	      if (lastArg->argData.points[lastArg->length*2-2].y <
		  lastArg->argData.points[lastArg->length*2-1].y) {
		  ly = lastArg->argData.points[lastArg->length*2-2].y;
		  hy = lastArg->argData.points[lastArg->length*2-1].y;
	      } else {
		  ly = lastArg->argData.points[lastArg->length*2-1].y;
		  hy = lastArg->argData.points[lastArg->length*2-2].y;
	      }
	      wnForceRedraw(spot->theWin, lx, ly, hx-lx, hy-ly);
	      return delOneItemCmd(spot, cmdList);
	  } else {
	      /* Full redraw */
	      wnQWindow(spot->theWin);
	  }
    }
    return VEM_ARGMOD;
}



/*ARGSUSED*/
vemStatus pushCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This command pushes the current context onto the context stack
 * and gives the user a new context.  Note this DOES NOT remove
 * previous arguments from the screen. 
 */
{
    int level;
    STR new_prompt;

    level = pushContext();
    if (level) {
	cur_con.state = 0;

	new_prompt = VEMARRAYALLOC(char, 10);
	(void) sprintf(new_prompt, "vem %d> ", level);
	cur_con.prompt = new_prompt;

	cur_con.typeInLength = 0;

	echoNew();

	cur_con.commandArgs = lsCreate();

	/* invocation remains the same */
	return VEM_OK;
    } else {
	vemMessage("Push limit exceeded\n", MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }
}


/*ARGSUSED*/
vemStatus popCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This command pops off an input context from the context stack
 * and replaces the current context with that context.  Thus,
 * an old context pushed with pushCmd is restored.
 */
{
    if (popContext(0)) {
	/* It is possible to pop - destroy the current one */
	VEMFREE(cur_con.prompt);
	vuWipeList(cur_con.commandArgs);
	lsDestroy(cur_con.commandArgs, VEMFREENAME);
	/* Actually do the pop */
	popContext(1);
    } else {
	vemMessage("Nothing left to pop\n", MSG_NOLOG|MSG_DISP);
    }
    return VEM_ARGRESP;
}



/*ARGSUSED*/
static int show_bindings(spot, binding, typeFlag)
vemPoint *spot;			/* Where command was invoked */
bdBinding *binding;		/* Command binding           */
int typeFlag;
/*
 * This routine is a replacement for vemInvokeCommand that reports
 * the key, menu, and alias bindings for the specified command
 * then pops the context back to normal.  It is used by vemBindCmd.
 */
{
    STR keys, menus, aliases;

    /* Handle some type-out stuff */
    if (!(typeFlag & NO_OUTPUT)) {
	/* Output command name if key or menu binding */
	if (typeFlag & TYPE_OUT) {
	    STRMOVE(cur_con.typeInCommand, binding->dispName);
	    cur_con.typeInLength = STRLEN(cur_con.typeInCommand);
	    cur_con.state = COMMAND_STATE;
	}
	STRCONCAT(cur_con.typeInCommand, "\n");
	cur_con.typeInLength += 1;
	echoUpdate(cur_con.commandArgs);
	XFlush(vemDisp);
    }

    (void) sprintf(errMsgArea, "Description: %s\n", binding->help_str);
    vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
    bdQuery(binding, &keys, &menus, &aliases);
    if (STRLEN(keys) > 0) {
	(void) sprintf(errMsgArea, "Key bindings: %s\n", keys);
	vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
    } else {
	vemMessage("No key bindings\n", MSG_NOLOG|MSG_DISP);
    }
    if (STRLEN(menus) > 0) {
	(void) sprintf(errMsgArea, "Menu bindings: %s\n", menus);
	vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
    } else {
	vemMessage("No menu bindings\n", MSG_NOLOG|MSG_DISP);
    }
    if (STRLEN(aliases) > 0) {
	(void) sprintf(errMsgArea, "Type-in names: %s\n", aliases);
	vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
    } else {
	vemMessage("No type-in bindings\n", MSG_NOLOG|MSG_DISP);
    }

    /* Now pop the context */
    vuWipeList(cur_con.commandArgs);
    lsDestroy(cur_con.commandArgs, VEMFREENAME);
    popContext(1);

    /* Do an ARGRESP */
    cur_con.state = 0;
    cur_con.typeInLength = 0;
    cur_con.typeInCommand[cur_con.typeInLength] = NULL_CHAR;
    vemPrompt();
    return 0;
}

/*ARGSUSED*/
vemStatus vemBindCmd(spot, cmdList)
vemPoint *spot;			/* Where command was invoked */
lsList cmdList;			/* Argument list             */
/*
 * This command asks the user for a command and displays all
 * of its current key, menu, and alias bindings.  This is done
 * by pushing the current context and replacing it with a dummy
 * context which asks for one command and then pops.
 */
{
    int level;

    vemMessage("Specify a command using a single key, a menu, or by type-in:\n",
	       MSG_NOLOG|MSG_DISP);
    level = pushContext();
    if (level) {
	/* Set up context */
	cur_con.state = 0;

	cur_con.prompt = "vem bindings> ";
	cur_con.typeInLength = 0;

	echoNew();

	cur_con.commandArgs = lsCreate();
	cur_con.invocation = show_bindings;
	return VEM_OK;
    } else {
	vemMessage("Push limit exceeded\n", MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }
}


#define CONTROL_F	'\006'

static void vemConsWrite(data, str)
Pointer data;			/* Paassed data    */
char *str;			/* String to write */
/*
 * This function is called when data is to be written to
 * the console window.  The old console window allowed
 * certain control sequences for changing color and
 * font.  These sequences are filtered out for the
 * new console window (which doesn't support these
 * features).
 */
{
    Widget cw = (Widget) data;
    int ignore = 0;

    for (/* null */;  *str;  str++) {
	if (ignore) {
	    ignore = 0;
	} else {
	    if (*str == CONTROL_F) ignore = 1;
	    else tsWriteChar(cw, *str);
	}
    }
}




static vemStatus echoNew()
/*
 * This routine is used to signal the entry of a new command list.
 * This routine and its companion echoUpdate present a textual
 * representation of the argument list in the console window.
 * After calling echoNew,  each time the argument list is modified
 * echoUpdate should be called.  It will determine what needs to
 * be redrawn and draw it in the console window.
 */
{
    cur_con.echoList1[0] = NULL_CHAR;
    cur_con.echoList2[0] = NULL_CHAR;
    cur_con.newList = cur_con.echoList1;
    cur_con.oldList = cur_con.echoList2;
    return VEM_OK;
}

static vemStatus echoUpdate(curList)
lsList curList;		/* Newly modifed argument list */
/*
 * This routine compares the previous argument list to the new
 * list and redraws the portion of the list which has changed.
 * This is done by backspacing the proper amount and sending
 * the new command list string representation.  This string
 * representation is stored for use the next time echoUpdate
 * is called.
 */
{
    char *tempPtr, tempStr[20];
    lsGen theGen;
    vemOneArg *anArg;
    int changeSpot, index, newLen, oldLen, strFlag, backSpaces;

    /* Build the new one in newList */
    cur_con.newList[0] = NULL_CHAR;
    theGen = lsStart(curList);
    strFlag = 0;
    while (lsNext(theGen, (lsGeneric *) &anArg, LS_NH) == LS_OK) {
	if (strFlag) {
	    STRCONCAT(cur_con.newList, "\" ");
	    strFlag = 0;
	}
	switch (anArg->argType) {
	case VEM_POINT_ARG:
	    (void) sprintf(tempStr, "points(%d) ", anArg->length);
	    STRCONCAT(cur_con.newList, tempStr);
	    break;
	case VEM_BOX_ARG:
	    (void) sprintf(tempStr, "boxes(%d) ", anArg->length);
	    STRCONCAT(cur_con.newList, tempStr);
	    break;
	case VEM_LINE_ARG:
	    (void) sprintf(tempStr, "lines(%d) ", anArg->length-1);
	    STRCONCAT(cur_con.newList, tempStr);
	    break;
	case VEM_TEXT_ARG:
	    STRCONCAT(cur_con.newList, "\"");
	    STRNCONCAT(cur_con.newList, anArg->argData.vemTextArg, anArg->length);
	    strFlag = 1;
	    break;
	case VEM_OBJ_ARG:
	    (void) sprintf(tempStr, "objects(%d) ", anArg->length);
	    STRCONCAT(cur_con.newList, tempStr);
	    break;
	}
    }
    lsFinish(theGen);
    if (strFlag && (cur_con.state != TEXT_STATE)) {
	STRCONCAT(cur_con.newList, "\" ");
	strFlag = 0;
    }
    if (cur_con.state == COMMAND_STATE) {
	/* In command input mode */
	STRCONCAT(cur_con.newList, ": ");
	STRNCONCAT(cur_con.newList, cur_con.typeInCommand, cur_con.typeInLength);
    }
    /*
     * Finished building the new representation.  Compare
     * to the old one and write out the changes.
     */
    newLen = STRLEN(cur_con.newList);
    oldLen = STRLEN(cur_con.oldList);
    for (changeSpot = 0;  changeSpot < VEMMIN(newLen, oldLen);  changeSpot++) {
	if (cur_con.newList[changeSpot] != cur_con.oldList[changeSpot]) break;
    }
    /* First, compute the number of backspaces required: */
    backSpaces = VEMMAX(oldLen - changeSpot, 0);
    if (backSpaces > 0)
      for (index = 0;  index < backSpaces;  index++)
	vemMessage("\010", MSG_NOLOG|MSG_DISP);
    /* Write out remaining portion of string */
    if (changeSpot < newLen) {
	tempPtr = &(cur_con.newList[changeSpot]);
vemMessage("\0061", MSG_NOLOG|MSG_DISP);
	vemMessage(tempPtr, MSG_NOLOG|MSG_DISP);
vemMessage("\0060", MSG_NOLOG|MSG_DISP);
    }
    /* Swap the old and new pointers */
    tempPtr = cur_con.newList;
    cur_con.newList = cur_con.oldList;
    cur_con.oldList = tempPtr;
    return VEM_OK;
}


static vemStatus echoRedraw(cmdList)
lsList cmdList;		/* Current argument list */
/*
 * This routine causes the entire command argument list to
 * be redrawn in the console window.  It simply makes the
 * old string null and calls echoUpdate.
 */
{
    cur_con.oldList[0] = NULL_CHAR;
    echoUpdate(cmdList);
    return VEM_OK;
}


vemStatus vemPrompt()
/*
 * This routine automatically redisplays the current
 * vem prompt.
 */
{
    vemMessage(cur_con.prompt, MSG_NOLOG|MSG_DISP);
    echoRedraw(cur_con.commandArgs);
    return VEM_OK;
}


static char *_vem_dsp_name = (char *) 0;
static int _vem_chrom = TAP_BW;

static void vemIDT(disp, c_flag)
Display *disp;			/* X Connection */
int c_flag;			/* Color flag   */
/*
 * This routine initializes the current display name and chromatism.
 */
{
    _vem_dsp_name = ServerVendor(disp);
    if (c_flag) {
	_vem_chrom = TAP_COLOR;
    } else {
	_vem_chrom = TAP_BW;
    }
    tapSetDisplayType(_vem_dsp_name, _vem_chrom);
}



#ifdef NO_RPC

/*
 * Dummy routines for turning off RPC
 */

/*ARGSUSED*/
vemStatus killAppCmd(spot, cmdList)
vemPoint *spot;
lsList cmdList;
{
    return VEM_NOIMPL;
}

/*ARGSUSED*/
vemStatus rpcAnyApplication(spot, cmdList)
vemPoint *spot;
lsList cmdList;
{
    return VEM_NOIMPL;
}

/*ARGSUSED*/
vemStatus rpcReset(spot, cmdList)
vemPoint *spot;
lsList cmdList;
/*
 * tell a window that an rpc application is not running in it
 * (for Permission Denied and File Not Found errors before initialization)
 */
{
    return VEM_NOIMPL;
}

char *
rpcVerString()
/*
 * return the rpc version
 */
{
    return (char *) 0;
}


/*ARGSUSED*/
vemStatus RPCExecApplication(host, path, spot, cmdList, flag)
STR host, path;
vemPoint *spot;
lsList cmdList;
int flag;
{
    return VEM_NOIMPL;
}

/*ARGSUSED*/
vemStatus RPCUserFunction(appNum, appIndex, spot, cmdList, flag)
int appNum, appIndex;
vemPoint *spot;
lsList cmdList;
int flag;
{
    return VEM_NOIMPL;
}

#endif
