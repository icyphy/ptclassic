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
 * Segment Changes
 *
 * David Harrison
 * University of California,  Berkeley
 * 1988, 1990
 *
 * This file contains the implementation for the change layer
 * and change width commands in symbolic.  These commands
 * allow the user to change attributes of segments.  Once
 * changed,  these commands also update the segment endpoint
 * terminals appropriately.
 *
 * Work to be done:
 *  - Revise to use new symSegChange call
 *  - Revise header file usage
 */

#include "general.h"
#include "symbolic.h"
#include "oct.h"
#include "commands.h"
#include "list.h"
#include "tap.h"
#include "message.h"
#include "vemUtil.h"
#include "windows.h"
#include "se.h"

/* Forward declarations */
static vemStatus cs_format();


vemStatus seChangeSegs(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* Argument list            */
/*
 * This command changes either the width or the layer of
 * those segments on the argument list to those of the
 * layer specified as the last argument or the layer under
 * the cursor.  The endpoint connectors are upgraded automatically.
 */
{
    octGenerator gen;
    octObject spot_facet, obj_facet, new_lyr, bag, seg, old_lyr;
    octId finalFctId;
    octCoord new_width;

    seInit();

    spot_facet.objectId = spot->theFct;
    VEM_OCTCKRVAL(octGetById(&spot_facet), VEM_CORRUPT);
    if (lsLength(cmdList) == 0) {
	return cs_format((STR) 0, VEM_CORRUPT);
    }
    if (vuParseLayer(spot, cmdList, &spot_facet, &new_lyr) == VEM_OK) {
	/* Ready to try to parse the object set */
	if (vuObjArgParse(spot, cmdList, &finalFctId, &bag, 0) == VEM_OK) {
	    if (bufWritable(finalFctId) != VEM_OK) {
		octDelete(&bag);
		return cs_format("Buffer is read only.", VEM_ARGRESP);
	    }
	    obj_facet.objectId = finalFctId;
	    VEM_OCTCKRVAL(octGetById(&obj_facet), VEM_CORRUPT);
	    VEM_OCTCKRVAL(octGetOrCreate(&obj_facet, &new_lyr),
			  VEM_CORRUPT);
	    new_width = bufPathWidth(finalFctId, &new_lyr);
	    /* Segments in the bag are passed to the change routine */
	    VEM_OCTCKRVAL(octInitGenContents(&bag, OCT_PATH_MASK, &gen),
			  VEM_CORRUPT);
	    while (octGenerate(&gen, &seg) == OCT_OK) {
		/* Check to see if we need to post a warning */
		if (new_width < tapGetMinWidth(&new_lyr)) {
		    vemMsg(MSG_C, "WARNING: new width less than minimum for layer %s.\n",
			   old_lyr.contents.layer.name);
		}
		symSegChange(&obj_facet, &seg, &new_lyr, new_width,
			     (symConFunc *) 0);
	    }
	    if (bag.objectId != oct_null_id) {
		VEM_OCTCKRVAL(octDelete(&bag), VEM_CORRUPT);
	    }
	} else {
	    vemMsg(MSG_C, "Unable to parse argument list\n");
	    return VEM_ARGRESP;
	}
    } else {
	vemMsg(MSG_C, "No new layer specified\n");
	return VEM_ARGRESP;
    }
    return VEM_OK;
}



static vemStatus cs_format(msg, ret)
STR msg;			/* Message to display    */
vemStatus ret;			/* Return code to return */
/*
 * Displays format message for change width command.
 */
{
    if (msg) {
	vemMsg(MSG_C, "Error: %s\n", msg);
    }
    vemMessage("format: pnts, lines, boxes, or objects : change-segment (over layer)\n",
	       MSG_NOLOG|MSG_DISP);
    vemMessage("        pnts, lines, boxes, or objects \"layername\" : change-segment\n",
	       MSG_NOLOG|MSG_DISP);
    return ret;
}
