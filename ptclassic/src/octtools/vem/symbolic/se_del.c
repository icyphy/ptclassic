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
 * VEM Specific Delete Interface
 *
 * David Harrison
 * University of California, Berkeley
 * 1989, 1990
 *
 * This is the interface portion of symbolic delete.  It parses
 * the argument list and calls the underlying symbolic policy
 * function for delete.
 */

#include "general.h"
#include "symbolic.h"
#include "commands.h"
#include "message.h"
#include "list.h"
#include "vemUtil.h"
#include "region.h"
#include "windows.h"
#include "se.h"

void seDelFunc(item, data)
octObject *item;		/* Object that is about to be deleted */
char *data;			/* Callback function                  */
/*
 * This routine redisplays an area before the item is deleted.
 * With automatic redisplay, only formal terminals will require
 * this action.
 */
{
    struct octBox bb;
    octId fct_id = (octId) data;

    if ((item->type == OCT_TERM) && (item->contents.term.instanceId == 0)) {
	if (regFindImpl(item, &bb) == REG_OK) {
#ifndef OLD_DISPLAY
	    wnQRedisplay(fct_id, &bb);
#else
	    wnQRedraw(fct_id, &bb);
#endif
	}
    } 
#ifdef OLD_DISPLAY	
    else {
	/* Anything that has a bounding box */
	if (octBB(item, &bb)) {
	    wnQRegion(fct_id, &bb);
	}
    }
#endif
}

vemStatus seDelete(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This is the interface to symbolic delete.  It takes a set
 * of selected objects and deletes them.
 */
{
    vemOneArg		*firstArg;
    octObject		facet, delSetBag;
    int			len, retCode;
    symDelItemFunc	callback;

    seInit();

    /* First,  all kinds of validity checking */
    len = lsLength(cmdList);
    if (len < 1) {
	vemMsg(MSG_C, "format:  [points or boxes] [layer] delete or\n");
	vemMsg(MSG_C, "         [object set] delete \n");
	return VEM_ARGRESP;
    }
    if (vuCkStyle(spot, cmdList) != VEM_OK) return VEM_ARGRESP;

    /* Parse the argument list */
    lsFirstItem(cmdList, (Pointer *) &firstArg, LS_NH);
    delSetBag.type = OCT_BAG;
    delSetBag.contents.bag.name = "DeleteBag";
    if ((retCode=vuObjArgParse(spot, cmdList, 
			       &(facet.objectId), &delSetBag, 1)) != VEM_OK)
	return retCode;

    vuOffSelect(firstArg);
    lsDelBegin(cmdList, (Pointer *) &firstArg);

    if (bufWritable(facet.objectId) != VEM_OK) {
	vemMsg(MSG_C, "Buffer is read only\n", MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }
    if (octGetById(&facet) != OCT_OK) {
	vemMsg(MSG_A, "The buffer is inaccessable:\n  %s\n", octErrorString());
	return VEM_CORRUPT;
    }
    callback.func = seDelFunc;
    callback.data = (char *) facet.objectId;
    symDeleteObjects(&delSetBag, &callback);
    (void) octDelete(&delSetBag);
    return VEM_OK;
}
