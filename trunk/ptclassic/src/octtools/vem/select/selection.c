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
 * VEM Selection Commands
 *
 * David Harrison
 * University of California,  Berkeley
 * 1986
 *
 * This file contains commands for doing operations on selected
 * sets.  A selected set is a VEM_OBJ_ARG (an oct bag) which
 * is on the argument list.  The basic selection commands (select
 * and unselect) add and delete objects from the bag.  The selection
 * operations (move, copy, delete, changeLayer) change the objects
 * in the selected set.
 */

#include "general.h"		/* General VEM definitions      */
#include <X11/Xlib.h>		/* C Language Interface to X    */
#include "message.h"		/* Message display module       */
#include "list.h"		/* List handling package        */
#include "oct.h"		/* OCT data manager             */
#include "attributes.h"		/* For attribute handling       */
#include "buffer.h"		/* Buffer management            */
#include "windows.h"		/* Window management            */
#include "selset.h"		/* Selected Sets                */
#include "region.h"		/* Region package               */
#include "commands.h"		/* Definitions for VEM commands */
#include "selection.h"		/* Self declaration             */
#ifdef OLDBAGS
#include "bags.h"		/* Bag name manager             */
#endif
#include "vemUtil.h"		/* Various assistance routines  */
#include "tr.h"			/* Transformation package       */
#include "objsel.h"		/* Object selection package     */
#include "defaults.h"		/* Default handling package     */

#define ARGBAGNAME	";;; ArgBag ;;;"
#define SUBTRACT	1

static vemStatus selTemplate();
static int getNextRotSpec();



vemStatus selectCmd(spot, cmdList)
vemPoint *spot;			/* Where command was invoked */
lsList cmdList;			/* Argument list             */
/*
 * This command selects raw geometry and places it on the argument
 * list.  It takes any number of points, lines, or boxes.  For points,
 * all objects under the point are selected.  For lines,  all objects
 * intersecting the bounding box of the segment are selected.
 * For boxes,  all objects completely contained inside the box
 * are selected.  This routine no longer handles selection
 * conditionalized on a layer.
 */
{
    wnOpts opts;

    if (wnGetInfo(spot->theWin, &opts, (octId *) 0, (octId *) 0,
		  (bdTable *) 0, (struct octBox *) 0) == VEM_OK)
      {
	  return selTemplate(spot, cmdList, vuAddArgToBag,
			     (Pointer) opts.off_layers, 0);
      } else {
	  return selTemplate(spot, cmdList, vuAddArgToBag, (Pointer) 0, 0);
      }
}


