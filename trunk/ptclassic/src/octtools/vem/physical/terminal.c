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
 * VEM Physical Editing Commands
 *
 * David Harrison
 * University of California,  Berkeley
 * 1986, 1987
 *
 * This file contains commands for dealing with terminals.
 * The current exported commands are:
 *   phyTermCmd:	Creates a new terminal
 */

#include "general.h"		/* General VEM defintions */
#include <X11/Xlib.h>		/* C language interface to X */
#include "oct.h"		/* Oct Data Manager       */
#include "windows.h"		/* Window management      */
#include "commands.h"		/* Argument list defns    */
#include "list.h"		/* List handling routines */
#include "region.h"		/* Region package         */
#include "message.h"		/* Message handler        */
#include "vemDM.h"		/* Simple dialog interface */
#include "vemUtil.h"		/* General VEM utilities  */

#define PHYTEMPBAGNAME	";;; PhysTempBag ;;;"

static STR phyBagName = (STR) 0;



static vemStatus termFormat(msg, retcode)
STR msg;
vemStatus retcode;
/*
 * Prints out the format of the physical terminal command along
 * with `msg'.  Returns `retcode'.
 */
{
    vemMessage(msg, MSG_NOLOG|MSG_DISP);
    vemMessage("format: \"terminal name\" : terminal (over implementing geometry)\n",
	       MSG_NOLOG|MSG_DISP);
    vemMessage("        objects \"terminal name\" : terminal\n",
	       MSG_NOLOG|MSG_DISP);
    return retcode;
}



static vemStatus termFetch(fctId, name, termFacet, term)
octId fctId;			/* Containing facet id */
STR name;			/* Name of terminal  */
octObject *termFacet;		/* Returned facet    */
octObject *term;		/* Returned terminal */
/*
 * This routine creates a new formal terminal named `name' in
 * the buffer `fctId'.  If a terminal named `name' already exists,
 * all of its implementing geometry is detached from it and
 * the necessary regions for these geometries are queued for
 * redraw.
 */
{
    octGenerator theGen;
    octObject implGeo;
    struct octBox bb;
    char area[1024];

    termFacet->objectId = fctId;
    if (octGetById(termFacet) != OCT_OK) return VEM_CORRUPT;

    term->type = OCT_TERM;
    term->contents.term.name = name;
    term->contents.term.instanceId = (octId) 0;
    term->contents.term.width = 1;
    if (octGetByName(termFacet, term) >= OCT_OK) {
	/* Already a terminal named this */
	(void) sprintf(area, "There is already a terminal\nnamed `%s'.  Do you\nreally\
 want to replace it?", term->contents.term.name);
	if (dmConfirm("Non-unique Formal Terminal", area, "Yes", "No") != VEM_OK) {
	    return VEM_FALSE;
	}
	/* Detach each item queueing up its bounding box for redisplay */
	VEM_OCTCKRVAL(octInitGenContents(term, OCT_GEO_MASK, &theGen), VEM_CORRUPT);
	while (octGenerate(&theGen, &implGeo) == OCT_OK) {
	    if (octBB(&implGeo, &bb) == OCT_OK) {
#ifndef OLD_DISPLAY
		wnQRedisplay(fctId, &bb);
#else
		wnQRegion(fctId, &bb);
#endif
	    }
	    VEM_OCTCKRVAL(octDetach(term, &implGeo), VEM_CORRUPT);
	}
    } else {
	/* Can't find the terminal -- create it */
	VEM_OCTCKRVAL(octCreate(termFacet, term), VEM_CORRUPT);
	return VEM_OK;
    }
    return VEM_OK;
}


/*
 * Heuristic Formal Terminal Implementation Scheme
 */

typedef struct cen_item {
    struct octPoint center;
    struct octBox bb;
} cenItem;

/* Computes center COMPCENTER(struct octBox *bb, struct octPoint *cen) */
#define COMPCENTER(bb, cen) \
(cen)->x = ((bb)->lowerLeft.x + (bb)->upperRight.x) / 2; \
(cen)->y = ((bb)->lowerLeft.y + (bb)->upperRight.y) / 2;

