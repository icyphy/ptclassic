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
 * Dialog Definition System
 * Numerical Component
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 */

#include "dds_internal.h"

#include <X11/Xmu/Converters.h>
#include "Lbl.h"
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Scrollbar.h>

#define MAX_NUM_LEN	128

static ddsHandle new_number();
static void get_number();
static void set_number();
static void del_number();
static Widget parent_number();
static void place_number();

ddsClass _dds_number = {
    DDS_NUMBER, new_number, get_number, set_number, del_number,
    parent_number, place_number
};

static Widget new_value_label();
static void set_value();
static void set_bar();
static int check_values();
static void update_label();

/*
 * Callback functions
 */

static void scr_proc(w, client, call)
Widget w;			/* Scrollbar widget  */
XtPointer client;		/* (ddsFullNumber *) */
XtPointer call;			/* Position (int)    */
/*
 * This routine is called when the left or right button
 * is pressed in the scroll bar.  It increments or
 * decrements the value appropriately.
 */
{
    ddsFullNumber *item = (ddsFullNumber *) client;
    double previous;
    int position = (int) call;

    previous = item->user_spec.value;
    if (position > 0) {
	/* down */
	item->user_spec.value -= item->user_spec.increment;
	if (item->user_spec.value < item->user_spec.minimum) {
	    item->user_spec.value = item->user_spec.minimum;
	}
    } else {
	/* up */
	item->user_spec.value += item->user_spec.increment;
	if (item->user_spec.value > item->user_spec.maximum) {
	    item->user_spec.value = item->user_spec.maximum;
	}
    }
    if (ABS(previous - item->user_spec.value) >= (item->user_spec.increment/2)) {
	set_value(item);
	set_bar(item);
	if (item->user_spec.callback) {
	    (*item->user_spec.callback)((ddsHandle) item);
	}
    }
}

static void jump_proc(w, client, call)
Widget w;			/* Scrollbar widget  */
XtPointer client;		/* (ddsFullNumber *) */
XtPointer call;			/* Percent (float *) */
/*
 * Called when the user is dragging the scroll bar value
 * interactively.
 */
{
    ddsFullNumber *item = (ddsFullNumber *) client;
    float percent = *((float *) call);
    double span, previous;
    int increments;

    span = (item->user_spec.maximum - item->user_spec.minimum);
    increments = (int) (((double)percent*span)/item->user_spec.increment+0.5);
    previous = item->user_spec.value;
    item->user_spec.value = item->user_spec.minimum +
      (increments * item->user_spec.increment);
    if (ABS(previous - item->user_spec.value) >= (item->user_spec.increment/2)) {
	set_value(item);
	if (item->user_spec.callback) {
	    (*item->user_spec.callback)((ddsHandle) item);
	}
    }
}



