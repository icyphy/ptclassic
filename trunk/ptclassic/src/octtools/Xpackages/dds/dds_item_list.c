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
 * Dialog Definition system
 * Item lists
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 */

#include "dds_internal.h"

#include <X11/Xaw/Label.h>
#include "Maxport.h"
#include "Table.h"
#include "Tgl.h"

#define PORT_CLASS	maxportWidgetClass
#define PORT_HEIGHT	XtNmaxHeight

static ddsHandle new_item_list();
static void get_item_list();
static void set_item_list();
static void del_item_list();
static Widget parent_item_list();
static void place_item_list();

ddsClass _dds_item_list = {
    DDS_ITEM_LIST, new_item_list, get_item_list, set_item_list, del_item_list,
    parent_item_list, place_item_list
};

static ddsFlagItem *copy_items();
static int build_body();
static void do_selected();
static void item_list_callback();
static int update_item_list();



static ddsHandle new_item_list(parent, class, data)
ddsInternal *parent;		/* Should be DDS_TOP or DDS_COMPOSITE */
ddsClass *class;		/* Should be DDS_EDIT_TEXT            */
ddsData data;			/* Should be (ddsEditText *)          */
/*
 * Makes a new item list component.  This consists of a viewport widget
 * that contains a table widget.  The table widget contains check-box/label 
 * pairs.  The viewport size is set by estimating the size of the items
 * in the component.
 */
{
    ddsFullItemList *item;
    ddsInternal *same;
    int item_height;
    Widget pw;

    if (parent) {
	item = ALLOC(ddsFullItemList, 1);
	same = (ddsInternal *) item;
	item->base.class = class;
	item->base.parent = parent;
	item->user_spec = *((ddsItemList *) data);
	item->user_spec.items = copy_items(item->user_spec.num_items,
					   item->user_spec.items);
	pw = (*parent->base.class->_dds_parent_widget)(parent, same);
	if (item->user_spec.scroll_p == DDS_TRUE) {
	    item->optVport =
	      XtVaCreateManagedWidget(DDS_ILVPORT_NAME, PORT_CLASS, pw,
				      XtNallowVert, True,
				      XtNforceBars, True,
				      NULL);
	    (*parent->base.class->_dds_place)(parent, same, 1,
					      &item->optVport,
					      item->user_spec.max_items);
	    item->bodyTable =
	      XtVaCreateManagedWidget(DDS_ILTBL_NAME, tableWidgetClass,
				      item->optVport, NULL);
	} else {
	    item->optVport = (Widget) 0;
	    item->bodyTable =
	      XtVaCreateManagedWidget(DDS_ILTBL_NAME, tableWidgetClass, pw,
				      NULL);
	    (*parent->base.class->_dds_place)(parent, same, 1,
					      &item->bodyTable,
					      item->user_spec.max_items);
	}
	dds_add_cursor_widget(item->bodyTable, XtNcursor);
	XtAddCallback(item->bodyTable, XtNdestroyCallback,
		      _dds_destroy_callback, (XtPointer) item);
	item_height = build_body(item);
	if (item->optVport &&
	    (item->user_spec.num_items > item->user_spec.max_items)) {
	    int full_height = item_height * item->user_spec.max_items;

	    XtVaSetValues(item->optVport, PORT_HEIGHT, full_height, NULL);
	}
	do_selected(item, (ddsItemList *) 0);
	return (ddsHandle) item;
    } else {
	errRaise(ddsPackageName, DDS_BAD_PARENT,
		 "Items of type DDS_ITEM_LIST must have a parent");
	/*NOTREACHED*/
    }
    return (ddsHandle)NULL;
}



static ddsFlagItem *copy_items(nm, items)
int nm;				/* Number of items */
ddsFlagItem items[];		/* Items to copy   */
/*
 * Copies the array of flag item structures including all strings.
 */
{
    ddsFlagItem *result = ALLOC(ddsFlagItem, nm);
    int i;

    for (i = 0;  i < nm;  i++) {
	result[i] = items[i];
	result[i].label = util_strsav(items[i].label);
    }
    return result;
}

