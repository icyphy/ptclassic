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
 * The routines defined here are internal for use by the rest of
 * the package.
 */

#include "vemDMint.h"

char *vemdm_pkg_name = "vemdm";
int _vemdm_initialized = 0;
ddsEvtHandler _dmCompatHandler;



void dmCompatInit(evt_handler)
int (*evt_handler)();
/*
 * Application top level widget for resource gathering.
 */
{
    _vemdm_initialized = 1;
    _dmCompatHandler.other_handler = evt_handler;
    _dmCompatHandler.user_data = (ddsData) 0;
}


/*
 * Standard control button callback functions
 */

static void std_cb(item, preserve, status)
ddsHandle item;
int preserve;
vemStatus status;
/*
 * Standard callback template for pushing "Ok" or "Cancel" buttons.
 * Sets the status in the callback data and unposts (or destroys)
 * the dialog.
 */
{
    ddsControl data;
    con_callback *cb_info;

    dds_get(item, (ddsData) &data);
    cb_info = (con_callback *) data.user_data;
    cb_info->status = status;
    if (cb_info->help) dds_destroy(cb_info->help);
    if (preserve) {
	dds_unpost(item);
    } else {
	dds_destroy(item);
    }
}

void _vemdm_general_ok(item)
ddsHandle item;
/*
 * Called when the "Ok" button is pressed in dmWhichOne or dmMultiWhich.
 */
{
    std_cb(item, 0, VEM_OK);
}

void _vemdm_general_cancel(item)
ddsHandle item;
/*
 * Called when the "Ok" button is pressed in dmWhichOne or dmMultiWhich.
 */
{
    std_cb(item, 0, VEM_NOSELECT);
}

void _vemdm_general_no(item)
ddsHandle item;
/*
 * Called when "no" button is pushed in dmConfirm
 */
{
    std_cb(item, 0, VEM_FALSE);
}

void _vemdm_preserve_ok(item)
ddsHandle item;
/*
 * Called when the "Ok" button is pressed in dmMultiText.
 */
{
    std_cb(item, 1, VEM_OK);
}

void _vemdm_preserve_cancel(item)
ddsHandle item;
/*
 * Called when the "Ok" button is pressed in dmMultiText.
 */
{
    std_cb(item, 1, VEM_NOSELECT);
}



