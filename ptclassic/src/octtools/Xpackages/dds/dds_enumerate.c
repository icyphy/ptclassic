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
 * Enumerated Items
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 */

#include "dds_internal.h"

#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>

static ddsHandle new_enumerate();
static void get_enumerate();
static void set_enumerate();
static void del_enumerate();
static Widget parent_enumerate();
static void place_enumerate();

ddsClass _dds_enumerate = {
    DDS_ENUMERATE, new_enumerate, get_enumerate, set_enumerate, del_enumerate,
    parent_enumerate, place_enumerate
};

static WidgetList build_body();
static void enumerate_callback();
static void update_label();
static void update_enum_list();
static char **copy_items();
static void dump_items();



static ddsHandle new_enumerate(parent, class, data)
ddsInternal *parent;		/* Should be DDS_TOP or DDS_COMPOSITE */
ddsClass *class;		/* Should be DDS_ENUMERATE            */
ddsData data;			/* Should be (ddsEnumerate *)         */
/*
 * Makes a new enumerated value component.  This consists of a label
 * and a button.  When the button is pushed, a menu pops up under
 * the button to allow the user to choose a fixed number of values.
 */
{
    ddsFullEnumerate *item;
    ddsInternal *same;
    Widget pw, both[2];
    int wc;

    if (parent) {
	item = ALLOC(ddsFullEnumerate, 1);
	same = (ddsInternal *) item;
	item->base.class = class;
	item->base.parent = parent;
	item->user_spec = *((ddsEnumerate *) data);
	item->user_spec.label = util_strsav(item->user_spec.label);
	if (item->user_spec.num_items > 0) {
	    item->user_spec.items = copy_items(item->user_spec.num_items,
					       item->user_spec.items);
	    wc = 0;
	    pw = (*parent->base.class->_dds_parent_widget)(parent, same);
	    if (item->user_spec.label) {
		item->user_spec.label = util_strsav(item->user_spec.label);
		item->label =
		  XtVaCreateManagedWidget(DDS_ENLBL_NAME, labelWidgetClass, pw,
					  XtNlabel, item->user_spec.label, NULL);
		both[wc++] = item->label;
	    } else {
		item->label = (Widget) 0;
	    }
	    if ((item->user_spec.selected >= 0) &&
		(item->user_spec.selected < item->user_spec.num_items)) {
		item->value =
		  XtVaCreateManagedWidget(DDS_ENVAL_NAME, menuButtonWidgetClass,
					  pw, XtNlabel,
					  item->user_spec.items
					  [item->user_spec.selected],
					  XtNmenuName, DDS_ENMENU_NAME,
					  XtNresize, False,
					  NULL);
		both[wc++] = item->value;
		XtAddCallback(item->value, XtNdestroyCallback,
			      _dds_destroy_callback, (XtPointer) item);
		(*parent->base.class->_dds_place)(parent, same, wc, both, 1);
		item->simpleMenu =
		  XtCreatePopupShell(DDS_ENMENU_NAME, simpleMenuWidgetClass,
				     item->value, (ArgList) 0, 0);
		item->sub_items = build_body(item);
		return (ddsHandle) item;
	    } else {
		errRaise(ddsPackageName, DDS_BAD_VALUE,
			 "Selected item of DDS_ENUMERATE is out of range");
		/*NOTREACHED*/
	    }
	} else {
	    errRaise(ddsPackageName, DDS_BAD_VALUE,
		     "DDS_ENUMERATE components must have at least one item");
	    /*NOTREACHED*/
	}
    } else {
	errRaise(ddsPackageName, DDS_BAD_PARENT,
		 "Items of type DDS_ENUMERATE must have a parent");
	/*NOTREACHED*/
    }
    return (ddsHandle)NULL;
}



static WidgetList build_body(item)
ddsFullEnumerate *item;
/*
 * Constructs sub-widgets for pop-up menu from list given by user.
 */
{
    int i;
    WidgetList result;

    result = ALLOC(Widget, item->user_spec.num_items);
    for (i = 0;  i < item->user_spec.num_items;  i++) {
	result[i] =
	  XtVaCreateManagedWidget(DDS_ENCHOICE_NAME, smeBSBObjectClass,
				  item->simpleMenu,
				  XtNlabel, item->user_spec.items[i],
				  NULL);
	XtAddCallback(result[i], XtNcallback, enumerate_callback, (caddr_t) item);
    }
    return result;
}



static void get_enumerate(item, data)
ddsInternal *item;		/* DDS_ENUMERATE     */
ddsData data;			/* (ddsEnumerate *)  */
/*
 * Copies out the current state of the enumerated component.  The
 * list of items and the label string are locally owned by the
 * package and should be copied by the caller before modification.
 */
{
    ddsEnumerate *actual = (ddsEnumerate *) data;

    *actual = item->enumerate.user_spec;
}



