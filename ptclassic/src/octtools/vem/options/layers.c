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
 * VEM Window Options Editing
 *
 * David Harrison
 * University of California, Berkeley
 * 1989, 1990
 *
 * This file contains an implementation for the displayed layers editor.
 */

#include "general.h"		/* General VEM definitions  */
#include "commands.h"		/* Command definitions      */
#include "dds.h"		/* Dialog interface         */
#include "vemDM.h"		/* Compatibility            */
#include "st.h"			/* For interlocking dialogs */
#include "xvals.h"		/* Important X Values       */
#include "windows.h"		/* Windows module           */
#include "list.h"		/* For command processing   */
#include "message.h"		/* Console messages         */
#include "vemUtil.h"		/* General VEM utilities    */

#include "internal.h"		/* Local definitions        */

static st_table *all_layer_editors = (st_table *) 0;

/*
 * Forward Declarations
 */

static int spawn_layer_editor();
static char *gen_title();
static st_table *traceLayers();
static void trace_fct();

static void lyr_ok();
static void lyr_dismiss();
static void lyr_apply();
static void lyr_all();
static void lyr_clear();

static st_table *find_off_layers();
static int comp();
static int comp_layers();

typedef struct new_lyr_info_defn {
    ddsHandle top;
    st_table *off_layers;
    ddsFlagItem *items;
    int offset;
} new_lyr_info;

typedef struct cb_data_defn {
    Window win;			/* Window for options */
    ddsHandle lyr_list;		/* Layer list         */
    ddsHandle help;		/* Help dialog        */
} cb_data;




#define ND	(ddsData) 0

static ddsComposite lyr_info = { (char *) 0, DDS_VERTICAL, 1 };
static ddsItemList list_info = {
    0, (ddsFlagItem *) 0, DDS_FALSE, -1, DDS_TRUE, 10, (void (*)()) 0, (ddsData) 0
};
static ddsControl lyr_cons[] = {
    { "Ok\n(Ret)", "<Key>Return|<Key>F1", lyr_ok, ND },
    { "Dismiss\n(Del)", "<Key>Delete|<Key>F2", lyr_dismiss, ND },
    { "Apply", (char *) 0, lyr_apply, ND },
    { "All\n(F4)", "<Key>F4", lyr_all, ND },
    { "Clear\n(F5)", "<Key>F5", lyr_clear, ND }
};
#define NUM_CONS	sizeof(lyr_cons)/sizeof(ddsControl)

static char *lyr_help =
"This dialog is used for changing the visible layers associated with\
 a window.  All of the possible layers for the window are shown in the\
 dialog.  Those that are currently visible have buttons to the left\
 of the layer name that are dark.  Click the left mouse button inside\
 the button for a layer to toggle the visibility of the layer.  No change\
 will occur until one of the control buttons at the bottom of the dialog\
 is pressed.  Clicking in \"OK\" will save the visibility state of the\
 layers and close the layer editor window.  Clicking in \"Dismiss\" will\
 close the options editor but not make any changes to the visible layers.\
 Clicking in \"Apply\" will cause updates to layer visibility but will not\
 close the options editor.  The buttons \"All\" and \"Clear\" turn on or off\
 all of the layers in the dialog.  They do not affect the display of layers\
 in the window (use \"Apply\" or \"Ok\" to update the graphics window).";

static ddsPosition lyr_pos = {
    DDS_WINDOW, DDS_ABOVE, DDS_RIGHT, 0, 0, (Window) 0
};


