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
 * Table testing program
 */
#define ASCII_STRING

#include <stdio.h>
#include <port.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#include <X11/Xaw/Command.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/AsciiText.h>

#include "Table.h"

#define MAX_ARGS	10
void dump_core(msg)
String msg;
{
    (void) fprintf(stderr, "Fatal toolkit error: %s\n", msg);
    abort();
}

/*ARGSUSED*/
static void ThreePush(w, client, call)
Widget w;
caddr_t client;
caddr_t call;
{
    Widget parent = XtParent(w);
    Arg arg_list[MAX_ARGS];
    int arg_len = 0;
    Dimension old_spacing = 0;

    XtSetArg(arg_list[arg_len], XtNrowSpacing, &old_spacing); arg_len++;
    XtGetValues(parent, arg_list, arg_len);

    /* Increases row spacing */
    arg_len = 0;
    XtSetArg(arg_list[arg_len], XtNrowSpacing, old_spacing+5); arg_len++;
    XtSetValues(parent, arg_list, arg_len);
}

static void MakeButtons(parent)
Widget parent;
/*
 * Makes a whole bunch of buttons.
 */
{
    Widget button, title, input;
    static char *strs[] = {
	"One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight",
	"Nine", "Ten", "Eleven", "Twelve", "Thirteen", "Fourteen",
	"Fifteen", "Sixteen", "Seventeen", "Eightteen", "Nineteen",
	"Twenty", "Twenty-one", "Twenty-two", "Twenty-three",
	"Twenty-four", "Twenty-five"
    };
    Arg arg_list[MAX_ARGS];
    int arg_len;
    int i, num, snum;
    double sqrt();
    char *input_string;
    static XtCallbackRec callbacks[] = { { ThreePush, NULL }, { NULL, NULL } };

    num = sizeof(strs)/sizeof(char *);
    snum = (int) (sqrt((double) num) + 0.5);
    /* First a title button */
    arg_len = 0;
    XtSetArg(arg_list[arg_len], XtNlabel, "Title");  arg_len++;
    title = XtCreateManagedWidget("title", labelWidgetClass,
				   parent, arg_list, arg_len);
#ifdef HARD_SPEC
    XtTblConfig(title, 0, 0, snum, 1, TBL_SM_HEIGHT);
#endif
    for (i = 0;  i < num;  i++) {
	arg_len = 0;
	XtSetArg(arg_list[arg_len], XtNlabel, strs[i]);	arg_len++;
	if (i == 2) {
	    XtSetArg(arg_list[arg_len], XtNcallback, (XtArgVal) callbacks);
	    arg_len++;
	}
	button = XtCreateManagedWidget(strs[i], commandWidgetClass,
				       parent, arg_list, arg_len);
#ifdef HARD_SPEC
	XtTblPosition(button, i%snum, i/snum + 1);
#endif
    }
    /* Add a text widget on the bottom */
    arg_len = 0;
    input_string = malloc(512);
    XtSetArg(arg_list[arg_len], XtNstring, input_string);	arg_len++;
    XtSetArg(arg_list[arg_len], XtNlength, 512);		arg_len++;
#ifdef NEVER
    XtSetArg(arg_list[arg_len], XtNtextOptions,
	     editable|resizeWidth);				arg_len++;
    XtSetArg(arg_list[arg_len], XtNeditType, XttextEdit);	arg_len++;
    input = XtCreateManagedWidget("input", asciiStringWidgetClass,
				  parent, arg_list, arg_len);
#else
    input = XtCreateManagedWidget("input", asciiTextWidgetClass,
				  parent, arg_list, arg_len);
#endif

#ifdef HARD_SPEC
    XtTblConfig(input, 0, num/snum+1, snum, 1, 0);
#endif
}

void press_func(w, data, event)
Widget w;
caddr_t data;
XEvent *event;
{
    printf("finished\n");
    exit(0);
}

int
main(argc, argv)
int argc;
char *argv[];
{
    Widget top_level_widget;
    Widget table_widget;
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
    top_level_widget = XtAppCreateShell(argv[0], "Form",
					applicationShellWidgetClass,
					disp, arg_list, arg_len);
    arg_len = 0;
    table_widget = XtCreateManagedWidget("table", tableWidgetClass,
					 top_level_widget, arg_list, arg_len);
    XtAddEventHandler(table_widget, ButtonPressMask|ButtonReleaseMask,
		      False, press_func, (caddr_t) 0);
    MakeButtons(table_widget);
    XtRealizeWidget(top_level_widget);
    XtAppMainLoop(context);
    return 0;
}

