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
 * Window Options Utilities
 *
 * David Harrison
 * University of California, Berkeley
 * 1989
 *
 * These are utilities shared by all of the options editors.
 */

#include "port.h"
#include "general.h"
#include "vemUtil.h"
#include "windows.h"
#include "message.h"

#include "internal.h"

#define TITLE_PAD	20

char *optTitle(fct, prefix)
octObject *fct;			/* Primary facet for editor */
char *prefix;			/* Text to prefix           */
/*
 * Allocates and constructs appropriate title for options editing
 * dialog.  The string is allocated and should be freed when
 * no longer needed.
 */
{
    char *cell_name, *title;

    if (octGetById(fct) == OCT_OK) {
	cell_name = vuCellName(fct);
	title = VEMARRAYALLOC(char, STRLEN(cell_name)+STRLEN(prefix)+TITLE_PAD);
	STRMOVE(title, prefix);
	STRCONCAT(title, cell_name);
	VEMFREE(cell_name);
    } else {
	title = VEMARRAYALLOC(char, STRLEN(prefix) + TITLE_PAD);
	STRMOVE(title, prefix);
	STRCONCAT(title, "unknown cell");
    }
    return title;
}

vemStatus optToggleGridCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This routine toggles the grid display of the window specified
 * in spot.  It is faster than bringing up the window options
 * dialog.
 */
{
    wnOpts opts;

    if (spot == (vemPoint *) 0) {
	vemMsg(MSG_C, "There is no grid associated with the console\n");
    } else {
	if (wnGetInfo(spot->theWin, &opts, (octId *) 0, (octId *) 0,
		      (bdTable *) 0, (struct octBox *) 0) == VEM_OK) {
	    if (opts.disp_options & VEM_GRIDLINES) {
		opts.disp_options &= ~(VEM_GRIDLINES);
	    } else {
		opts.disp_options |= VEM_GRIDLINES;
	    }
	    (void) wnSetInfo(spot->theWin, &opts, (bdTable *) 0);
	}
    }
    return VEM_ARGRESP;
}
