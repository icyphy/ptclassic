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
 * VEM Utility Routines
 * Parsing Assistance
 * 
 * David Harrison
 * University of California,  Berkeley
 * 1987
 *
 * This file contains utility routines for various forms of
 * parsing assistance.  This includes parsing facet and instance
 * specifications,  common forms of argument lists,  etc.
 */

#include "general.h"		/* General VEM definitions */
#include <X11/Xlib.h>		/* C language interface to X */
#include "oct.h"		/* Oct data manager        */
#include "vemUtil.h"		/* Self declaration        */
#include "buffer.h"		/* Buffer managment        */
#include "message.h"		/* Message handling        */
#include "list.h"		/* List handling package   */
#include "commands.h"		/* Command formats         */
#include "oh.h"			/* Oct helper library      */

#define MAX_SPEC_SIZE	1024


#ifdef OLDPARSE

static void unpack_facet_name(name, facet)
char *name;
octObject *facet;
/*
 * Unpacks a facet name into a facet.  Written by R. Rudell.
 * Adapted to VEM by D. Harrison.
 */
{
    static char fct[MAX_SPEC_SIZE];
    char *s;

    /* Check for silly input */
    if (name == NIL(char) || strcmp(name, "") == 0) return;
	
    /* Make a copy of the input string (because we will tear it apart) */ 
    VEM_CHECK(STRLEN(name) < MAX_SPEC_SIZE, "Facet specification too big");
    STRMOVE(fct, name);
    s = fct;

    if (s[0] != ':') facet->contents.facet.cell = s;
    if ((s = strchr(s, ':')) != NIL(char)) {
	*s++ = '\0';
	if (s[0] != ':') facet->contents.facet.view = s;
	if ((s = strchr(s, ':')) != NIL(char)) {
	    *s++ = '\0';
	    if (s[0] != ':') facet->contents.facet.facet = s;
	    if ((s = strchr(s, ':')) != NIL(char)) {
		*s++ = '\0';
		if (s[0] != ':') facet->contents.facet.version = s;
	    }
	}
    }
}


vemStatus vuParseFacet(facetSpec, theFacet)
STR facetSpec;			/* String specification of facet */
octObject *theFacet;		/* Facet to fill in              */
/*
 * This routine parses a facet specification.  The form of a facet
 * specification is as follows:
 *   "cell:view[:facet[:version]]"
 */
{
    if (!facetSpec) return VEM_NOTSUFF;

    theFacet->type = OCT_FACET;
    theFacet->objectId = oct_null_id;
    theFacet->contents.facet.cell = "";
    theFacet->contents.facet.view = "physical";
    theFacet->contents.facet.facet = "contents";
    theFacet->contents.facet.version = OCT_CURRENT_VERSION;

    unpack_facet_name(facetSpec, theFacet);
    return VEM_OK;
}

#endif /* OLDPARSE*/

vemStatus vuParseInst(masterSpec, inst, rtn)
STR masterSpec;			/* Master specification string  */
octObject *inst;		/* Instance template to fill in */
STR *rtn;			/* Remaining portion of string  */
/*
 * This routine parses an instance specification string of
 * the form:  "cell:view{:facet{:version}} {name}".
 * If the specification is missing,  it will return VEM_FALSE
 * (but still fill in the name).
 */
{
    octObject theFacet;
    static char ispec[MAX_SPEC_SIZE];
    char *p, *iv, *in;
    
    if (!masterSpec) return VEM_FALSE;

    VEM_CHECK(STRLEN(masterSpec) < MAX_SPEC_SIZE, "Instance spec too big");
    STRMOVE(ispec, masterSpec);
    p = ispec;

    iv = p;
    in = *rtn = (char *) 0;
    if ((p = strchr(p, ' ')) != NIL(char)) {
	*p++ = '\0';
	in = p;
	if ((p = strchr(p, ' ')) != NIL(char)) {
	    *p++ = '\0';
	    *rtn = p;
	}
    }

    if (strchr(iv, ':') != NIL(char)) {
	theFacet.contents.facet.cell = inst->contents.instance.master;
	theFacet.contents.facet.view = inst->contents.instance.view;
	theFacet.contents.facet.facet = inst->contents.instance.facet;
	theFacet.contents.facet.version = OCT_CURRENT_VERSION;
	if (ohUnpackFacetName(&theFacet, iv) == OCT_OK) {
	    inst->contents.instance.master = theFacet.contents.facet.cell;
	    inst->contents.instance.view = theFacet.contents.facet.view;
	    inst->contents.instance.facet = theFacet.contents.facet.facet;
	    inst->contents.instance.version = theFacet.contents.facet.version;
	    if (in) {
		inst->contents.instance.name = in;
	    }
	    return VEM_OK;
	} else {
	    inst->contents.instance.name = iv;
	    *rtn = in;
	    return VEM_FALSE;
	}
    } else {
	inst->contents.instance.name = iv;
	*rtn = in;
	return VEM_FALSE;
    }
}



static vemStatus ckStyleError(style)
int style;			/* An editing style */
/*
 * Produces an error message for ckList and returns VEM_FALSE.
 */
{
    switch (style) {
    case BUF_PHYSICAL:
	vemMessage("All arguments must be in PHYSICAL style cells\n",
		   MSG_NOLOG|MSG_DISP);
	break;
    case BUF_SYMBOLIC:
	vemMessage("All arguments must be in SYMBOLIC style cells\n",
		   MSG_NOLOG|MSG_DISP);
	break;
    case BUF_SCHEMATIC:
	vemMessage("All arguments must be in SCHEMATIC style cells\n",
		   MSG_NOLOG|MSG_DISP);
	break;
    default:
	vemMessage("Destination buffer has no style\n", MSG_NOLOG|MSG_DISP);
	break;
    }
    return VEM_FALSE;
}