static void findCenter(cl, bb)
lsList cl;			/* List of all coincident centers */
struct octBox *bb;		/* Implementing geo bb            */
/*
 * This routine trys to find the center of `bb' in the list `cl'.
 * If its found,  `bb' is merged with the currently kept bounding
 * box for that center.  If its not found,  a new item is created
 * and put into the list with `bb' as its initial bounding box.
 */
{
    struct octPoint center;
    cenItem *nextItem;
    lsGen theGen;
    lsStatus retcode;

    COMPCENTER(bb, &center);
    /* Look for it in the current list */
    theGen = lsStart(cl);
    while ((retcode = lsNext(theGen, (lsGeneric *) &nextItem, LS_NH)) == LS_OK) {
	if ((center.x == nextItem->center.x)&&(center.y == nextItem->center.y))
	  break;
    }
    lsFinish(theGen);
    if (retcode == LS_OK) {
	/* Merge in bounding box */
	nextItem->bb.lowerLeft.x = VEMMIN(nextItem->bb.lowerLeft.x,
					  bb->lowerLeft.x);
	nextItem->bb.lowerLeft.y = VEMMIN(nextItem->bb.lowerLeft.y,
					  bb->lowerLeft.y);
	nextItem->bb.upperRight.x = VEMMAX(nextItem->bb.upperRight.x,
					   bb->upperRight.x);
	nextItem->bb.upperRight.y = VEMMAX(nextItem->bb.upperRight.y,
					   bb->upperRight.y);
    } else {
	/* Make a new item */
	nextItem = VEMALLOC(cenItem);
	nextItem->center = center;
	nextItem->bb = *bb;
	lsNewBegin(cl, (lsGeneric) nextItem, LS_NH);
    }
}

static vemStatus findSmallCen(cl, cen, spot)
lsList cl;			/* List of coincident centers */
struct octPoint *cen;		/* Returned center            */
struct octPoint *spot;		/* Disambiguation point       */
/*
 * Finds the smallest bounding box in the list of coincident centers.
 * If there are none,  it will return VEM_FALSE.  As a major side-effect,
 * the list itself is destroyed.  If there is a tie (different centers,
 * same bbs),  the ones with a center closest to `spot' are chosen.
 */
{
    int low_perim = VEMMAXINT;
    int this_perim;
    cenItem *nextItem;
    lsGen theGen;

    if (lsLength(cl) == 0) {
	/* Nothing appropriate */
	lsDestroy(cl, (void (*)()) VEMFREENAME);
	return VEM_FALSE;
    } else {
	/* Ready to check */
	theGen = lsStart(cl);
	while (lsNext(theGen, (lsGeneric *) &nextItem, LS_NH) == LS_OK) {
	    this_perim = VEMABS(nextItem->bb.upperRight.x - nextItem->bb.lowerLeft.x) +
	      VEMABS(nextItem->bb.upperRight.y - nextItem->bb.lowerLeft.y);
	    if (this_perim < low_perim) {
		*cen = nextItem->center;
		low_perim = this_perim;
	    } else if (this_perim == low_perim) {
		int dist1, dist2;

		/* Which one is closer to `spot'? */
		dist1 = VEMDIST(*cen, *spot);
		dist2 = VEMDIST(nextItem->center, *spot);
		if (dist2 < dist1) {
		    *cen = nextItem->center;
		}
	    }
	    lsDelBefore(theGen, (lsGeneric *) &nextItem);
	    VEMFREE(nextItem);
	}
	lsFinish(theGen);
	lsDestroy(cl, (void (*)()) VEMFREENAME);
	return VEM_OK;
    }
}

