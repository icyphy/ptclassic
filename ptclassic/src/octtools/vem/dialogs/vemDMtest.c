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

/*
 * Possible tests: CONF_TEST, MESSAGE_TEST, TXT_TEST, WW_TEST, MULW_TEST,
 *		   ENUM_TEST, NUMBER_TEST
 */

#define CONF_TEST

#define isize(iw)	(sizeof(iw)/sizeof(dmWhichItem))
#define tsize(tw)	(sizeof(tw)/sizeof(dmTextItem))
#define lsize(en)	(sizeof(en)/sizeof(char *))
#define esize(en)	(sizeof(en)/sizeof(dmEnumItem))
#define nsize(nn)	(sizeof(nn)/sizeof(dmNumItem))

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

#define LS(ary)	(sizeof(ary)/sizeof(char *))

char *enum_one[] = { "One", "Two", "Three" };
char *enum_two[] = { "Many", "Possibilities", "Are", "Possible" };
char *enum_long[] = { 
    "The", "New", "Function", "dmMultiEnum", "Is", "Capable", "of", "Allowing", 
    "Users", "To", "Choose", "Among", "Many", "Different", "Values.", "This",
    "May", "Include", "Very", "Long", "Enumerated", "Lists"
};


dmEnumItem enum_test[] = {
    { "Small", lsize(enum_one), enum_one, 1, (char *) 0 },
    { "Medium", lsize(enum_two), enum_two, 2, (char *) 0 },
    { "Large", lsize(enum_long), enum_long, 5, (char *) 0 }
};

dmNumItem number_test[] = {
    { "Hours", 0.0, 24.0, 1.0, 8.0, "%2.0lf", (Pointer) 0 },
    { "Minutes", 0.0, 60.0, 1.0, 0.0, "%2.0lf", (Pointer) 0 },
    { "Seconds", 0.0, 60.0, 0.1, 60.0, "%2.1f", (Pointer) 0 }
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

char mid_text[] =
"This is some sample text used for testing the limits\n\
of the newly discovered bug in either vemDM, dds, or\n\
in the X toolkit.  So far, the bug cannot be isolated.";

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

char enum_help[] = 
"You can choose a different enumerated value by pressing\n\
the mouse in the value area.  A list of choices will appear.\n\
You can choose another value by moving over it and releasing\n\
the mouse.";

char number_help[] =
"You can vary the values displayed in the dialog by manipulating\n\
the scrollbar next to the value.  Pressing the left button deincrements\n\
the value.  Pressing the right button increments the value.  Pressing\n\
and dragging the middle button changes the value interactively.";

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

print_enums(num, items)
int num;
dmEnumItem *items;
{
    int i, j;

    for (i = 0;  i < num;  i++) {
	printf("`%s' = %d (`%s')\n", items[i].itemPrompt,
	       items[i].selected, items[i].enum_vals[items[i].selected]);
    }
    printf("finished printing enums\n");
}

print_ns(num, items)
int num;
dmNumItem *items;
{
    int i;

    for (i = 0;  i < num;  i++) {
	printf("`%s' is %lg\n", items[i].itemPrompt, items[i].value);
    }
    printf("finished printing numbers\n");
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
#ifdef NOTDEF
    printf("Other event:\n");
    pevent(stdout, evt);
#endif
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

    dds_initialize(&argc, argv);
    dmCompatInit(other_events);
#ifdef CONF_TEST
    if (dmConfirm("Fatal Error", explan, "Continue", "Abort") == VEM_OK) {
	printf("first button pushed.\n");
    } else {
	printf("second button pushed.\n");
    }
    if (dmConfirm("Middle test", mid_text, "Yes", "No") == VEM_OK) {
	printf("first\n");
    } else {
	printf("second\n");
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
    selected = 4;
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
#ifdef ENUM_TEST
    if (dmMultiEnum("Enumerated Test", esize(enum_test), enum_test, enum_help) == VEM_OK) {
	print_enums(esize(enum_test), enum_test);
    } else {
	printf("Canceled.\n");
    }
#endif
#ifdef NUMBER_TEST
    if (dmMultiNumber("Number Test", nsize(number_test), number_test,
		      number_help) == VEM_OK) {
	print_ns(nsize(number_test), number_test);
    } else {
	printf("Canceled.\n");
    }
#endif


#ifdef MESSAGE_TEST
    dds_loop(0);
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
