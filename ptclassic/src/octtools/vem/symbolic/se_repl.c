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
 * Instance Replacement
 *
 * David Harrison
 * University of California, Berkeley
 * 1988, 1990
 *
 * This file contains an implementation for instance replacement
 * that uses a number of heuristics for attempting to recover
 * from deleted, renamed, and additional terminals for the replacement
 * instance.
 */

#include "general.h"		/* General VEM definitions */
#include "oct.h"		/* Oct Data Manager        */
#include "windows.h"		/* Window management       */
#include "commands.h"		/* command parsing         */
#include "buffer.h"		/* VEM Buffer managment    */
#include "message.h"		/* VEM Messages            */
#include "list.h"		/* List handling package   */
#include "symbolic.h"		/* General symbolic defns  */
#include "vemUtil.h"		/* VEM Utilities           */
#include "oh.h"			/* Oct Helper		   */
#include "defaults.h"		/* Default handling        */
#include "se.h"			/* Symbolic editing defns  */

static void do_revert();
  /* Attempt to reread masters                   */
static void revert_it();
  /* Reread one master                           */
static vemStatus replFormat();
  /* Displays replace instance format message    */
static vemStatus parseInstSpec();
  /* Looks for and parses an instance specification */



vemStatus seReplace(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This command takes a set points, lines, boxes, or objects
 * and an instance specification.  The instances under the points,
 * intersecting the lines, inside the the boxes, and in the object
 * set are replaced with the specified instance.  The instance
 * can be specified either by explicitly typing its name or moving
 * over some other instance.
 */
{
    octId finalFctId;
    octObject repl_inst, bag, fct, inst;
    octGenerator gen;
    int count;			/* Count instances to be replaced. */
    char m_name[VEMMAXSTR*4], v_name[VEMMAXSTR], f_name[VEMMAXSTR];

    seInit();

    /* Find the instance specification */
    repl_inst.contents.instance.master = m_name;
    repl_inst.contents.instance.view = v_name;
    repl_inst.contents.instance.facet = f_name;
    if (parseInstSpec(spot, cmdList, &repl_inst) != VEM_OK) {
	return replFormat("No replacement instance specified", VEM_ARGRESP);
    }
    /* Parse the argument list */
    if (vuObjArgParse(spot, cmdList, &finalFctId, &bag, 0) == VEM_OK) {
	if (bufWritable(finalFctId) != VEM_OK) {
	    octDelete(&bag);
	    return replFormat("Buffer is not writable", VEM_ARGRESP);
	}
	fct.objectId = finalFctId;
	VEM_OCTCKRVAL(octGetById(&fct), VEM_CORRUPT);

	/* Attempt to re-read specified instance master and interface */
	/* do_revert(&repl_inst); */ /* No longer reread (AC) */

	/* Send instances in bag to replacement routine */
	count = ohCountContents( &bag, OCT_INSTANCE_MASK );
	VEM_OCTCKRVAL(octInitGenContents(&bag, OCT_INSTANCE_MASK, &gen), VEM_CORRUPT);
	/* XXX - new instances will also be attached to this bag ... */
	while (octGenerate(&gen, &inst) == OCT_OK) {
	    /* Make new instance */
	    repl_inst.contents.instance.name = inst.contents.instance.name;
	    repl_inst.contents.instance.transform = inst.contents.instance.transform;
	    symReplaceInst(&fct, &inst, &repl_inst);
	    if ( count-- == 0 ) {
		break;		/* Make sure you avoid infinit loops. */
	    }
	}
	octFreeGenerator( &gen );
	if (bag.objectId != oct_null_id) {
	    VEM_OCTCKRVAL(octDelete(&bag), VEM_CORRUPT);
	}
    } else {
	return replFormat("Can't parse argument list", VEM_ARGRESP);
    }
    return VEM_OK;
}


static vemStatus parseInstSpec(spot, cmdList, inst)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* Argument list            */
octObject *inst;		/* Instance (if any)        */
/*
 * This routine looks for an instance specification.  The instance
 * specification can be either a textual specification on the
 * end of the argument list or the instance under the cursor.
 * The routine assumes the master and view fields in `inst'
 * are initially set to areas large enough to receive appropriate
 * names.
 */
{
    octObject found, spot_fct;
    vemOneArg *lastArg;
    STR remain;

    inst->type = OCT_INSTANCE;
    inst->contents.instance.transform.transformType = OCT_NO_TRANSFORM;
    inst->contents.instance.version = OCT_CURRENT_VERSION;

    lsLastItem(cmdList, (lsGeneric *) &lastArg, LS_NH);
    if (lastArg && (lastArg->argType == VEM_TEXT_ARG)) {
	/* Parse instance specification */
	spot_fct.objectId = spot->theFct;
	if (octGetById(&spot_fct) == OCT_OK) {
	    inst->contents.instance.master = spot_fct.contents.facet.cell;
	    inst->contents.instance.view = spot_fct.contents.facet.view;
	    inst->contents.instance.facet = spot_fct.contents.facet.facet;
	}
	if (vuParseInst(lastArg->argData.vemTextArg, inst, &remain) != VEM_OK) {
	    return replFormat("Cannot parse instance specification.",
			      VEM_ARGRESP);
	}
    } else {
	/* Look under spot */
	if (vuFindSpot(spot, &found, OCT_INSTANCE_MASK) == VEM_OK) {
	    STRMOVE(inst->contents.instance.master,
		    found.contents.instance.master);
	    STRMOVE(inst->contents.instance.view,
		    found.contents.instance.view);
	    STRMOVE(inst->contents.instance.facet,
		    found.contents.instance.facet);
	} else {
	    return replFormat("No replacement instance specified.",
			      VEM_ARGRESP);
	}
    }
    return VEM_OK;
}

#ifdef NEVER

static void do_revert(inst)
octObject *inst;		/* Instance itself     */
/*
 * This routine attempts to automatically revert both the
 * contents and interface facets of the master of `inst'.
 */
{
    octObject master;

    master.type = OCT_FACET;
    master.contents.facet.mode = "r";
    master.contents.facet.cell = inst->contents.instance.master;
    master.contents.facet.view = inst->contents.instance.view;
    master.contents.facet.facet = "contents";
    master.contents.facet.version = OCT_CURRENT_VERSION;
    if (bufOpen(&master, 0) == VEM_OK) {
	revert_it(&master);
    }
    dfGetString("interface", &master.contents.facet.facet);
    if (bufOpen(&master, 0) == VEM_OK) {
	revert_it(&master);
    }
}
#endif

static void revert_it(fct)
octObject *fct;			/* Facet to revert  */
/*
 * This routine attempts to revert the specified buffer/facet
 * pair.  It queues up the region for redisplay as well.
 */
{
    struct octBox bb;

    vemMsg(MSG_C, "Rereading %s.\n", vuMakeName(fct));
    if (octBB(fct, &bb) == OCT_OK) {
	wnQRedisplay(fct->objectId, &bb);
    }
    if (bufRevert(&(fct->objectId)) == VEM_OK) {
	if ((octGetById(fct) == OCT_OK) && (octBB(fct, &bb) == OCT_OK)) {
	    wnQRedisplay(fct->objectId, &bb);
	}
    }
}



static vemStatus replFormat(msg, ret)
STR msg;			/* Message to display   */
vemStatus ret;			/* Return code to return */
/*
 * Displays format message for replace instance command.
 */
{
    if (msg) {
	vemMsg(MSG_C, "Error: %s\n", msg);
    }
    vemMsg(MSG_C, "format: points, lines, boxes, or objects \"inst. spec\" : replace-instance\n");
    vemMsg(MSG_C, "        points, lines, boxes, or objects : replace-instance (over instance)\n");
    return ret;
}