static void dump_items(nm, items)
int nm;				/* Number of items  */
ddsFlagItem *items;		/* Items to destroy */
/* 
 * Frees allocated storage associated with `items'.
 */
{
    int i;

    for (i = 0;  i < nm;  i++) {
	FREE(items[i].label);
    }
    FREE(items);
}



static int build_body(item)
ddsFullItemList *item;
/*
 * Builds the internal portion of an item list.  This includes
 * the check boxes and the labels.  It formats these in the
 * table in vertical columns.  If scrolling is on, it creates
 * only one long column that will be put in a viewport for
 * scrolling.
 */
{
    int i;
    char *name;
    Arg arg_list[MAX_ARGS];
    int arg_len;
    int tot_height = 0, height;
    int cur_row, cur_col;
    int items_per_col;

    item->checkBoxes = ALLOC(Widget, item->user_spec.num_items);
    item->checkLabels = ALLOC(Widget, item->user_spec.num_items);
    arg_len = 0;
    if (item->user_spec.exclusive_p == DDS_TRUE) {
	name = DDS_ILEXBOX_NAME;
	item->leader = XtVaCreateWidget(name, tglWidgetClass,
					item->bodyTable, NULL);
    } else {
	name = DDS_ILMULBOX_NAME;
	item->leader = (Widget) 0;
    }
    items_per_col = item->user_spec.max_items;
    cur_row = cur_col = 0;
    for (i = 0;  i < item->user_spec.num_items;  i++) {
	arg_len = 0;
	if (item->user_spec.exclusive_p == DDS_TRUE) {
	    XtSetArg(arg_list[arg_len], XtNleader, item->leader);  arg_len++;
	}
	if (item->user_spec.items[i].selected_p == DDS_TRUE) {
	    XtSetArg(arg_list[arg_len], XtNstate, True);  arg_len++;
	}
	XtSetArg(arg_list[arg_len], XtNjustify, XtJustifyRight);  arg_len++;
	item->checkBoxes[i] =
	  XtCreateManagedWidget(name, tglWidgetClass, item->bodyTable,
				arg_list, arg_len);
	XtAddCallback(item->checkBoxes[i], XtNcallback, item_list_callback,
		      (XtPointer) item);
	XtTblConfig(item->checkBoxes[i], cur_col*2, cur_row,
		    1, 1, TBL_SM_WIDTH);
	item->checkLabels[i] =
	  XtVaCreateManagedWidget(DDS_ILEXLBL_NAME, labelWidgetClass,
				  item->bodyTable,
				  XtNjustify, XtJustifyLeft,
				  XtNlabel, item->user_spec.items[i].label,
				  NULL);
	XtTblPosition(item->checkLabels[i], cur_col*2+1, cur_row);
	cur_row++;
	if ((item->user_spec.scroll_p == DDS_FALSE) &&
	    (cur_row >= items_per_col)) {
	    cur_row = 0;
	    cur_col++;
	}
	height = 0;
	XtVaGetValues(item->checkBoxes[i], XtNheight, &height, NULL);
	if (height > tot_height) tot_height = height;
	XtVaGetValues(item->checkLabels[i], XtNheight, &height, NULL);
	if (height > tot_height) tot_height = height;
    }
    return tot_height;
}



static void get_item_list(item, data)
ddsInternal *item;		/* DDS_ITEM_LIST   */
ddsData data;			/* (ddsItemList *) */
/*
 * Copies out the current state of the item list.  The list
 * of items and the strings in these items are locally owned
 * and should be copied by the user before modification.
 */
{
    ddsItemList *actual = (ddsItemList *) data;

    *actual = item->item_list.user_spec;
}



