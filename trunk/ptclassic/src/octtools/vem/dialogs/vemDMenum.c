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
 * This file contains routines implementing the dmMultiEnum interface.
 */

#include "vemDMint.h"


vemStatus dmMultiEnum(title, count, items, help_buf)
STR title;			/* Title at top of dialog */
int count;			/* Number of items        */
dmEnumItem *items;		/* Enumerated items       */
STR help_buf;			/* Help text              */
/*
 * This routine presents a moded dialog that asks the user
 * to specify a number of enumerated values.  The dialog
 * consists of a title, "Ok", "Cancel", and "Help" buttons,
 * and list of items that specify a prompt and an enumerated
 * value.  The user is expected to move the mouse into the
 * enumerated value area and depress the mouse.  A menu will
 * pop up containing all of the legal choices.  The user
 * can choose a value by letting up the mouse over it.
 * Once the values are chosen, the user can press the OK button
 * to end the dialog.  The routine returns VEM_OK in this case.
 * The `selected' field in the callers item list will be
 * updated to the offset of the chosen item.  This field
 * is also used on start-up to determine what value to
 * display.  If the user presses the "Cancel" button,
 * none of the fields are modified and the routine returns
 * VEM_NOSELECT.  Pressing the "Help" button causes the
 * text `help_buf' to be shown.  If no help text is provided,
 * no help button will be present.  The caller can specify the initial
 * item by writing into the chosen field.
 */
{
    ddsHandle top, con, *ens;
    static con_callback cb_info;
    static ddsComposite top_data;
    static ddsEnumerate enum_data;
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
    ens = VEMARRAYALLOC(ddsHandle, count);
    for (i = 0;  i < count;  i++) {
	enum_data.label = items[i].itemPrompt;
	enum_data.num_items = items[i].ni;
	enum_data.items = items[i].enum_vals;
	enum_data.selected = items[i].selected;
	enum_data.callback = (void (*)()) 0;
	enum_data.user_data = (ddsData) 0;
	ens[i] = dds_component(top, DDS_ENUMERATE, (ddsData) &enum_data);
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
	    dds_get(ens[i], (ddsData) &enum_data);
	    items[i].selected = enum_data.selected;
	}
    }
    dds_destroy(top);
    return cb_info.status;
}