/*ARGSUSED*/
extern vemStatus optLECmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* Argument list            */
/*
 * This routine posts an unmoded dialog for editing the visible
 * layers on the window given in `spot'.  Only one editor can
 * be running on a window at a time.  This precondition is
 * enforced.
 */
{
    if (!all_layer_editors) {
	all_layer_editors = st_init_table(st_numcmp, st_numhash);
    }
    if (!st_is_member(all_layer_editors, (char *) spot->theWin)) {
	if (spawn_layer_editor(spot->theWin)) {
	    (void) st_insert(all_layer_editors, (char *) spot->theWin, (char *) 0);
	} else {
	    vemMsg(MSG_C, "Cannot create editor for specified window.\n");
	    return VEM_ARGRESP;
	}
    } else {
	vemMsg(MSG_C, "Editor already running for specified window.\n");
	return VEM_ARGRESP;
    }
    return VEM_ARGMOD;
}


static enum st_retval new_lyr(key, value, arg)
char *key, *value, *arg;
/*
 * Called to create a new layer entry.
 */
{
    new_lyr_info *info = (new_lyr_info *) arg;

    info->items[info->offset].label = key;
    if (info->off_layers && st_is_member(info->off_layers, key)) {
	info->items[info->offset].selected_p = DDS_FALSE;
    } else {
	info->items[info->offset].selected_p = DDS_TRUE;
    }
    info->items[info->offset].user_data = (ddsData) 0;
    info->offset += 1;
    return ST_CONTINUE;
}

static int spawn_layer_editor(win)
Window win;			/* Window for spawning */
/*
 * Spawns a new layer editor for `win'.  A new description is built
 * dynamically.  The dialog consists of a multi-which for selecting
 * the layers that are to be displayed.
 */
{
    ddsHandle lh, cbtn;
    wnOpts opts;
    octObject facet;
    st_table *all_layers;
    new_lyr_info li;
    cb_data *cb;
    int i;

    if (wnGetInfo(win, &opts, &(facet.objectId), (octId *) 0,
		  (bdTable *) 0, (struct octBox *) 0) == VEM_OK) {
	all_layers = traceLayers(win);
	if (all_layers) {
	    lyr_info.title = gen_title(&facet);
	    lh = dds_component(0, DDS_TOP, (ddsData) &lyr_info);
	    list_info.num_items = st_count(all_layers);
	    list_info.items = VEMARRAYALLOC(ddsFlagItem, list_info.num_items);
	    li.top = lh;
	    li.off_layers = opts.off_layers;
	    li.items = list_info.items;
	    li.offset = 0;
	    st_foreach(traceLayers(win), new_lyr, (char *) &li);
	    cb = VEMALLOC(cb_data);
	    cb->win = win;
	    cb->lyr_list = dds_component(lh, DDS_ITEM_LIST, (ddsData) &list_info);
	    for (i = 0;  i < NUM_CONS;  i++) {
		lyr_cons[i].user_data = (ddsData) cb;
		cbtn = dds_component(lh, DDS_CONTROL, (ddsData) (lyr_cons+i));
	    }
	    cb->help = dmHelpButton(lh, lyr_help);
	    lyr_pos.win = win;
	    dds_post(lh, &lyr_pos, (ddsEvtHandler *) 0);
	    return 1;
	} else {
	    vemMsg(MSG_C, "There are no layers in this window\n");
	    return 0;
	}
    } else {
	return 0;
    }
}



static char *gen_title(fct)
octObject *fct;			/* Primary facet for editor */
/*
 * Allocates and constructs appropriate title for layer editing
 * dialog.  The string is allocated and should be freed when
 * no longer needed.
 */
{
    return optTitle(fct, "Layer editor for ");
}

