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
 * Window Options Editing Dialog
 *
 * David Harrison
 * University of California, Berkeley
 * 1989
 *
 * This file contains an X11 replacement for the separate window options
 * dialogs constructed for the X10 version of VEM.  This implementation
 * uses the new dm interface to construct one dialog containing all
 * of the possible options.  The interface is still unmoded.
 */

#include "general.h"		/* General VEM definitions  */
#include "commands.h"		/* Command definitions      */
#include "vemDM.h"		/* Compatibility            */
#include "st.h"			/* For interlocking dialogs */
#include "xvals.h"		/* Important X Values       */
#include "windows.h"		/* Windows module           */
#include "list.h"		/* For command processing   */
#include "message.h"		/* Console messages         */
#include "vemUtil.h"		/* General VEM utilities    */

#include "internal.h"		/* Local definitions        */

#include "opts.h"		/* Some Public functions    */

static st_table *all_opt_editors = (st_table *) 0;

/*
 * For automatic handling of options structure.  Stolen
 * from the Xt Intrinsics.
 */
#define OFFSET(field) \
(((char *) (&(((wnOpts *)NULL)->field))) - ((char *) NULL))

/* Forward declarations */
static int spawn_opt_editor();
static char *gen_title();

static void ch_int();
#ifdef USE_CH_DBL
static void ch_dbl();
#endif
static void ch_flag();

static void opt_ok();
static void opt_dismiss();
static void opt_apply();

static int changes_p();


/*
 * Definitions describing window options dialog
 */

#define ND	(ddsData) 0

static ddsFlagItem window_options[] = {
    { "Expert mode", 		DDS_FALSE, 	(ddsData) VEM_EXPERT },
    { "Visible Grid", 		DDS_FALSE, 	(ddsData) VEM_GRIDLINES },
    { "Dotted Grid", 		DDS_FALSE, 	(ddsData) VEM_DOTGRID   },
    { "Manhattan Argument Entry", DDS_FALSE, 	(ddsData) VEM_MANLINE   },
    { "Argument Gravity", 	DDS_FALSE, 	(ddsData) VEM_GRAVITY   },
    { "Show Instance Bounding Boxes", DDS_FALSE, (ddsData) VEM_SHOWBB   },
    { "Show Actual Terminals", 	DDS_FALSE, 	(ddsData) VEM_SHOWAT    },
    { "Expand Instances", 	DDS_FALSE, 	(ddsData) VEM_EXPAND    },
    { "Visible Title Bar", 	DDS_FALSE, 	(ddsData) VEM_TITLEDISP }
};
#define NUM_WINOPTS	sizeof(window_options)/sizeof(ddsFlagItem)

static ddsNumber grid_nums[] = {
    { "Oct units per Lambda", 1.0, 100.0, 1.0, 20.0, "%3.0lf",
	ch_int, (ddsData) OFFSET(lambda) },
    { "Snap (oct units)", 1.0, 1000.0, 1.0, 20.0, "%4.0lf",
	ch_int, (ddsData) OFFSET(snap) },
    { "Major Grid Spacing (oct units)", 1.0, 1000.0, 1.0, 20.0, "%4.0lf",
	ch_int, (ddsData) OFFSET(grid_majorunits) },
    { "Minor Grid Spacing (oct units)", 1.0, 1000.0, 1.0, 20.0, "%4.0lf",
	ch_int, (ddsData) OFFSET(grid_minorunits) },
    { "Log Grid Base", 0.0, 10.0, 1.0, 0.0, "%2.0lf",
	ch_int, (ddsData) OFFSET(grid_base) },
    { "Log Grid Minimum Base", 0.0, 10.0, 1.0, 0.0, "%2.0lf",
	ch_int, (ddsData) OFFSET(grid_minbase) },
    { "Minimum Grid Difference", 0.0, 50.0, 1.0, 8.0, "%2.0lf",
	ch_int, (ddsData) OFFSET(grid_diff) }
};

#define NUM_GRID	sizeof(grid_nums)/sizeof(ddsNumber)    

