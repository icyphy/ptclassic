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
 * Table Widget Example - Using resources
 *
 * Place the following entries in your resources file:
 *   TableExample.table.Layout: title 0 0 2 1 H; bt1 0 1; bt2 1 1
 *   TableExample*title.label: Title Bar
 *   TableExample*bt1.label: Button One
 *   TableExample*bt2.label: Button Two
 */

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#include <X11/Xaw/Label.h>
#include <X11/Xaw/Command.h>

#include "Table.h"

#define MAX_ARGS	10
#define APP_NAME	"TableExample"

main(argc, argv)
int argc;
char *argv[];
{
    Widget initialize(), top, table, title, bt1, bt2;
    XtAppContext context;
    Arg arg_list[MAX_ARGS];
    int arg_len;

    top = initialize(&argc, argv, &context);
    arg_len = 0;
    table = XtCreateManagedWidget("table", tableWidgetClass,
				  top, arg_list, arg_len);
    title = XtCreateManagedWidget("title", labelWidgetClass,
				  table, arg_list, arg_len);
    bt1 = XtCreateManagedWidget("bt1", commandWidgetClass,
				table, arg_list, arg_len);
    bt2 = XtCreateManagedWidget("bt2", commandWidgetClass,
				table, arg_list, arg_len);
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
