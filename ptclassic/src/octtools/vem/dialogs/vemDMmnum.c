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
/*LINTLIBRARY*/
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
 * This file contains routines implementing the dmMultiNumber interface.
 */

#include "vemDMint.h"



vemStatus dmMultiNumber(title, count, items, help_buf)
STR title;			/* Title at top of dialog */
int count;			/* Number of items        */
dmNumItem *items;		/* Number items           */
STR help_buf;			/* Help text (optional)   */
/*
 * This routine presents a moded dialog that asks the user
 * to specify a number of real number values.  The dialog
 * consists of a title, "Ok", "Cancel", and "Help" buttons,
 * and list of items that specify a prompt and a real number
 * value.  Next to the value is a scroll bar.  The user is
 * expected to move the mouse into the scrollbar and use
 * its functionality to change the value.  Pressing the
 * left button will deincrement the value.  Pressing the
 * right button will increment the value.  Pressing and
 * dragging the middle button will cause the value to change
 * interactively.  Once the values are chosen, the user can 
 * press the OK button to end the dialog.  The routine returns 
 * VEM_OK in this case.  The value chosen will always be between
 * the `min' and `max' specified for the value and will be
 * an even multiple of `step' (plus `min').  If the user presses 
 * the "Cancel" button none of the fields are modified and the 
 * routine returns VEM_NOSELECT.  Pressing the "Help" button causes the
 * text `help_buf' to be shown.  If no help text is provided,
 * no help button will be present.  
 */
{
    ddsHandle top, con, *nums;
    static con_callback cb_info;
    static ddsComposite top_data;
    static ddsNumber num_data;
    static ddsControl controls[] = {
	{ "Ok\n(Ret)", "<Key>Return|<Key>F1",
	    _vemdm_preserve_ok, (ddsData) &cb_info },
	{ "Cancel\n(Del)", "<Key>Delete|<Key>F2",
	    _vemdm_preserve_cancel, (ddsData) &cb_info },
    };
    static int num_cons = sizeof(controls)/sizeof(ddsControl);
    ddsPosition pos;
    int i;

    if (!_vemdm_initialized) {
	errRaise(vemdm_pkg_name, 0,
		  "Package must be initialized with call to dmCompatInit");
	/*NOTREACHED*/
    }
    top_data.title = title;
    top_data.just = DDS_VERTICAL;
    top_data.max_items = MAX_ENUM_ITEMS;
    top = dds_component((ddsHandle) 0, DDS_TOP, (ddsData) &top_data);
    nums = VEMARRAYALLOC(ddsHandle, count);
    for (i = 0;  i < count;  i++) {
	num_data.label = items[i].itemPrompt;
	num_data.minimum = items[i].min;
	num_data.maximum = items[i].max;
	num_data.increment = items[i].step;
	num_data.value = items[i].value;
	num_data.format = items[i].format;
	nums[i] = dds_component(top, DDS_NUMBER, (ddsData) &num_data);
    }
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
    if (cb_info.status == VEM_OK) {
	for (i = 0;  i < count;  i++) {
	    dds_get(nums[i], (ddsData) &num_data);
	    items[i].value = num_data.value;
	}
    }
    dds_destroy(top);
    return cb_info.status;
}