static void set_enumerate(item, data)
ddsInternal *item;		/* DDS_ENUMERATE     */
ddsData data;			/* (ddsEnumerate *)  */
/*
 * Changes the state of the specified enumerated component.  All
 * aspects of an enumerated component may be changed.
 */
{
    ddsEnumerate *actual = (ddsEnumerate *) data;

    if (item->enumerate.user_spec.label) {
	update_label(&(item->enumerate), actual);
    }
    update_enum_list(&(item->enumerate), actual);
    item->enumerate.user_spec.callback = actual->callback;
    item->enumerate.user_spec.user_data = actual->user_data;
}


static void update_label(current, new)
ddsFullEnumerate *current;	/* Current state */
ddsEnumerate *new;		/* Desired state */
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



static void update_enum_list(current, new)
ddsFullEnumerate *current;	/* Current state */
ddsEnumerate *new;		/* Desired state */
/*
 * Updates the list of possible choices for the enumerated value.
 * It reuses the old ones and makes new ones if necessary.
 */
{
    int old_num, i;
    WidgetList old_items;
    Arg arg_list[MAX_ARGS];
    int arg_len;

    old_num = current->user_spec.num_items;
    old_items = current->sub_items;

    current->user_spec.num_items = new->num_items;
    current->sub_items = ALLOC(Widget, current->user_spec.num_items);
    dump_items(old_num, current->user_spec.items);
    current->user_spec.items = copy_items(new->num_items, new->items);
    for (i = 0;  i < new->num_items;  i++) {
	arg_len = 0;
	XtSetArg(arg_list[arg_len], XtNlabel, current->user_spec.items[i]);
	arg_len++;
	if (i < old_num) {
	    /* Change old one */
	    current->sub_items[i] = old_items[i];
	    XtSetValues(current->sub_items[i], arg_list, arg_len);
	} else {
	    /* Create new one */
	    current->sub_items[i] =
	      XtCreateManagedWidget(DDS_ENCHOICE_NAME, smeBSBObjectClass,
				    current->sub_items[0], arg_list, arg_len);
	}
    }
    for (i = new->num_items;  i < old_num;  i++) {
	XtDestroyWidget(old_items[i]);
    }
    FREE(old_items);
    /* Now selected item */
    if ((new->selected >= 0) && (new->selected < new->num_items)) {
	XtVaSetValues(current->value, XtNlabel,
		      current->user_spec.items[new->selected], NULL);
	current->user_spec.selected = new->selected;
    }
}


static void del_enumerate(item)
ddsInternal *item;		/* DDS_ENUMERATE */
/*
 * Frees all non-widget resources consumed by the enumerated component.
 */
{
    if (item->enumerate.user_spec.label) FREE(item->enumerate.user_spec.label);
    dump_items(item->enumerate.user_spec.num_items,
	       item->enumerate.user_spec.items);
    FREE(item->enumerate.sub_items);
    FREE(item);
}



/*ARGSUSED*/
static Widget parent_enumerate(parent, child)
ddsInternal *parent;		/* DDS_ENUMERATE */
ddsInternal *child;		/* New child     */
{
    errRaise(ddsPackageName, DDS_BAD_PARENT,
	     "DDS_ENUMERATE components may not have child components");
    /*NOTREACHED*/
    return (Widget)NULL;
}

/*ARGSUSED*/
static void place_enumerate(parent, child, nw, wlist, vspan)
ddsInternal *parent;		/* DDS_ENUMERATE */
ddsInternal *child;		/* New child     */
int nw;				/* Number of widgets */
WidgetList wlist;		/* New children  */
int vspan;
{
    errRaise(ddsPackageName, DDS_BAD_PARENT,
	     "DDS_ENUMERATE components may not have child components");
    /*NOTREACHED*/
}



static void enumerate_callback(w, client, call)
Widget w;			/* smeBSBObjectClass */
XtPointer client;		/* (ddsInternal *)   */
XtPointer call;			/* Call data (none)  */
/*
 * Updates the current state of the enumerated value component
 * and calls user callback if state changes.
 */
{
    ddsInternal *item = (ddsInternal *) client;
    int i, previous;

    for (i = 0;  item->enumerate.user_spec.num_items;  i++) {
	if (w == item->enumerate.sub_items[i]) break;
    }
    if ((i < item->enumerate.user_spec.num_items) &&
	(i != item->enumerate.user_spec.selected)) {
	XtVaSetValues(item->enumerate.value, XtNlabel,
		      item->enumerate.user_spec.items[i], NULL);
	previous = item->enumerate.user_spec.selected;
	item->enumerate.user_spec.selected = i;
	if (item->enumerate.user_spec.callback) {
	    (*item->enumerate.user_spec.callback)((ddsHandle) item, previous);
	}
    }
}



static char **copy_items(nm, items)
int nm;
char **items;
/*
 * Allocates a copy of the given list of strings.
 */
{
    char **item_copy;
    int i;

    item_copy = ALLOC(char *, nm);
    for (i = 0;  i < nm;  i++) {
	item_copy[i] = util_strsav(items[i]);
    }
    return item_copy;
}

static void dump_items(nm, items)
int nm;
char **items;
/*
 * Disposes of a list of strings.
 */
{
    int i;

    for (i = 0;  i < nm;  i++) {
	FREE(items[i]);
    }
    FREE(items);
}