static st_table *traceLayers(win)
Window win;			/* Target window           */
/*
 * This routine traces the heirarchy from the buffer attached
 * to `win' and builds an st_table containing all of the
 * layers encountered.
 */
{
    st_table *the_table, *master_table;
    octObject theFacet;
    wnOpts theOpts;

    VEM_CKRVAL(wnGetInfo(win, &theOpts, &(theFacet.objectId),
			 (octId *) 0, (bdTable *) 0,
			 (struct octBox *) 0) == VEM_OK,
	       "Can't find window", (st_table *) 0);
    VEM_OCTCKRVAL(octGetById(&theFacet), (st_table *) 0);
    the_table = st_init_table(strcmp, st_strhash);
    master_table = st_init_table(st_numcmp, st_numhash);
    if (theOpts.disp_options & VEM_EXPAND) {
	trace_fct(&theFacet, the_table, master_table, "contents");
    } else {
	trace_fct(&theFacet, the_table, master_table, theOpts.interface);
    }
    st_free_table(master_table);
    if (st_count(the_table) <= 0) {
	st_free_table(the_table);
	return (st_table *) 0;
    } else return the_table;
}

static void trace_fct(fct, tbl, m_tbl, ifct)
octObject *fct;			/* Facet to trace            */
st_table *tbl;			/* Hash table of layer names */
st_table *m_tbl;		/* Master table              */
STR ifct;			/* Interface name            */
/*
 * This routine generates all layers and instances in `fct'
 * and adds the layer names into `tbl' while recursively
 * searching the unique masters of the instances found in `fct'.
 */
{
    octGenerator octGen;
    octObject obj, master;

    VEM_OCTCKRET(octInitGenContents(fct, OCT_LAYER_MASK|OCT_INSTANCE_MASK,
				    &octGen));
    while (octGenerate(&octGen, &obj) == OCT_OK) {
	if (obj.type == OCT_LAYER) {
	    /* Add to overall table */
	    st_insert(tbl, obj.contents.layer.name, (char *) 0);
	} else {
	    /* Check master against table of masters */
	    master.type = OCT_FACET;
	    master.contents.facet.facet = ifct;
	    master.contents.facet.mode = "r";
	    master.contents.facet.version = OCT_CURRENT_VERSION;
	    if (octOpenMaster(&obj, &master) >= OCT_OK) {
		if (!st_insert(m_tbl, (char *) master.objectId,
			       (char *) 0))
		  {
		      /* Must check this master */
		      trace_fct(&master, tbl, m_tbl, ifct);
		  }
	    }
	}
    }
}



static int comp(source, dest)
st_table *source, *dest;	/* Tables for comparison */
/*
 * Iterates through `source' and makes sure all items are in
 * `dest'.  If any aren't,  it stops and returns a non-zero
 * value.
 */
{
    st_generator *gen;
    char *name;

    gen = st_init_gen(source);
    while (st_gen(gen, &name, (char **) 0)) {
	if (!st_is_member(dest, name)) {
	    st_free_gen(gen);
	    return 1;
	}
    }
    st_free_gen(gen);
    return 0;
}

static int comp_layers(first, second)
st_table *first, *second;	/* Tables for comparison */
/*
 * This routine compares the two tables for equality.  If
 * they are the same,  it returns zero.  If they differ,
 * it returns one.
 */
{
    if (!first) {
	return !((second == (st_table *) 0) || (st_count(second) == 0));
    } else if (!second) {
	return !((first == (st_table *) 0) || (st_count(first) == 0));
    } else {
	return comp(first, second) || comp(second, first);
    }
}



static st_table *find_off_layers(item_data)
ddsItemList *item_data;
/*
 * Builds a hash table of all layers marked as `off' in `item_data'.
 */
{
    st_table *off_layers = (st_table *) 0;
    int i;

    for (i = 0;  i < item_data->num_items;  i++) {
	if (item_data->items[i].selected_p == DDS_FALSE) {
	    if (!off_layers) off_layers = st_init_table(strcmp, st_strhash);
	    (void) st_insert(off_layers,
			     VEMSTRCOPY(item_data->items[i].label),
			     (char *) 0);
	}
    }
    return off_layers;
}

static enum st_retval can_lyr(key, value, arg)
char *key, *value, *arg;
{
    VEMFREE(key);
    return ST_CONTINUE;
}

