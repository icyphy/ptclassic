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
 * Dialog Defintion System
 * Composite Component
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 */

#include "dds_internal.h"

#include <X11/Shell.h>
#include <X11/Xaw/Label.h>
#include "Table.h"

static ddsHandle new_top();
static ddsHandle new_comp();
static void get_comp();
static void set_comp();
static void del_comp();

static Widget get_parent();
static void comp_placement();

ddsClass _dds_top_composite = {
    DDS_TOP, new_top, get_comp, set_comp, del_comp,
    get_parent, comp_placement
};

ddsClass _dds_norm_composite = {
    DDS_COMPOSITE, new_comp, get_comp, set_comp, del_comp,
    get_parent, comp_placement
};

static ddsHandle fill_composite();
static void place_control();
static void place_body();


/*
** Our sun4 library seems to have a bug, whose effect is to
** always pop transient shells in the top left corner.
*/
#ifdef sun
#define XLIB_BUG
#endif

static ddsHandle new_top(parent, class, data)
ddsInternal *parent;		/* Should be DM_INITIALIZE        */
ddsClass *class;		/* Should be DM_TOP               */
ddsData data;			/* Should be (dmComposite *)      */
/*
 * Creates a new top level composite component.  This is just like
 * a regular composite except it defines a top-level shell for
 * later posting.
 */
{
    ddsFullComposite *item;
    ddsInternal *same;
    Widget pw;

    if (parent && (parent->base.class->type == DDS_INITIALIZE)) {
	item = ALLOC(ddsFullComposite, 1);
	same = (ddsInternal *) item;
	item->base.class = class;
	item->base.parent = parent;
	item->top_info = ALLOC(ddsTop, 1);
	pw = (*parent->base.class->_dds_parent_widget)(parent, same);
#ifdef XLIB_BUG
	item->top_info->top_shell =
	  XtVaCreatePopupShell(DDS_POPUP_NAME, topLevelShellWidgetClass, pw,
			       XtNinput, True,
			       XtNallowShellResize, True,
			       NULL);
#else
	item->top_info->top_shell =
	  XtVaCreatePopupShell(DDS_POPUP_NAME, transientShellWidgetClass, pw,
			       XtNinput, True,
			       XtNallowShellResize, True,
			       XtNtransientFor, pw, /* pw  already a shell widget */
			       /* XtNoverrideRedirect, True, */
			       /* XtNsaveUnder, True, */
			       NULL);
#endif
	(*parent->base.class->_dds_place)(parent, same, 1,
					  &item->top_info->top_shell, 1);
#ifdef FOCUS
	_dds_focus_init((ddsInternal *) item);
#ifdef ACCELERATORS
	item->top_info->installed_p = DDS_FALSE;
#endif /* ACCELERATORS */
#endif /* FOCUS */
	item->overallTable =
	  XtCreateManagedWidget(DDS_OVERALL_NAME, tableWidgetClass,
				item->top_info->top_shell, (ArgList) 0, 0);
	dds_add_cursor_widget(item->overallTable, XtNcursor);
	return fill_composite(item, data);
    } else {
	errRaise(ddsPackageName, DDS_BAD_PARENT,
		 "The parent of DDS_TOP components must be of type DDS_INITIALIZE"); 
	/*NOTREACHED*/
    }
    return (ddsHandle)NULL;
}


static ddsHandle new_comp(parent, class, data)
ddsInternal *parent;		/* Should be DM_COMPOSITE         */
ddsClass *class;		/* Should be DM_TOP               */
ddsData data;			/* Should be (dmComposite *)      */
/*
 * Creates a new composite component.  A composite component consists
 * of an optional title, a body, and a set of control buttons all formatted
 * using a Table widget.  New items may be added to the body.
 */
{
    ddsFullComposite *item;
    ddsInternal *same;
    Widget pw;

    if (parent && ((parent->base.class->type == DDS_TOP) ||
		   (parent->base.class->type == DDS_COMPOSITE))) {
	item = ALLOC(ddsFullComposite, 1);
	same = (ddsInternal *) item;
	item->base.class = class;
	item->base.parent = parent;
	item->top_info = (ddsTop *) 0;
	pw = (*parent->base.class->_dds_parent_widget)(parent, same);
	item->overallTable =
	  XtCreateManagedWidget(DDS_OVERALL_NAME, tableWidgetClass, pw,
				(ArgList) 0, 0);
	(*parent->base.class->_dds_place)(parent, same, 1,
					  &item->overallTable, 1);
	dds_add_cursor_widget(item->overallTable, XtNcursor);
	return fill_composite(item, data);
    } else {
	errRaise(ddsPackageName, DDS_BAD_PARENT,
		 "The parent of DDS_COMPOSITE components must be of type DDS_TOP or DDS_COMPOSITE"); 
	/*NOTREACHED*/
    }
    return (ddsHandle)NULL;
}



