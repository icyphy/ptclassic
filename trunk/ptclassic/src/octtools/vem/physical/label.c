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
 * VEM Physical Editing Commands
 * 
 * David Harrison
 * University of California, Berkeley
 * 1989
 *
 * This file contains a unmoded dialog-based editor for creating
 * and changing Oct labels.
 */

#include "general.h"		/* General VEM Definitions    */
#include "oct.h"		/* Oct Data Manager           */
#include "dds.h"		/* Low-level dialog interface */
#include "xvals.h"		/* Important X values         */
#include "windows.h"		/* For window options         */
#include "vemDM.h"		/* VEM dialog functions       */

typedef struct lbl_info_defn {
    Window win;			/* VEM window      */
    octId fct_id;		/* What facet      */
    octObject lyr;		/* What layer      */
    octObject lbl;		/* Label itself    */
    struct octPoint pnt;	/* Reference point */
    ddsHandle text;		/* Label text      */
    ddsHandle help;		/* Help dialog     */
} lbl_info;

static void make_height();
static void make_just();

static void ref_pnt();
static void lbl_params();
static void lbl_box();
static int label_differences();


/*
 * Static description of label editing dialog
 * 
 * Additional fields are set upon dialog creation.
 */

static ddsComposite lbl_top_data = { "Label Editor", DDS_VERTICAL, 4 };

static void change_height();
static ddsNumber lbl_height_data = {
    "Text Height (lambda)", 1.0, 100, 1.0, 10.0, "%3.0lf",
    change_height, (ddsData) 0
};

static void change_hjust();
static char *hjust_vals[] = {
    "Left", "Center", "Right"
};
#define NUM_HJUST	sizeof(hjust_vals)/sizeof(char *)
static ddsEnumerate lbl_hor_data = {
    "Horizontal Justification", NUM_HJUST, hjust_vals, 1,
    change_hjust, (ddsData) 0
};

static void change_vjust();
static char *vjust_vals[] = {
    "Bottom", "Center", "Top"
};
#define NUM_VJUST	sizeof(vjust_vals)/sizeof(char *)
static ddsEnumerate lbl_ver_data = {
    "Vertical Justification", NUM_VJUST, vjust_vals, 1,
    change_vjust, (ddsData) 0
};

static void change_ljust();
static char *ljust_vals[] = {
    "Left", "Center", "Right"
};
#define NUM_LJUST	sizeof(ljust_vals)/sizeof(char *)
static ddsEnumerate lbl_line_data = {
    "Line Justification", NUM_LJUST, ljust_vals, 1,
    change_ljust, (ddsData) 0
};

static ddsEditText lbl_text_data = { "Label", "", 4, 40 };

static void lbl_ok();
static void lbl_dismiss();
static void lbl_apply();

static ddsControl lbl_cons[] = {
    { "Ok\n(M-Ret)", "Meta<Key>Return|<Key>F1", lbl_ok, (ddsData) 0 },
    { "Dismiss\n(M-Del)", "Meta<Key>Delete|<Key>F2", lbl_dismiss, (ddsData) 0 },
    { "Apply\n(F4)", "<Key>F4", lbl_apply, (ddsData) 0 }
};
#define NUM_LBL_CONS	sizeof(lbl_cons)/sizeof(ddsControl)

static ddsPosition lbl_pos = {
    DDS_WINDOW, DDS_ABOVE, DDS_RIGHT, 0, 0, (Window) 0 
};

static char *lbl_help_str =
"You can edit the label text using standard line\
 editing commands.  Those familiar with Emacs will find that most\
 of the emacs editing commands also work.  You can change the justification\
 of the label by clicking the mouse in the box next to the desired\
 justification.  A menu displaying the possible justifications will\
 appear beneath the box.  Let up the mouse over your choice to change\
 the justification.\
 The horizontal and vertical justifications are computed\
 relative to the reference point you used to create the label.\n\n\
 Once you have changed the label,  choose an operation from the list\
 of buttons at the bottom of the dialog:\n\n\
 OK: Modify the label and dismiss the dialog.\n\n\
 Apply: Modify the label and update it on the screen but don't\
 remove the dialog.\n\n\
 Dismiss: Remove the dialog but don't update the label.";



