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
 * Help support for VEM dialogs
 *
 * David Harrison
 * University of California, Berkley
 * 1990
 */

#include "vemDM.h"
#include "dds.h"



static void help_con(item)
ddsHandle item;
/*
 * Called when the "Dismiss" button is activated under help.
 */
{
    dds_unpost(item);
}

static void help_up(item)
ddsHandle item;
/*
 * Called when the "Help" button in the primary dialog is activated.
 */
{
    ddsControl data;
    ddsHandle help;
    ddsPosition pos;

    dds_get(item, (ddsData) &data);
    help = (ddsHandle) data.user_data;
    pos.pos = DDS_WINDOW;  pos.vjust = DDS_ABOVE;  pos.hjust = DDS_RIGHT;
    pos.win = dds_window(item);
    dds_post(help, &pos, (ddsEvtHandler *) 0);
}

static ddsComposite top_data = {
    "Help", DDS_VERTICAL, 1 
};
static ddsShowText show_data = {
    DDS_FROM_STRING, 10, 60, (char *) 0, DDS_TRUE, DDS_TRUE
};
static ddsControl con_data = {
    "Dismiss\n(Ret)", "<Key>Return|<Key>F1", help_con, (ddsData) 0
};
static ddsControl up_data = {
    "Help\n(F3)", "<Key>F3|<Key>Help", help_up, (ddsData) 0
};

ddsHandle dmHelpButton(parent, help_buf)
ddsHandle parent;		/* Primary dialog */
char *help_buf;			/* Help string    */
/*
 * Makes a simple help dialog and returns it.  It also
 * creates a control button under `top' for posting
 * this dialog.  It is up to the caller to insure
 * the help dialog is deleted when the main dialog
 * goes away.
 */
{
    ddsHandle top, show, con, main_con;

    top = dds_component(0, DDS_TOP, (ddsData) &top_data);
    show_data.text = help_buf;
    show = dds_component(top, DDS_SHOW_TEXT, (ddsData) &show_data);
    con_data.user_data = (ddsData) top;
    con = dds_component(top, DDS_CONTROL, (ddsData) &con_data);
    up_data.user_data = (ddsData) top;
    main_con = dds_component(parent, DDS_CONTROL, (ddsData) &up_data);
    return top;
}
