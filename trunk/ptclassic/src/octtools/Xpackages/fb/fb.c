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
 * Top level interface for graphic file browser
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 *
 * This is a library implementing a simple graphical file browser.
 * It shows a list of files each with an icon.  Clicking on a file
 * causes it to become highlighted.  Double clicking on it will
 * cause it to be opened (by calling a user defined callback function).
 */

#include "port.h"
#include "fb.h"

#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Box.h>
#include "Maxport.h"
#include "LImage.h"

#define MAX_ARGS	10

/*static XContext BrowserContext = (XContext) 0;*/
static void Select();
static void set_items();
static void destroy();

static caddr_t *fb_find();
static void fb_assoc();
static caddr_t fb_unassoc();

typedef struct context_info_defn {
    Widget selected;		/* Selected widget    */
    Widget box;			/* Box widget         */
    int num_items;		/* Number of items    */
    WidgetList sub_items;	/* Sub widgets        */
    caddr_t *udl;		/* User data list     */
    void (*callback)();		/* Function to call   */
    caddr_t data;		/* Global user data   */
} context_info;



Widget fb_create(parent, num_items, items, callback, data)
Widget parent;			/* Browser created under this widget */
int num_items;			/* Number of items displayed         */
fb_item items[];		/* Array of items to display         */
void (*callback)();		/* Callback function                 */
caddr_t data;			/* User data                         */
/*
 * This routine will create a new browser under the widget `parent'.
 * The browser consists of a viewport for possible scrolling (named
 * "browser"), a box to contain the files (named "box"), and the files 
 * themselves (created with the class name specified in `items').
 * One image/name pair will be created for each item in `items'.
 * They will be shown in the order given.  The image shown is not
 * directly specified by the browser.  The user should specify the
 * image as a resource using the class name for the item.  When
 * the user double clicks on an image, `callback' will be called
 * with the arguments described below:
 *   void callback(class, label, item_data, global_data)
 *   char *class, *label;
 *   caddr_t item_data, global_data;
 * The `item_data' is that associated with the item.  The `global_data'
 * is the `data' parameter to this function.
 */
{
    Widget vport;
    int arg_len;
    Arg arg_list[MAX_ARGS];
    context_info *info;

    arg_len = 0;
    vport = XtCreateManagedWidget("browser", maxportWidgetClass,
				  parent, arg_list, arg_len);
    info = (context_info *) XtMalloc(sizeof(context_info));
    XtAddCallback(vport, XtNdestroyCallback, destroy, (XtPointer) info);
    info->selected = (Widget) 0;
    info->box = XtCreateManagedWidget("box", boxWidgetClass,
				vport, arg_list, arg_len);
    info->num_items = 0;
    info->sub_items = (WidgetList) 0;
    info->udl = (caddr_t *) 0;
    info->callback = callback;
    info->data = data;
    fb_assoc(vport, (caddr_t) info);
    fb_assoc(info->box, (caddr_t) info);

    set_items(info, num_items, items);

    return vport;
}


void fb_update(fb, num_items, items)
Widget fb;			/* File browser    */
int num_items;			/* Number of items */
fb_item items[];		/* New array       */
/*
 * This routine updates the file browser `fb' with new information.
 * If the information is the same as the previous information, there will
 * be no change to the displayed state.  If there are any changes,
 * the displayed browser will be rebuilt.
 */
{
    caddr_t *spot;
    context_info *info;

    if ( (spot = fb_find(fb)) ) {
	info = (context_info *) *spot;
	set_items(info, num_items, items);
    } else {
	XtError("Can't find context information for browser");
    }
}




static void set_items(info, nm, items)
context_info *info;
int nm;
fb_item *items;
/*
 * Compares the old list to the new list.  If they differ, the old
 * ones are destroyed and the new ones are made.  If there are no
 * differences, no change occurs.
 */
{
    Arg arg_list[MAX_ARGS];
    int arg_len;
    int redo = 0;
    int i;
    char *label;
    
    if (info->num_items == nm) {
	for (i = 0;  i < nm;  i++) {
	    arg_len = 0;
	    XtSetArg(arg_list[arg_len], XtNlabel, &label);  arg_len++;
	    XtGetValues(info->sub_items[i], arg_list, arg_len);
	    if ((strcmp(label, items[i].name) != 0) ||
		(strcmp(XtName(info->sub_items[i]), items[i].class) != 0) ||
		(info->udl[i] != items[i].data)) {
		redo = 1;
		break;
	    }
	}
    } else {
	redo = 1;
    }
    if (redo) {
	if (info->udl) {
	    XtFree((char *) info->udl);
	}
	if (info->sub_items) {
	    for (i = 0;  i < info->num_items;  i++) {
		XtDestroyWidget(info->sub_items[i]);
	    }
	    XtFree((char *) info->sub_items);
	}
	info->num_items = nm;
	info->udl = (caddr_t *) XtCalloc(nm, sizeof(caddr_t));
	info->sub_items = (WidgetList) XtCalloc(nm, sizeof(Widget));
	info->selected = (Widget) 0;

	for (i = 0;  i < nm;  i++) {
	    arg_len = 0;
	    XtSetArg(arg_list[arg_len], XtNlabel, items[i].name);  arg_len++;
	    info->sub_items[i] = XtCreateManagedWidget(items[i].class,
				   lImageWidgetClass, info->box,
				   arg_list, arg_len);
	    info->udl[i] = items[i].data;
	    XtAddCallback(info->sub_items[i], XtNcallback,
			  Select, (caddr_t) items[i].data);
	}
    }
}