void phyLESpawn(win, fctId, layer, label)
Window win;			/* Window spawned from */
octId fctId;			/* Facet ident.    */
octObject *layer;		/* Layer for label */
octObject *label;		/* Label itself    */
/*
 * This routine spawns a new unmoded dialog for editing
 * a label.  If the object identifier of the label is
 * null,  the editor will create the label attached
 * to the specified layer.  If the object identifier is
 * non-null,  it will modify the label.  The editor
 * provides fields for modifying the label text,  the
 * text height, and the justification of the label about
 * a reference point which is calculated from the label
 * box and the justification parameters.   The control
 * buttons provided are OK (update and exit),  APPLY (update),
 * and DISMISS (exit).
 */
{
    ddsHandle top, cb;
    int i;
    lbl_info *info;

    info = VEMALLOC(lbl_info);
    info->win = win;
    info->fct_id = fctId;
    info->lyr = *layer;
    info->lbl = *label;
    ref_pnt(label, &(info->pnt));

    top = dds_component(0, DDS_TOP, (ddsData) &lbl_top_data);
    
    make_height(top, info);
    make_just(top, info, &lbl_hor_data, info->lbl.contents.label.horizJust);
    make_just(top, info, &lbl_ver_data, info->lbl.contents.label.vertJust);
    make_just(top, info, &lbl_line_data, info->lbl.contents.label.lineJust);
    if (octIdIsNull(label->objectId)) {
	lbl_text_data.text = "";
    } else {
	lbl_text_data.text = label->contents.label.label;
    }
    info->text = dds_component(top, DDS_EDIT_TEXT, (ddsData) &lbl_text_data);
    
    for (i = 0;  i < NUM_LBL_CONS;  i++) {
	lbl_cons[i].user_data = (ddsData) info;
	cb = dds_component(top, DDS_CONTROL, (ddsData) (lbl_cons+i));
    }
    info->help = dmHelpButton(top, lbl_help_str);

    lbl_pos.win = win;
    dds_post(top, &lbl_pos, (ddsEvtHandler *) 0);
}



static void make_height(top, info)
ddsHandle top;			/* Top component of dialog */
lbl_info *info;			/* Callback information    */
/*
 * Makes a new text height component.  This involves translating
 * the given label height from lambda into oct units and
 * setting that value into the newly created ddsNumber component.
 */
{
    int oct_per_lambda = 20;
    wnOpts opts;

    if (wnGetInfo(info->win, &opts, (octId *) 0, (octId *) 0,
		  (bdTable *) 0, (struct octBox *) 0) == VEM_OK) {
	oct_per_lambda = opts.lambda;
    }
    lbl_height_data.value =
      (double) (info->lbl.contents.label.textHeight/oct_per_lambda);
    lbl_height_data.user_data = (ddsData) info;
    (void) dds_component(top, DDS_NUMBER, (ddsData) &lbl_height_data);
}

static void make_just(top, info, data, just_value)
ddsHandle top;			/* Dialog               */
lbl_info *info;			/* Callback information */
ddsEnumerate *data;		/* Description          */
int just_value;			/* Justification value  */
/*
 * Makes a new justification component setting its initial
 * value to `just_value'.
 */
{
    data->selected = just_value;
    data->user_data = (ddsData) info;
    (void) dds_component(top, DDS_ENUMERATE, (ddsData) data);
}



static void change_height(num)
ddsHandle num;			/* Handle to number component */
/*
 * Called when the text height value is changed by the user.  It
 * fills the new value into the label structure.
 */
{
    int oct_per_lambda = 20;
    wnOpts opts;
    lbl_info *info;
    ddsNumber num_data;

    dds_get(num, (ddsData) &num_data);
    info = (lbl_info *) num_data.user_data;
    if (wnGetInfo(info->win, &opts, (octId *) 0, (octId *) 0,
		  (bdTable *) 0, (struct octBox *) 0) == VEM_OK) {
	oct_per_lambda = opts.lambda;
    }
    info->lbl.contents.label.textHeight =
      ((int) num_data.value) * oct_per_lambda;
}

static void change_hjust(item, previous)
ddsHandle item;			/* Enumerated item    */
int previous;			/* Previous selection */
/*
 * This routine is called when the horizontal justification
 * is changed.
 */
{
    lbl_info *info;
    ddsEnumerate enum_data;

    dds_get(item, (ddsData) &enum_data);
    info = (lbl_info *) enum_data.user_data;
    info->lbl.contents.label.horizJust = enum_data.selected;
}

