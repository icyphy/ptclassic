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
 * Dialog Definition System
 * Exported interface
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 */

#include "dds_internal.h"
#include "errtrap.h"

char *ddsPackageName = "dds";

/*
 * Below is the global table of component classes.  Each component
 * should export at least one of these structures (statically
 * initialized) for inclusion in this table.  The classes are
 * declared in dds_internal.h.
 */

static ddsClass *dds_class_table[DDS_LAST_TYPE] = {
    &_dds_initialize,
    &_dds_top_composite,
    &_dds_norm_composite,
    &_dds_item_list,
    &_dds_edit_text,
    &_dds_enumerate,
    &_dds_show_text,
    &_dds_number,
    &_dds_control,
    &_dds_extern,
};

/*
 * The widget below is used to keep track of moded dialogs
 */

static Widget last_unposted;

/*
 * The top level initializer component is kept here
 */
static ddsInternal *init_component = (ddsInternal *) 0;

static void position();
static void rel_win();


ddsHandle dds_component(parent, type, data)
ddsHandle parent;		/* Parent or null if top level */
ddsType type;			/* Kind of component           */
ddsData data;			/* Fields for component        */
/*
 * Creates a new component of a dialog.  When creating subcomponents
 * of a composite, the `parent' parameter should be set to the parent
 * item.  When creating a top-level composite, the parent should be
 * zero.  Dialogs will be popped up relative to this widget.  Only 
 * DDS_TOP items may be at the top level.  The `data' parameter is a 
 * pointer to a structure of the appropriate type as given in the 
 * definition of dialog types and cast to (ddsData).  Copies will be 
 * made of all strings and arrays in the user passed structure.
 */
{
    ddsInternal *real_parent;

    if (parent) real_parent = (ddsInternal *) parent;
    else real_parent = _dds_get_init(DDS_TRUE);

    if ((((int) type) >= 0) && (((int) type) < ((int)DDS_LAST_TYPE))) {
	ddsClass *class = dds_class_table[(int)type];

	return (*class->_dds_new)(real_parent, class, data);
    } else {
	errRaise(ddsPackageName, DDS_BAD_TYPE,
		 "Unknown component type: %d", (int) type);
    }
    /*NOTREACHED*/
    return (ddsHandle)NULL;
}



ddsType dds_type(item)
ddsHandle item;
/*
 * Returns the type of the specified component.
 */
{
    ddsInternal *real_item = (ddsInternal *) item;

    return real_item->base.class->type;
}



void dds_get(item, data)
ddsHandle item;			/* Item created using dds_component */
ddsData data;			/* User passed structure            */
/*
 * Fills in the passed user structure with the current state of the
 * component.  Allocated fields in these structures are owned by
 * the package and must not be changed.  It is assumed the passed
 * structure is of the right type.  The behaviour of the routine
 * is undefined if the user passes the wrong kind of structure
 * in `data'.
 */
{
    ddsInternal *real_item = (ddsInternal *) item;

    (*real_item->base.class->_dds_get)(real_item, data);
}



void dds_set(item, data)
ddsHandle item;			/* Item created using dds_component */
ddsData data;			/* User passed structure            */
/*
 * Sets the state of the specified item using the user passed structure.
 * Any change in visual aspects of the component will be reflected
 * on the screen immediately if the dialog is posted.  Copies will be 
 * made of all strings and arrays passed as data.
 */
{
    ddsInternal *real_item = (ddsInternal *) item;

    (*real_item->base.class->_dds_set)(real_item, data);
}



