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
 * Short tester for compatibility package
 */

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include "vemDM.h"

#define WW_TEST

#define isize(iw)	(sizeof(iw)/sizeof(dmWhichItem))
#define tsize(tw)	(sizeof(tw)/sizeof(dmTextItem))

void set_topw();

dmWhichItem mulw_one[] = {
    { "First", "one", 0 },
    { "Second", "two", 1 },
    { "Third", "three", 0 }
};

dmWhichItem mulw_two[] = {
    { "Only one", "blah", 1 }
};

dmTextItem txt_test[] = {
    { "Name", 1, 30, "<Name>", (char *) 0 },
    { "Value", 3, 40, "<Value here>", (char *) 0 },
    { "Extra", 2, 20, (char *) 0, (char *) 0 }
};

dmWhichItem mulw_three[] = {
    { "Many", "four", 1 },
    { "Strings", "score", 0 },
    { "Can", "and", 0 },
    { "BE", "seven", 0 },
    { "Entered", "years", 0 },
    { "In This", "ago", 0 },
    { "Kind of", "our", 0 },
    { "Dialog", "fathers", 0 }
};



char mulw_help[] =
"You may choose any number of the items shown in the body of the dialog.\
Once you are satisfied with the selections,  press the Ok button.\
If you want to cancel the selection,  press the Cancel button.";

char ww_help[] =
"You may choose one of the items shown in the body of the dialog.\n\
Once you are satisfied with your selection,  press the Ok button.\n\
If you want to cancel the selection,  press the Cancel button.";

char explan[] =
"This is only a test.  If this had been\n\
an actual error,  you would have been given\n\
a brief explanation of the error and some\n\
idea of how to proceed.  Instead,  you may\n\
now continue with your work.";

char long_message[] =
"This is a long message to test the scrolling\n\
capability of the new message function.  This\n\
function allows users to display unmoded messages\n\
in a dialog box on the screen.  This box does not\n\
go away until the \"Dismiss\" button is pressed near\n\
the bottom of the dialog.\n\
\n\
New paragraphs can be entered by putting in a newline\n\
like the one above.  Formatting for these dialogs\n\
is under user control unless a flag is provided that\n\
specifies automatic formatting.  Automatic formatting\n\
breaks the text into lines automatically.  This frees\n\
the caller from formatting the message manually.";

print_sels(num, items)
int num;
dmWhichItem *items;
{
    int i;

    for (i = 0;  i < num;  i++) {
	if (items[i].flag) {
	    printf("`%s' selected.\n", items[i].itemName);
	}
    }
    printf("finished printing selected items.\n");
}

print_vals(num, items)
int num;
dmTextItem *items;
{
    int i;

    for (i = 0;  i < num;  i++) {
	printf("`%s'", items[i].itemPrompt);
	if (items[i].value) {
	    printf(" = `%s'\n", items[i].value);
	} else {
	    printf(" no value\n");
	}
    }
    printf("finished printing values\n");
}


int mulw_func(idx, value, data)
int idx;
int value;
Pointer data;
{
    printf("Button %d was pressed (state is %d, data is `%s')\n",
	   idx, value, data);
    return 0;
}

int ww_func(idx, data)
int idx;
Pointer data;
{
    printf("Button %d was activated (data is `%s')\n", idx, data);
    return 0;
}

#define MAX_ARGS 10

int other_events(evt)
XEvent *evt;
{
    printf("Other event.\n");
    return 0;
}

main(argc, argv)
int argc;
char *argv[];
{
    Widget top_level_widget;
    XtAppContext context;
    Arg arg_list[MAX_ARGS];
    int arg_len;
    Display *disp;
    int selected;

    XtToolkitInitialize();
#ifdef NO_APP_BUG
    context = XtCreateApplicationContext();
#else
    context = (XtAppContext) 0;
#endif    
    disp = XtOpenDisplay(context, "", argv[0], "VemDM", (XrmOptionDescRec *) 0,
			 0, &argc, argv);
    arg_len = 0;
    top_level_widget = XtAppCreateShell(argv[0], "VemDM",
					applicationShellWidgetClass,
					disp, arg_list, arg_len);
    set_topw(top_level_widget);
    dmCompatInit(other_events);
#ifdef CONF_TEST
    if (dmConfirm("Fatal Error", explan, "Continue", "Abort") == VEM_OK) {
	printf("first button pushed.\n");
    } else {
	printf("second button pushed.\n");
    }
    if (dmConfirm("Which One Help", ww_help, "Dismiss", (char *) 0) == VEM_OK) {
	printf("first button pushed.\n");
    } else {
	printf("second button pushed.\n");
    }
#endif
#ifdef MESSAGE_TEST
    dmMessage("Hi there", "This is a very short\nmessage.", 0);
    dmMessage("Important Note", long_message, 0);
#endif
#ifdef TXT_TEST
    if (dmMultiText("Enter Requested Data", tsize(txt_test), txt_test) == VEM_OK) {
	print_vals(tsize(txt_test), txt_test);
    } else {
	printf("Canceled.\n");
    }
#endif
#ifdef WW_TEST
    if (dmWhichOne("One Which One", isize(mulw_three), mulw_three,
		   &selected, ww_func, ww_help) == VEM_OK) {
	printf("Item %d was selected.\n", selected);
    } else {
	printf("Canceled.\n");
    }
    if (dmWhichOne("One Which One", isize(mulw_two), mulw_two,
		   &selected, ww_func, ww_help) == VEM_OK) {
	printf("Item %d was selected.\n", selected);
    } else {
	printf("Canceled.\n");
    }
    if (dmWhichOne("One Which One", isize(mulw_one), mulw_one,
		   &selected, ww_func, ww_help) == VEM_OK) {
	printf("Item %d was selected.\n", selected);
    } else {
	printf("Canceled.\n");
    }
#endif
#ifdef MULW_TEST
    if (dmMultiWhich("First One", isize(mulw_one), mulw_one,
		     mulw_func, mulw_help) == VEM_OK) {
	print_sels(isize(mulw_one), mulw_one);
    } else {
	printf("Canceled.\n");
    }
    if (dmMultiWhich("Next Test", isize(mulw_two), mulw_two,
		     mulw_func, mulw_help) == VEM_OK) {
	print_sels(isize(mulw_two), mulw_two);
    } else {
	printf("Canceled.\n");
    } 
    if (dmMultiWhich("Last Test", isize(mulw_three), mulw_three,
		     mulw_func, mulw_help) == VEM_OK) {
	print_sels(isize(mulw_three), mulw_three);
    } else {
	printf("Canceled.\n");
    }
#endif
#ifdef MESSAGE_TEST
    XtMainLoop();
#endif
    return 0;
}

static Widget topw = (Widget) 0;

void set_topw(w)
Widget w;
{
    topw = w;
}

Widget xv_topw()
{
    return topw;
}