static void change_vjust(item, previous)
ddsHandle item;			/* Enumerated item    */
int previous;			/* Previous selection */
/*
 * This routine is called when the vertical justification
 * is changed.
 */
{
    lbl_info *info;
    ddsEnumerate enum_data;

    dds_get(item, (ddsData) &enum_data);
    info = (lbl_info *) enum_data.user_data;
    info->lbl.contents.label.vertJust = enum_data.selected;
}

static void change_ljust(item, previous)
ddsHandle item;			/* Enumerated item    */
int previous;			/* Previous selection */
/*
 * This routine is called when the line justification
 * is changed.
 */
{
    lbl_info *info;
    ddsEnumerate enum_data;

    dds_get(item, (ddsData) &enum_data);
    info = (lbl_info *) enum_data.user_data;
    info->lbl.contents.label.lineJust = enum_data.selected;
}



static void lbl_dismiss(item)
ddsHandle item;			/* Control button */
/*
 * This routine is called when the user pushes the `Dismiss' button
 * in a label editing dialog.  It checks to make sure no significant
 * changes have been made then deletes the dialog and reclaims
 * resources.
 */
{
    ddsControl con_data;
    ddsEditText ed_data;
    octObject old_label;
    lbl_info *info;
    int proceed = 1;
    static char *diff_warn =
"You have made changes to the label specification\n\
without saving them using \"Ok\" or \"Apply\".  Do\n\
you really want to discard these changes?";
    static char *lbl_gone =
"The label you were editing does not exist\n\
in the database.  Do you really want to exit\n\
anyway?";

    dds_get(item, (ddsData) &con_data);
    info = (lbl_info *) con_data.user_data;
    dds_get(info->text, (ddsData) &ed_data);
    info->lbl.contents.label.label = ed_data.text;
    old_label.objectId = info->lbl.objectId;
    if (octGetById(&old_label) == OCT_OK) {
	if (label_differences(&(old_label.contents.label),
			      &(info->lbl.contents.label))) {
	    if (dmConfirm("Warning", diff_warn, "Yes", "No") != VEM_OK) {
		proceed = 0;
	    }
	}
    } else {
	if (dmConfirm("Warning", lbl_gone, "Yes", "No") != VEM_OK) {
	    proceed = 0;
	}
    }
    if (proceed) {
	dds_destroy(info->help);
	dds_destroy(item);
	VEMFREE(info);
    }
}



static void lbl_apply(item)
ddsHandle item;			/* Control button */
/*
 * This routine is called when the user presses the `Apply'
 * button.  This updates the copy of the label in the database.
 */
{
    ddsControl con_data;
    ddsEditText ed_data;
    lbl_info *info;

    dds_get(item, (ddsData) &con_data);
    info = (lbl_info *) con_data.user_data;
    dds_get(info->text, (ddsData) &ed_data);
    info->lbl.contents.label.label = ed_data.text;
    lbl_box(&info->lbl, &info->pnt);
    if (octIdIsNull(info->lbl.objectId)) {
	VEM_OCTCKRET(octCreate(&(info->lyr), &(info->lbl)));
    } else {
	VEM_OCTCKRET(octModify(&info->lbl));
    }
}

static void lbl_ok(item)
ddsHandle item;			/* Control button */
/* 
 * This routine is called when the user presses the `Ok' button
 * in a label editing dialog.  This is basically `Apply' followed
 * by `dismiss'.
 */
{
    lbl_apply(item);
    lbl_dismiss(item);
}



/*
 * Label box computation
 */