static ddsHandle new_number(parent, class, data)
ddsInternal *parent;		/* Should be DDS_TOP or DDS_COMPOSITE */
ddsClass *class;		/* Should be DDS_NUMBER               */
ddsData data;			/* Should be (ddsNumber *)            */
/*
 * Makes a new number value component.  This consists of a label on
 * the left, a numerical value in the center, and a scroll bar on
 * the right.  The value is manipulated using the scroll bar.
 */
{
    ddsFullNumber *item;
    ddsInternal *same;
    Widget pw, all[3];
    int wc;

    if (parent) {
	item = ALLOC(ddsFullNumber, 1);
	same = (ddsInternal *) item;
	item->base.class = class;
	item->base.parent = parent;
	item->user_spec = *((ddsNumber *) data);
	if (check_values(&(item->user_spec))) {
	    if (!item->user_spec.format) {
		item->user_spec.format = "%lg";
	    }
	    item->user_spec.format = util_strsav(item->user_spec.format);
	    wc = 0;
	    pw = (*parent->base.class->_dds_parent_widget)(parent, same);
	    if (item->user_spec.label) {
		item->user_spec.label = util_strsav(item->user_spec.label);
		item->label =
		  XtVaCreateManagedWidget(DDS_NUMLBL_NAME, labelWidgetClass, pw,
					  XtNlabel, item->user_spec.label,
					  NULL);
		all[wc++] = item->label;
	    } else {
		item->label = (Widget) 0;
	    }
	    item->number = new_value_label(pw, item);
	    all[wc++] = item->number;
	    item->bar =
	      XtVaCreateManagedWidget(DDS_NUMSCR_NAME, scrollbarWidgetClass,pw,
				      XtNorientation, XtorientHorizontal,
				      NULL);
	    XtAddCallback(item->bar, XtNscrollProc, scr_proc,(XtPointer) item);
	    XtAddCallback(item->bar, XtNjumpProc, jump_proc,(XtPointer) item);
	    all[wc++] = item->bar;
	    set_bar(item);
	    (*parent->base.class->_dds_place)(parent, same, wc, all, 1);

	    /* Cursor management */
	    dds_add_cursor_widget(item->bar, XtNscrollDCursor);
	    dds_add_cursor_widget(item->bar, XtNscrollHCursor);
	    dds_add_cursor_widget(item->bar, XtNscrollLCursor);
	    dds_add_cursor_widget(item->bar, XtNscrollRCursor);
	    dds_add_cursor_widget(item->bar, XtNscrollUCursor);
	    dds_add_cursor_widget(item->bar, XtNscrollVCursor);
	} else {
	    ddsNumber *num = &(item->user_spec);
	    errRaise(ddsPackageName, DDS_BAD_VALUE,
		     "Bad values for minimum (%d), maximum(%d), value(%d), or increment(%d) to DDS_NUMBER",
		     num->minimum, num->maximum, num->value, num->increment);
	    /*NOTREACHED*/
	}
	return (ddsHandle) item;
    } else {
	errRaise(ddsPackageName, DDS_BAD_PARENT,
		 "Items of type DDS_NUMBER must have a parent");
	/*NOTREACHED*/
    }
    return (ddsHandle)NULL;
}



static void get_number(item, data)
ddsInternal *item;		/* DDS_NUMBER    */
ddsData data;			/* (ddsNumber *) */
/*
 * Copes out the current state of the number component.  The
 * strings are locally owned by the package and should be
 * copied by the caller before modification.
 */
{
    ddsNumber *actual = (ddsNumber *) data;

    *actual = item->number.user_spec;
}

static void set_number(item, data)
ddsInternal *item;		/* DDS_NUMBER    */
ddsData data;			/* (ddsNumber *) */
/*
 * Changes the state of the specified number component.  All
 * aspects of the number component may be changed.  Note that
 * the value will be changed to conform to the specified
 * increment.
 */
{
    ddsNumber *actual = (ddsNumber *) data;
    double new_value;
    char *new_format;
    int num, revise = 0;

    if (item->number.user_spec.label) {
	update_label(&(item->number), actual);
    }
    if (check_values(actual)) {
	item->number.user_spec.minimum = actual->minimum;
	item->number.user_spec.maximum = actual->maximum;
	num = (actual->value - actual->minimum)/actual->increment;
	new_value = actual->minimum + num*actual->increment;
	if (!(ABS(new_value - item->number.user_spec.value) < actual->increment/2)) {
	    revise = 1;
	}
	item->number.user_spec.value = new_value;
    }
    new_format = actual->format;
    if (!new_format) new_format = "%lg";
    new_format = util_strsav(new_format);
    if (strcmp(new_format, item->number.user_spec.format) != 0) {
	revise = 1;
    }
    FREE(item->number.user_spec.format);
    item->number.user_spec.format = new_format;
    if (revise) {
	set_value(&(item->number));
	set_bar(&(item->number));
    }
    item->number.user_spec.callback = actual->callback;
    item->number.user_spec.user_data = actual->user_data;
}


static void del_number(item)
ddsInternal *item;		/* DDS_NUMBER */
/*
 * Frees non-widget related resources.
 */
{
    /* Cursor management */
    dds_remove_cursor_widget(item->number.bar, XtNscrollDCursor);
    dds_remove_cursor_widget(item->number.bar, XtNscrollHCursor);
    dds_remove_cursor_widget(item->number.bar, XtNscrollLCursor);
    dds_remove_cursor_widget(item->number.bar, XtNscrollRCursor);
    dds_remove_cursor_widget(item->number.bar, XtNscrollUCursor);
    dds_remove_cursor_widget(item->number.bar, XtNscrollVCursor);

    if (item->number.user_spec.label) FREE(item->number.user_spec.label);
    FREE(item->number.user_spec.format);
    FREE(item);
}


