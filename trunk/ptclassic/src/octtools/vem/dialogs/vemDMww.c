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
 * VEM Dialog Manager Interface Routines
 *
 * David Harrison
 * University of California,  Berkeley
 * 1986, 1989, 1990
 *
 * This file contains easy to use programming interfaces for common
 * dialog operations.  These are usually short moded dialogs which
 * ask questions that require an answer immediately.  The prefix of
 * these routines is "dm".
 *
 * This package is now implemented on top of the more powerful
 * Xt toolkit based package "dds".  It is provided for
 * compatibility.  The library must be initialized using
 * dmCompatInit().
 *
 * This file contains routines implementing dmWhichOne and dmMultiWhich.
 */

#include "vemDMint.h"

typedef struct item_callback_defn {
    int *flag_ptr;		/* Pointer to flag variable */
    Pointer user_data;		/* Item specific data       */
} item_callback;

typedef struct mulw_cb_data_defn {
    int (*item_func)();		/* Callback   */
} mulw_cb_data;


static void mulw_bcb(item_list, toggled)
ddsHandle item_list;
int toggled;
/* 
 * Called when a button is pushed - calls user callback (if any).
 * Also sets flag appropriately
 */
{
    mulw_cb_data *data;
    ddsItemList idata;
    item_callback *cb;

    dds_get(item_list, (ddsData) &idata);
    cb = (item_callback *) (idata.items[toggled].user_data);
    *(cb->flag_ptr) = idata.items[toggled].selected_p == DDS_TRUE;
    data = (mulw_cb_data *) idata.user_data;
    if (data->item_func) {
	(void) (*data->item_func)(toggled, *(cb->flag_ptr), cb->user_data);
    }
}



static void mulw_base(item, value)
ddsHandle item;
ddsBoolean value;
/*
 * This routine either sets or clears all of the items of a dmMultiWhich
 * based on `value'.
 */
{
    ddsControl data;
    ddsHandle item_list;
    ddsItemList item_data;
    mulw_cb_data *cb_data;
    item_callback *item_cb;
    ddsBoolean opposite;
    int i;

    if (value == DDS_TRUE) opposite = DDS_FALSE;
    else opposite = DDS_TRUE;
    dds_get(item, (ddsData) &data);
    item_list = (ddsHandle) data.user_data;
    dds_get(item_list, (ddsData) &item_data);
    cb_data = (mulw_cb_data *) item_data.user_data;
    for (i = 0;  i < item_data.num_items;  i++) {
	item_cb = (item_callback *) item_data.items[i].user_data;
	if (item_data.items[i].selected_p == opposite) {
	    item_data.items[i].selected_p = value;
	    *(item_cb->flag_ptr) = (value == DDS_TRUE);
	    if (cb_data->item_func) {
		(void) (*cb_data->item_func)(i, (*item_cb->flag_ptr),
					     item_cb->user_data);
	    }
	}
    }
    dds_set(item_list, (ddsData) &item_data);
}

static void mulw_all(item)
ddsHandle item;
/*
 * Called when the "All" button is pressed in a dmMultiWhich dialog.
 * Causes all of the items to become selected.
 */
{
    mulw_base(item, DDS_TRUE);
}

static void mulw_clear(item)
ddsHandle item;
/*
 * Called when the "Clear" button is pressed in a dmMultiWhich dialog.
 * Causes all of the items to become unselected.
 */
{
    mulw_base(item, DDS_FALSE);
}



static ddsHandle item_body(parent, item_data, item_cb, count, items, sel, excl_p, cb, cb_data)
ddsHandle parent;		/* Parent for component      */
ddsItemList *item_data;		/* Structure to use for info */
item_callback **item_cb;	/* Per item callback info  */
int count;			/* Number of items in body */
dmWhichItem *items;		/* Array of items          */
int sel;			/* Selected item           */
ddsBoolean excl_p;		/* Whether exclusive       */
void (*cb)();			/* Callback function       */
ddsData cb_data;		/* Callback data           */
/*
 * Builds a item list component for both dmMultiWhich and dmWhichOne.
 */
{
    int i;

    item_data->num_items = count;
    item_data->items = VEMARRAYALLOC(ddsFlagItem, count);
    item_data->exclusive_p = excl_p;
    item_data->selected = (sel>=0&&sel<count)? sel: 0 ;	/* If sel is garbage, default to item 0. */
    item_data->scroll_p = (count > SCROLL_THRESHOLD ? DDS_TRUE : DDS_FALSE);
    item_data->max_items = SCROLL_THRESHOLD;
    item_data->callback = cb;

    *item_cb = VEMARRAYALLOC(item_callback, count);
    for (i = 0;  i < count;  i++) {
	item_data->items[i].label = items[i].itemName;
	if (excl_p) {
	    item_data->items[i].selected_p = DDS_FALSE;
	} else {
	    item_data->items[i].selected_p =
	      (items[i].flag ? DDS_TRUE : DDS_FALSE);
	}
	(*item_cb)[i].flag_ptr = &(items[i].flag);
	(*item_cb)[i].user_data = items[i].userData;
	item_data->items[i].user_data = (ddsData) ((*item_cb)+i);
    }
    item_data->user_data = cb_data;
    return dds_component(parent, DDS_ITEM_LIST, (ddsData) item_data);
}