static ddsNumber other_nums[] = {
    { "Solid Fill Threshold", 0.0, 5000.0, 1.0, 10.0, "%4.0lf",
	ch_int, (ddsData) OFFSET(solid_thres) },
#ifdef USE_CH_DBL
/* XXX RLS - seems to be a bug with double values - so for the time
   being, skip it
    { "Name Threshold", 0.0, 5.0, 0.1, 1.0, "%1.1lf",
	ch_dbl, (ddsData) OFFSET(bb_thres) },
 */
#endif
    { "Bounding Threshold", 0.0, 2000.0, 5.0, 10.0, "%4.0lf",
	ch_int, (ddsData) OFFSET(bb_min) },
    { "Abstraction Threshold", 0.0, 2000.0, 5.0, 10.0, "%4.0lf",
	ch_int, (ddsData) OFFSET(con_min) }
};
#define NUM_OTHER	sizeof(other_nums)/sizeof(ddsNumber)

static ddsEditText other_strs[] = {
    { "Interface Facet", "", 1, 20 }
};
static int str_offs[] = {
    OFFSET(interface)
};
#define NUM_STRS	sizeof(other_strs)/sizeof(ddsEditText)

typedef struct con_udata_defn {
    Window win;			/* Window           */
    wnOpts *opts;		/* Modified options */
    ddsHandle ets[NUM_STRS];	/* Edit text fields */
    ddsHandle help;		/* Help dialog      */
} con_udata;

static ddsControl opt_cons[] = {
    { "Ok\n(Ret)", "<Key>Return|<Key>F1", opt_ok, ND },
    { "Dismiss\n(Del)", "<Key>Delete|<Key>F2", opt_dismiss, ND },
    { "Apply", (char *) 0, opt_apply, ND }
};
#define NUM_CONS	sizeof(opt_cons)/sizeof(ddsControl)

static ddsComposite top_info = { (char *) 0, DDS_VERTICAL, 10 };

static ddsItemList list_info = {
    NUM_WINOPTS, window_options, DDS_FALSE, -1, DDS_FALSE, NUM_WINOPTS,
    ch_flag, ND
};

static ddsPosition opt_pos = {
    DDS_WINDOW, DDS_ABOVE, DDS_RIGHT, 0, 0, (Window) 0
};

static char *opt_help =
"This dialog is used for examining and modifying various options associated\
 with a window.  The options are classed into two areas: flag options and\
 grid and general options.  Flag options are those that can be turned\
 on and off.  Click in the button next to the item to change the state\
 of the corresponding option.  Grid and general options control the display\
 of the grid, instances, and bounding boxes of a window.  These parameters\
 are changed by editing the values in the text area next to each parameter.\
 The meaning of these parameters is described in the document \"Customizing\
 VEM\".";