vemStatus selLayerCmd(spot, cmdList)
vemPoint *spot;			/* Where command was invoked */
lsList cmdList;			/* Argument list             */
/*
 * This command selects all geometry on a particular layer.
 * This layer is either specified as the last text argument of
 * the command or is inherited by `spot'.  Point, line, and
 * box arguments are all interpreted in the same manner as
 * selectCmd().
 */
{
    octObject spotFacet, layer;
    wnOpts opts;

    spotFacet.objectId = spot->theFct;
    if (octGetById(&spotFacet) != OCT_OK) {
	vemMsg(MSG_A, "Unable to determine buffer of spot:\n  %s\n",
	       octErrorString());
	return VEM_ARGRESP;
    }
    if (vuParseLayer(spot, cmdList, &spotFacet, &layer) == VEM_OK) {
	/* Get layers that are turned off */
	if (wnGetInfo(spot->theWin, &opts, (octId *) 0, (octId *) 0,
		      (bdTable *) 0, (struct octBox *) 0) == VEM_OK)
	  {
	      /* Check to make sure this one is on */
	      if (opts.off_layers &&
		  st_is_member(opts.off_layers, layer.contents.layer.name))
		{
		    sprintf(errMsgArea,
			    "Layer `%s' is turned off in this window\n",
			    layer.contents.layer.name);
		    vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
		    return VEM_ARGRESP;
		}
	  }
	return selTemplate(spot, cmdList, vuAddLyrToBag,
			   (Pointer) layer.contents.layer.name, 0);
    } else {
	vemMessage("Unable to determine conditional layer\n",
		   MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }
}

vemStatus selTermCmd(spot, cmdList)
vemPoint *spot;			/* Where command was invoked */
lsList cmdList;			/* Argument list             */
/*
 * This command selects all terminals that have implementations
 * which intersect the objects found by examining the argument
 * list.  The argument list items have the same semantics as
 * those described for selectCmd.
 */
{
    return selTemplate(spot, cmdList, vuAddTermToBag, (Pointer) 0, 0);
}


vemStatus unSelectCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This command removes objects from a VEM_OBJ_ARG argument assumed
 * to be the first argument on the argument list.  It takes any number 
 * of points and boxes.  Point arguments causes all objects under the 
 * point to be removed from the selected set.   Box 
 * arguments cause all objects COMPLETELY CONTAINED in the box to be removed
 * from the selected set.  No conditionalizing on the layer is supported.
 * This routine is also an argument modification routine and leaves a 
 * single VEM_OBJ_ARG argument on the list.
 * All unselected items become un-highlighted.
 */
{
    wnOpts opts;

    if (wnGetInfo(spot->theWin, &opts, (octId *) 0, (octId *) 0,
		  (bdTable *) 0, (struct octBox *) 0) == VEM_OK)
      {
	  return selTemplate(spot, cmdList, vuSubArgFromBag,
			     (Pointer) opts.off_layers, SUBTRACT);
      } else {
	  return selTemplate(spot, cmdList, vuSubArgFromBag, (Pointer) 0,
			     SUBTRACT);
      }
}



static vemStatus selTemplate(spot, cmdList, func, extra, sub_flag)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
vemStatus (*func)();		/* Function to call         */
Pointer extra;			/* Extra stuff to `func'    */
int sub_flag;			/* If set, subtracts        */
/*
 * This is a template for selection style commands.  It adds (or subtracts)
 * objects to a VEM_OBJ_ARG on the argument list.  It takes
 * any number of points, lines, and boxes as arguments.
 * For each argument type,  the routine calls `func'
 * to find and add the items to a bag.  The routine
 * has the following form:
 *   vemStatus func(fctId, bag, arg, extra)
 *   octId fctId;
 *   octObject *bag;
 *   vemOneArg *arg;
 *   Pointer extra;
 * The routine should add items cooresponding to `arg' to
 * `bag'.  All items selected will be (un)highlighted.  The command 
 * modifies the argument list leaving a single VEM_OBJ_ARG on the list.
 */
{
    vemOneArg *firstArg, *thisArg;
    atrHandle selAttr;
    lsGen theGen;
    octObject argFacet, sptFacet, tempBag, selItem, selBag;
    octGenerator octGen;
    int len, count;
    vemStatus (*add_or_sub)();

    len = lsLength(cmdList);
    /* Lets get a hold of the buffer */
    sptFacet.objectId = spot->theFct;
    if (octGetById(&sptFacet) != OCT_OK) {
	vemMsg(MSG_A, "Cannot get a hold of facet:\n  %s\n", octErrorString());
	return VEM_CORRUPT;
    }
    if (vuCkStyle(spot, cmdList) != VEM_OK) return VEM_ARGRESP;
    if (vuCkBuffer(cmdList, &(argFacet.objectId)) != VEM_OK) return VEM_ARGRESP;
    if (octGetById(&argFacet) != OCT_OK) {
	argFacet = sptFacet;
    }

    /* Is the first item VEM_OBJ_ARG? */
    if ((len == 0) || (lsFirstItem(cmdList, (Pointer *) &firstArg, LS_NH),
		       firstArg->argType != VEM_OBJ_ARG))
      {
	  /* Make a selected set the first argument */
	  dfGetAttr("select", &selAttr);
	  firstArg = vemAttrSelSet(argFacet.objectId, selAttr);

	  /* Put it at the start of the list */
	  lsNewBegin(cmdList, (Pointer) firstArg, LS_NH);
      }

    /*
     * At this point,  we have a VEM_OBJ_ARG as the first item in
     * the command list and this item is also in 'firstArg'.
     * We start at the second argument and add the necessary items 
     * to the bag and select them using the basic selection routines 
     * provided in selset.c.
     */
    count = 0;
    if (sub_flag == SUBTRACT) add_or_sub = vemDelSelSet;
    else add_or_sub = vemAddSelSet;
    selBag.objectId = firstArg->argData.vemObjArg.theBag;
    VEM_OCTCKRVAL(octGetById(&selBag), VEM_CORRUPT);
    theGen = lsStart(cmdList);
    while (lsNext(theGen, (Pointer *) &thisArg, LS_NH) == LS_OK) {
	if (count > 0) {
	    /* New argument.  Make a new temporary bag */
	    tempBag.type = OCT_BAG;
	    tempBag.contents.bag.name = ARGBAGNAME;
	    if (octCreate(&argFacet, &tempBag) != OCT_OK)
	      continue;		/* Can't create bag - go on to next */
	    if ((*func)(&argFacet, &tempBag, thisArg, &selBag, extra) != VEM_OK)
	      continue;
	    /* Clean out the argument */
	    vuOffSelect(thisArg);
	    /* Now add the items to the selected set */
	    if (octInitGenContents(&tempBag, OCT_ALL_MASK, &octGen) != OCT_OK)
	      continue;		/* Again,  try to push on  */
	    
	    while (octGenerate(&octGen, &selItem) == OCT_OK) {
		/* Turn on selection */
		(*add_or_sub)(firstArg, selItem.objectId);
	    }
	    /* Get rid of the temporary bag */
	    octDelete(&tempBag);
	    /* Delete this item from the argument list */
	    lsDelBefore(theGen, (Pointer *) &thisArg);
	}
	count++;
    }
    if (count <= 1) {
	vemOneArg tempArg;

	/* No additional stuff - select what's under spot */
	tempArg.argType = VEM_POINT_ARG;
	tempArg.length = 1;
	tempArg.alloc_size = 1;
	tempArg.argData.points = spot;
	/* Make temporary bag */
	tempBag.type = OCT_BAG;
	tempBag.contents.bag.name = ARGBAGNAME;
	if ((octCreate(&sptFacet, &tempBag) == OCT_OK) &&
	    ((*func)(&sptFacet, &tempBag, &tempArg, &selBag, extra) == VEM_OK) &&
	    (octInitGenContents(&tempBag, OCT_ALL_MASK, &octGen) == OCT_OK))
	  {
	      while (octGenerate(&octGen, &selItem) == OCT_OK) {
		  /* Turn on selection */
		  (*add_or_sub)(firstArg, selItem.objectId);
	      }
	      octDelete(&tempBag);
	  }
    }
    if (firstArg->length == 0) {
	/* No objects in bag -- wipe the current list */
	vemMessage("No objects selected\n", MSG_NOLOG|MSG_DISP);
	return VEM_CORRUPT;
    } return VEM_ARGMOD;
}




vemStatus selectNetOrBag(netBagFctId, cmdList, netOrBag, conFlag)
octId netBagFctId;		/* What facet is net in     */
lsList cmdList;			/* Argument list            */
octObject *netOrBag;		/* Net or bag object        */
int conFlag;			/* Contents or not          */
/*
 * This routine adds a net or bag to the given argument list.
 * The routine adds the net or bag as a whole to the argument
 * list.  The implementation of the net or bag is automatically
 * highlighted using the selset routine vemAddSelSet.
 * This is not meant to be a command.  It is called by some
 * command which has already made sure the argument list contains
 * no arguments or one argument of type VEM_OBJ_ARG.  If 'conFlag'
 * is non-zero,  the items attached to the net or bag is selected
 * rather than the net or bag itself.
 */
{
    vemOneArg *firstArg;
    atrHandle selAttr;
    octObject newItem;
    octGenerator theGen;
    int len;

    len = lsLength(cmdList);
    if (len > 0) {
	lsFirstItem(cmdList, (Pointer *) &firstArg, LS_NH);
    } else {
	/* Create a new set */
	dfGetAttr("select", &selAttr);
	firstArg = vemAttrSelSet(netBagFctId, selAttr);

	/* Put it at the start of the list */
	lsNewBegin(cmdList, (Pointer) firstArg, LS_NH);
    }
    /* 
     * The first argument is now an object argument and the
     * bag to attach to is in 'argBag'.  
     */
    if (conFlag) {
	/* Attach items attached to it */
	if (octInitGenContents(netOrBag, OCT_ALL_MASK, &theGen) == OCT_OK) {
	    while (octGenerate(&theGen, &newItem) == OCT_OK) {
		if (!vemAddSelSet(firstArg, newItem.objectId)) {
		    vemMsg(MSG_C,"Warning: could not select %s.\n",
			   vuDispObject(netOrBag));
		}
	    }
	} else {
	    sprintf(errMsgArea,
		    "Unable to generate items from net or bag:\n  %s\n",
		    octErrorString());
	    vemMessage(errMsgArea, MSG_DISP);
	}
    } else {
	/* Attach the whole thing */
	if (!vemAddSelSet(firstArg, netOrBag->objectId)) {
	    vemMsg(MSG_C, "Warning: could not select %s.\n",
		   vuDispObject(netOrBag));
	}
    }
    return VEM_OK;
}



/*
 * The folloing is for use by other modules who require the deletion of a
 * selected set on the argument list.
 */

extern vemStatus selectWipe(arg)
vemOneArg *arg;
/*
 * This routine takes a VEM_OBJ_ARG argument and turns off
 * highlighting of the objects in the argument bag and releases
 * all preallocated space for the argument.  The argument itself
 * is also freed (using free).  It can be deleted from an
 * argument using lsDelete.  Its primary user is vuOffSelect
 * in octUtil.c.
 */
{
    atrHandle selAttr;

    return vemFreeAttrSet(arg, &selAttr);
}



/*ARGSUSED*/
vemStatus selTransCmd(spot, cmdList)
vemPoint *spot;
lsList cmdList;
/*
 * Takes a selected set of objects and transforms them.  The objects
 * remain on the selected set.  The command takes (up to) three arguments:
 * a set of objects to work on,  a text rotation specification and 
 * two points indicating a relative translation.  The set of objects
 * must be supplied.  At least one form of transform must be given.
 * The rotation specification is a list of keywords separated by colons:
 *   mx		Mirror X
 *   my		Mirror Y
 *   90		Rotate 90 degrees
 *  180		Rotate 180 degrees
 *  270		Rotate 270 degrees
 * If both the rotation and translation are given,  the rotation specification
 * should come first.
 */
{
    vemOneArg *theArg, *endArg;
    tr_stack *local_stack;
    STR rotspec;
    char *rotidx, rotbuf[VEMMAXSTR];
    struct octPoint off1, off2;
    struct octTransform newTransform;
    struct octBox selBB;
    struct octPoint selCenter;
    octTransformType op;
    int len, snap_val;

    len = lsLength(cmdList);
    if ((len < 1) || (len > 3)) {
	vemMessage("format: [objects] [\"90:180:270:mx:my\"] [pnts] transform\n",
		   MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }
    if (vuCkStyle(spot, cmdList) != VEM_OK) return VEM_ARGRESP;
    lsFirstItem(cmdList, (Pointer *) &theArg, LS_NH);
    if (theArg->argType != VEM_OBJ_ARG) {
	vemMessage("format: [objects] [\"90:180:270:mx:my\"] [pnts] transform\n",
		   MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }	

    rotspec = (char *) 0;
    off1.x = off1.y = 0;  off2 = off1;
    switch (len) {
    case 1:
	/* Rotate 90 degrees */
	rotspec = "90";
	break;
    case 2:
	/* Figure out whether its rotation or translation */
	lsLastItem(cmdList, (Pointer *) &endArg, LS_NH);
	if (endArg->argType == VEM_TEXT_ARG) {
	    STRMOVE(rotbuf, endArg->argData.vemTextArg);
	    rotspec = rotbuf;
	} else if (endArg->argType == VEM_POINT_ARG) {
	    if (endArg->length == 2) {
		off1 = endArg->argData.points[0].thePoint;
		off2 = endArg->argData.points[1].thePoint;
	    } else {
		vemMessage("Only two points for offset", MSG_NOLOG|MSG_DISP);
		return VEM_ARGRESP;
	    }
	} else {
	    vemMessage("format: [objects] [\"90:180:270:mx:my\"] [pnts] transform\n",
		       MSG_NOLOG|MSG_DISP);
	    return VEM_ARGRESP;
	}
	/* Eat this argument */
	vuOffSelect(endArg);
	lsDelEnd(cmdList, (Pointer *) &endArg);
	break;
    case 3:
	/* Both rotation and translation */
	lsLastItem(cmdList, (Pointer *) &endArg, LS_NH);
	if (endArg->argType != VEM_POINT_ARG) {
	    /* Format error */
	    vemMessage("format: [objects] [\"90:180:270:mx:my\"] [pnts] transform\n",
		       MSG_NOLOG|MSG_DISP);
	    return VEM_ARGRESP;
	}
	if (endArg->length != 2) {
	    vemMessage("Only two points for offset", MSG_NOLOG|MSG_DISP);
	    return VEM_ARGRESP;
	}
	off1 = endArg->argData.points[0].thePoint;
	off2 = endArg->argData.points[1].thePoint;
	/* Eat the argument */
	vuOffSelect(endArg);
	lsDelEnd(cmdList, (Pointer *) &endArg);
	/* Now the end argument should be the text specification */
	lsLastItem(cmdList, (Pointer *) &endArg, LS_NH);
	if (endArg->argType != VEM_TEXT_ARG) {
	    /* Format error */
	    vemMessage("format: [objects] [\"90:180:270:mx:my\"] [pnts] transform\n",
		       MSG_NOLOG|MSG_DISP);
	    return VEM_ARGRESP;
	}
	STRMOVE(rotbuf, endArg->argData.vemTextArg);
	rotspec = rotbuf;
	/* Eat this argument */
	vuOffSelect(endArg);
	lsDelEnd(cmdList, (Pointer *) &endArg);
	break;
    }

    /* All argument parsing is done.  Now apply transform to argument */
    local_stack = tr_create();
    tr_identity(local_stack);
    tr_add_transform(local_stack, &(theArg->argData.vemObjArg.tran), 0);

    if (rotspec) {
	/* Rotated with respect to its center (snapped) */
	if (objTranBB(&(theArg->argData.vemObjArg), &selBB) == VEM_OK) {
	    selCenter.x = (selBB.lowerLeft.x + selBB.upperRight.x)/2;
	    selCenter.y = (selBB.lowerLeft.y + selBB.upperRight.y)/2;

	    /* Snap to snap point */
	    dfGetInt("snap", &snap_val);
	    selCenter.x = (selCenter.x / snap_val) * snap_val;
	    selCenter.y = (selCenter.y / snap_val) * snap_val;

	    tr_translate(local_stack, -selCenter.x, -selCenter.y, 1);

	    rotidx = (char *) rotspec;
	    while (getNextRotSpec(&rotidx, &op)) {
		tr_do_op(local_stack, op, 1);
	    }
	    tr_translate(local_stack, selCenter.x, selCenter.y, 1);
	}
    }
    /* Then translation (if any) */
    tr_translate(local_stack, off2.x - off1.x, off2.y - off1.y, 1);

    /* Extract the transform and apply */
    tr_get_oct_transform(local_stack, &newTransform);

    objTran(&(theArg->argData.vemObjArg), &newTransform);
    tr_free(local_stack);
    return VEM_ARGMOD;
}


static int getNextRotSpec(rotidx, nextop)
char **rotidx;			/* Index into rotate string */
octTransformType *nextop;	/* Next rotation type       */
/*
 * This routine parses a string of the form "xx:xx:xx" where
 * the `xx's are one of: `mx', `my', `90', `180', `270'.
 * It returns the proper type for these keywords (TR_MIRROR_X,
 * TR_MIRROR_Y, TR_ROT90, TR_ROT180, TR_ROT270) in `nextop'.
 * The routine returns zero if it has reached the end of the
 * string or there is a parsing problem.
 */
{
    char *start;
    int len;

    if (**rotidx && (**rotidx == ':')) {
	start = ++(*rotidx);
    } else {
	start = *rotidx;
    }
    while (**rotidx && (**rotidx != ':')) {
	*rotidx += 1;
    }
    if (start == *rotidx) return 0;
    len = (*rotidx - start);
    if (STRNCOMP(start, "90", len) == 0) {
	*nextop = TR_ROT90;
    } else if (STRNCOMP(start, "180", len) == 0) {
	*nextop = TR_ROT180;
    } else if (STRNCOMP(start, "270", len) == 0) {
	*nextop = TR_ROT270;
    } else if (STRNCOMP(start, "mx", len) == 0) {
	*nextop = TR_MIRROR_X;
    } else if (STRNCOMP(start, "my", len) == 0) {
	*nextop = TR_MIRROR_Y;
    } else {
	*nextop = TR_NO_TRANSFORM;
    }
    return 1;
}