vemStatus dmMultiWhich(title, count, items, item_func, help_buf)
STR title;			/* Question at top of dialog  */
int count;			/* Number of items            */
dmWhichItem *items;		/* List of items              */
int (*item_func)();		/* Function when item pressed */
STR help_buf;			/* Help text (if any)         */
/*
 * This routine presents a moded dialog which asks the user
 * to select zero or more items from the list provided.  The
 * dialog consists of a title,  a list of strings with selection
 * buttons,  and three control buttons: "ok", "cancel", and "help".
 * In all cases,  the routine marks the selected items in the returned
 * array using the 'flag' field.  If the user selects "ok",  the
 * routine returns VEM_OK.  If he selects "cancel",  the routine
 * returns VEM_NOSELECT.  `item_func' is a function which
 * is called by dmMultiWhich whenever a button changes state.
 * It should be declared:
 *    int item_func(item_index, item_value, item_data)
 *    int item_index;
 *    int item_value;
 *    Pointer item_data;
 * 'item_index' is the index of the entry which changes state.
 * 'item_value' is the boolean value of the button (0 is off, 1 is on).
 * 'item_data' is the data associated with the item in 'items'.
 * If the user selects 'all',  'item_func' will be called for each
 * button that is off. If the routine returns 0,  the dialog will 
 * terminate even if the "ok" and "abort" buttons have not been touched.
 * If the user selects "help",  a moded dialog containing `help_buf'
 * will be posted.  This message can be multi-line.
 */
{
    ddsHandle top, item_list, con;
    static con_callback cb_info;
    static ddsComposite top_data;
    static ddsItemList item_data;
    static ddsControl controls[] = {
	{ "Ok\n(Ret)", "<Key>Return|<Key>F1", _vemdm_general_ok,
	    (ddsData) &cb_info },
	{ "Cancel\n(Del)", "<Key>Delete|<Key>F2", _vemdm_general_cancel,
	    (ddsData) &cb_info },
	{ "All\n(F4)", "<Key>F4", mulw_all, (ddsData) 0 },
	{ "Clear\n(F5)", "<Key>F5", mulw_clear, (ddsData) 0 }
    };
    ddsPosition pos;
    static int num_cons = sizeof(controls)/sizeof(ddsControl);
    mulw_cb_data data;
    item_callback *item_cb;
    int i;

    if (!_vemdm_initialized) {
	errRaise(vemdm_pkg_name, 0,
		  "Package must be initialized with call to dmCompatInit");
	/*NOTREACHED*/
    }
    top_data.title = title;
    top_data.just = DDS_VERTICAL;
    top_data.max_items = 1;
    top = dds_component((ddsHandle) 0, DDS_TOP, (ddsData) &top_data);
    data.item_func = item_func;
    item_list = item_body(top, &item_data, &item_cb, count, items, -1, DDS_FALSE,
			  mulw_bcb, (ddsData) &data);
    controls[2].user_data = (ddsData) item_list;
    controls[3].user_data = (ddsData) item_list;
    for (i = 0;  i < num_cons;  i++) {
	con = dds_component(top, DDS_CONTROL, (ddsData) (controls+i));
    }
    if (help_buf) {
	cb_info.help = dmHelpButton(top, help_buf);
    } else {
	cb_info.help = (ddsHandle) 0;
    }
    pos.pos = DDS_MOUSE;  pos.vjust = DDS_MIDDLE;  pos.hjust = DDS_CENTER;
    dds_post(top, &pos, &_dmCompatHandler);
    VEMFREE(item_data.items);
    VEMFREE(item_cb);
    return cb_info.status;
}