/*ARGSUSED*/
extern vemStatus optWinCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* Argument list            */
/*
 * This routine posts an unmoded dialog for editing the window
 * options on the window given in `spot'.  Only one editor can
 * be running on a window at a time.  This precondition is
 * enforced.
 */
{
    if (!all_opt_editors) {
	all_opt_editors = st_init_table(st_numcmp, st_numhash);
    }
    if (!st_is_member(all_opt_editors, (char *) spot->theWin)) {
	if (spawn_opt_editor(spot->theWin)) {
	    (void) st_insert(all_opt_editors, (char *) spot->theWin, (char *) 0);
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

/* Routines to handle the expert mode of operation. */


int vemExpert( spot )
    vemPoint* spot;
{
    static int last = 0;
    if ( spot ) {
	wnOpts opts;
	if ( wnGetInfo(spot->theWin, &opts, (octId *) 0, (octId *)0,(bdTable *)0,(struct octBox *)0) == VEM_OK ) {
	    last = ( opts.disp_options & VEM_EXPERT ) > 0;
	}
    }
    return last;
}



/*
 * Due to an ultrix 3.1 cc bug we have to do this
 */
#define FSAME(f1, f2)	(((int (*)()) f1) == ((int (*)()) f2))

static int spawn_opt_editor(win)
Window win;			/* Window for spawning */
/*
 * Spawns a new editor for window `win'.  A new description is allocated
 * and built.  The dialog consists of a multi-which for the boolean options,
 * a multi-text for grid and other parameters.
 */
{
    ddsHandle top, list, num, cbtn;
    octObject fct;
    wnOpts *opts;
    ddsNumber num_data;
    con_udata *c_data;
    int *i_value;
    float *f_value;
    int i;

    opts = VEMALLOC(wnOpts);
    if (wnGetInfo(win, opts, &(fct.objectId), (octId *) 0, (bdTable *) 0,
		  (struct octBox *) 0) == VEM_OK) {
	top_info.title = gen_title(&fct);
	top = dds_component(0, DDS_TOP, (ddsData) &top_info);
	c_data = VEMALLOC(con_udata);
	c_data->win = win;
	c_data->opts = opts;
	for (i = 0;  i < list_info.num_items;  i++) {
	    window_options[i].selected_p =
	      (opts->disp_options & ((int) window_options[i].user_data) ?
	       DDS_TRUE : DDS_FALSE);
	}
	list_info.user_data = (ddsData) opts;
	list = dds_component(top, DDS_ITEM_LIST, (ddsData) &list_info);
	for (i = 0;  i < NUM_GRID;  i++) {
	    num_data = grid_nums[i];
	    if (FSAME(num_data.callback, ch_int)) {
		i_value = (int *) (((char *) opts) + ((int) num_data.user_data));
		num_data.value = (double) (*i_value);
		num_data.user_data = (ddsData) i_value;
	    } else {
		f_value = (float *) (((char *) opts) + ((int) num_data.user_data));
		num_data.value = (double) (*f_value);
		num_data.user_data = (ddsData) f_value;
	    }
	    num = dds_component(top, DDS_NUMBER, (ddsData) &num_data);
	}
	for (i = 0;  i < NUM_OTHER;  i++) {
	    num_data = other_nums[i];
	    if (FSAME(num_data.callback, ch_int)) {
		i_value = (int *) (((char *) opts) + ((int) num_data.user_data));
		num_data.value = (double) (*i_value);
		num_data.user_data = (ddsData) i_value;
	    } else {
		f_value = (float *) (((char *) opts) + ((int) num_data.user_data));
		num_data.value = (double) (*f_value);
		num_data.user_data = (ddsData) f_value;
	    }
	    num = dds_component(top, DDS_NUMBER, (ddsData) &num_data);
	}
	for (i = 0;  i < NUM_STRS;  i++) {
	    c_data->ets[i] = dds_component(top, DDS_EDIT_TEXT,
					   (ddsData) other_strs+i);
	}
	for (i = 0;  i < NUM_CONS;  i++) {
	    opt_cons[i].user_data = (ddsData) c_data;
	    cbtn = dds_component(top, DDS_CONTROL, (ddsData) (opt_cons+i));
	}
	c_data->help = dmHelpButton(top, opt_help);
	opt_pos.win = win;
	dds_post(top, &opt_pos, (ddsEvtHandler *) 0);
	return 1;
    } else {
	return 0;
    }
}


static char *gen_title(fct)
octObject *fct;			/* Primary facet for editor */
/*
 * Allocates and constructs appropriate title for options editing
 * dialog.  The string is allocated and should be freed when
 * no longer needed.
 */
{
    return optTitle(fct, "Options editor for ");
}


static void ch_int(num)
ddsHandle num;			/* Number component */
/*
 * This routine is called when an integer value is changed
 * in a number component.  It modifies the proper field
 * in the structure and returns.
 */
{
    ddsNumber data;
    int *value;

    dds_get(num, (ddsData) &data);
    value = (int *) data.user_data;
    *value = (int) (data.value + 0.5);
}

#ifdef USE_CH_DBL
static void ch_dbl(num)
ddsHandle num;			/* Number component */
/*
 * This routine is called when a double value is changed
 * in a number component.  It modifies the proper field
 * in the structure and returns.
 */
{
    ddsNumber data;
    double *value;

    dds_get(num, (ddsData) &data);
    value = (double *) data.user_data;
    *value = (double) data.value;
}
#endif

static void ch_flag(item_list, toggled)
ddsHandle item_list;		/* Item list modified */
int toggled;			/* What item changed  */
/*
 * This routine is called when one of the flag options
 * is changed.  It modifies the display options
 * appropriately and returns.
 */
{
    ddsItemList data;
    int mask;
    wnOpts *opts;

    dds_get(item_list, (ddsData) &data);
    mask = (int) data.items[toggled].user_data;
    opts = (wnOpts *) data.user_data;
    if (data.items[toggled].selected_p == DDS_TRUE) {
	opts->disp_options |= mask;
    } else {
	opts->disp_options &= ~mask;
    }
}


static void opt_apply(item)
ddsHandle item;			/* Control button */
/*
 * This routine updates the window options with the values
 * from the current dialog.  All of the fields are kept
 * interactively up to date except the string fields.
 * We extract that information manually.
 */
{
    ddsControl data;
    ddsEditText etxt;
    con_udata *c_data;
    char **field;
    wnOpts cur_opts;
    int i;

    dds_get(item, (ddsData) &data);
    c_data = (con_udata *) data.user_data;
    (void) wnGetInfo(c_data->win, &cur_opts, (octId *) 0, (octId *) 0,
		     (bdTable *) 0, (struct octBox *) 0);
    for (i = 0;  i < NUM_STRS;  i++) {
	dds_get(c_data->ets[i], (ddsData) &etxt);
	field = (char **) (((char *) c_data->opts)+str_offs[i]);
	*field = VEMSTRCOPY(etxt.text);
    }
    c_data->opts->off_layers = cur_opts.off_layers;
    c_data->opts->inst_prio = cur_opts.inst_prio;
/* XXXX WORK AROUND A BUG - RLS */    
    c_data->opts->interface = util_strsav("interface");
/* XXXX */    
    wnSetInfo(c_data->win, c_data->opts, (bdTable *) 0);
}

static void opt_dismiss(item)
ddsHandle item;			/* Control button */
/*
 * Destroys both the main dialog and any help dialog associated
 * with `item'.  First checks to make sure no changes have been
 * made and asks for confirmation if there have been changes
 * that haven't been applied.
 */
{
    ddsControl data;
    ddsEditText etxt;
    con_udata *c_data;
    char **field, *value;
    Window win;
    wnOpts cur_opts;
    int i;
    static char *warn_str =
"You have made changes to some\n\
 parameters. Do you really want\n\
 to exit without saving them?";

    dds_get(item, (ddsData) &data);
    c_data = (con_udata *) data.user_data;
    for (i = 0;  i < NUM_STRS;  i++) {
	dds_get(c_data->ets[i], (ddsData) &etxt);
	field = (char **) (((char *) c_data->opts)+str_offs[i]);
	*field = etxt.text;
    }
    (void) wnGetInfo(c_data->win, &cur_opts, (octId *) 0, (octId *) 0,
		     (bdTable *) 0, (struct octBox *) 0);
    if (changes_p(c_data->opts, &cur_opts)) {
	if (dmConfirm("Warning", warn_str, "Yes", "No") != VEM_OK) {
	    return;
	}
    }
    /* Remove from table */
    win = c_data->win;
    (void) st_delete(all_opt_editors, (char **) &win, &value);
    dds_destroy(c_data->help);
    dds_destroy(item);
    VEMFREE(c_data->opts);
    VEMFREE(c_data);
}

static void opt_ok(item)
ddsHandle item;			/* Control button */
/*
 * Called when the user activates the "Ok" button.  This
 * is essentially "Apply" followed by "Dismiss".
 */
{
    opt_apply(item);
    opt_dismiss(item);
}



#define FLTDIFF(a, b, mindiff) (VEMABS((a)-(b)) > mindiff)

static int changes_p(dest, src)
wnOpts *dest;			/* Destination options */
wnOpts *src;			/* Source options      */
/*
 * Compares the two option structures and returns a non-zero
 * value if there are any differences.
 */
{
    return (dest->disp_options != src->disp_options) ||
      (dest->lambda != src->lambda) ||
      (dest->snap != src->snap) ||
      (dest->grid_base != src->grid_base) ||
      (dest->grid_minbase != src->grid_minbase) ||
      (dest->grid_majorunits != src->grid_majorunits) ||
      (dest->grid_minorunits != src->grid_minorunits) ||
      (dest->grid_diff != src->grid_diff) ||
      FLTDIFF(dest->bb_thres, src->bb_thres, 0.05) ||
      (dest->bb_min != src->bb_min) ||
      (dest->con_min != src->con_min) ||
      /* XXX RLS BUG STRCOMP(dest->interface, src->interface) || */
      (dest->solid_thres != src->solid_thres);
}

