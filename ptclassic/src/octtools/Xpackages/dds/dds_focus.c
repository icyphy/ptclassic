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
 * Focus Management Routines
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 */

#include "dds_internal.h"

#define INIT_SIZE	10

static void add_widget();
static void set_focus();
static void call_cb();


void _dds_focus_init(item)
ddsInternal *item;		/* DDS_TOP */
/*
 * Initializes the focus state of the supplied DDS_TOP
 * component.
 */
{
    ddsFocus *info = ALLOC(ddsFocus, 1);

    item->composite.top_info->focus = info;
    info->alloc = INIT_SIZE;
    info->count = 0;
    info->current = -1;
    info->list = ALLOC(ddsFocusItem, info->alloc);
    info->def_focus = (Widget) 0;
}


void _dds_default_focus(item, widget)
ddsInternal *item;		/* Component    */
Widget widget;			/* Focus widget */
/*
 * This routine sets the default focus widget for a dialog
 * that contains the component `item' to `widget'.  This
 * will be the focus of the widget if there are no real
 * focus widgets for the dialog.  This must be a widget
 * capable of handling translations so that accelerators work.
 */
{
    ddsInternal *top = _dds_find_top(item);
    ddsFocus *info = top->composite.top_info->focus;

    if (!info->def_focus) info->def_focus = widget;
}


void _dds_focus_widget(item, widget, cb, client)
ddsInternal *item;		/* Component        */
Widget widget;			/* Focus widget     */
XtCallbackProc cb;		/* Focus callback   */
XtPointer client;		/* Closure          */
/*
 * This routine adds `widget' to the list of widgets that
 * may obtain focus under the dialog that contains `item'.
 * When focus changes, `cb' will be called.  The `call'
 * data will be zero if focus is leaving the widget and
 * one if focus is moving into the widget.
 */
{
    ddsInternal *top = _dds_find_top(item);
    ddsFocus *info = top->composite.top_info->focus;
    /*Widget tw = top->composite.top_info->top_shell;*/

    add_widget(info, widget, cb, client);
}


void _dds_reset_focus(item)
ddsInternal *item;		/* Component */
/*
 * Resets the focus of the dialog containing `item' so
 * that the first item has focus.
 */
{
    ddsInternal *top = _dds_find_top(item);
    ddsFocus *info = top->composite.top_info->focus;
    Widget tw = top->composite.top_info->top_shell;
    int i;

    if (info->count > 0) {
	for (i = 0;  i < info->count;  i++) {
	    call_cb(info->list+i, 0);
	}
	info->current = -1;
	set_focus(tw, info, 0);
    } else {
	if (info->def_focus) {
	    XtSetKeyboardFocus(tw, info->def_focus);
	} else {
	    XtSetKeyboardFocus(tw, None);
	}
    }
}


#ifdef ACCELERATORS

void _dds_install_accelerators(item)
ddsInternal *item;		/* Component */
/*
 * This routine installs registered accelerators on all of
 * the edit text components in the dialog as well as the
 * top level shell.  These are the only components that
 * receive text focus.
 */
{
    ddsInternal *top = _dds_find_top(item);
    ddsFocus *info = top->composite.top_info->focus;
    Widget tw = top->composite.top_info->top_shell;
    int i;

    if (info->def_focus) {
	XtInstallAllAccelerators(info->def_focus, tw);
    }
    for (i = 0;  i < info->count;  i++) {
	XtInstallAllAccelerators(info->list[i].w, tw);
    }
}

#endif /* ACCELERATORS */


void _dds_advance_focus(item)
ddsInternal *item;		/* Component */
/*
 * Cause the focus of the dialog containing `item' to move
 * forward one field.
 */
{
    ddsInternal *top = _dds_find_top(item);
    ddsFocus *info = top->composite.top_info->focus;
    Widget tw = top->composite.top_info->top_shell;

    if (info->current+1 < info->count) {
	set_focus(tw, info, info->current+1);
    } else {
	set_focus(tw, info, 0);
    }
}


void _dds_retreat_focus(item)
ddsInternal *item;		/* Component */
/*
 * Causes the focus of the dialog containing `item' to
 * move backward one field.
 */
{
    ddsInternal *top = _dds_find_top(item);
    ddsFocus *info = top->composite.top_info->focus;
    Widget tw = top->composite.top_info->top_shell;

    if (info->current-1 >= 0) {
	set_focus(tw, info, info->current-1);
    } else {
	set_focus(tw, info, info->count-1);
    }
}


void _dds_set_focus(item, widget)
ddsInternal *item;		/* Component    */
Widget widget;			/* Focus widget */
/*
 * Sets the focus of the dialog containing `item' to `widget'.
 * The widget must have been added to the focus list using
 * _dds_focus_widget().
 */
{
    ddsInternal *top = _dds_find_top(item);
    ddsFocus *info = top->composite.top_info->focus;
    Widget tw = top->composite.top_info->top_shell;
    int i;

    for (i = 0;  i < info->count;  i++) {
	if (info->list[i].w == widget) break;
    }
    if (i < info->count) set_focus(tw, info, i);
}



void _dds_free_focus(item)
ddsInternal *item;		/* Component */
/*
 * Frees up focus information for the dialog containing
 * the component `item'.
 */
{
    ddsInternal *top = _dds_find_top(item);
    ddsFocus *info = top->composite.top_info->focus;

    XtSetKeyboardFocus(top->composite.top_info->top_shell, None);
    FREE(info->list);
    FREE(info);
}


static void add_widget(info, w, cb, client)
ddsFocus *info;			/* Focus information */
Widget w;			/* Widget itself     */
XtCallbackProc cb;		/* Callback          */
XtPointer client;		/* Closure           */
/*
 * Adds the widget to the current focus widget list.  Makes
 * more space if necessary.
 */
{
    if (info->count >= info->alloc) {
	info->alloc *= 2;
	info->list = REALLOC(ddsFocusItem, info->list, info->alloc);
    }
    info->list[info->count].w = w;
    info->list[info->count].cb = cb;
    info->list[info->count].client = client;
    info->count++;
}

static void call_cb(item, call)
ddsFocusItem *item;		/* Focus item info  */
XtPointer call;			/* Call information */
/*
 * Calls the specified routine.
 */
{
    if (item->cb) (*item->cb)(item->w, item->client, call);
}

static void set_focus(topw, info, idx)
Widget topw;			/* Top widget        */
ddsFocus *info;			/* Focus information */
int idx;			/* Offset of focus   */
/*
 * Actually sets the focus.  Right now, this simply moves
 * the focus using XtSetKeyboardFocus.  Later, it might
 * call routines that modify the components to show
 * changes.
 */
{
    if (XtIsSensitive(info->list[idx].w)) {
	if ((info->current >= 0) && (info->current < info->count)) {
	    call_cb(info->list+info->current, (XtPointer) 0);
	}
	XtSetKeyboardFocus(topw, info->list[idx].w);
	info->current = idx;
	call_cb(info->list+info->current, (XtPointer) 1);
    }
}
