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
 * Control Buttons
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 */

#include "dds_internal.h"

#include <X11/Xaw/Command.h>

static ddsHandle new_control();
static void get_control();
static void set_control();
static void del_control();
static Widget control_parent();
static void control_placement();

static void control_callback();

ddsClass _dds_control = {
    DDS_CONTROL, new_control, get_control, set_control, del_control,
    control_parent, control_placement
};

#ifdef ACCELERATORS
static void do_accelerator();
#endif /* ACCELERATORS */



static ddsHandle new_control(parent, class, data)
ddsInternal *parent;		/* Should be DDS_COMPOSITE  */
ddsClass *class;		/* Should be DDS_CONTROL    */
ddsData data;			/* Should be (ddsControl *) */
/*
 * Creates a new control button component.  This maps directly
 * to a command widget in the athena widget set.  Associates
 * a user callback with this button.
 */
{
    ddsFullControl *item;
    ddsInternal *same;
    Widget pw;

    if (parent && ((parent->base.class->type == DDS_TOP) ||
		   (parent->base.class->type == DDS_COMPOSITE))) {
	item = ALLOC(ddsFullControl, 1);
	same = (ddsInternal *) item;
	item->base.class = class;
	item->base.parent = parent;
	item->user_spec = *((ddsControl *) data);
	if (item->user_spec.name) {
	    item->user_spec.name = util_strsav(item->user_spec.name);
	} else {
	    item->user_spec.name = util_strsav(DDS_CONBTN_NAME);
	}
	
	pw = (*parent->base.class->_dds_parent_widget)(parent, same);

	item->button =
	  XtVaCreateManagedWidget(DDS_CONBTN_NAME, commandWidgetClass, pw,
				  XtNlabel, item->user_spec.name,
				  NULL);
	(*parent->base.class->_dds_place)(parent, same, 1, &item->button, 1);
	XtAddCallback(item->button, XtNcallback, control_callback,
		      (XtPointer) item);
	XtAddCallback(item->button, XtNdestroyCallback, _dds_destroy_callback,
		      (XtPointer) item);
#ifdef ACCELERATORS
	if (item->user_spec.accelerator) {
	    do_accelerator(item);
	}
#endif
	return (ddsHandle) item;
    } else {
	errRaise(ddsPackageName, DDS_BAD_PARENT,
		 "The parent of DDS_CONTROL components must be of type DDS_TOP or DDS_COMPOSITE"); 
	/*NOTREACHED*/
    }
    return (ddsHandle)NULL;
}



static void control_callback(w, client, call)
Widget w;			/* Widget                               */
XtPointer client;		/* Client data (actually ddsInternal *) */
XtPointer call;			/* Call data (none in this case)        */
/*
 * Invokes the user callback (if defined).
 */
{
    ddsInternal *item = (ddsInternal *) client;

    if (item && item->control.user_spec.callback) {
	(*item->control.user_spec.callback)((ddsHandle) item);
    }
}



static void get_control(item, data)
ddsInternal *item;		/* DDS_CONTROL    */
ddsData data;			/* (ddsControl *) */
/*
 * Copies out the current state of the control button.  Note that
 * the name is a locally allocated copy and should not be changed
 * by the application.
 */
{
    ddsControl *actual = (ddsControl *) data;

    *actual = item->control.user_spec;
}

static void set_control(item, data)
ddsInternal *item;		/* DDS_CONTROL    */
ddsData data;			/* (ddsControl *) */
/*
 * Sets the state of the control button based on the user supplied
 * information.  If the name string is different, it is updated
 * in the button.
 */
{
    ddsControl *actual = (ddsControl *) data;
    
    if (actual->name == (char *) 0) {
	actual->name = DDS_CONBTN_NAME;
    }
    actual->name = util_strsav(actual->name);
    if (strcmp(actual->name, item->control.user_spec.name) != 0) {
	XtVaSetValues(item->control.button, XtNlabel, actual->name, NULL);
    }
    FREE(item->control.user_spec.name);
    item->control.user_spec = *actual;
}



static void del_control(item)
ddsInternal *item;		/* DDS_CONTROL */
/*
 * Deletes the non-widget components of a control component.
 * The widget components are disposed of automatically by
 * the high level XtDestroyWidget.
 */
{
#ifdef ACCELERATORS
    if (item->control.user_spec.accelerator)
      FREE(item->control.user_spec.accelerator);
#endif
    FREE(item->control.user_spec.name);
    FREE(item);
}



/*ARGSUSED*/
static Widget control_parent(parent, child)
ddsInternal *parent;		/* DDS_CONTROL */
ddsInternal *child;		/* New child   */
/*
 * The control component cannot have children.
 */
{
    errRaise(ddsPackageName, DDS_BAD_PARENT,
	     "DDS_CONTROL components may not have child components");
    /*NOTREACHED*/
    return (Widget)NULL;
}

/*ARGSUSED*/
static void control_placement(parent, child, nw, wlist, vspan)
ddsInternal *parent;		/* DDS_CONTROL              */
ddsInternal *child;		/* New component            */
int nw;				/* Number of widgets        */
WidgetList wlist;		/* Widgets themselves       */
int vspan;
/*
 * This is an error.  The control component cannot have children.
 */
{
    errRaise(ddsPackageName, DDS_BAD_PARENT,
	     "DDS_CONTROL components may not have child components");
    /*NOTREACHED*/
}



#ifdef ACCELERATORS

#define MAX_BUF		1024

static void do_accelerator(item)
ddsFullControl *item;		/* Control button structure */
/*
 * This routine handles the installation of accelerators specified
 * for a control button.  These may only be set when the button
 * is first created (they may not be changed using dds_set).
 * The form of the accelerator is that of an "event" as given
 * in the syntax for X Toolkit translation tables.  This routine
 * builds a full translation table from the spec, parses it,
 * and sets the resource on the control button.  The translations
 * are then installed on all of the edit text components in
 * dds_post via the focus management routine _dds_install_accelerators.
 */
{
    char trans_table[MAX_BUF], copy[MAX_BUF], line[MAX_BUF];
    char *ptr, *spec;
    XtAccelerators accelerators;

    item->user_spec.accelerator = util_strsav(item->user_spec.accelerator);
    (void) strcpy(trans_table, "#override");
    (void) strcpy(copy, item->user_spec.accelerator);
    spec = copy;
    while ( (ptr = strchr(spec, '|')) ) {
	*ptr = '\0';
	(void) sprintf(line, "\n%s:\tset()notify()unset()", spec);
	(void) strcat(trans_table, line);
	spec = ptr+1;
    }
    (void) sprintf(line, "\n%s:\tset()notify()unset()", spec);
    (void) strcat(trans_table, line);
    accelerators = XtParseAcceleratorTable(trans_table);
    XtVaSetValues(item->button, XtNaccelerators, accelerators, NULL);
    _dds_default_focus((ddsInternal *) item, item->button);
}

#endif /* ACCELERATORS */
