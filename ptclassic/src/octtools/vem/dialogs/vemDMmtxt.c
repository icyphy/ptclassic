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
 * This file contains routines implementing the dmMultiText interface.
 */

#include "vemDMint.h"



vemStatus dmMultiText(title, count, items)
STR title;			/* Question at top of dialog */
int count;			/* Number of items           */
dmTextItem *items;		/* List of items             */
/*
 * This routine presents a moded dialog which asks the
 * user to fill in a number of fields with text.  The
 * dialog consists of a title,  an OK button,  an ABORT
 * button,  and a list of items which specify the prompt,
 * size of the type-in field,  default value for the type-in,
 * and optional user data.  The user is expected to move between
 * fields with tabs filling in the fields until he is satisfied,
 * and then activate the OK button.  In this case,  the routine
 * returns VEM_OK and sets the 'value' field in each item
 * to the typed in values.  ** This string is owned by the dialog
 * package and must not be disturbed **.  It will be reclaimed
 * the next time dmMultiText is called.  Pressing the abort
 * button causes the routine to return VEM_NOSELECT.
 */
{
    ddsHandle top, con, *ets;
    static con_callback cb_info;
    static ddsComposite top_data;
    static ddsEditText edit_data;
    static ddsControl controls[] = {
	{ "Ok\n(M-Ret)", "Meta<Key>Return|<Key>F1",
	    _vemdm_preserve_ok, (ddsData) &cb_info },
	{ "Cancel\n(M-Del)", "Meta<Key>Delete|<Key>F2",
	    _vemdm_preserve_cancel, (ddsData) &cb_info },
    };
    static int num_cons = sizeof(controls)/sizeof(ddsControl);
    ddsPosition pos;
    static char **string_ring = (char **) 0;
    static int string_count = 0;
    int i;

    if (!_vemdm_initialized) {
	errRaise(vemdm_pkg_name, 0,
		  "Package must be initialized with call to dmCompatInit");
	/*NOTREACHED*/
    }
    top_data.title = title;
    top_data.just = DDS_VERTICAL;
    top_data.max_items = MAX_EDIT_TEXT;
    top = dds_component((ddsHandle) 0, DDS_TOP, (ddsData) &top_data);
    ets = VEMARRAYALLOC(ddsHandle, count);
    for (i = 0;  i < count;  i++) {
	edit_data.label = items[i].itemPrompt;
	edit_data.text = items[i].value;
	edit_data.rows = items[i].rows;
	edit_data.cols = items[i].cols;
	ets[i] = dds_component(top, DDS_EDIT_TEXT, (ddsData) &edit_data);
    }
    cb_info.help = (ddsHandle) 0;
    for (i = 0;  i < num_cons;  i++) {
	con = dds_component(top, DDS_CONTROL, (ddsData) (controls+i));
    }
    pos.pos = DDS_MOUSE;  pos.vjust = DDS_MIDDLE;  pos.hjust = DDS_CENTER;
    dds_post(top, &pos, &_dmCompatHandler);
    if (string_ring) {
	for (i = 0;  i < string_count;  i++) {
	    VEMFREE(string_ring[i]);
	}
	VEMFREE(string_ring);
	string_ring = (char **) 0;
	string_count = 0;
    }
    if (cb_info.status == VEM_OK) {
	string_ring = VEMARRAYALLOC(char *, count);
	string_count = count;
	for (i = 0;  i < count;  i++) {
	    dds_get(ets[i], (ddsData) &edit_data);
	    string_ring[i] = VEMSTRCOPY(edit_data.text);
	    items[i].value = string_ring[i];
	}
    }
    dds_destroy(top);
    return cb_info.status;
}