static void set_item_list(item, data)
ddsInternal *item;		/* DDS_ITEM_LIST   */
ddsData data;			/* (ddsItemList *) */
/*
 * Changes the state of the specified item list.  The caller
 * can change all aspects of the item except exclusive_p and
 * scroll_p.  These can only be set at initialization.
 */
{
    ddsItemList *actual = (ddsItemList *) data;
    int new_height;

    item->item_list.user_spec.max_items = actual->max_items;
    new_height = update_item_list(&(item->item_list), actual);
    if (item->item_list.optVport &&
	(item->item_list.user_spec.num_items >
	 item->item_list.user_spec.max_items)) {
	XtVaSetValues(item->item_list.optVport, XtNheight,
		      new_height * item->item_list.user_spec.max_items,
		      NULL);
    }
    do_selected(&(item->item_list), actual);
    item->item_list.user_spec.callback = actual->callback;
    item->item_list.user_spec.user_data = actual->user_data;
}



static int update_item_list(current, new)
ddsFullItemList *current;	/* Current state */
ddsItemList *new;		/* New state     */
/*
 * This routine steps through the new item list and makes the
 * necessary changes to the check boxes and labels.  It will
 * reuse old check boxes and labels.
 */
{
    int old_num;
    WidgetList old_boxes, old_labels;
    int i;
    char *name;
    ddsFlagItem *old_items;
    Arg arg_list[MAX_ARGS];
    int arg_len;
    int tot_height = 0, height;
    int cur_row, cur_col;
    int items_per_col;

    old_num = current->user_spec.num_items;
    old_items = current->user_spec.items;
    old_boxes = current->checkBoxes;
    old_labels = current->checkLabels;
    if (current->user_spec.exclusive_p == DDS_TRUE) {
	name = DDS_ILEXBOX_NAME;
    } else {
	name = DDS_ILMULBOX_NAME;
    }

    current->user_spec.num_items = new->num_items;
    current->checkBoxes = ALLOC(Widget, current->user_spec.num_items);
    current->checkLabels = ALLOC(Widget, current->user_spec.num_items);
    current->user_spec.items = copy_items(new->num_items, new->items);
    dump_items(old_num, old_items);
    cur_row = cur_col = 0;
    items_per_col = new->max_items;
    for (i = 0;  i < new->num_items;  i++) {
	arg_len = 0;
	if (current->user_spec.items[i].selected_p == DDS_TRUE) {
	    XtSetArg(arg_list[arg_len], XtNstate, True);  arg_len++;
	} else {
	    XtSetArg(arg_list[arg_len], XtNstate, False);  arg_len++;
	}
	if (i < old_num) {
	    /* Use old one */
	    current->checkBoxes[i] = old_boxes[i];
	    XtSetValues(current->checkBoxes[i], arg_list, arg_len);
	} else {
	    /* Make new one */
	    if (current->user_spec.exclusive_p == DDS_TRUE) {
		XtSetArg(arg_list[arg_len], XtNleader, current->leader);
		arg_len++;
	    }
	    XtSetArg(arg_list[arg_len], XtNjustify, XtJustifyRight);  arg_len++;
	    current->checkBoxes[i] =
	      XtCreateManagedWidget(name, tglWidgetClass, current->bodyTable,
				    arg_list, arg_len);
	    XtAddCallback(current->checkBoxes[i], XtNcallback,
			  item_list_callback, (XtPointer) current);
	    XtTblConfig(current->checkBoxes[i], cur_col*2, cur_row,
			1, 1, TBL_SM_WIDTH);
	}

	arg_len = 0;
	XtSetArg(arg_list[arg_len], XtNlabel, current->user_spec.items[i].label);
	arg_len++;
	if (i < old_num) {
	    /* Use old one */
	    current->checkLabels[i] = old_labels[i];
	    XtSetValues(current->checkLabels[i], arg_list, arg_len);
	} else {
	    /* Make new one */
	    XtSetArg(arg_list[arg_len], XtNjustify, XtJustifyLeft);
	    arg_len++;
	    current->checkLabels[i] =
	      XtCreateManagedWidget(DDS_ILEXLBL_NAME, labelWidgetClass,
				    current->bodyTable, arg_list, arg_len);
	    XtTblPosition(current->checkLabels[i], cur_col*2+1, cur_row);
	}
	cur_row++;
	if ((current->user_spec.scroll_p == DDS_FALSE) &&
	    (cur_row >= items_per_col)) {
	    cur_row = 0;
	    cur_col++;
	}
	height = 0;
	XtVaGetValues(current->checkBoxes[i], XtNheight, &height, NULL);
	if (height > tot_height) tot_height = height;
	XtVaGetValues(current->checkLabels[i], XtNheight, &height, NULL);
	if (height > tot_height) tot_height = height;
    }
    for (i = new->num_items;  i < old_num;  i++) {
	XtDestroyWidget(old_boxes[i]);
	XtDestroyWidget(old_labels[i]);
    }
    FREE(old_boxes);
    FREE(old_labels);
    return tot_height;
}



