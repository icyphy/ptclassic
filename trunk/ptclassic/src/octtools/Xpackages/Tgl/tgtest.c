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
 * Toggle tester (very short and simple)
 */

#include "port.h"
#include <stdio.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xaw/Label.h>
#include "Table.h"
#include "Tgl.h"

#define MAX_ARGS	10

void dump_core(msg)
String msg;
{
    (void) fprintf(stderr, "Fatal toolkit error: %s\n", msg);
    abort();
}

void press_func(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
/*
 * Call back function
 */
{
    Arg args[MAX_ARGS];
    int arg_len;
    Boolean state;
    String which_one = (String) client_data;

    arg_len = 0;
    XtSetArg(args[arg_len], XtNstate, (XtArgVal) &state);  arg_len++;
    XtGetValues(w, args, arg_len);
    if (state) {
	printf("%s: State is on\n", which_one);
    } else {
	printf("%s: State is off\n", which_one);
    }
}

static XtCallbackRec redback[] = { { press_func, (caddr_t) "Red" }, { 0, 0 } };
static XtCallbackRec greenback[] = { { press_func, (caddr_t) "Green" }, { 0, 0 } };
static XtCallbackRec blueback[] = { { press_func, (caddr_t) "Blue" }, { 0, 0 } };

void MakeDialog(table)
Widget table;
/*
 * Sets up small dialog for testing toggle widget
 */
{
    Arg arg_list[MAX_ARGS];
    int arg_len;
    Widget leader;
    Widget toggle_widget, label_widget;
    static char *strs[] = { "Red", "Green", "Blue" };
    static XtCallbackRec *callbacks[] = { redback, greenback, blueback };
    int i;

    /* Unmanaged leader */
    arg_len = 0;
    leader = XtCreateWidget("leader", tglWidgetClass,
			    table, arg_list, arg_len);

    for (i = 0;  i < sizeof(strs)/sizeof(char *);  i++) {
	arg_len = 0;
	XtSetArg(arg_list[arg_len], XtNcallback, (XtArgVal) callbacks[i]);
	arg_len++;
	XtSetArg(arg_list[arg_len], XtNborderWidth, 0);  arg_len++;
/*	XtSetArg(arg_list[arg_len], XtNleader, leader);  arg_len++; */
	XtSetArg(arg_list[arg_len], XtNstate, 1);	 arg_len++;
	XtSetArg(arg_list[arg_len], XtNjustify, XtJustifyRight);  arg_len++;
	toggle_widget = XtCreateManagedWidget("toggle", tglWidgetClass,
					      table, arg_list, arg_len);
	XtTblPosition(toggle_widget, 0, i);
/*	XtTblOptions(toggle_widget, TBL_SM_WIDTH); */
	arg_len = 0;
	XtSetArg(arg_list[arg_len], XtNlabel, strs[i]);  arg_len++;
	XtSetArg(arg_list[arg_len], XtNborderWidth, 0);  arg_len++;
	XtSetArg(arg_list[arg_len], XtNjustify, XtJustifyLeft); arg_len++;
	label_widget = XtCreateManagedWidget(strs[i], labelWidgetClass,
					     table, arg_list, arg_len);
	XtTblPosition(label_widget, 1, i);
    }
}

int main(argc, argv)
int argc;
char *argv[];
{
    Widget top_level_widget;
    Widget table_widget;
    /*Widget toggle_widget;*/
    XtAppContext context;
    Arg arg_list[MAX_ARGS];
    int arg_len;
    Display *disp;

    XtSetErrorHandler(dump_core);
    XtToolkitInitialize();
    context = XtCreateApplicationContext();
    disp = XtOpenDisplay(context, "", argv[0], "Form", (XrmOptionDescRec *) 0,
			 0, &argc, argv);
    arg_len = 0;
    XtSetArg(arg_list[arg_len], XtNallowShellResize, True);  arg_len++;
    top_level_widget = XtAppCreateShell(argv[0], "ToggleTester",
					applicationShellWidgetClass,
					disp, arg_list, arg_len);
    arg_len = 0;
    table_widget = XtCreateManagedWidget("table", tableWidgetClass,
					 top_level_widget, arg_list, arg_len);
    MakeDialog(table_widget);
    XtRealizeWidget(top_level_widget);
    XtAppMainLoop(context);
    return 0;
}