static int termSpot(theFacet, pos, theBag)
octObject *theFacet;		/* Facet containing geos     */
struct octPoint *pos;		/* Where command was invoked */
octObject *theBag;		/* Returned geometry         */
/*
 * This routine figures out the implementing geometry for a terminal
 * given a single location in the facet.  The algorithm for accomplishing
 * this feat is heuristic:  the set of geometry with coincident bounding-box
 * centers with the overall smallest bounding box is chosen as the
 * implementation set for the terminal.  This set is returned in
 * 'theBag' along with the number of geometries specified.  Note
 * the routine can return zero.
 */
{
    lsList centers;		/* List of all centers */
    struct octPoint finalCen;	/* Final center        */
    struct octBox tempBB;
    struct octPoint oneCen;
    octObject nextGeo;
    regObjGen theGen;
    octGenerator octGen;
    int count;

    centers = lsCreate();
    /* Pass one:  find all co-incident centers and their bounding boxes */
    tempBB.lowerLeft = tempBB.upperRight = *pos;
    if (regObjStart(theFacet, &tempBB, OCT_GEO_MASK, &theGen, 0) == REG_FAIL) {
	vemMessage("regObjStart failed", 0);
	return 0;
    }
    count = 0;
    while (regObjNext(theGen, &nextGeo) == REG_OK) {
	if (octBB(&nextGeo, &tempBB) == OCT_OK) {
	    /* Attach all geos to bag for first pass */
	    octAttach(theBag, &nextGeo);
	    count++;
	    /* Fold center into list of centers */
	    findCenter(centers, &tempBB);
	}
    }
    /* Find the smallest of the centers */
    if (findSmallCen(centers, &finalCen, pos) == VEM_OK) {
	/* Remove geos that don't have this center */
	if (octInitGenContents(theBag, OCT_GEO_MASK, &octGen) < OCT_OK) {
	    (void) sprintf(errMsgArea, "Bag generation failed:\n  %s\n",
		    octErrorString());
	    vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
	    return 0;
	}
	while (octGenerate(&octGen, &nextGeo) == OCT_OK) {
	    octBB(&nextGeo, &tempBB);
	    COMPCENTER(&tempBB, &oneCen);
	    if ((oneCen.x != finalCen.x) || (oneCen.y != finalCen.y)) {
		/* Remove it */
		octDetach(theBag, &nextGeo);
		count--;
	    }
	}
    }
    return count;
}

static vemStatus termAttach(fctId, theTerm, theBag, count)
octId fctId;			/* Containing facet id         */
octObject *theTerm;		/* Terminal to attach items to */
octObject *theBag;		/* Items to attach to term     */
int count;			/* Number of items to attach   */
/*
 * This routine attaches implementing geometry in `theBag' to
 * `theTerm'.  If there aren't any implementing geometries (i.e.
 * count is zero),  the routine deletes the terminal.  A redisplay
 * of the geos to show the implementation is done.
 */
{
    octGenerator theGen;
    octObject nextGeo;
    struct octBox tempBB;

    if (count > 0) {
	/* Implementing geometry */
	if (octInitGenContents(theBag, OCT_GEO_MASK, &theGen) < OCT_OK)
	  return VEM_CORRUPT;
	while (octGenerate(&theGen, &nextGeo) == OCT_OK) {
	    /* Attach and queue region */
	    VEM_OCTCKRVAL(octAttach(theTerm, &nextGeo), VEM_CORRUPT);
	    if (octBB(&nextGeo, &tempBB)) {
#ifndef OLD_DISPLAY
		wnQRedisplay(fctId, &tempBB);
#else
		wnQRegion(fctId, &tempBB);
#endif
	    }
	}
    } else {
	if (octDelete(theTerm) == OCT_OK) {
	    (void) sprintf(errMsgArea, "Deleted terminal `%s'\n",
		    theTerm->contents.term.name);
	    vemMessage(errMsgArea, MSG_DISP|MSG_NOLOG);
	} else {
	    (void) sprintf(errMsgArea, "Unable to delete terminal `%s':\n  %s\n",
		    theTerm->contents.term.name, octErrorString());
	    vemMessage(errMsgArea, MSG_DISP|MSG_NOLOG);
	}
    }
    return VEM_OK;
}


