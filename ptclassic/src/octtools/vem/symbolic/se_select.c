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
 * Net selection
 *
 * David Harrison
 * University of California, Berkeley
 * 1988
 * 
 * This command selects the net which contains the objects under
 * the cursor.
 *
 * No changes required but should be VEM specific symbolic code.
 */

#include "general.h"
#include "symbolic.h"
#include "list.h"
#include "oct.h"
#include "commands.h"
#include "message.h"
#include "region.h"
#include "vemDM.h"
#include "vemUtil.h"
#include "se.h"

/* Stolen from selection.c -- better export required */
extern vemStatus selectNetOrBag();

static vemStatus netFormat(msg, code)
STR msg;
vemStatus code;
{
    vemMessage("error: ", MSG_NOLOG|MSG_DISP);
    vemMessage(msg, MSG_NOLOG|MSG_DISP);
    vemMessage("\nformat: [objects] [pnt, box, line, or text] : select-net\n",
	       MSG_NOLOG|MSG_DISP);
    return code;
}


static octObject *seResolveNets(fct, num_nets, nets, help)
octObject *fct;			/* Facet                */
int num_nets;			/* Number of nets  */
octObject *nets;		/* Nets themselves */
char *help;			/* Help message if any  */
/*
 * Posts a dialog asking the user to choose exactly one
 * of the nets listed in `nets'.
 */
{
    octObject *result = (octObject *) 0;
    dmWhichItem *items;
    int i;
    int ri = 0;			/* Selected net. Default is the first. */
    char buf[VEMMAXSTR];

    items = VEMARRAYALLOC(dmWhichItem, num_nets);
    for (i = 0;  i < num_nets;  i++) {
	if (symHumanNamedNetP(&nets[i])) {
	    (void) sprintf(buf, "Net `%s'", nets[i].contents.net.name);
	} else {
	    (void) sprintf(buf, "Machine Named Net");
	}
	items[i].itemName = VEMSTRCOPY(buf);
    }
    if (dmWhichOne("Which Net?",num_nets,items,&ri,(int(*)())0,help)==VEM_OK) {
	if ( ri >= 0 && ri < num_nets ) {
	    result = nets + ri;
	} else {
	    printf( "dmWhichOne glitch %d\n", ri );
	} 
    }
    for (i = 0;  i < num_nets;  i++) VEMFREE(items[i].itemName);
    VEMFREE(items);
    return result;
}

#define MAX_NETS_TO_SELECT 16

void AddNetToArray( array, lastindex, net )
    octObject* array;
    int* lastindex;
    octObject* net;
{
    int i = 0;
    if ( *lastindex == MAX_NETS_TO_SELECT - 1 ) {
	return;			/* Array is full. */
    }
    for ( i = 0; i < *lastindex; i++ ) {
	if ( octIdsEqual( array[i].objectId, net->objectId ) ) {
	    return;
	}
    }
    array[ (*lastindex)++ ] = *net;
}