void dds_post(dialog, pos, other_handler)
ddsHandle dialog;		/* Top level component     */
ddsPosition *pos;		/* Position to post dialog */
ddsEvtHandler *other_handler;	/* Moded dialog callback   */
/*
 * Makes `dialog' visible on the screen.  `dialog' must be a DM_TOP
 * component created using dm_component().  The dialog will be
 * positioned as given by `pos' (see position()).
 *
 * If `other_handler' is provided, the dialog will be moded.  This
 * means the routine will not return until the dialog is unposted
 * using a control button callback.  dds_post will call `other_handler' 
 * with all dialog related events.
 */
{
    ddsInternal *init = _dds_get_init(DDS_TRUE);
    ddsInternal *item = (ddsInternal *) dialog;
    Widget posted;
    XEvent evt;

    if (item->base.class->type == DDS_TOP) {
	if (other_handler && other_handler->other_handler) {
	    last_unposted = (Widget) 0;
	    posted = item->composite.top_info->top_shell;
	    position(posted, pos);
	    /* Moded dialog */

	    XtPopup(posted, XtGrabExclusive);
	    
#ifdef FOCUS
#ifdef ACCELERATORS
	    if (item->composite.top_info->installed_p == DDS_FALSE) {
		_dds_install_accelerators(item);
		item->composite.top_info->installed_p = DDS_TRUE;
	    }
#endif /* ACCELERATORS */
	    _dds_reset_focus(item);
#endif /* FOCUS */
	    /* Local loop */
	    while (last_unposted != posted) {
		XtAppNextEvent(init->initialize.context, &evt);
		if (XtDispatchEvent(&evt) == False) {
		    if ((*other_handler->other_handler)
			(&evt, other_handler->user_data)) {
			return;
		    }
		}
	    }
	    while (XtAppPending(init->initialize.context)) {
		XtAppNextEvent(init->initialize.context, &evt);
		if (XtDispatchEvent(&evt) == False) {
		    if ((*other_handler->other_handler)
			(&evt, other_handler->user_data)) {
			return;
		    }
		}
	    }
	} else {
	    /* Unmoded dialog */
	    position(item->composite.top_info->top_shell, pos);
	    XtPopup(item->composite.top_info->top_shell, XtGrabNonexclusive);
#ifdef FOCUS
#ifdef ACCELERATORS
	    if (item->composite.top_info->installed_p == DDS_FALSE) {
		_dds_install_accelerators(item);
		item->composite.top_info->installed_p = DDS_TRUE;
	    }
#endif /* ACCELERATORS */
	    _dds_reset_focus(item);
#endif /* FOCUS */
	}
    } else {
	errRaise(ddsPackageName, DDS_BAD_TYPE,
		 "Only components of type DDS_TOP may be posted");
	/*NOTREACHED*/
    }
}



static void position(shell, spec)
Widget shell;			/* Shell to be positioned */
ddsPosition *spec;		/* Where to put shell     */
/*
 * This routine attempts to place the dialog at the location
 * given by `spec':
 *   DDS_ABSOLUTE:	Position dialog at pixel locations given
 *			in the `x' and `y' fields subject to
 *			justification given in `vjust' and `hjust'
 *                      fields.
 *   DDS_MOUSE:		Like DDS_ABSOLUTE except location is taken
 *                      from the current mouse position.
 *   DDS_WINDOW:	Like DDS_ABSOLUTE except the location is
 *                      relative to the window provided in 
 *                      window field.  Justification is relative
 *                      to the vertical window borders.
 * Does not allow the dialog to be posted off-screen.
 */
{
    Screen *scrn;
    Window root, child;
    ddsPosition pos_cpy;
    int x, y, root_x, root_y;
    unsigned int width, height, bdr, mask;

    pos_cpy = *spec;
    XtRealizeWidget(shell); 	/* Realize it right away. */
    scrn = XtScreen(shell);
    switch (pos_cpy.pos) {
    case DDS_ABSOLUTE:
	x = pos_cpy.x;  y = pos_cpy.y;
	break;
    case DDS_MOUSE:
	x = y = 0;
	(void) XQueryPointer(DisplayOfScreen(scrn), RootWindowOfScreen(scrn),
			     &root, &child, &root_x, &root_y, &x, &y, &mask);
	break;
    case DDS_WINDOW:
	rel_win(DisplayOfScreen(scrn), &pos_cpy, &x, &y);
	break;
    }
    width = height = bdr = 0;
    XtVaGetValues(shell, XtNwidth, &width, XtNheight, &height,
		  XtNborderWidth, &bdr, NULL);
    width += 2*bdr;
    height += 2*bdr;
    switch (pos_cpy.vjust) {
    case DDS_ABOVE:
	y -= height;
	break;
    case DDS_MIDDLE:
	y -= (height/2);
	break;
    case DDS_BELOW:
	break;
    }
    switch (pos_cpy.hjust) {
    case DDS_RIGHT:
	break;
    case DDS_CENTER:
	x -= (width/2);
	break;
    case DDS_LEFT:
	x -= width;
    }
    /* Make sure it is on screen */
    /* Bug alert!  Under Openwindows, the edit-label fails to bring up
     * a dialog box because the call to XtVaGetValues on line 280
     * returns astronomical values for height and width. 
     * This is only a workaround, a real bug fix would help.
     */
#ifdef NEVER
    if (x < 0) x = 0;
    else if (x + width > WidthOfScreen(scrn)) x = WidthOfScreen(scrn) - width;
    if (y < 0) y = 0;
    else if (y + height > HeightOfScreen(scrn)) y = HeightOfScreen(scrn) - height;
#endif
    if (x + width > WidthOfScreen(scrn)) x = WidthOfScreen(scrn) - width;
    if (x < 0) x = 0;
    if (y + height > HeightOfScreen(scrn)) y = HeightOfScreen(scrn) - height;
    if (y < 0) y = 0;

    XtVaSetValues(shell, XtNx, x, XtNy, y, NULL);
}


