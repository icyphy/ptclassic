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
 * VEM Physical Editing Commands
 *
 * David Harrison
 * University of California,  Berkeley
 * 1986, 1987
 *
 * This file contains commands that deal with selected sets
 * of items. The following commands are exported from this
 * module:
 *   phyCngLayer:	Changes the layer of selected items
 *   phyDelCmd:		Deletes physical objects
 *   phyMoveCmd:	Moves physical objects
 *   phyCopyCmd:	Copies physical objects
 */

#include "general.h"		/* General VEM definitions   */
#include "oct.h"		/* The Oct Data Manager      */
#include "commands.h"		/* Argument list definitions */
#include "buffer.h"		/* Buffer module             */
#include "list.h"		/* List handling package     */
#include "message.h"		/* Message handling package  */
#include "tr.h"			/* Transformation package    */
#include "selset.h"		/* Selection set modification */
#include "vemUtil.h"		/* Utility functions         */
#include "objsel.h"		/* Direct object selection   */
#include "windows.h"		/* Window management funcs   */

#define MOVECOPYBAG	";;; genbag ;;;"
/*
 * Forward Declarations
 */

static vemStatus changeFormat();
static vemStatus MCFormat();
vemStatus phyCopyOrMove();
vemStatus MCParse();


vemStatus phyCngLayer(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * Changes the layer of all objects selected to the layer
 * specified by spot.
 */
{
    octId targetFctId;
    octObject spotFacet, itemFacet;
    octObject oldLayer, newLayer;
    octObject changeSetBag, changeItem;
    octGenerator theGen, subGen;
    int len, retCode;

    /* First,  all kinds of validity checking */
    len = lsLength(cmdList);
    if (len < 1) return changeFormat("Too few arguments", VEM_CORRUPT);
    if (vuCkStyle(spot, cmdList) != VEM_OK) return VEM_ARGRESP;

    /* Parse the argument list */
    if ((retCode = vuObjArgParse(spot, cmdList,
				 &targetFctId, &changeSetBag, 0)) != VEM_OK)
      return changeFormat("Syntax error", retCode);

    /* Discover the new layer */
    spotFacet.objectId = spot->theFct;
    VEM_OCTCKRVAL(octGetById(&spotFacet), VEM_CORRUPT);
    if (vuParseLayer(spot, cmdList, &spotFacet, &newLayer) != VEM_OK)
      return changeFormat("No layer specified", VEM_ARGRESP);

    /* Now really get (or create) the new layer */
    if (bufWritable(targetFctId) == VEM_OK) {
	itemFacet.objectId = targetFctId;
	if (octGetById(&itemFacet) != OCT_OK) {
	    vemMsg(MSG_A, "Cannot get facets of objects:\n  %s\n",
		   octErrorString());
	    return VEM_CORRUPT;
	}
    } else {
	vemMessage("Buffer is not writable\n", MSG_DISP);
	return VEM_ARGRESP;
    }
    if (octGetOrCreate(&itemFacet, &newLayer) != OCT_OK) {
	(void) sprintf(errMsgArea, "Unable to find or make layer `%s':\n  %s\n",
		newLayer.contents.layer.name, octErrorString());
	vemMessage(errMsgArea, MSG_DISP);
	return VEM_CORRUPT;
    }

    /* Now do the changes */
    octInitGenContents(&changeSetBag, OCT_GEO_MASK, &theGen);
    while (octGenerate(&theGen, &changeItem) == OCT_OK) {
	/* Detach the item from all of its layers */
	octInitGenContainers(&changeItem, OCT_LAYER_MASK, &subGen);
	while (octGenerate(&subGen, &oldLayer) == OCT_OK) {
	    if (octDetach(&oldLayer, &changeItem) != OCT_OK) {
		(void) sprintf(errMsgArea, "Object did not detach:\n  %s\n",
			octErrorString());
		vemMessage(errMsgArea, MSG_DISP);
	    }
	}
	/* Attach to new layer */
	if (octAttach(&newLayer, &changeItem) != OCT_OK) {
	    (void) sprintf(errMsgArea, "Could not attach object:\n  %s\n",
		    octErrorString());
	    vemMessage(errMsgArea, MSG_DISP);
	}
	/* Queue up a redisplay */
#ifdef OLD_DISPLAY
	octBB(&changeItem, &bb);
	wnQRegion(targetFctId, &bb);
#endif
    }
    if (changeSetBag.objectId != oct_null_id) {
	octDelete(&changeSetBag);
    }
    return VEM_OK;
}


static vemStatus changeFormat(msg, code)
STR msg;
vemStatus code;
/*
 * Outputs the formatting message for change layer and returns
 * the specified error code.
 */
{
    sprintf(errMsgArea, "Error: %s\n", msg);
    vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
    vemMessage("format:  [objects] [\"layer\"] change-layer\n",
	       MSG_NOLOG|MSG_DISP);
    return code;
}    



vemStatus phyDelCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This is the basic deletion command for physical editing.  It takes
 * as arguments points, lines, and boxes or an object set.  If
 * the first form is selected,  objects under the points,  intersecting
 * the lines,  or totally inside the boxes,  are deleted.  If no
 * arguments are provided,  those objects under the spot are deleted.
 * If the second form is chosen,  all objects in the object set are
 * deleted.  This only occurs if the transform of the set is OCT_NO_TRANSFORM
 * with no translation.
 */
{
    vemOneArg *firstArg;
    octId targetFctId;
    octObject delSetBag;
    octObject delObj, objTerm, termImpl;
    octGenerator theGen, subGen;
    struct octBox bb;
    int len, retCode, delCount, delBagFlag;

    /* First,  all kinds of validity checking */
    len = lsLength(cmdList);
    if (len < 1) {
	vemMessage("format:  [points, lines, or boxes] [layer] delete or\n",
		   MSG_NOLOG|MSG_DISP);
	vemMessage("         [object set] delete \n", MSG_NOLOG|MSG_DISP);
	return VEM_CORRUPT;
    }
    if (vuCkStyle(spot, cmdList) != VEM_OK) return VEM_ARGRESP;
    /* Parse the argument list */
    delBagFlag = 0;
    lsFirstItem(cmdList, (lsGeneric *) &firstArg, LS_NH);
    if ((retCode = vuObjArgParse(spot, cmdList,
				 &targetFctId, &delSetBag, 0)) != VEM_OK)
      {
	  return retCode;
      } else {
	  vuOffSelect(firstArg);
	  lsDelBegin(cmdList, (lsGeneric *) &firstArg);
	  delBagFlag = 1;
      }
    if (bufWritable(targetFctId) == VEM_FALSE) {
	vemMessage("Buffer is read-only\n", MSG_DISP);
	return VEM_CORRUPT;
    }
    /*
     * Ok - we have all the deletion items in delSetBag.  Now,
     * we go through deleting them.  If they implement a terminal
     * however,  we get rid of the terminal (not the object).
     * If its not geometry,  we simply delete the item.
     */
    delCount = 0;
    octInitGenContents(&delSetBag, OCT_ALL_MASK, &theGen);
    while (octGenerate(&theGen, &delObj) == OCT_OK) {
	switch (delObj.type) {
	case OCT_POLYGON:
	case OCT_BOX:
	case OCT_CIRCLE:
	case OCT_PATH:
	case OCT_LABEL:
	    /* Got one - implement a terminal? */
	    if (octInitGenContainers(&delObj, OCT_TERM_MASK,
				     &subGen) != OCT_OK)
	      continue;  /* Delete fails */
	    objTerm.objectId = oct_null_id;
	    while ((retCode = octGenerate(&subGen, &objTerm)) == OCT_OK)
	      {
		  /* Do nothing: use last terminal found */
	      }
	    if (retCode != OCT_GEN_DONE)
	      continue;  /* Delete fails again */
	    if (objTerm.objectId != oct_null_id) {
		/* Delete terminal */
		octBB(&delObj, &bb);
		if (octDelete(&objTerm) != OCT_OK)
		  continue;  /* Yet another failure condition */
		else {
#ifdef OLD_DISPLAY
		    wnQRegion(spot->theFct, &bb);
#endif
		    delCount++;
		}
	    } else {
		/* Delete the object itself */
		octBB(&delObj, &bb);
		if (octDelete(&delObj) != OCT_OK)
		  continue;  /* Yet another failure condition */
		else {
#ifdef OLD_DISPLAY
		    wnQRegion(spot->theFct, &bb);
#endif
		    delCount++;
		}
	    }
	    break;
	case OCT_INSTANCE:
	    /* Delete and queue region */
	    octBB(&delObj, &bb);
	    if (octDelete(&delObj) == OCT_OK) {
#ifdef OLD_DISPLAY
		wnQRegion(spot->theFct, &bb);
#endif
		delCount++;
	    } else {
		(void) sprintf(errMsgArea, "Unable to delete instance:\n  %s\n",
			octErrorString());
		vemMessage(errMsgArea, MSG_DISP);
	    }
	    break;
	case OCT_TERM:
	    /* Queue up all implementing regions */
	    if (octInitGenContents(&delObj, OCT_GEO_MASK, &subGen) != OCT_OK)
	      continue;		/* Delete fails */
	    while (octGenerate(&subGen, &termImpl) == OCT_OK) {
		octBB(&termImpl, &bb);
#ifdef OLD_DISPLAY
		wnQRegion(spot->theFct, &bb);
#endif
	    }
	    /* Delete the terminal itself */
	    octDelete(&delObj);
	    delCount++;
	    break;
	default:
	    /* Just delete the item */
	    octDelete(&delObj);
	    delCount++;
	    break;
	}
    }
    /* If its the second form,  we get rid of the deletion set bag */
    if (delBagFlag) {
	octDelete(&delSetBag);
    }
    (void) sprintf(errMsgArea, "%d items deleted\n", delCount);
    vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
    return VEM_OK;
}


vemStatus phyMoveCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
{
    vemStatus rtn_code;
    vemOneArg *theArg;

    if ((rtn_code = MCParse(spot,cmdList,&theArg,0,(octId *) 0))==VEM_OK) {
	return (phyCopyOrMove(cmdList, theArg, 0, (octId) 0));
    } else {
	return rtn_code;
    }
}

vemStatus phyCopyCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
{
    vemStatus rtn_code;
    vemOneArg *theArg;
    octId destFctId;

    if ((rtn_code = MCParse(spot, cmdList, &theArg, 1, &destFctId)) == VEM_OK) {
	return (phyCopyOrMove(cmdList, theArg, 1, destFctId));
    } else {
	return rtn_code;
    }
}


/*ARGSUSED*/
vemStatus MCParse(spot, cmdList, theArg, copyFlag, destFctId)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
vemOneArg **theArg;		/* Returned argument        */
int copyFlag;			/* If set, copy command     */
octId *destFctId;		/* Returned facet           */
/*
 * This routine parses the argument list to the move and
 * copy commands.  These routines expect an object set
 * as the first argument,  and an optional translation (specified
 * as two points) for the second.  If the translation is
 * provided,  the routine adds the translation to the first
 * argument's transform.  Also,  if the last point is in a different
 * buffer than the objects,  this new buffer will be returned
 * in `destFctId' (normally this is zero). If there were parsing errors,
 * it will report an error but try to leave the arguments
 * untouched.
 */
{
    vemOneArg *firstArg, *lastArg;
    struct octTransform newTransform;
    int len;

    len = lsLength(cmdList);
    if ((len < 1) || (len > 2)) {
	return MCFormat("Too few or too many arguments", copyFlag);
    }
    if (vuCkStyle(spot, cmdList) != VEM_OK) return VEM_ARGRESP;
    lsFirstItem(cmdList, (lsGeneric *) &firstArg, LS_NH);
    if (firstArg->argType != VEM_OBJ_ARG) {
	return MCFormat("First argument must be objects", copyFlag);
    }
    if (destFctId) *destFctId = oct_null_id;
    if (len > 1) {
	lsLastItem(cmdList, (lsGeneric *) &lastArg, LS_NH);
	if ((lastArg->argType != VEM_POINT_ARG) || (lastArg->length != 2)) {
	    return MCFormat("Second argument must be two points", copyFlag);
	}
	if (firstArg->argData.vemObjArg.theFct != lastArg->argData.points[1].theFct) {
	    /* Only valid for copy */
	    if (copyFlag) {
		if (destFctId)
		  *destFctId = lastArg->argData.points[1].theFct;
	    } else {
		return MCFormat("Cannot move objects between buffers", copyFlag);
	    }
	}
	/* Add to transform */
	newTransform = firstArg->argData.vemObjArg.tran;
	newTransform.translation.x += 
	  lastArg->argData.points[1].thePoint.x -
	    lastArg->argData.points[0].thePoint.x;
	newTransform.translation.y +=
	  lastArg->argData.points[1].thePoint.y -
	    lastArg->argData.points[0].thePoint.y;
	objTran(&(firstArg->argData.vemObjArg), &newTransform);
	/* Eat the argument */
	vuOffSelect(lastArg);
	lsDelEnd(cmdList, (lsGeneric *) &lastArg);
    }
    *theArg = firstArg;
    return VEM_OK;
}

static vemStatus MCFormat(msg, copyFlag)
STR msg;			/* Message to display   */
int copyFlag;			/* If set, copy command */
/*
 * This routine writes out a format message for the move and
 * copy commands along with the message `msg'.
 */
{
    sprintf(errMsgArea, "Error: %s\n", msg);
    vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
    if (copyFlag) {
	vemMessage("format: [objects] [points] : copy-objects\n",
		   MSG_NOLOG|MSG_DISP);
    } else {
	vemMessage("format: [objects] [points] : move-objects\n",
		   MSG_NOLOG|MSG_DISP);
    }
    return VEM_ARGRESP;
}




static struct octPoint *MCpntAry = (struct octPoint *) 0;
static int MCpntCnt = 0;

#define INITMCPNTS 20

/*ARGSUSED*/
vemStatus phyCopyOrMove(cmdList, theArg, copy, destFctId)
lsList cmdList;			/* Used to modify list if necessary */
vemOneArg *theArg;		/* Object set argument      */
int copy;			/* If set,  copy objects    */
octId destFctId;		/* Other buffer if any      */
/*
 * The new version of this command takes an object set which has
 * an integral transformation.  This transformation is applied
 * to each of the objects to yield a new object.  If `copy' is
 * set,  this object is created.  Otherwise,  it is modified.
 * If `destFctId' is provided,  the objects will be copied
 * into that facet instead of the same facet.  In this case,
 * `theArg' is modified to point at a new selected set.
 */
{
    octObject searchBag, genBag, theObject, objLayer;
    octGenerator theGen;
    struct octBox bb;
    tr_stack *local_stack;
    struct octTransform finalTran;
    int32 realSize;
    vemOneArg *newCopySet = (vemOneArg *) 0;

    local_stack = tr_create();
    tr_identity(local_stack);
    tr_add_transform(local_stack, &(theArg->argData.vemObjArg.tran), 0);
    tr_get_oct_transform(local_stack, &finalTran);

    searchBag.objectId = theArg->argData.vemObjArg.theBag;
    VEM_OCTCKRVAL(octGetById(&searchBag), VEM_CORRUPT);

    /*
     * Things on `searchBag' may be replaced by the copy operation.
     * Thus,  we make a copy to generate through the objects.
     */
    genBag.type = OCT_BAG;
    genBag.contents.bag.name = MOVECOPYBAG;
    VEM_OCTCKRVAL(vuCreateTemporary(&searchBag, &genBag), VEM_CORRUPT);
    VEM_CKRVAL(vuAddBag(&genBag, &searchBag) == VEM_OK,
	       "Can't copy bag", VEM_CORRUPT);

    VEM_OCTCKRVAL(octInitGenContents(&genBag,
				     OCT_GEO_MASK|OCT_INSTANCE_MASK, &theGen),
		  VEM_CORRUPT);

    while (octGenerate(&theGen, &theObject) == OCT_OK) {
	if (!copy) {
	    if (octBB(&theObject, &bb) == OCT_OK) {
#ifdef OLD_DISPLAY
		wnQRegion(theArg->argData.vemObjArg.theFct, &bb);
#endif
	    }
	}
	switch (theObject.type) {
	case OCT_TERM:
	case OCT_NET:
	case OCT_INSTANCE:
	case OCT_BOX:
	case OCT_CIRCLE:
	case OCT_LABEL:
	    /* Normal case - no points */
	    octTransformGeo(&theObject, &finalTran);
	    break;
	case OCT_POLYGON:
	case OCT_PATH:
	    /* Transform points as well */
	    if (MCpntCnt == 0) {
		/* Initial allocation */
		MCpntCnt = INITMCPNTS;
		MCpntAry = VEMARRAYALLOC(struct octPoint, MCpntCnt);
	    }
	    /* We always want to allocate one extra just in case */
	    realSize = MCpntCnt-1;
	    if (octGetPoints(&theObject, &realSize, MCpntAry) == OCT_TOO_SMALL) {
		/* Reallocate */
		MCpntCnt = realSize + 5;
		MCpntAry = VEMREALLOC(struct octPoint, MCpntAry, MCpntCnt);
		realSize = MCpntCnt-1;
		VEM_OCTCKRVAL(octGetPoints(&theObject, &realSize, MCpntAry),
			      VEM_CORRUPT);
	    }
	    octTransformGeo(&theObject, &finalTran);
	    octTransformPoints(realSize, MCpntAry, &finalTran);
	    break;
	default:
	    /* Unsupported type - fail */
	    vemFail("Unsupported type", __FILE__, __LINE__);
	    break;
	}
	if (copy) {
	    octId oldObjId;

	    oldObjId = theObject.objectId;
	    /* Create new object */
	    if (theObject.type == OCT_INSTANCE) {
		octObject theFacet;

		if (destFctId) {
		    theFacet.objectId = destFctId;
		    VEM_OCTCKRVAL(octGetById(&theFacet), VEM_CORRUPT);
		    if (!newCopySet) {
			newCopySet = vemAttrSelSet(theFacet.objectId,
						   theArg->argData.vemObjArg.attr);
		    }
		    if (octCreate(&theFacet, &theObject) == OCT_OK) {
			vemAddSelSet(newCopySet, theObject.objectId);
		    } else {
			vemMessage("Can't create instance - not copied\n",
				   MSG_DISP);
		    }
		} else {
		    theFacet.objectId = theArg->argData.vemObjArg.theFct;
		    VEM_OCTCKRVAL(octGetById(&theFacet), VEM_CORRUPT);
		    if (octCreate(&theFacet, &theObject) == OCT_OK) {
			vemRCSelSet(theArg, oldObjId, theObject.objectId);
		    } else {
			vemMessage("Can't create instance - not copied\n",
				   MSG_DISP);
		    }
		}
	    } else if (octGenFirstContainer(&theObject,OCT_LAYER_MASK,&objLayer) == OCT_OK) {
		if (destFctId) {
		    octObject theFacet;

		    /* Must be copied to a different buffer */
		    theFacet.objectId = destFctId;
		    VEM_OCTCKRVAL(octGetById(&theFacet), VEM_CORRUPT);
		    if (!newCopySet) {
			newCopySet = vemAttrSelSet(theFacet.objectId,
						   theArg->argData.vemObjArg.attr);
		    }
		    if (octGetOrCreate(&theFacet, &objLayer) != OCT_OK) {
			vemMessage("Can't create new layer\n", MSG_DISP);
			continue;
		    }
		}
		/* Create on that layer */
		if (octCreate(&objLayer, &theObject) == OCT_OK) {
		    if ((theObject.type == OCT_POLYGON) ||
			(theObject.type == OCT_PATH))
		      {
			  octPutPoints(&theObject, realSize, MCpntAry);
		      }
		    if (destFctId) {
			/* Move to new set */
			vemAddSelSet(newCopySet, theObject.objectId);
		    } else {
			/* Replace on selected set */
			vemRCSelSet(theArg, oldObjId, theObject.objectId);
		    }
		} else {
		    vemMessage("Can't create object - not copied\n", MSG_DISP);
		}
	    } else {
		vemMessage("Object has no layer - not copied\n", MSG_DISP);
	    }
	} else {
	    /* Modify old one */
	    if ((theObject.type == OCT_POLYGON)||(theObject.type == OCT_PATH)) {
		octPutPoints(&theObject, realSize, MCpntAry);
	    } else {
		if (octModify(&theObject) != OCT_OK) {
		    sprintf(errMsgArea,
			    "Object can't be modified - not moved:\n  %s\n",
			    octErrorString());
		    vemMessage(errMsgArea, MSG_DISP);
		}
	    }
	}
	if (octBB(&theObject, &bb) == OCT_OK) {
#ifdef OLD_DISPLAY
	    if (destFctId) {
		wnQRegion(destFctId, &bb);
	    } else {
		wnQRegion(theArg->argData.vemObjArg.theFct, &bb);
	    }
#endif
	}
    }
    /* Clean up temporary bag */
    VEM_OCTCKRVAL(octDelete(&genBag), VEM_CORRUPT);
    if (destFctId) {
	atrHandle selAttr;
	vemOneArg *tempArg;

	lsDelBegin(cmdList, (lsGeneric *) &tempArg);
	vemFreeAttrSet(theArg, &selAttr);
	lsNewBegin(cmdList, (lsGeneric) newCopySet, LS_NH);
    } else {
	/* Change the transform to null */
	theArg->argData.vemObjArg.tran.translation.x = 0;
	theArg->argData.vemObjArg.tran.translation.y = 0;
	theArg->argData.vemObjArg.tran.transformType = OCT_NO_TRANSFORM;
    }
    return VEM_ARGRESP;
}