static ddsHandle fill_composite(item, data)
ddsFullComposite *item;		/* DDS_TOP or DDS_COMPOSITE */
ddsData data;			/* (ddsComposite *)         */
/*
 * Fills in the common portion of the composite component and
 * creates the necessary widgets.
 */
{
    int this_row = 0;

    XtAddCallback(item->overallTable, XtNdestroyCallback,
		  _dds_destroy_callback, (XtPointer) item);
    item->user_spec = *((ddsComposite *) data);
    if (item->user_spec.title) {
	item->user_spec.title =
	  util_strsav(item->user_spec.title);
	item->title =
	  XtVaCreateManagedWidget(DDS_TITLE_NAME, labelWidgetClass,
				  item->overallTable,
				  XtNlabel, item->user_spec.title,
				  NULL);
	XtTblConfig(item->title, 0, this_row, 1, 1, TBL_SM_HEIGHT);
	this_row++;
    } else {
	item->title = (Widget) 0;
    }
    item->bodyTable =
      XtCreateManagedWidget(DDS_BODY_NAME, tableWidgetClass,
			    item->overallTable,
			    (ArgList) 0, 0);
    XtTblPosition(item->bodyTable, 0, this_row);
    this_row++;
    item->controlTable =
      XtCreateManagedWidget(DDS_CONTROL_NAME, tableWidgetClass,
			    item->overallTable,
			    (ArgList) 0, 0);
    XtTblConfig(item->controlTable, 0, this_row, 1, 1, TBL_SM_HEIGHT);
    item->cur_row = 0;
    item->cur_col = 0;
    item->max_vspan = 0;
    item->last_control = 0;
    return (ddsHandle) item;
}



static Widget get_parent(parent, child)
ddsInternal *parent;		/* DDS_TOP or DDS_COMPOSITE */
ddsInternal *child;		/* New child component      */
/*
 * Returns the widget that other widgets are created under.
 */
{
    if (child->base.class->type == DDS_CONTROL) {
	return parent->composite.controlTable;
    } else {
	return parent->composite.bodyTable;
    }
}

static void comp_placement(parent, child, nw, wlist, vspan)
ddsInternal *parent;		/* DDS_TOP or DDS_COMPOSITE */
ddsInternal *child;		/* New component            */
int nw;				/* Number of widgets        */
WidgetList wlist;		/* Widgets themselves       */
int vspan;			/* Vertical span            */
/*
 * Places items into a composite widget.  Control button items are
 * placed in a row in the controlTable.  All other components are
 * placed in the body widget based on the current row and column.
 * Body items may have one, two, or three widgets that are always
 * aligned horizontally.  The `vspan' gives the placement algorithm
 * a hint at the vertical size of the component.  It is measured
 * in `base components' where each base component is a simple
 * leaf widget.
 */
{
    if (child->base.class->type == DDS_CONTROL) {
	place_control(parent, nw, wlist);
    } else {
	place_body(parent, nw, wlist, vspan);
    }
}



static void place_control(item, nw, wlist)
ddsInternal *item;		/* DDS_TOP or DDS_COMPOSITE */
int nw;				/* Number of widgets        */
WidgetList wlist;		/* Widget list              */
/*
 * Places a control component into a composite component.  It
 * is added to the controlTable and placed at the next available
 * slot.
 */
{
    if (nw == 1) {
	XtTblPosition(wlist[0], item->composite.last_control, 0);
	item->composite.last_control++;
    } else {
	errRaise(ddsPackageName, DDS_INTERNAL,
		 "Composite component can format at most one control button at a time");
	/*NOTREACHED*/
    }
}