static void rel_win(disp, spec, ax, ay)
Display *disp;
ddsPosition *spec;
int *ax, *ay;
/*
 * Computes the absolute location of the dialog relative to another window
 * all in relation to the root window.  This hacks around window managers
 * to discover decoration sizes and compenstate accordingly.
 */
{
    int tx, ty, x, y, sx, sy;
    unsigned int w, h, bdr, tw, th, tbdr, sw, sh, sbdr, dp, nchild;
    Window root, target, parent, *children;

    /* Begin by finding top level window to compenstate for decoration */
    *ax = *ay = 0;
    target = spec->win;
    (void) XGetGeometry(disp, spec->win, &root, &sx, &sy, &sw, &sh, &sbdr, &dp);
    for (;;) {
	if (!XQueryTree(disp, target, &root, &parent, &children, &nchild)) return;
	XFree((char *) children);
	if (parent == root) break;
	target = parent;
	(void) XGetGeometry(disp, target, &root, &x, &y, &w, &h, &bdr, &dp);
	sx += x;
	sy += y;
    }
    (void) XGetGeometry(disp, target, &root, &tx, &ty, &tw, &th, &tbdr, &dp);
    switch (spec->vjust) {
    case DDS_ABOVE:
	if (spec->hjust != DDS_CENTER) {
	    spec->vjust = DDS_BELOW;
	    *ay = ty + (sy-ty) + tbdr;
	} else {
	    *ay = ty;
#ifdef ORIG
	    *ay = ty - (th-(sh+sbdr));
#endif /* ORIG */
	}
	break;
    case DDS_MIDDLE:
	*ay = ty + (th/2);
	break;
    case DDS_BELOW:
	if (spec->hjust != DDS_CENTER) {
	    spec->vjust = DDS_ABOVE;
	    *ay = ty + th;
#ifdef ORIG
	    *ay = ty + th - ((ty+th) - (sy+sh+2*sbdr)) - tbdr;
#endif /* ORIG */
	} else {
	    *ay = ty + th + (sy-ty) + tbdr;
	}
	break;
    }
    switch (spec->hjust) {
    case DDS_LEFT:
	*ax = tx - (sx - tx) - tbdr;
	break;
    case DDS_CENTER:
	*ax = tx + (tw/2);
	break;
    case DDS_RIGHT:
	*ax = tx + tw + 2*tbdr + (sx-tx+tbdr);
	break;
    }
}



void dds_unpost(component)
ddsHandle component;		/* Component of dialog */
/*
 * This routine removes the posted dialog from the screen.  The
 * resources for the dialog are not reclaimed -- it can be posted
 * again later using dm_post().  This function is normally
 * called by control button callbacks.
 */
{
    ddsInternal *item = (ddsInternal *) component;

    last_unposted = _dds_find_top(item)->composite.top_info->top_shell;
    XtPopdown(last_unposted);
}



Window dds_window(component)
ddsHandle component;		/* Component of dialog */
/*
 * Returns the containing window of the dialog that contains
 * the component `component'.
 */
{
    ddsInternal *item = (ddsInternal *) component;

    return XtWindow(_dds_find_top(item)->composite.top_info->top_shell);
}



/*
There is a serious bug in X11, having to do with the cashing of some
widget hierarchies. This bug is excited by VEM, which crashes with
a seg fault. 
The correct solution is to fix X11lib, which has not been possible so far.
The patch proposed by David is to call XtUnrealize rather than 
XtDestroy. The patch works for VEM, but does not work for other
applications that use DDS more aggressively than VEM does, because XtUnrealize
does not do the right thing with the keyboard focus.

The next patch is to declare a variable to control the behavior of dds_destroy.
This variable will not be publicized, because it hopefully
won't be needed soon.
*/

int ddsReallyDestroyFlag = 0;	/* Control the behavior of dds_destroy() */

void dds_destroy(component)
ddsHandle component;		/* Component of doomed dialog */
/*
 * This routine both removes the dialog from the screen and
 * reclaims resources consumed by the dialog.  It is a no-no
 * to reference the dialog or any of its sub-components after
 * using this function.
 */
{

    if ( ddsReallyDestroyFlag ) {
      /* When X11 is fixed, this clause should be chosen. */
      ddsInternal *real_item = (ddsInternal *) component;
      last_unposted = _dds_find_top(real_item)->composite.top_info->top_shell;
      XtDestroyWidget(last_unposted); 
    } else {
      /* Until X11 has the bug, this clause can help get around the bug. */
      ddsInternal *real_item = (ddsInternal *) component;
      dds_unpost( component );
      last_unposted = _dds_find_top(real_item)->composite.top_info->top_shell;
      XtUnrealizeWidget(last_unposted);      /* This is a serious core leak */
    }
}



