#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/*
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
/*
 * Short testing program for the typescript widget
 */

#include <stdio.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Command.h>
#include "ts.h"

#define MAX_ARGS	10

char *XtClassName = "TStest";
static void XtStart();

void dump_core(msg)
String msg;
{
    (void) fprintf(stderr, "Fatal toolkit error: %s\n", msg);
    abort();
}

int
main(argc, argv)
int argc;
char *argv[];
{
    Widget top_level_widget;
    XtAppContext context;
    Display *disp;
    int arg_len;
    Arg arg_list[MAX_ARGS];

    XtSetErrorHandler(dump_core);
    XtToolkitInitialize();
    context = XtCreateApplicationContext();
    disp = XtOpenDisplay(context, "", argv[0], XtClassName,
			 (XrmOptionDescRec *) 0, 0,
			 &argc, argv);
    arg_len = 0;
    XtSetArg(arg_list[arg_len], XtNallowShellResize, True);	arg_len++;
    XtSetArg(arg_list[arg_len], XtNinput, True);		arg_len++;
    top_level_widget = XtAppCreateShell(argv[0], XtClassName,
					applicationShellWidgetClass,
					disp, arg_list, arg_len);
    XtStart(argc, argv, context, top_level_widget);
    XtAppMainLoop(context);
    return 0;
}

static void std_char(w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
    ts_call_data *data = (ts_call_data *) call;
    int i;

    for (i = 0;  i < data->len;  i++) {
	tsWriteChar(w, (int) (data->buf[i]));
    }
}

static void done(w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
    XtDestroyWidget((Widget) client);
}

static void XtStart(argc, argv, app, tw)
int argc;
char *argv[];
XtAppContext app;
Widget tw;
{
    Widget pane, cmd, ts;

    pane = XtCreateManagedWidget("pane", panedWidgetClass, tw, 0, 0);
    cmd = XtCreateManagedWidget("cmd", commandWidgetClass, pane, 0, 0);
    XtAddCallback(cmd, XtNcallback, done, (XtPointer) tw);
    ts = tsCreate(app, "console", pane, 100, std_char, (caddr_t) 0,
		  (ArgList) 0, 0);
    XtRealizeWidget(tw);
}