static void incr_body(item, vspan)
ddsInternal *item;		/* DDS_TOP or DDS_COMPOSITE */
int vspan;			/* Vertical span            */
/*
 * Increments the current row and column based on the direction
 * and maximum number of items.
 */
{
    switch (item->composite.user_spec.just) {
    case DDS_HORIZONTAL:
	item->composite.cur_col += 1;
	if (vspan > item->composite.max_vspan) {
	    item->composite.max_vspan = vspan;
	}
	if (item->composite.cur_col >= item->composite.user_spec.max_items) {
	    item->composite.cur_col = 0;
	    item->composite.cur_row += item->composite.max_vspan;
	    item->composite.max_vspan = 0;
	}
	break;
    case DDS_VERTICAL:
	item->composite.cur_row += vspan;
	if (item->composite.cur_row >= item->composite.user_spec.max_items) {
	    item->composite.cur_row = 0;
	    item->composite.cur_col += 1;
	}
	break;
    default:
	errRaise(ddsPackageName, DDS_BAD_VALUE,
		 "Unknown composite justification: %d",
		 (int) item->composite.user_spec.just);
	/*NOTREACHED*/
    }
}

static void place_body(item, nw, wlist, vspan)
ddsInternal *item;		/* DDS_TOP or DDS_COMPOSITE */
int nw;				/* Number of widgets        */
WidgetList wlist;		/* Widget list              */
int vspan;			/* Vertical span            */
/*
 * Places a new body component into a composite componeent.  It
 * is added to the bodyTable.  The new child component may have
 * one, two, or three widgets.  The widgets all span across
 * three columns.  The vertical span is used to determine
 * the row span.  The direction is used to determine the 
 * increment direction.  The maximum number of items is used 
 * to determine when to wrap items.  Currently, this placement 
 * algorithm is a little naive.
 */
{
    switch (nw) {
    case 1:
	XtTblConfig(wlist[0], item->composite.cur_col*3, item->composite.cur_row,
		    3, vspan, 0);
	incr_body(item, vspan);
	break;
    case 2:
	XtTblConfig(wlist[0], item->composite.cur_col*3,
		    item->composite.cur_row, 1, vspan, TBL_SM_WIDTH);
	XtTblConfig(wlist[1], item->composite.cur_col*3+1,
		    item->composite.cur_row, 2, vspan, 0);
	incr_body(item, vspan);
	break;
    case 3:
	XtTblConfig(wlist[0], item->composite.cur_col*3,
		    item->composite.cur_row, 1, vspan, TBL_SM_WIDTH);
	XtTblConfig(wlist[1], item->composite.cur_col*3+1,
		    item->composite.cur_row, 1, vspan, TBL_SM_WIDTH);
	XtTblConfig(wlist[2], item->composite.cur_col*3+2,
		    item->composite.cur_row, 1, vspan, 0);
	incr_body(item, vspan);
	break;
    default:
	errRaise(ddsPackageName, DDS_INTERNAL,
		 "Unsupported number of subwidgets to DDS_COMPOSITE");
	/*NOTREACHED*/
    }
}



static void get_comp(item, data)
ddsInternal *item;		/* DDS_TOP          */
ddsData data;			/* (ddsComposite *) */
/*
 * Returns the current state of the specified DDS_TOP item.  It is
 * assumed data is (ddsComposite *).
 */
{
    ddsComposite *actual = (ddsComposite *) data;

    *actual = item->composite.user_spec;
}

static void set_comp(item, data)
ddsInternal *item;		/* DDS_TOP          */
ddsData data;			/* (ddsComposite *) */
/*
 * Sets the current state of the DDS_TOP item.  The state
 * of a composite widget cannot be changed.
 */
{
    errRaise(ddsPackageName, DDS_NOT_SUPPORTED,
	     "Cannot set the state of DDS_TOP or DDS_COMPOSITE items");
    /*NOTREACHED*/
}



static void del_comp(item)
ddsInternal *item;		/* DDS_COMPOSITE */
/*
 * Releases resources consumed by a normal composite component.
 * This is called as the result of the toolkit destroy callback.
 * Thus, all widgets will be recliamed automatically.  Only
 * allocated space need be freed.
 */
{
    if (item->composite.top_info) {
#ifdef FOCUS
	if (item->composite.top_info->focus) {
	    _dds_free_focus(item);
	}
#endif /* FOCUS */
	FREE(item->composite.top_info);
    }
    dds_remove_cursor_widget(item->composite.overallTable, XtNcursor);
    FREE(item->composite.user_spec.title);
    FREE(item);
}