static void ref_pnt(label, pnt)
octObject *label;		/* Label itself    */
struct octPoint *pnt;		/* Reference point */
/*
 * This routine computes the label reference point from the
 * label box and its justification.  The reference point is
 * used at the end of the label editing session to recompute
 * the label box.
 */
{
    switch (label->contents.label.horizJust) {
    case OCT_JUST_LEFT:
	pnt->x = label->contents.label.region.lowerLeft.x;
	break;
    case OCT_JUST_CENTER:
	pnt->x = (label->contents.label.region.lowerLeft.x +
		  label->contents.label.region.upperRight.x)/2;
	break;
    case OCT_JUST_RIGHT:
	pnt->x = label->contents.label.region.upperRight.x;
	break;
    }
    switch (label->contents.label.vertJust) {
    case OCT_JUST_BOTTOM:
	pnt->y = label->contents.label.region.lowerLeft.y;
	break;
    case OCT_JUST_CENTER:
	pnt->y = (label->contents.label.region.lowerLeft.y +
		  label->contents.label.region.upperRight.y)/2;
	break;
    case OCT_JUST_TOP:
	pnt->y = label->contents.label.region.upperRight.y;
	break;
    }
}


static void lbl_params(text, rows, cols)
STR text;			/* Label text                         */
int *rows, *cols;		/* Maximum number of rows and columns */
/*
 * Examines label for line breaks.  Returns the number of lines found
 * in `rows' and the maximum number of characters per line in `cols'.
 */
{
    int new_line_flag = 0;
    int count;

    *rows = 1;
    *cols = count = 0;
    while (*text) {
	if (*text == '\n') {
	    if (count > *cols) *cols = count;
	    count = 0;
	    new_line_flag = 1;
	} else {
	    count++;
	    if (new_line_flag) {
		*rows += 1;
		new_line_flag = 0;
	    }
	}
	text++;
    }
    if (count > *cols) *cols = count;
}

#define HEIGHT_TO_WIDTH(val)	((val)*3/5)

static void lbl_box(label, pnt)
octObject *label;		/* Label under edit */
struct octPoint *pnt;		/* Reference point  */
/* 
 * This routine determines the region box for the label
 * given its reference point, justification, and text
 * height.  The height of the box is simply the text
 * height times the number of lines.  The width is
 * computed by multiplying the largest number of
 * characters in a line times an estimate of the
 * character width (which is derived from the given
 * text height).  The position of the box is then
 * computed from the reference point and the horizontal
 * and vertical justification.
 */
{
    int rows, cols;
    int width, height;
    
    lbl_params(label->contents.label.label, &rows, &cols);
    height = rows * label->contents.label.textHeight;
    width = cols * HEIGHT_TO_WIDTH(label->contents.label.textHeight);
    switch (label->contents.label.horizJust) {
    case OCT_JUST_LEFT:
	label->contents.label.region.lowerLeft.x = 0;
	label->contents.label.region.upperRight.x = width;
	break;
    case OCT_JUST_CENTER:
	label->contents.label.region.lowerLeft.x = -(width/2);
	label->contents.label.region.upperRight.x = width - (width/2);
	break;
    case OCT_JUST_RIGHT:
	label->contents.label.region.lowerLeft.x = -width;
	label->contents.label.region.upperRight.x = 0;
	break;
    }
    switch (label->contents.label.vertJust) {
    case OCT_JUST_BOTTOM:
	label->contents.label.region.lowerLeft.y = 0;
	label->contents.label.region.upperRight.y = height;
	break;
    case OCT_JUST_CENTER:
	label->contents.label.region.lowerLeft.y = -(height/2);
	label->contents.label.region.upperRight.y = height - (height/2);
	break;
    case OCT_JUST_TOP:
	label->contents.label.region.lowerLeft.y = -height;
	label->contents.label.region.upperRight.y = 0;
	break;
    }
    label->contents.label.region.lowerLeft.x += pnt->x;
    label->contents.label.region.upperRight.x += pnt->x;
    label->contents.label.region.lowerLeft.y += pnt->y;
    label->contents.label.region.upperRight.y += pnt->y;
}



static int label_differences(one, two)
struct octLabel *one, *two;	/* Label bodies */
/*
 * Compares the two labels and returns a non-zero status if the
 * labels differ significantly.
 */
{
    return (STRCOMP(one->label, two->label) != 0) ||
      (one->region.lowerLeft.x != two->region.lowerLeft.x) ||
	(one->region.lowerLeft.y != two->region.lowerLeft.y) ||
	  (one->region.upperRight.x != two->region.upperRight.x) ||
	    (one->region.upperRight.y != two->region.upperRight.y) ||
	      (one->textHeight != two->textHeight) ||
		(one->vertJust != two->vertJust) ||
		  (one->horizJust != two->horizJust) ||
		    (one->lineJust != two->lineJust);
}