/*ARGSUSED*/
static Widget parent_number(parent, child)
ddsInternal *parent;		/* DDS_NUMBER */
ddsInternal *child;		/* New child     */
{
    errRaise(ddsPackageName, DDS_BAD_PARENT,
	     "DDS_NUMBER components may not have child components");
    /*NOTREACHED*/
    return (Widget)NULL;
}

/*ARGSUSED*/
static void place_number(parent, child, nw, wlist, vspan)
ddsInternal *parent;		/* DDS_NUMBER */
ddsInternal *child;		/* New child     */
int nw;				/* Number of widgets */
WidgetList wlist;		/* New children  */
int vspan;
{
    errRaise(ddsPackageName, DDS_BAD_PARENT,
	     "DDS_NUMBER components may not have child components");
    /*NOTREACHED*/
}



#define CK_VAL(val) \
(void) sprintf(num_buf, item->user_spec.format, val);\
if (strlen(num_buf) > strlen(max_buf)) strcpy(max_buf, num_buf);

static Widget new_value_label(pw, item)
Widget pw;
ddsFullNumber *item;
/*
 * This routine creates a new label widget for holding the value.
 * It uses some heuristics to try to set the label width to
 * be large enough so that it will not resize as the value changes.
 */
{
    Widget result;
    char num_buf[MAX_NUM_LEN], max_buf[MAX_NUM_LEN];
    double last;

    max_buf[0] = '\0';
    CK_VAL(item->user_spec.minimum);
    CK_VAL(item->user_spec.minimum+item->user_spec.increment);
    CK_VAL(item->user_spec.maximum);
    last = item->user_spec.minimum +
      (int) ((item->user_spec.maximum - item->user_spec.minimum)
	     /item->user_spec.increment)
	* item->user_spec.increment;
    CK_VAL(last);
    result = XtVaCreateManagedWidget(DDS_NUMVAL_NAME, lblWidgetClass, pw,
				     XtNlabel, max_buf,
				     XtNjustify, XtJustifyRight,
				     NULL);
    XtVaSetValues(result, XtNresize, False, NULL);
    item->user_spec.value = item->user_spec.minimum +
      (int) ((item->user_spec.value - item->user_spec.minimum)/
	     item->user_spec.increment) * item->user_spec.increment;
    (void) sprintf(num_buf, item->user_spec.format, item->user_spec.value);
    XtVaSetValues(result, XtNlabel, num_buf, NULL);
    return result;
}


static void set_value(item)
ddsFullNumber *item;
/*
 * Sets the numerical value of the number component.
 */
{
    char num_buf[MAX_NUM_LEN];

    (void) sprintf(num_buf, item->user_spec.format, item->user_spec.value);
    XtVaSetValues(item->number, XtNlabel, num_buf, NULL);
}

static void set_bar(item)
ddsFullNumber *item;
/*
 * Sets the value of the bar
 */
{
    float percent;

    percent = (float) ((item->user_spec.value - item->user_spec.minimum)/
		       (item->user_spec.maximum - item->user_spec.minimum));
    XawScrollbarSetThumb(item->bar, percent, 0.0);
}


static void update_label(current, new)
ddsFullNumber *current;		/* Current state */
ddsNumber *new;			/* Desired state */
/*
 * If the label is different, the change is updated to the underlying
 * widgets.
 */
{
    if (current->user_spec.label != new->label) {
	FREE(current->user_spec.label);
	if (!new->label) new->label = DDS_ENLBL_NAME;
	current->user_spec.label = util_strsav(new->label);
	XtVaSetValues(current->label, XtNlabel, current->user_spec.label, NULL);
    }
}



static int check_values(num)
ddsNumber *num;			/* Number structure to check */
/*
 * Checks the validity of the values supplied in num.  Here are
 * the checks:
 *   1.  minimum < maximum
 *   2.  increment < (maximum-minimum)
 *   3.  value >= minimum and value <= maximum
 *   4.  increment > 0
 * Returns non-zero if the values pass the test.
 */
{
    return (num->minimum < num->maximum) &&
      (num->increment < (num->maximum - num->minimum)) &&
	(num->value >= num->minimum) && (num->value <= num->maximum) &&
	  (num->increment > 0);
}

