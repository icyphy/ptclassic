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
 * Table Widget Example - Direct specification
 *
 * This program creates one button for each string on the command line.
 */

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#include <X11/Xaw/Label.h>
#include <X11/Xaw/Command.h>

#include "Table.h"

#define MAX_ARGS	10
#define APP_NAME	"TableExample"
#define NUM_ROWS	3

main(argc, argv)
int argc;
char *argv[];
{
    Widget initialize(), top, table, title, button;
    XtAppContext context;
    Arg arg_list[MAX_ARGS];
    int arg_len, i, cols;

    top = initialize(&argc, argv, &context);
    arg_len = 0;
    table = XtCreateManagedWidget("table", tableWidgetClass,
				  top, arg_list, arg_len);
    XtSetArg(arg_list[arg_len], XtNlabel, "Title Bar");  arg_len++;
    title = XtCreateManagedWidget("title", labelWidgetClass,
				  table, arg_list, arg_len);
    /* Each column will have three rows */
    cols = (argc-1)/NUM_ROWS + 1;
    XtTblConfig(title, 0, 0, cols, 1, TBL_SM_HEIGHT);
    for (i = 1;  i < argc;  i++) {
	arg_len = 0;
	XtSetArg(arg_list[arg_len], XtNlabel, argv[i]);  arg_len++;
	button = XtCreateManagedWidget("button", commandWidgetClass,
				       table, arg_list, arg_len);
	if (i < argc-1) {
	    XtTblPosition(button, (i-1)/NUM_ROWS, (i-1)%NUM_ROWS + 1);
	} else {
	    /* Last one spans to bottom */
	    XtTblConfig(button, (i-1)/NUM_ROWS, (i-1)%NUM_ROWS + 1,
			1, 3 - ((i-1)%NUM_ROWS), TBL_DEF_OPT);
	}
    }
    XtRealizeWidget(top);
    XtAppMainLoop(context);
}

Widget initialize(argc_p, argv, context)
int *argc_p;
char *argv[];
XtAppContext *context;
{
    Widget top;
    Display *disp;
    Arg arg_list[MAX_ARGS];
    int arg_len;

    XtToolkitInitialize();
    *context = XtCreateApplicationContext();
    disp = XtOpenDisplay(*context, "", argv[0], APP_NAME,
			 (XrmOptionDescRec *) 0, 0, argc_p, argv);
    arg_len = 0;
    XtSetArg(arg_list[arg_len], XtNallowShellResize, True); arg_len++;
    top = XtAppCreateShell(argv[0], APP_NAME, applicationShellWidgetClass,
			   disp, arg_list, arg_len);
    return top;
}
