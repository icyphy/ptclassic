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
 * Reconnection of Instances
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 *
 * This file contains an implementation of a command that checks
 * the connections to an instance.  Bad connections are removed.
 * Incorrect connections are fixed.
 */

#include "general.h"
#include "oct.h"
#include "commands.h"
#include "message.h"
#include "vemUtil.h"
#include "se.h"


static vemStatus reconn_format(msg, code)
char *msg;			/* Message to display */
vemStatus code;			/* Code to return     */
/*
 * Displays a format message for the command.
 */
{
    if (msg) {
	vemMsg(MSG_C, "Error: %s\n", msg);
    }
    vemMsg(MSG_C, "format: points, lines, boxes, or objects : reconnect\n");
    return code;
}


vemStatus seBaseReconnect(spot, cmdList, confun)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
symConFunc *confun;		/* Connector function       */
/*
 * This command takes a set of points, lines, boxes, or objects.
 * The instances under the poins, intersecting the lines, inside
 * the boxes, and in the object set are checked for connectivity
 * problems.  Any problems found are corrected.
 */
{
    octId fct_id;
    octObject bag, inst;
    octGenerator gen;
    symDelItemFunc callback;

    seInit();

    /* Parse argument list */
    if (vuObjArgParse(spot, cmdList, &fct_id, &bag, 0) == VEM_OK) {
	if (bufWritable(fct_id) != VEM_OK) {
	    VEM_OCTCKRVAL(octDelete(&bag), VEM_CORRUPT);
	    return reconn_format("Buffer is not writable", VEM_ARGRESP);
	}
	VEM_OCTCKRVAL(octInitGenContents(&bag, OCT_INSTANCE_MASK, &gen), VEM_CORRUPT);
	callback.func = seDelFunc;
	callback.data = (char *) fct_id;
	while (octGenerate(&gen, &inst) == OCT_OK) {
	    symReconnect(&inst, confun, &callback);
	}
    } else {
	return reconn_format("Can't parse argument list", VEM_ARGRESP);
    }
    return VEM_OK;
}

vemStatus seReconnect(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This is a wrapper around seBaseReconnect for symbolic cells.
 */
{
    return seBaseReconnect(spot, cmdList, symDefConFunc);
}
