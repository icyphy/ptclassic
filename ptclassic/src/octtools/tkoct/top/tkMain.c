/* 
	tclTkMain.c :: TOP Library

	Taken from wish's main.c

    Author: Kennard White

Copyright (c) 1990-%Q% The Regents of the University of California.
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
#if !defined(lint) && !defined(SABER)
static char SccsId[] = "$Id$";
#endif

#include "topFixup.h"
#include <tk.h>

#include "topStd.h"
#include "topMem.h"
#include "topMisc.h"
#include "topTcl.h"

extern int isatty();

typedef struct _TOPTkMainInfo TOPTkMainInfo;
struct _TOPTkMainInfo {
    Tcl_Interp*		ip;
    Tk_Window		win;
    Tcl_DString		cmdbuf;
    int			ttyB;
    char*		prompt;
};

static void
_topStdinProc(ClientData clientData, int mask) {
    TOPTkMainInfo	*pInfo = (TOPTkMainInfo*) clientData;
#define BUFFER_SIZE 4000
    char		input[BUFFER_SIZE+1];
    static int		gotPartial = 0;
    char		*cmd;
    int			code, count;

    count = read(fileno(stdin), input, BUFFER_SIZE);
    if (count <= 0) {
	if (!gotPartial) {
	    if (pInfo->ttyB) {
		Tcl_Eval(pInfo->ip,"exit");
		exit(1);
	    } else {
		Tk_DeleteFileHandler(0);
	    }
	    return;
	} else {
	    count = 0;
	}
    }
    cmd = Tcl_DStringAppend(&pInfo->cmdbuf, input, count);
    if (count!=0 ) {
	if ( input[count-1]!='\n' && input[count-1]!=';' ) {
	    gotPartial = 1;
	    goto prompt;
	}
	if (!Tcl_CommandComplete(cmd)) {
	    gotPartial = 1;
	    goto prompt;
	}
    }
    gotPartial = 0;

    Tk_CreateFileHandler(0, TK_READABLE|TK_EXCEPTION, _topStdinProc,
	(ClientData) pInfo);
    code = Tcl_RecordAndEval( pInfo->ip, cmd, 0);
    Tk_CreateFileHandler(0, TK_READABLE|TK_EXCEPTION, _topStdinProc,
	(ClientData) pInfo);
    Tcl_DStringFree(&pInfo->cmdbuf);
#ifdef oustersway
    if (pInfo->ip->result != NULL) {
        if ((code != TCL_OK) || (pInfo->ttyB)) {
	    printf("%s\n", pInfo->ip->result);
	}
    }
#else
    printf( "%s\n", topTclGetRetMsg(pInfo->ip,code));
#endif
    prompt:;
    if (pInfo->ttyB) {
	topTclPrompt( pInfo->ip, gotPartial, pInfo->prompt);
    }
}

/* ARGSUSED */
static void
_topStructureProc(clientData, eventPtr)
    ClientData clientData;	/* Information about window. */
    XEvent *eventPtr;		/* Information about event. */
{
    TOPTkMainInfo	*pInfo = (TOPTkMainInfo*) clientData;

    if (eventPtr->type == DestroyNotify) {
	pInfo->win = NULL;
    }
}

/*
 * Information for parsing command-line options:
 */
static int synchronize = 0;
static char *file = NULL;
static char *name = NULL;
static char *display = NULL;
static char *geometry = NULL;
static int _TopTclshB = 0;
static char *_TopWishFile = NULL;

static Tk_ArgvInfo argTable[] = {
    {"-file", TK_ARGV_STRING, (char *) NULL, (char *) &file,
	"File from which to read commands"},
    {"-geometry", TK_ARGV_STRING, (char *) NULL, (char *) &geometry,
	"Initial geometry for window"},
    {"-display", TK_ARGV_STRING, (char *) NULL, (char *) &display,
	"Display to use"},
    {"-name", TK_ARGV_STRING, (char *) NULL, (char *) &name,
	"Name to use for application"},
    {"-sync", TK_ARGV_CONSTANT, (char *) 1, (char *) &synchronize,
	"Use synchronous mode for display server"},
    {"-serverfix", TK_ARGV_CONSTANT, (char *) 1, (char *) &TOPXServerFixB,
	"Work around x server XDrawLines bug"},
    {"-tclsh", TK_ARGV_CONSTANT, (char *) 1, (char *) &_TopTclshB,
	"Emulate tclsh."},
    {"-wish", TK_ARGV_STRING, (char *) NULL, (char *) &_TopWishFile,
	"Emulate wish."},
    {(char *) NULL, TK_ARGV_END, (char *) NULL, (char *) NULL,
	(char *) NULL}
};