typedef struct ww_cb_data_defn {
    int (*item_func)();		/* Callback                         */
    int *sel;			/* Index to currently selected item */
} ww_cb_data;

static void ww_bcb(item_list, toggled)
ddsHandle item_list;
int toggled;
/* 
 * Called when a button is pushed - calls user callback (if any).
 * Also sets flag appropriately
 */
{
    ww_cb_data *data;
    ddsItemList idata;
    item_callback *cb;

    dds_get(item_list, (ddsData) &idata);
    cb = (item_callback *) (idata.items[toggled].user_data);
    *(cb->flag_ptr) = idata.items[toggled].selected_p == DDS_TRUE;
    data = (ww_cb_data *) idata.user_data;
    if (*(cb->flag_ptr)) {
	*(data->sel) = toggled;
	if (data->item_func) {
	    (void) (*data->item_func)(toggled, cb->user_data);
	}
    } else {
	*(data->sel) = -1;
    }
}



/*ARGSUSED*/
vemStatus dmWhichOne(title, count, items, itemSelect, item_func, help_buf)
STR title;			/* Question at top of dialog     */
int count;			/* Number of items               */
dmWhichItem *items;		/* List of item labels           */
int *itemSelect;		/* Which item was selected       */
int (*item_func)();		/* Item select function          */
STR help_buf;			/* Help text (if any)            */
/*
 * This routine presents a moded dialog which asks the user
 * to select exactly one item from the list provided.  The
 * dialog consists of a title,  "Ok", "Cancel", and "help" buttons,  
 * and a list of strings each with a blender control.  If a selection
 * is made (the "Ok" button is selected),  the routine will 
 * return VEM_OK and set 'item_select' to the appropriate number.  If 
 * the selection is aborted, the routine will return VEM_NOSELECT 
 * and item_select will be set to -1.  'item_func' is a function which
 * is called by dmWhichOne whenever a new blender is selected.  It
 * should be declared:
 *   int item_func(item_index, item_data)
 *   int item_index;
 *   Pointer item_data;
 * 'item_index' is the index of the entry which has been selected.
 * 'item_data' is the data passed initially in 'items'.
 * Since no record of the previous selection is kept,  the user may
 * want to notate this in some fashion.  If the routine returns 0,
 * the dialog will terminate even if the done button has not been pushed.
 * If `help_buf' is non-zero,  the help button will be activated.  If
 * selected,  a dialog will appear with `help_buf' in it (which may
 * be many lines).
 */
{
    ddsHandle top, item_list, con;
    static con_callback cb_info;
    static ddsComposite top_data;
    static ddsItemList item_data;
    static ddsControl controls[] = {
	{ "Ok\n(Ret)", "<Key>Return|<Key>F1", _vemdm_general_ok,
	    (ddsData) &cb_info },
	{ "Cancel\n(Del)", "<Key>Delete|<Key>F2", _vemdm_general_cancel,
	    (ddsData) &cb_info },
    };
    ddsPosition pos;
    static int num_cons = sizeof(controls)/sizeof(ddsControl);
    ww_cb_data data;
    item_callback *item_cb;
    int i;

    if (!_vemdm_initialized) {
	errRaise(vemdm_pkg_name, 0,
		  "Package must be initialized with call to dmCompatInit");
	/*NOTREACHED*/
    }
    top_data.title = title;
    top_data.just = DDS_VERTICAL;
    top_data.max_items = 1;
    top = dds_component((ddsHandle) 0, DDS_TOP, (ddsData) &top_data);
    data.item_func = item_func;
    data.sel = itemSelect;
    item_list = item_body(top, &item_data, &item_cb, count, items,
			  *itemSelect, DDS_TRUE, ww_bcb, (ddsData) &data);
    for (i = 0;  i < num_cons;  i++) {
	con = dds_component(top, DDS_CONTROL, (ddsData) (controls+i));
    }
    if (help_buf) {
	cb_info.help = dmHelpButton(top, help_buf);
    } else {
	cb_info.help = (ddsHandle) 0;
    }
    pos.pos = DDS_MOUSE;  pos.vjust = DDS_MIDDLE;  pos.hjust = DDS_CENTER;
    dds_post(top, &pos, &_dmCompatHandler);
    VEMFREE(item_data.items);
    VEMFREE(item_cb);
    /* Make sure something is selected */
    return ( *data.sel >= 0 ) ? cb_info.status : VEM_NOSELECT; /* AC May 3 1991 */
}