static void Select(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
    int arg_len, push_count;
    Boolean select_flag;
    String label;
    Arg arg_list[MAX_ARGS];
    context_info *info;
    caddr_t *spot;

    if ( (spot = fb_find(XtParent(w))) ) {
	info = (context_info *) *spot;
	arg_len = 0;
	XtSetArg(arg_list[arg_len], XtNselect, &select_flag);  arg_len++;
	XtSetArg(arg_list[arg_len], XtNlabel, &label);         arg_len++;
	XtGetValues(w, arg_list, arg_len);

	if (select_flag == True) {
	    push_count = (int) call_data;
	    if (push_count == 1) {
		/* Callback */
		(*info->callback)(XtName(w), label, client_data, info->data);
	    }
	} else {
	    arg_len = 0;
	    XtSetArg(arg_list[arg_len], XtNselect, 1);  arg_len++;
	    XtSetValues(w, arg_list, arg_len);
	    /* Turn off other one */
	    if (info->selected != (Widget) 0) {
		arg_len = 0;
		XtSetArg(arg_list[arg_len], XtNselect, 0);  arg_len++;
		XtSetValues(info->selected, arg_list, arg_len);
	    }
	    info->selected = w;
	}
    } else {
	XtError("Can't find context information for browser");
    }
}



static void destroy(w, client, call)
Widget w;			/* Widget to destroy */
XtPointer client;		/* Client data       */
XtPointer call;			/* Call data         */
/*
 * Called when a file browser is destroyed.  Breaks the
 * association link and recovers all non-widget related
 * values.
 */
{
    context_info *info = (context_info *) fb_unassoc(w);

    XtFree((char *)info->sub_items);
    XtFree((char *)info->udl);
    XtFree((char *)info);
}




typedef struct fb_tbl_defn {
    Widget w;
    caddr_t data;
} fb_tbl_type;

#define FB_TBL_INIT_SIZE	20

static int fb_tbl_alloc = FB_TBL_INIT_SIZE;
static int fb_tbl_last  = 0;
static fb_tbl_type *fb_tbl = (fb_tbl_type *) 0;

static caddr_t *fb_find(widget)
Widget widget;			/* Find associated data */
/*
 * Finds the data associated with widget.  Linear search.
 */
{
    int i;

    for (i = 0;  i < fb_tbl_last;  i++) {
	if (fb_tbl[i].w == widget) {
	    return &(fb_tbl[i].data);
	}
    }
    return (caddr_t *) 0;
}

static void fb_assoc(widget, data)
Widget widget;			/* Widget to associate data with */
caddr_t data;			/* Data itself                   */
/*
 * Adds information to a very simple mapping table between widgets
 * and data. The implementation asssumes only a small number
 * of associations will be made.
 */
{
    caddr_t *spot;

    if (!fb_tbl) {
	fb_tbl = (fb_tbl_type *) XtCalloc(fb_tbl_alloc, sizeof(fb_tbl_type));
    }
    if ( (spot = fb_find(widget)) ) {
	*spot = data;
    } else {
	if (fb_tbl_last >= fb_tbl_alloc) {
	    /* Make bigger */
	    fb_tbl_alloc *= 2;
	    fb_tbl = (fb_tbl_type *)
	      XtRealloc((char *)fb_tbl, fb_tbl_alloc*sizeof(fb_tbl_type));
	}
	fb_tbl[fb_tbl_last].w = widget;
	fb_tbl[fb_tbl_last].data = data;
	fb_tbl_last++;
    }
}


static caddr_t fb_unassoc(widget)
Widget widget;
/*
 * Returns the data associated with the widget and breaks
 * the association link.
 */
{
    caddr_t rtn, *val;

    val = fb_find(widget);
    rtn = *val;
    *val = (caddr_t) 0;
    return rtn;
}
