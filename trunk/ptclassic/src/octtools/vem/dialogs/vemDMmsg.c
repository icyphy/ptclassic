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
 * This file contains routines implementing the dmMessage interface.
 */

#include "vemDMint.h"


static void unmoded_ok(item)
ddsHandle item;
/*
 * Called when the "Dismiss" button is pushed in a dmMessage dialog.
 */
{
    dds_destroy(item);
}

void dmMessage(title, message, auto_format)
STR title;			/* Title at top of dialog */
STR message;			/* Message to display     */
int auto_format;		/* Automatic formatting   */
/*
 * This routine presents an unmoded dialog that shows `message'
 * in a window.  The text may have newlines.  If there are more
 * than 10 lines,  the window will be made scrollable.  One
 * control button will be placed down at the bottom that causes
 * the window to go away.  If `auto_format' is non-zero, the
 * text of the message will be broken into lines automatically.
 * New paragraphs are started by blank lines.  If `auto_format'
 * is zero,  the formatting will the same as that given in the
 * message.
 */
{
    ddsHandle top, text_body, con;
    static ddsComposite top_data;
    static ddsShowText text_data;
    static ddsControl con_data;
    ddsPosition pos;

    if (!_vemdm_initialized) {
	errRaise(vemdm_pkg_name, 0,
		  "Package must be initialized with call to dmCompatInit");
	/*NOTREACHED*/
    }
    top_data.title = title;
    top_data.just = DDS_VERTICAL;
    top_data.max_items = 1;
    top = dds_component((ddsHandle) 0, DDS_TOP, (ddsData) &top_data);
    text_data.where = DDS_FROM_STRING;
    text_data.rows = 10;    text_data.cols = 60;
    text_data.text = message;
    text_data.word_wrap_p = (auto_format ? DDS_TRUE : DDS_FALSE);
    text_data.compute_size_p = DDS_TRUE;
    text_body = dds_component(top, DDS_SHOW_TEXT, (ddsData) &text_data);
    con_data.name = "Dismiss\n(Ret)";
    con_data.accelerator = "<Key>Return|<Key>F1";
    con_data.callback = unmoded_ok;
    con_data.user_data = (ddsData) 0;
    con = dds_component(top, DDS_CONTROL, (ddsData) &con_data);
    pos.pos = DDS_MOUSE;  pos.vjust = DDS_MIDDLE;  pos.hjust = DDS_CENTER;
    dds_post(top, &pos, (ddsEvtHandler *) 0);
}
