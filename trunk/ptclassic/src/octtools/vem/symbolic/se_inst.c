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
 * Instance Placement User Interface
 *
 * David Harrison
 * University of California,  Berkeley
 * 1987, 1990
 *
 * This code parses the argument list for creating symbolic instances.
 */

#include "general.h"
#include "symbolic.h"
#include "commands.h"
#include "vemUtil.h"
#include "vemDM.h"
#include "message.h"
#include "defaults.h"
#include "oh.h"
#include "region.h"
#include "se.h"

static void ckUnderInst
  ARGS((octObject *fct, octObject *inst));



static vemStatus format(rt)
vemStatus rt;
{
    vemMsg(MSG_C, "%s\n%s\n",
	   "format: points(n) \"cell:view\" or",
	   "        points(n) (over an instance)");
    return rt;
}

vemStatus seCreateInst(spot, cmdList, inst_fct)
vemPoint *spot;			/* Where command was invoked */
lsList cmdList;			/* Argument list             */
char *inst_fct;			/* Instance facet            */
/*
 * This command instantiates new instances in symbolic.  The format
 * of the command is points followed by an optional master specification.
 * If the master specification is missing,  the routine will attempt
 * to discover a master by looking at the instance under `spot'.
 */
{
    vemOneArg *pntArg, *txtArg;
    octObject newInst, foundObj, fct, spot_fct;
    int len, do_spot, i, errs, con_flag;
    STR remain;
    char masterName[VEMMAXSTR*2], viewName[VEMMAXSTR], fctName[VEMMAXSTR];

    len = lsLength(cmdList);
    if ((len < 1) || (len > 2)) format(VEM_ARGRESP);
    lsFirstItem(cmdList, (Pointer *) &pntArg, LS_NH);
    if (pntArg->argType != VEM_POINT_ARG) return format(VEM_ARGRESP);

    /* Get ready to find new instance specification */
    newInst.type = OCT_INSTANCE;
    newInst.contents.instance.name = "";
    newInst.contents.instance.master = ";;; NOMASTER ;;;";
    newInst.contents.instance.view = ";;; NOVIEW ;;;";
    newInst.contents.instance.facet = inst_fct;
    newInst.contents.instance.transform.transformType = OCT_NO_TRANSFORM;
    newInst.contents.instance.version = OCT_CURRENT_VERSION;
    do_spot = 1;
    con_flag = 0;
    if (len == 2) {
	/* Text specification of master is possible */
	lsLastItem(cmdList, (Pointer *) &txtArg, LS_NH);
	if (txtArg->argType != VEM_TEXT_ARG) return format(VEM_ARGRESP);
	spot_fct.objectId = spot->theFct;
	if (octGetById(&spot_fct) == OCT_OK) {
	    newInst.contents.instance.master = spot_fct.contents.facet.cell;
	    newInst.contents.instance.view = spot_fct.contents.facet.view;
	}
	do_spot = vuParseInst(txtArg->argData.vemTextArg, &newInst,
			      &remain) != VEM_OK;
    }
    if (do_spot) {
	/* Look at instance under spot */
	/* Try to find using spot -- use region package to find it */
	if (vuFindSpot(spot, &foundObj, OCT_INSTANCE_MASK) == VEM_OK) {
	    /* Lets use the spots master and view names and transformation */
	    newInst.contents.instance.transform =
	      foundObj.contents.instance.transform;
	    newInst.contents.instance.master = masterName;
	    newInst.contents.instance.view = viewName;
	    newInst.contents.instance.facet = fctName;
	    STRMOVE(newInst.contents.instance.master,
		   foundObj.contents.instance.master);
	    STRMOVE(newInst.contents.instance.view,
		   foundObj.contents.instance.view);
	    STRMOVE(newInst.contents.instance.facet,
		    foundObj.contents.instance.facet);
	} else {
	    vemMsg(MSG_C, "Can't find any instance under spot\n");
	    return format(VEM_ARGRESP);
	}
    }
	
    errs = 0;
    for (i = 0;  i < pntArg->length;  i++) {
	if (bufWritable(pntArg->argData.points[i].theFct) != VEM_OK) {
	    vemMsg(MSG_C, "Buffer is read only\n");
	    errs++; continue;
	}
	fct.objectId = pntArg->argData.points[i].theFct;
	if (octGetById(&fct) != OCT_OK) {
	    vemMsg(MSG_C, "Buffer is inaccessible:\n  %s\n", octErrorString());
	    errs++; continue;
	}
	/* Set up translation */
	newInst.contents.instance.transform.translation.x =
	  pntArg->argData.points[i].thePoint.x;
	newInst.contents.instance.transform.translation.y =
	  pntArg->argData.points[i].thePoint.y;
	/* Create instance */
	symInstance(&fct, &newInst);
	ckUnderInst(&fct, &newInst);
	bufMarkActive( fct.objectId );
    }
    if (errs) {
	vemMsg(MSG_C, "%d errors occurred\n", errs);
	return VEM_CORRUPT;
    }
    return VEM_OK;
}

static void ckUnderInst(fct, inst)
octObject *fct;			/* Facet to look at       */
octObject *inst;		/* Newly created instance */
/*
 * This routine checks the area under `inst' for any objects
 * other than itself.  If any are found, a warning is produced.
 * If expert mode is on, the warning is printed to the console.
 * If not, it produces a dialog that allows the user to choose
 * what to do.  He may choose not to proceed and the instance
 * will be deleted.  The routine attaches all of the instances
 * that should be deleted to `dead'.
 */
{
    octObject dest_obj;
    regObjGen gen;
    struct octBox bb;
#ifdef OLD_CODE
    char warning[1024];
#endif
    (void) vemExpert( 0 );

    /* Check for things under instance */
    if ((octBB(inst, &bb) == OCT_OK) &&
	(regObjStart(fct, &bb, OCT_GEO_MASK|OCT_INSTANCE_MASK,
		     &gen, 0) == REG_OK)) {
	while (regObjNext(gen, &dest_obj) == REG_OK) {
	    if (!octIdsEqual(dest_obj.objectId, inst->objectId)) {
#ifdef OLD_CODE		
		if (expert) {
		    /* Warning */
		    vemMsg(MSG_C, 
"WARNING: there are objects intersecting\n\
         %s.\n\
         NO CONNECTIONS MADE to these objects\n", ohFormatName(inst));
		} else {
		    /* Novice dialog */
		    (void) sprintf(warning, 
"There are objects intersecting\n\
%s.\n\
NO CONNECTIONS WILL BE MADE to these objects.\n\
\n\
If you proceed, you can connect to these objects\n\
using the reconnect command.\n\
\n\
Do you still want to create the instance?", ohFormatName(inst));
		    if (dmConfirm("New instance warning",
				  warning, "No", "Yes") == VEM_OK) {
			/* Delete instance */
			seDeleteOne(inst);
		    }
		}
#else 
		{
		    vemMsg(MSG_C, "AUTOCONNECT new instance\n" );
		    symReconnect( inst, symDefConFunc, 0 );
		}
#endif
		break;
	    }
	}
	regObjFinish(gen);
    }
}