static void do_selected(current, new)
ddsFullItemList *current;	/* Current state */
ddsItemList *new;		/* New state     */
/*
 * This routine handles the update to the selected field.  This
 * field only makes sense if it is an exclusive item list.  If
 * so, it selects the indexed number.
 */
{
    if (current->user_spec.exclusive_p == DDS_TRUE) {
	if (new) {
	    if (new->selected != current->user_spec.selected) {
		if ((new->selected >= 0) &&
		    (new->selected < current->user_spec.num_items)) {
		    XtVaSetValues(current->checkBoxes[new->selected],
				  XtNstate, True, NULL);
		} else {
		    XtVaSetValues(current->checkBoxes[current->user_spec.selected],
				  XtNstate, False, NULL);
		}
	    }
	} else {
	    if ((current->user_spec.selected >= 0) &&
		(current->user_spec.selected < current->user_spec.num_items)) {
		XtVaSetValues(current->checkBoxes[current->user_spec.selected],
			      XtNstate, True, NULL);
	    }
	}
    }
}



static void del_item_list(item)
ddsInternal *item;		/* DDS_ITEM_LIST */
/*
 * Releases the non-widget related data associated with an
 * item list.
 */
{
    dds_remove_cursor_widget(item->item_list.bodyTable, XtNcursor);
    dump_items(item->item_list.user_spec.num_items,
	       item->item_list.user_spec.items);
    FREE(item->item_list.checkBoxes);
    FREE(item->item_list.checkLabels);
    FREE(item);
}



/*ARGSUSED*/
static Widget parent_item_list(parent, child)
ddsInternal *parent;		/* DDS_ITEM_LIST */
ddsInternal *child;		/* New child     */
{
    errRaise(ddsPackageName, DDS_BAD_PARENT,
	     "DDS_ITEM_LIST components may not have child components");
    /*NOTREACHED*/
    return (Widget)NULL;
}

/*ARGSUSED*/
static void place_item_list(parent, child, nw, wlist, vspan)
ddsInternal *parent;		/* DDS_ITEM_LIST */
ddsInternal *child;		/* New child     */
int nw;				/* Number of widgets */
WidgetList wlist;		/* New children  */
int vspan;
{
    errRaise(ddsPackageName, DDS_BAD_PARENT,
	     "DDS_ITEM_LIST components may not have child components");
    /*NOTREACHED*/
}



static void item_list_callback(w, client, call)
Widget w;			/* Widget                      */
XtPointer client;		/* Client data (ddsInternal *) */
XtPointer call;			/* Call data (none)            */
/*
 * Updates the current state and calls user callback function.
 */
{
    ddsInternal *item = (ddsInternal *) client;
    Boolean selected = 0;
    int i;

    XtVaGetValues(w, XtNstate, &selected, NULL);
    for (i = 0;  i < item->item_list.user_spec.num_items;  i++) {
	if (w == item->item_list.checkBoxes[i]) break;
    }
    if (i < item->item_list.user_spec.num_items) {
	item->item_list.user_spec.items[i].selected_p =
	  ((selected == True) ? DDS_TRUE : DDS_FALSE);
	if (item->item_list.user_spec.exclusive_p == DDS_TRUE) {
	    item->item_list.user_spec.selected = i;
	}
	if (item->item_list.user_spec.callback) {
	    (*item->item_list.user_spec.callback)
	      ((ddsHandle) item, i);
	}
    }
}