static void zap_layers(lyrs)
st_table *lyrs;
/*
 * Frees up `lyrs'.
 */
{
    if (lyrs) {
	st_foreach(lyrs, can_lyr, (char *) 0);
	st_free_table(lyrs);
    }
}


static void lyr_apply(item)
ddsHandle item;			/* Control button */
/*
 * This routine causes an update to the displayed layers.
 */
{
    ddsControl data;
    ddsItemList item_data;
    cb_data *cb;
    wnOpts opts;

    dds_get(item, (ddsData) &data);
    cb = (cb_data *) data.user_data;
    dds_get(cb->lyr_list, (ddsData) &item_data);
    if (wnGetInfo(cb->win, &opts, (octId *) 0, (octId *) 0,
		  (bdTable *) 0, (struct octBox *) 0) == VEM_OK) {
	/* Small core leak here -- old table not freed */
	opts.off_layers = find_off_layers(&item_data);
	(void) wnSetInfo(cb->win, &opts, (bdTable *) 0);
    }
}


static void lyr_dismiss(item)
ddsHandle item;			/* Control button */
/*
 * Destroys both the main dialog and any help dialog associated
 * with `item'.  It first checks to make sure the user hasn't
 * made any changes and asks for confirmation if there are
 * changes.
 */
{
    ddsControl data;
    ddsItemList item_data;
    st_table *off_layers;
    wnOpts opts;
    cb_data *cb;
    int proceed = 1;
    static char *warn_str =
"You have made changes to the visibility\n\
 of some layers.\n\nDo you really want to\n\
 exit without saving them?";

    dds_get(item, (ddsData) &data);
    cb = (cb_data *) data.user_data;
    dds_get(cb->lyr_list, (ddsData) &item_data);
    off_layers = find_off_layers(&item_data);
    if (wnGetInfo(cb->win, &opts, (octId *) 0, (octId *) 0,
		  (bdTable *) 0, (struct octBox *) 0) == VEM_OK) {
	if (comp_layers(opts.off_layers, off_layers)) {
	    if (dmConfirm("Warning", warn_str, "Yes", "No") != VEM_OK) {
		proceed = 0;
	    }
	}
	if (proceed) {
	    Window win;
	    char *value;

	    win = cb->win;
	    (void) st_delete(all_layer_editors, (char **) &win,
			     (char **) &value);
	    zap_layers(off_layers);
	    dds_destroy(cb->help);
	    dds_destroy(item);
	    VEMFREE(cb);
	}
    } else {
	/* Should be a warning of some kind here */
    }
}



static void lyr_ok(item)
ddsHandle item;			/* Control button */
/*
 * Called when user activates the "Ok" button.  Essentially,
 * this is `apply' followed by `dismiss'.
 */
{
    lyr_apply(item);
    lyr_dismiss(item);
}



static void lyr_set_all(item, value)
ddsHandle item;			/* Control button */
ddsBoolean value;		/* Value to set   */
/*
 * Turns on or off all of the layers in the layer display editor
 * that contains the control button `item' based on the value
 * of `value'.
 */
{
    ddsControl data;
    ddsItemList item_data;
    cb_data *cb;
    int i;

    dds_get(item, (ddsData) &data);
    cb = (cb_data *) data.user_data;
    dds_get(cb->lyr_list, (ddsData) &item_data);
    for (i = 0;  i < item_data.num_items;  i++) {
	item_data.items[i].selected_p = value;
    }
    dds_set(cb->lyr_list, (ddsData) &item_data);
}

static void lyr_all(item)
ddsHandle item;
/*
 * Turns on all items in the layer display editor whose "All" control button
 * is `item'.
 */
{
    lyr_set_all(item, DDS_TRUE);
}

static void lyr_clear(item)
ddsHandle item;			/* Control button */
/*
 * Turns off all of the layers in the layer display editor whose "Clear"
 * button is `item'.
 */
{
    lyr_set_all(item, DDS_FALSE);
}