static TOPTkMainInfo	info;

int
topTkInitialize1( Tcl_Interp *ip, int *argc, char **argv, Tk_Window *pWin) {
    Tk_3DBorder			border;
    char			*showname;
    Tk_Window			win = NULL;
    int				r;

    info.ip = ip;

    /*
     * Deal with arguments, parsing them and simple using
     */
    if ( *argc >= 2 && argv != NULL ) {
	if ( Tk_ParseArgv(ip, (Tk_Window) NULL, argc, argv, argTable, 0)
	  != TCL_OK ) {
	    Tcl_AppendResult(ip,"\nError parsing argv", NULL);
	    return TCL_ERROR;
	}
    }
    if ( name == NULL )
	name = topBasename( argv[0]);
    topSetAppName( name);
    showname = topBasename( file ? file : name);
    info.prompt = showname;
    if ( display != NULL )
	Tcl_SetVar2(ip, "env", "DISPLAY", display, TCL_GLOBAL_ONLY);

    if ( _TopTclshB ) {
	if ( file == NULL ) {
	    if ( topTclMainLoop( ip, NULL, name, 0) != TCL_OK )
		exit(1);
	} else {
	    if ( (r = Tcl_EvalFile( ip, file)) != TCL_OK ) {
		fprintf( stderr, "%s: %s\n", name, topTclGetRetMsg( ip, r));
		exit(1);
	    }
	}
	exit(0);
    }

    if ( geometry != NULL ) {
	Tcl_SetVar( ip, "geometry", geometry, TCL_GLOBAL_ONLY);
    }

    /* 
     * Create a window
     */
    if ( win == NULL ) {
    	if ( (win = Tk_CreateMainWindow(ip, display, showname, "Tk")) == NULL ){
	    Tcl_AppendResult(ip,"\nError creating main window", NULL);
	    return TCL_ERROR;
	}
        /* Tk_SetClass(win, "Tk"); */
    }
    info.win = win;
    Tk_CreateEventHandler(win, StructureNotifyMask, _topStructureProc,
	    (ClientData) &info);
    Tk_GeometryRequest(win, 200, 200);
    border = Tk_Get3DBorder(ip, win, None, "#4eee94");
    if (border == NULL) {
	Tcl_SetResult(ip, (char *) NULL, TCL_STATIC);
	Tk_SetWindowBackground(win, WhitePixelOfScreen(Tk_Screen(win)));
    } else {
	Tk_SetBackgroundFromBorder(win, border);
    }
    XSetForeground(Tk_Display(win), DefaultGCOfScreen(Tk_Screen(win)),
	    BlackPixelOfScreen(Tk_Screen(win)));


    if (synchronize) {
	XSynchronize(Tk_Display(win), True);
    }

    if ( pWin )
	*pWin = info.win;
    return TCL_OK;
}


int
topTkInitialize2( Tcl_Interp *ip, Tk_Window win, int argc, char **argv) {
    char			*args;
    char			buf[20];
    int				result;

    args = Tcl_Merge(argc-1, argv+1);
    Tcl_SetVar(ip, "argv", args, TCL_GLOBAL_ONLY);
    ckfree(args);
    sprintf(buf, "%d", argc-1);
    Tcl_SetVar(ip, "argc", buf, TCL_GLOBAL_ONLY);
    Tcl_SetVar(ip, "argv0", (file!=NULL) ? file : argv[0], TCL_GLOBAL_ONLY);

    if ( (result=topTclSourceInitFile( ip, name, NULL) != TCL_OK) ) {
	Tcl_AppendResult(ip, "\nError in init file", NULL);
	return TCL_ERROR;
    }
    if (file != NULL) {
	if ( Tcl_VarEval(ip, "source ", file, NULL) != TCL_OK ) {
	    Tcl_AppendResult( ip, "\nError in command file", NULL);
	    return TCL_ERROR;
	}
	info.ttyB = 0;
    } else {
	info.ttyB = isatty(0);
	Tk_CreateFileHandler(0, TK_READABLE|TK_EXCEPTION, _topStdinProc,
		(ClientData) &info);
	if (info.ttyB) {
	    /* printf("window is %s\n", Tk_Name(info.win)); */
	    topTclPrompt(info.ip, 0, info.prompt);
	}
    }
    fflush(stdout);
    Tcl_DStringInit(&info.cmdbuf);
    (void) Tcl_Eval(ip, "update");

#ifdef notdef
    /*
     * The window could have been deleted as part of the startup
     * script.  If so, don't try to map it.
     */
    if (info.win != NULL) {
	Tk_MapWindow(info.win);
    }
#endif
    return TCL_OK;
}