vemStatus seSelectNet(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This command adds a net to the current argument list and
 * highlights it appropriately.  There are three forms for
 * the command:
 *  [objects] : select-net
 *  [objects] [point, line, or box] : select-net
 *  [objects] "net name" : select-net
 * In all cases `objects' is optional.  In the first case,  the
 * command will attempt to find the net by looking at objects
 * under the cursor where the command was invoked.  In the second case,
 * the same thing is done except the objects under the point, intersecting the
 * line,  or inside the box will be examined.  In the last case,
 * the net is looked up by name.   Note the net is added
 * to the set AS A WHOLE and is only one object.  Other
 * commands must be able to handle this.
 */
{
    octObject nets[MAX_NETS_TO_SELECT];
    int num_nets = 0;
    vemOneArg *firstArg, *lastArg;
    regObjGen theGen;
    octObject netFacet, majorNet, someObj, tempNet;
    struct octBox ext;
    octCoord temp;
    regTermInfo *term_ary;
    int len, inside, num_terms, i;

    seInit();

    /* First,  all kinds of validity checking */
    len = lsLength(cmdList);
    if (len > 2) {
	return netFormat("Too many arguments", VEM_ARGRESP);
    }
    if (vuCkStyle(spot, cmdList) != VEM_OK) return VEM_ARGRESP;
    netFacet.objectId = spot->theFct;
    VEM_OCTCKRVAL(octGetById(&netFacet), VEM_CORRUPT);
    /* Check the argument list for correctness */
    if (len > 0) {
	lsFirstItem(cmdList, (Pointer *) &firstArg, LS_NH);
	lsLastItem(cmdList, (Pointer *) &lastArg, LS_NH);
    }
    if (len == 2) {
	/* Better be objects followed by non-objects */
	if ((firstArg->argType != VEM_OBJ_ARG) ||
	    (lastArg->argType == VEM_OBJ_ARG))
	  {
	      return netFormat("Syntax error", VEM_ARGRESP);
	  }
    }
    if ((len > 0) && (lastArg->argType == VEM_TEXT_ARG)) {
	/* He has given us a net name */
	majorNet.type = OCT_NET;
	majorNet.contents.net.name = lastArg->argData.vemTextArg;
	if (octGetByName(&netFacet, &majorNet) != OCT_OK) {
	    sprintf(errMsgArea, "Unable to find a net named '%s'\n",
		    majorNet.contents.net.name);
	    vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
	    return VEM_ARGRESP;
	}
	lsDelEnd(cmdList, (Pointer *) &lastArg);
	vuOffSelect(lastArg);
    } else {
	if ((len == 0) || (lastArg->argType == VEM_OBJ_ARG)) {
	    /* Use spot */
	    ext.lowerLeft = spot->thePoint;
	    ext.upperRight = ext.lowerLeft;
	    inside = 0;
	} else {
	    switch (lastArg->argType) {
	    case VEM_POINT_ARG:
		if (lastArg->length != 1) {
		    return netFormat("Only one point allowed", VEM_ARGRESP);
		}
		ext.lowerLeft = lastArg->argData.points[0].thePoint;
		ext.upperRight = ext.lowerLeft;
		inside = 0;
		break;
	    case VEM_BOX_ARG:
		if (lastArg->length != 1) {
		    return netFormat("Only one box allowed", VEM_ARGRESP);
		}
		ext.lowerLeft = lastArg->argData.points[0].thePoint;
		ext.upperRight = lastArg->argData.points[1].thePoint;
		VEMBOXNORM(ext, temp);
		inside = 1;
		break;
	    case VEM_LINE_ARG:
		if (lastArg->length != 2) {
		    return netFormat("Only one line allowed", VEM_ARGRESP);
		}
		ext.lowerLeft = lastArg->argData.points[0].thePoint;
		ext.upperRight = lastArg->argData.points[1].thePoint;
		VEMBOXNORM(ext, temp);
		inside = 0;
		break;
	    default:
		return netFormat("Horrible syntax error", VEM_CORRUPT);
	    }
	    vuOffSelect(lastArg);
	    lsDelEnd(cmdList, (Pointer *) &lastArg);
	}
	/* We must find the net using spot */
	majorNet.objectId = oct_null_id;
	if (regObjStart(&netFacet, &ext, 
			OCT_GEO_MASK, &theGen, inside) == REG_FAIL)
	  {
	      vemMessage("Cannot generate paths\n", MSG_NOLOG|MSG_DISP);
	      return VEM_CORRUPT;
	  }
	while (regObjNext(theGen, &someObj) == REG_OK) {
	    /* Got a path - generate its net */
	    if (regFindNet(&someObj, &tempNet) == REG_OK) {
		AddNetToArray( nets, &num_nets, &tempNet );
	    }
	}
	regObjFinish(theGen);
	/* Now try to find an actual under the spot */
	if (regMultiActual(&netFacet, &ext, &num_terms, &term_ary) == REG_OK) {
	    /* Find the net associated with the terminals */
	    for (i = 0;  i < num_terms;  i++) {
		if (regFindNet(&(term_ary[i].term), &tempNet) == REG_OK) {
		    AddNetToArray( nets, &num_nets, &tempNet );
		}
	    }
	}
	if (num_nets == 0) {
	    vemMessage("No net specified\n", MSG_NOLOG|MSG_DISP);
	    return VEM_ARGRESP;
	}
	if (num_nets > 1) {
	    octObject *dummy;
	    dummy = seResolveNets(spot->theFct, num_nets, nets, 0);
	    if (dummy == NIL(octObject)) {
		vemMessage("No net specified\n", MSG_NOLOG|MSG_DISP);
		return VEM_ARGRESP;
	    }
	    majorNet = *dummy;
	} else {
	    majorNet = nets[0];
	}
    }

    /* We now have a valid net in majorNet */
    if (selectNetOrBag(spot->theFct, cmdList, &majorNet, 0) == VEM_OK) {
	if (symHumanNamedNetP(&majorNet)) {
	    sprintf(errMsgArea, "net %s has been selected\n",
		    majorNet.contents.net.name);
	} else {
	    sprintf(errMsgArea, "A machine generated net has been selected\n");
	}
	vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    } else {
	return VEM_CORRUPT;
    }
}