vemStatus vuCkStyle(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* Argument list            */
/*
 * Makes sure the editing style of `spot' is the same as all
 * arguments in `cmdList'.  If not,  it outputs an error
 * message automatically and returns VEM_FALSE.
 */
{
    vemOneArg *theArg;
    lsGen theGen;
    int the_style, idx;

    if (!spot) return VEM_OK;
    the_style = bufStyle(spot->theFct);
    theGen = lsStart(cmdList);
    while (lsNext(theGen, (Pointer *) &theArg, LS_NH) == LS_OK) {
	switch (theArg->argType) {
	case VEM_POINT_ARG:
	case VEM_LINE_ARG:
	    for (idx = 0;  idx < theArg->length;  idx++) {
		if (bufStyle(theArg->argData.points[idx].theFct)
		    != the_style)
		  {
		      lsFinish(theGen);
		      return ckStyleError(the_style);
		  }
	    }
	    break;
	case VEM_BOX_ARG:
	    for (idx = 0;  idx < theArg->length*2;  idx += 2) {
		if (bufStyle(theArg->argData.points[idx].theFct)
		    != the_style)
		  {
		      lsFinish(theGen);
		      return ckStyleError(the_style);
		  }
	    }
	    break;
	case VEM_OBJ_ARG:
	    if (bufStyle(theArg->argData.vemObjArg.theFct) != the_style) {
		lsFinish(theGen);
		return ckStyleError(the_style);
	    }
	    break;
	}
    }
    lsFinish(theGen);
    return VEM_OK;
}


static vemStatus ckBufferError(arg_num)
int arg_num;
/*
 * Produces an error message for vemCkBuffer() and returns VEM_FALSE.
 */
{
    vemMsg(MSG_C, "Argument %d is not in the same buffer as others\n",
	   arg_num);
    return VEM_FALSE;
}

vemStatus vuCkBuffer(cmdList, fctId)
lsList cmdList;			/* Argument list  */
octId *fctId;			/* Returned facet */
/*
 * This routine returns VEM_OK if all supplied arguments are in
 * the same facet.  It outputs an error message and returns
 * VEM_FALSE if this check fails.
 */
{
    vemOneArg *theArg;
    lsGen theGen;
    octId the_fct;
    int arg_count, idx;

    the_fct = oct_null_id;
    theGen = lsStart(cmdList);
    arg_count = 1;
    while (lsNext(theGen, (Pointer *) &theArg, LS_NH) == LS_OK) {
	switch (theArg->argType) {
	case VEM_POINT_ARG:
	case VEM_LINE_ARG:
	    for (idx = 0;  idx < theArg->length;  idx++) {
		if (!the_fct) the_fct = theArg->argData.points[idx].theFct;
		if (theArg->argData.points[idx].theFct != the_fct) {
		    lsFinish(theGen);
		    return ckBufferError(arg_count);
		}
	    }
	    break;
	case VEM_BOX_ARG:
	    for (idx = 0;  idx < theArg->length*2;  idx += 2) {
		if (!the_fct) the_fct = theArg->argData.points[idx].theFct;
		if (theArg->argData.points[idx].theFct != the_fct) {
		      lsFinish(theGen);
		      return ckBufferError(arg_count);
		  }
	    }
	    break;
	case VEM_OBJ_ARG:
	    if (!the_fct) the_fct = theArg->argData.vemObjArg.theFct;
	    if (theArg->argData.vemObjArg.theFct != the_fct) {
		lsFinish(theGen);
		return ckBufferError(arg_count);
	    }
	    break;
	}
	arg_count += 1;
    }
    lsFinish(theGen);
    *fctId = the_fct;
    return VEM_OK;
}



vemStatus vuParseLayer(spot, cmdList, sptFacet, lyr)
vemPoint *spot;			/* Where command was invoked */
lsList cmdList;			/* Argument list             */
octObject *sptFacet;		/* Facet of the spot         */
octObject *lyr;			/* Returned layer object     */
/*
 * This routine determines the layer specification of a command
 * given its spot and its argument list.  If the last argument
 * is text,  it is taken as the layer name and removed from
 * the argument list.  Otherwise,  it attempts to find the layer
 * using `spot'.  It returns zero if it can't determine a layer.
 * Note: the layer is not verified.
 */
{
    static char layerName[80];
    vemOneArg *lastArg;

    lsLastItem(cmdList, (Pointer *) &lastArg, LS_NH);
    lyr->type = OCT_LAYER;
    if (lastArg && (lastArg->argType == VEM_TEXT_ARG)) {
	/* We have been supplied with a layer name */

	lyr->contents.layer.name = layerName;
	STRMOVE(layerName, lastArg->argData.vemTextArg);
	vuOffSelect(lastArg);
	lsDelEnd(cmdList, (Pointer *) &lastArg);
    } else {
	lyr->contents.layer.name = layerName;
	if (vuFindLayer(sptFacet, &(spot->thePoint),
			lyr->contents.layer.name) != VEM_OK)
	  return VEM_FALSE;
    }
    return VEM_OK;
}