int dds_loop(other)
ddsEvtHandler *other;		/* Function for handling other events */
/*
 * Goes into an indefinte loop handling events and dispatching them
 * appropriately.  Events that cannot be dispatched are sent to
 * `other'.  If the handler returns a non-zero status, the routine
 * returns.
 */
{
    ddsInternal *real_item = _dds_get_init(DDS_TRUE);
    XEvent evt;
    int status;

    if (real_item->base.class->type == DDS_INITIALIZE) {
	for (;;) {
	    XtAppNextEvent(real_item->initialize.context, &evt);
	    if ((XtDispatchEvent(&evt) == False) &&
		(other != (ddsEvtHandler *) 0) &&
		(other->other_handler != (int (*)()) 0)) {
		if ((status =
		     (*other->other_handler)(&evt, other->user_data)) ) {
		    return status;
		}
	    }
	}
    } else {
	errRaise(ddsPackageName, DDS_BAD_TYPE,
		 "dds_loop() requires a component of type DDS_INITIALIZE");
	/*NOTREACHED*/
    }
    /*NOTREACHED*/
    return 0;
}



int dds_outstanding(other)
ddsEvtHandler *other;		/* Function for handling other events */
/*
 * This routine is similar to dds_loop except it handles all outstanding
 * events then returns control to the user.   All unhandled events are
 * passed to `other' (if provided).  If the handler returns a non-zero
 * value, the loop will be terminated immediately and that status
 * will be returned.  Normally, the routine returns zero when it
 * runs out of events to handle.
 */
{
    ddsInternal *real_item = _dds_get_init(DDS_TRUE);
    XEvent evt;
    int status;

    if (real_item->base.class->type == DDS_INITIALIZE) {
	while (XtAppPending(real_item->initialize.context)) {
	    XtAppNextEvent(real_item->initialize.context, &evt);
	    if ((XtDispatchEvent(&evt) == False) &&
		(other != (ddsEvtHandler *) 0) &&
		(other->other_handler != (int (*)()) 0)) {
		if ( (status =
		      (*other->other_handler)(&evt, other->user_data)) ) {
		    return status;
		}
	    }
	}
	return 0;
    } else {
	errRaise(ddsPackageName, DDS_BAD_TYPE,
		 "dds_loop() requires a component of type DDS_INITIALIZE");
	/*NOTREACHED*/
    }
    /*NOTREACHED*/
    return 0;
}



void _dds_destroy_callback(w, client, call)
Widget w;			/* Widget to destroy                    */
XtPointer client;		/* Client data (actually ddsInternal *) */
XtPointer call;			/* Call data (none in this case)        */
/*
 * Invokes the class destroy procedure (if any).
 */
{
    ddsInternal *item = (ddsInternal *) client;

    if (item && (item->base.class->_dds_del)) {
	(*item->base.class->_dds_del)(item);
    }
}

ddsInternal *_dds_find_top(item)
ddsInternal *item;		/* Component */
/*
 * Returns the pop-up shell associated with the component `item'.
 * Works its way up the component heirarchy looking for it.
 */
{
    while (item->base.class->type != DDS_TOP) {
	item = item->base.parent;
    }
    return item;
}


void _dds_set_init(init)
ddsInternal *init;		/* Initializer component */
/*
 * This routine sets the initializer component for use by other
 * routines.  _dds_get_init() retrieves it.  It also detects
 * multiple initializations.
 */
{
    if (!init_component) {
	init_component = init;
    } else {
	errRaise(ddsPackageName, DDS_MULTI_INIT,
		 "Package can only be initialized once");
	/*NOTREACHED*/
    }
}

ddsInternal *_dds_get_init(must_exist_p)
ddsBoolean must_exist_p;	/* If True, must exist */
/*
 * Retrieves the current initializer component.  If `must_exists_p'
 * is DDS_TRUE, then the routine will produce a fatal error if
 * the intializer component is not set.
 */
{
    if (init_component || (must_exist_p == DDS_FALSE)) {
	return init_component;
    } else {
	errRaise(ddsPackageName, DDS_NOT_INIT,
		 "Package must be initialized using dds_initialize()");
	/*NOTREACHED*/
    }
    return (ddsInternal*)NULL;
}