vemStatus phyTermCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This is the basic physical terminal creation command.  It has
 * two forms:  one where only the terminal name is provided and
 * implementing geometry is determined by the spot where the
 * command was invoked,  and a form where the implementing geometry
 * is provided in the form of objects on the argument list.  In
 * the first case,  the implementing geometry is determined
 * by the set of geometry with co-incident centers under the
 * spot which has the smallest overall bounding box.  See
 * termSpot for details.
 */
{
    vemOneArg *firstArg, *lastArg;
    octId targetFctId;
    octObject newTerm, termFacet,tempBag;
    int len, count;

    /* Figure out the buffer we are working in */
    targetFctId = spot->theFct;
    if (bufWritable(targetFctId) == VEM_FALSE) {
	vemMsg(MSG_C, "Buffer is read only\n");
	return VEM_CORRUPT;
    }
    /* Next,  parse the argument list */
    len = lsLength(cmdList);
    if ((len < 1) || (len > 2))
      return termFormat("Wrong number of arguments\n", VEM_CORRUPT);
    if (vuCkStyle(spot, cmdList) != VEM_OK) return VEM_ARGRESP;

    lsFirstItem(cmdList, (lsGeneric *) &firstArg, LS_NH);
    switch (firstArg->argType) {
    case VEM_TEXT_ARG:
	/* Can't be a second arg */
	if (len > 1) return termFormat("Too many arguments\n", VEM_CORRUPT);
	if (termFetch(targetFctId, firstArg->argData.vemTextArg,
		      &termFacet, &newTerm) != VEM_OK)
	  {
	      return VEM_CORRUPT;
	  }
	if (!phyBagName) {
	    phyBagName = PHYTEMPBAGNAME;
#ifdef OLDBAGS
	    vemNewBagName(phyBagName);
#endif
	}
	tempBag.type = OCT_BAG;
	tempBag.contents.bag.name = phyBagName;
	if (vuTemporary(&termFacet, &tempBag) < OCT_OK) {
	    (void) sprintf(errMsgArea, "Unable to create temp bag:\n  %s\n",
		    octErrorString());
	    vemMessage(errMsgArea, MSG_DISP);
	    return VEM_CORRUPT;
	}
	count = termSpot(&termFacet, &(spot->thePoint), &tempBag);
	if (termAttach(targetFctId, &newTerm, &tempBag, count) != VEM_OK) {
	    (void) sprintf(errMsgArea, "Unable to attach implementing geos:\n  %s\n",
		    octErrorString());
	    vemMessage(errMsgArea, MSG_DISP);
	    return VEM_CORRUPT;
	}
	octDelete(&tempBag);
	break;
    case VEM_OBJ_ARG:
	/* Should be a second text arg */
	if (len < 2) return termFormat("Missing terminal name\n",
					  VEM_ARGRESP);
	lsLastItem(cmdList, (lsGeneric *) &lastArg, LS_NH);
	if (lastArg->argType != VEM_TEXT_ARG)
	  return termFormat("Wrong last argument type\n", VEM_ARGRESP);
	if (termFetch(targetFctId, lastArg->argData.vemTextArg,
		      &termFacet, &newTerm) != VEM_OK)
	  {
	      (void) sprintf(errMsgArea, "Can't get terminal:\n  %s\n",
		      octErrorString());
	      vemMessage(errMsgArea, MSG_DISP);
	      return VEM_CORRUPT;
	  }
	tempBag.type = OCT_BAG;
	tempBag.objectId = firstArg->argData.vemObjArg.theBag;
	if (octGetById(&tempBag) < OCT_OK) {
	    (void) sprintf(errMsgArea, "Can't get argument bag:\n  %s\n",
		    octErrorString());
	    vemMessage(errMsgArea, MSG_DISP);
	    return VEM_CORRUPT;
	}
	if (termAttach(targetFctId, &newTerm, &tempBag,
		       lastArg->length) != VEM_OK)
	  {
	      (void) sprintf(errMsgArea, "Unable to attach implementing geos:\n  %s\n",
		      octErrorString());
	      vemMessage(errMsgArea, MSG_DISP);
	      return VEM_CORRUPT;
	  }
	break;
    default:
	/* Syntax error */
	return termFormat("Unknown argument type\n", VEM_CORRUPT);
    }
    return VEM_OK;
}
