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
 * 1986 1987
 *
 * This file contains commands for creating and modifying
 * geometry in a physical view.  The following commands
 * are exported:
 *   phyCreateCmd:	Creates boxes, paths, and polygons
 *   phyReplaceCmd:	Replaces a box, path, or polygon with another
 *   phyChgLayer:	Changes the layer of existing geometry
 *   phyCircleCmd:	Creates new circles
 *   phyInstance:	Creates instances in physical
 *   phyLayerCmd:	Creates or edits labels
 */

#include "general.h"		/* General VEM definitions   */
#include "ansi.h"		/* ANSI compatibility        */
#include "oct.h"		/* The Oct Data Manager      */
#include "commands.h"		/* Argument list definitions */
#include "buffer.h"		/* Buffer module             */
#include "list.h"		/* List handling package     */
#include "message.h"		/* Message handling package  */
#include "vemUtil.h"		/* General VEM utilities     */
#include "tap.h"		/* Technology Access Package */
#include "windows.h"		/* Window management         */

/*
 * Forward declarations
 */

static vemStatus createFormat();
static int initTarget();
static void newPolygon();
static void newPath();
static void newBoxes();
static vemStatus replFormat();


static int circleFormat();
static int compAngle();

extern void phyLESpawn
  ARGS((Window win, octId fctId, octObject *layer, octObject *label));
  /* From label.c */



vemStatus phyCreateCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This is the basic physical geometry creation routine.
 * The command format is [geometries] [layer specification].
 * Geometries may be box, line, point, or text arguments.  Boxes
 * create boxes,  lines create paths,  points create
 * polygons,  and strings create labels.  For creating labels,
 * the point set after the label is interpreted as target
 * points for the label.  The layer specification may be a string which
 * is interpreted as the layer name or the command may be
 * invoked over another object and the geometry will be
 * attached to the same layer as that object.
 */
{
    octObject spotFacet, dest, theLyr;
    vemOneArg *thisArg;
    octId outFctId;
    lsGen argGen;
    int len, idx, arg_count, stopspot;

    /* Some validity checking */
    len = lsLength(cmdList);
    if (len < 1) return createFormat("Too few arguments", VEM_CORRUPT);
    spotFacet.objectId = spot->theFct;
    if (octGetById(&spotFacet) != OCT_OK)
      return createFormat("Buffer under spot is inaccessible", VEM_CORRUPT);
    if (vuCkStyle(spot, cmdList) != VEM_OK) return VEM_ARGRESP;
    
    /* Find a layer */
    if (vuParseLayer(spot, cmdList, &spotFacet, &theLyr) != VEM_OK)
      return createFormat("No layer specified", VEM_ARGRESP);
    if (lsLength(cmdList) < 1)
      return createFormat("No new items specified", VEM_CORRUPT);
    
    /* 
     * All items must belong to the same buffer.  It is set the
     * first time by initTarget and remains the same.  All other
     * arguments are checked against it.
     */
    outFctId = oct_null_id;

    argGen = lsStart(cmdList);
    arg_count = 0;
    while (lsNext(argGen, (Pointer *) &thisArg, LS_NH) == LS_OK) {
	arg_count++;
	if ((thisArg->argType != VEM_TEXT_ARG) && (thisArg->argType != VEM_OBJ_ARG)) {
	    if (outFctId == oct_null_id) {
		outFctId = thisArg->argData.points[0].theFct;
		if (!initTarget(outFctId, &dest, &theLyr))
		  return VEM_ARGRESP;
	    }
	    /* Check the buffer of the incoming argument */
	    if (thisArg->argType == VEM_BOX_ARG) {
		stopspot = thisArg->length * 2;
	    } else {
		stopspot = thisArg->length;
	    }
	    for (idx = 0;  idx < stopspot;  idx++) {
		if (thisArg->argData.points[idx].theFct != outFctId) 
		  break;
	    }
	    if (idx < stopspot) {
		(void) sprintf(errMsgArea,
			"Argument %d: All points must be in same buffer\n",
			arg_count);
		vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
		/* Go get next argument */
		continue;
	    }
	}
	switch (thisArg->argType) {
	case VEM_POINT_ARG:
	    newPolygon(&dest, &theLyr, thisArg);
	    break;
	case VEM_LINE_ARG:
	    newPath(&dest, &theLyr, thisArg);
	    break;
	case VEM_BOX_ARG:
	    newBoxes(&dest, &theLyr, thisArg);
	    break;
	default:
	    /* Unknown argument: complain mildly but ignore it */
	    vemMessage("Unknown argument type -- ignoring.\n",
		       MSG_NOLOG|MSG_DISP);
	    break;
	}
    }
    lsFinish(argGen);
    return VEM_OK;
}



/*
 * Helpers for physical creation command
 */

static vemStatus createFormat(msg, retval)
STR msg;			/* Message to display */
vemStatus retval;		/* Value to return    */
/*
 * This routine prints a format message for the create command
 * along with a message supplied by the caller.  It then returns
 * `retval'.
 */
{
    (void) sprintf(errMsgArea, "Error: %s\n", msg);
    vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
    vemMessage("Format: [boxes] {\"layer\"} : create-geometry (boxes)\n",
	       MSG_NOLOG|MSG_DISP);
    vemMessage("        [lines] {\"layer\"} : create-geometry (paths)\n",
	       MSG_NOLOG|MSG_DISP);
    vemMessage("        [points] {\"layer\"} : create-geometry (polygons)\n",
	       MSG_NOLOG|MSG_DISP);
    vemMessage("        [\"text\"] [points] {\"layer\"} : create-geometry (labels)\n",
	       MSG_NOLOG|MSG_DISP);
    vemMessage("Layer can be specified by moving over another object\n",
	       MSG_NOLOG|MSG_DISP);
    return retval;
}


static int initTarget(inFctId, targetFacet, targetLayer)
octId inFctId;			/* Facet for processing  */
octObject *targetFacet;		/* Target facet (return) */
octObject *targetLayer;		/* Target layer (return) */
/*
 * This routine extracts the facet from buffer 'inBuffer' and
 * attempts to find the layer 'targetLayer' (the name field
 * should be set) in the facet.  If layer does not exist,  
 * it is compared to the layers in the technology.  If its 
 * not in the technology,  the routine fails (an error message 
 * is produced).  The routine also fails if the buffer is not 
 * writable or is the wrong editing style.
 */
{
    /* Check writability */
    if (bufWritable(inFctId) == VEM_FALSE) {
	vemMessage("Buffer is read-only\n", MSG_DISP);
	return 0;
    }
    /* Get the facet */
    targetFacet->objectId = inFctId;
    if (octGetById(targetFacet) != OCT_OK) {
	vemMsg(MSG_A, "initTarget: Cannot access facet:\n  %s\n",
	       octErrorString());
	return 0;
    }
    /* Get the layer */
    targetLayer->type = OCT_LAYER;
    if (octGetByName(targetFacet, targetLayer) != OCT_OK) {
	/* Not there.  Check the technology. */
	if (tapIsLayerDefined(targetFacet, targetLayer->contents.layer.name) == OCT_OK) {
	    /* Ok.  Create the layer. */
	    if (octCreate(targetFacet, targetLayer) != OCT_OK) {
		(void) sprintf(errMsgArea, "Cannot create layer '%s':\n  %s\n",
			targetLayer->contents.layer.name, octErrorString());
		vemMessage(errMsgArea, MSG_DISP);
		return 0;
	    }
	} else {
	    (void) sprintf(errMsgArea, "Layer `%s' is not in the technology\n",
		    targetLayer->contents.layer.name);
	    vemMessage(errMsgArea, MSG_DISP);
	    return 0;
	}
    }
    return 1;
}



static void newPolygon(fct, lyr, arg)
octObject *fct;			/* Facet for objects     */
octObject *lyr;			/* Layer to attach to    */
vemOneArg *arg;			/* Polygon point arg     */
/*
 * This routine creates a new polygon attached to layer `lyr'
 * and redraws it in buffer `buf'.  The points for the polygon 
 * are in `arg' (which is expected to be a points argument type).
 */
{
    octObject newObject;
    struct octPoint *newPoints;
    struct octBox bb;
    int idx;

    /* Assert: first and last points not the same */
    newObject.type = OCT_POLYGON;
    if (octCreate(lyr, &newObject) != OCT_OK) {
	(void) sprintf(errMsgArea, "Unable to create polygon:\n  %s\n",
		       octErrorString());
	vemMessage(errMsgArea, MSG_DISP);
	return;
    }
    /* Copy the points into an array */
    newPoints = (struct octPoint *)
      malloc((unsigned) (arg->length * sizeof(struct octPoint)));
    for (idx = 0;  idx < arg->length;  idx++) {
	newPoints[idx] = arg->argData.points[idx].thePoint;
    }
    /* Put the points */
    if (octPutPoints(&newObject, arg->length, newPoints) != OCT_OK) {
	(void) sprintf(errMsgArea,
		       "Unable to store points of polygon:\n  %s\n",
		       octErrorString());
	vemMessage(errMsgArea, MSG_DISP);
	return;
    }
    /* Queue up the bounding box of the new polygon */
    octBB(&newObject, &bb);
#ifdef OLD_DISPLAY
    wnQRegion(fct->objectId, &bb);
#endif
}



static void newPath(fct, lyr, arg)
octObject *fct;			/* Facet for object      */
octObject *lyr;			/* Layer to attach to    */
vemOneArg *arg;			/* Polygon point arg     */
/*
 * This routine creates a new path attached to the layer `lyr'
 * and redisplays it in the buffer `buf'.  The points for
 * the path are given in `arg'.  The width is stored with
 * the buffer on a layer by layer basis.
 */
{
    octObject newObject;
    struct octPoint *newPoints;
    struct octBox bb;
    int idx;

    newObject.type = OCT_PATH;
    newObject.contents.path.width = bufPathWidth(fct->objectId, lyr);
    if (octCreate(lyr, &newObject) != OCT_OK) {
	(void) sprintf(errMsgArea, "Unable to create path:\n  %s\n",
		       octErrorString());
	vemMessage(errMsgArea, MSG_DISP);
	return;
    }
    /* Copy the points into an array */
    newPoints = VEMARRAYALLOC(struct octPoint, arg->length);
    for (idx = 0;  idx < arg->length;  idx++) {
	newPoints[idx] = arg->argData.points[idx].thePoint;
    }
    /* Put the points */
    if (octPutPoints(&newObject, arg->length, newPoints) != OCT_OK) {
	(void) sprintf(errMsgArea, "Unable to store points of path:\n  %s\n",
		       octErrorString());
	vemMessage(errMsgArea, MSG_DISP);
	return;
    }
    VEMFREE(newPoints);
    /* Queue up the bounding box of the new path */
    octBB(&newObject, &bb);
    if ((bb.upperRight.x - bb.lowerLeft.x) < 2) {
	bb.lowerLeft.x -= 1;
	bb.upperRight.x += 1;
    } else if ((bb.upperRight.y - bb.lowerLeft.y) < 2) {
	bb.lowerLeft.y -= 1;
	bb.upperRight.y += 1;
    }
#ifdef OLD_DISPLAY
    wnQRegion(fct->objectId, &bb);
#endif
}


static void newBoxes(fct, lyr, arg)
octObject *fct;			/* Facet of object       */
octObject *lyr;			/* Layer to attach to    */
vemOneArg *arg;			/* Polygon point arg     */
/*
 * This routine creates new boxes on the layer `lyr' and
 * displays them in buffer `buf'.  The points for the boxes
 * are in `arg'.
 */
{
    octObject newObject;
    octCoord temp;
    int idx;

    /* Now,  each box must be created in turn */
    newObject.type = OCT_BOX;
    for (idx = 0;  idx < arg->length;  idx++) {
	/* Make sure box is normalized */
	newObject.contents.box.lowerLeft =
	  arg->argData.points[idx*2].thePoint;
	newObject.contents.box.upperRight =
	  arg->argData.points[idx*2+1].thePoint;
	VEMBOXNORM(newObject.contents.box, temp);
	/* Attempt to create the object */
	if (octCreate(lyr, &newObject) != OCT_OK) {
	    (void) sprintf(errMsgArea, "Unable to create box:\n  %s\n",
			   octErrorString());
	    vemMessage(errMsgArea, MSG_DISP);
	    continue;
	}
#ifdef OLD_DISPLAY
	/* Queue the region */
	wnQRegion(fct->objectId, &(newObject.contents.box));
#endif
    }
}


vemStatus phyReplaceCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* Argument list            */
/*
 * This command replaces a box, line, or polygon with the new
 * specification supplied on the argument list.  The format
 * of the command is: [box, line, or polygon] over the geometry
 * to be replaced.
 * Label box code added by AC ( July 1991 )
 */
{
    vemOneArg *theArg;
    octObject replObj;
    octObjectMask theMask;
    struct octBox bb;
    struct octPoint *newPoints;
    octCoord temp;
    int len, idx;

    len = lsLength(cmdList);
    if (len != 1) {
	return replFormat("Command requires exactly one argument", VEM_ARGRESP);
    } else {
	lsFirstItem(cmdList, (lsGeneric *) &theArg, LS_NH);
    }
    if (vuCkStyle(spot, cmdList) != VEM_OK) return VEM_ARGRESP;

    /* Find the geometry to replace */
    theMask = OCT_BOX_MASK|OCT_PATH_MASK|OCT_POLYGON_MASK|OCT_LABEL_MASK; /* AC */
    if (vuFindSpot(spot, &replObj, theMask) == VEM_OK) {
	/* Found it - now check against argument type */
	if (replObj.type == OCT_BOX) {
	    if (theArg->argType != VEM_BOX_ARG)
	      return replFormat("Box must be replaced with box argument",
				VEM_ARGRESP);
	    if (theArg->length != 1)
	      return replFormat("Only one box allowed", VEM_ARGRESP);
	    /* Now do the replacement */
#ifdef OLD_DISPLAY
	    wnQRegion(spot->theFct, &(replObj.contents.box));
#endif
	    replObj.contents.box.lowerLeft =
	      theArg->argData.points[0].thePoint;
	    replObj.contents.box.upperRight =
	      theArg->argData.points[1].thePoint;
	    VEMBOXNORM(replObj.contents.box, temp);
	    if (octModify(&replObj) >= OCT_OK) {
#ifdef OLD_DISPLAY
		wnQRegion(spot->theFct, &(replObj.contents.box));
#endif
	    } else {
		(void) sprintf(errMsgArea, "phyReplaceCmd: Cannot modify box:\n  %s\n",
			octErrorString());
		vemMessage(errMsgArea, 0);
	    }
	} else if ((replObj.type == OCT_PATH) || (replObj.type == OCT_POLYGON)) {
	    if ((replObj.type == OCT_PATH) && (theArg->argType != VEM_LINE_ARG))
	      return replFormat("Path must be replaced with line argument",
				VEM_ARGRESP);
	    if ((replObj.type == OCT_POLYGON) && (theArg->argType != VEM_POINT_ARG))
	      return replFormat("Polygon must be replaced with point argument",
				VEM_ARGRESP);
	    if ((replObj.type == OCT_POLYGON) && (theArg->length <= 2))
	      return replFormat("There must be at least three replacement points",
				VEM_ARGRESP);
	    /* Do the replacement */
	    VEM_OCTCKRVAL(octBB(&replObj, &bb), VEM_CORRUPT);
#ifdef OLD_DISPLAY
	    wnQRegion(spot->theFct, &bb);
#endif
	    newPoints = VEMARRAYALLOC(struct octPoint, theArg->length);
	    for (idx = 0;  idx < theArg->length;  idx++) {
		newPoints[idx] = theArg->argData.points[idx].thePoint;
	    }
	    if (octPutPoints(&replObj, theArg->length, newPoints) != OCT_OK) {
		(void) sprintf(errMsgArea,
			"Unable to store points of path or polygon:\n  %s\n",
			octErrorString());
		vemMessage(errMsgArea, MSG_DISP);
	    }
	    VEMFREE(newPoints);
	    VEM_OCTCKRVAL(octBB(&replObj, &bb), VEM_CORRUPT);
#ifdef OLD_DISPLAY
	    wnQRegion(spot->theFct, &bb);
#endif
	} else if (replObj.type == OCT_LABEL) {
	    if (theArg->argType != VEM_BOX_ARG) {
		return replFormat("Label box must be replaced with box argument", VEM_ARGRESP);
	    }
	    if (theArg->length != 1) {
		return replFormat("Only one box allowed", VEM_ARGRESP);
	    }
	    /* Now do the replacement */
	    replObj.contents.label.region.lowerLeft = theArg->argData.points[0].thePoint;
	    replObj.contents.label.region.upperRight = theArg->argData.points[1].thePoint;
	    VEMBOXNORM(replObj.contents.label.region, temp);
	    octModify(&replObj);
	    
	} else {
	    (void) sprintf(errMsgArea, "Unknown object type from vuFindSpot: %d\n",
		    replObj.type);
	    vemMessage(errMsgArea, MSG_DISP);
	    return VEM_CORRUPT;
	}
    } else {
	vemMessage("No object found to replace\n", MSG_DISP);
	return VEM_ARGRESP;
    }
    return VEM_OK;
}


static vemStatus replFormat(msg, code)
STR msg;
vemStatus code;
/* Outputs an error message, a format message, and returns `code'. */
{
    (void) sprintf(errMsgArea, "Error: %s\n", msg);
    vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
    vemMessage("format: [box, line, or points] : replace-geometry\n",
	       MSG_NOLOG|MSG_DISP);
    vemMessage("        (over object to be replaced: box,path,polygon,label)\n", MSG_NOLOG|MSG_DISP);
    return code;
}



vemStatus phyCircleCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * Because there is no circle primitive in the VEM argument
 * list,  we must have a separate command for creating circles.
 * This is that command.  Circles are specified in one of two
 * ways:  the first is a line followed by up to 2 points.  This
 * specifies a filled circle with the first point being the center
 * and the second specifies the outer radius.  If the next point
 * is supplied,  an arc is assumed bewteen points 2 and 3.  If
 * the last point is supplied,  it specifies the inner radius (otherwise
 * the inner radius is zero).  The second form takes 2 points and
 * an optional point.  It specifies a circle where the inner and
 * outer radius is the same.  If the last point is supplied an
 * arc is drawn instead.  A layer may be specified after the points.
 */
{
    vemOneArg *thisArg;
    octId targetFctId;
    lsGen argGen;
    octObject spotFacet, createFacet, createLayer, newCircle;
    struct octPoint allpnts[4];
    int len, idx, filledCircle = 0, count, num;

    /* First,  all kinds of validity checking */
    len = lsLength(cmdList);
    if ((len < 1) || (len > 3))
      return circleFormat();
    if (vuCkStyle(spot, cmdList) != VEM_OK) return VEM_ARGRESP;

    /* Get the facet of the spot */
    spotFacet.objectId = spot->theFct;
    if (octGetById(&spotFacet) != OCT_OK) {
	vemMsg(MSG_A, "Spot buffer is inaccessible:\n  %s\n",
	       octErrorString());
	return VEM_CORRUPT;
    }
    /* Next,  we determine the layer */
    if (vuParseLayer(spot, cmdList, &spotFacet, &createLayer) != VEM_OK) {
	vemMessage("No layer has been specified\n", MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }
    /* Parse the argument list */
    targetFctId = oct_null_id;
    argGen = lsStart(cmdList);
    count = num = 0;
    while (lsNext(argGen, (lsGeneric *) &thisArg, LS_NH) == LS_OK) {
	if (count == 0) {
	    if (thisArg->argType == VEM_LINE_ARG) {
		if (thisArg->length != 2)
		  return circleFormat();
		filledCircle = 1;
		targetFctId = thisArg->argData.points[0].theFct;
		if (!initTarget(targetFctId, &createFacet, &createLayer))
		  return VEM_ARGRESP;
		if (targetFctId != thisArg->argData.points[1].theFct) {
		    vemMessage("All points must be in the same facet\n",
			       MSG_NOLOG|MSG_DISP);
		    return VEM_CORRUPT;
		}
		allpnts[0] = thisArg->argData.points[0].thePoint;
		allpnts[1] = thisArg->argData.points[1].thePoint;
		num = 2;
	    } else if (thisArg->argType == VEM_POINT_ARG) {
		if ((thisArg->length < 2) || (thisArg->length > 3))
		  return circleFormat();
		filledCircle = 0;
		targetFctId = thisArg->argData.points[0].theFct;
		if (!initTarget(targetFctId, &createFacet, &createLayer))
		  return VEM_ARGRESP;
		for (idx = 0;  idx < thisArg->length;  idx++) {
		    if (thisArg->argData.points[idx].theFct!=targetFctId) {
			vemMessage("All points must be in the same facet\n",
				   MSG_NOLOG|MSG_DISP);
			return VEM_CORRUPT;
		    }
		    allpnts[idx] = thisArg->argData.points[idx].thePoint;
		}
		num = thisArg->length;
	    } else {
		return circleFormat();
	    }
	} else if (count == 1) {
	    if (filledCircle) {
		if (thisArg->argType == VEM_POINT_ARG) {
		    if (thisArg->length > 2)
		      return circleFormat();
		    for (idx = 0;  idx < thisArg->length;  idx++) {
			if (thisArg->argData.points[idx].theFct !=
			    targetFctId)
			  {
			     vemMessage("All points must be in the same facet\n",
				        MSG_NOLOG|MSG_DISP);
			     return VEM_CORRUPT;
			  }
			allpnts[idx+2] =
			  thisArg->argData.points[idx].thePoint;
			num++;
		    }
		} else if (thisArg->argType != VEM_TEXT_ARG) {
		    return circleFormat();
		}
	    } else if (thisArg->argType != VEM_TEXT_ARG) {
		return circleFormat();
	    }
	} else if (count == 2) {
	    if (thisArg->argType != VEM_TEXT_ARG) {
		return circleFormat();
	    }
	} else {
	    return circleFormat();
	}
	count++;
    }
		    
    /* Now we have all of the points in the right spot */
    newCircle.type = OCT_CIRCLE;
    newCircle.objectId = oct_null_id;
    newCircle.contents.circle.startingAngle = 0;
    newCircle.contents.circle.endingAngle = 3600;
    newCircle.contents.circle.center = allpnts[0];
    newCircle.contents.circle.innerRadius = 0;
    if (filledCircle) {
	newCircle.contents.circle.outerRadius = VEMDIST(allpnts[0], allpnts[1]);
	if (num >= 3) {
	    /* Its an arc */
	    newCircle.contents.circle.startingAngle =
	      compAngle(&(allpnts[0]), &(allpnts[1]));
	    newCircle.contents.circle.endingAngle =
	      compAngle(&(allpnts[0]), &(allpnts[2]));
	}
	if (num >= 4) {
	    /* It has an inner radius */
	    newCircle.contents.circle.innerRadius =
	      VEMDIST(allpnts[0], allpnts[3]);
	}
    } else {
	newCircle.contents.circle.outerRadius =
	  newCircle.contents.circle.innerRadius =
	    VEMDIST(allpnts[0], allpnts[1]);
	if (num >= 3) {
	    /* Its an arc */
	    newCircle.contents.circle.startingAngle =
	      compAngle(&(allpnts[0]), &(allpnts[1]));
	    newCircle.contents.circle.endingAngle =
	      compAngle(&(allpnts[0]), &(allpnts[2]));
	}
    }

    /* Now we can actually make the circle */
    if (octCreate(&createLayer, &newCircle) != OCT_OK) {
	(void) sprintf(errMsgArea, "Unable to create circle:\n  %s\n",
		octErrorString());
	vemMessage(errMsgArea, MSG_DISP);
	return VEM_CORRUPT;
    }
#ifdef OLD_DISPLAY
    if (octBB(&newCircle, &bb) == OCT_OK) {
	wnQRegion(targetFctId, &bb);
    }
#else 
    bufMarkActive( targetFctId );
#endif
    return VEM_OK;
}



static int circleFormat()
/*
 * This routine is called when there is an error in the circle
 * parsing.
 */
{
    vemMessage("format: [radius-line] {[endang-pnt inner-pnt]} create-circle\n",
	       MSG_NOLOG|MSG_DISP);
    vemMessage("        [center-pnt rad-pnt] {[endang-pnt]} create-circle\n",
	       MSG_NOLOG|MSG_DISP);
    vemMessage("First form creates a filled circle\n",
	       MSG_NOLOG|MSG_DISP);
    vemMessage("Second form creates a outline circle\n",
	       MSG_NOLOG|MSG_DISP);
    return VEM_CORRUPT;
}

static int compAngle(pnt1, pnt2)
struct octPoint *pnt1;		/* First point  */
struct octPoint *pnt2;		/* Second point */
/*
 * This routine computes the angle formed by the positive X axis
 * and the line formed by the two points.  The angle is measured
 * in tenths of degrees counterclockwise.
 */
{
    struct octPoint localPnt;
    double result;

    localPnt.x = pnt2->x - pnt1->x;
    localPnt.y = pnt2->y - pnt1->y;
    /* Table lookup for the easy ones */
    if (localPnt.x == 0) {
	if (localPnt.y < 0)
	  return 2700;
	else
	  return 900;
    } else if (localPnt.y == 0) {
	if (localPnt.x > 0)
	  return 0;
	else
	  return 1800;
    } else {
	/* General case */
	result = atan(((double) localPnt.y) / ((double) localPnt.x)) *
	  1800.0 / 3.141592654;
	if (localPnt.x < 0) {
	    return (int) (result + 1800.5);
	} else {
	    return (int) (result + 0.5);
	}
    }
}




vemStatus phyInstance(spot, cmdList)
vemPoint *spot;
lsList cmdList;
/*
 * Temporary command to instantiate an instance.  The format is:
 *    point "cell view {facet}" (or possibly over spot).
 */
{
    vemOneArg *firstArg, *lastArg;
    octObject newInstance, instFacet, spotFacet;
    int len, i;
    char *str;

    /* Type checking */
    len = lsLength(cmdList);
    if ((len < 1) || (len > 2)) {
	vemMessage("format: point [\"cell view\"] : instance\n",
		   MSG_NOLOG|MSG_DISP);
	return VEM_CORRUPT;
    }
    if (vuCkStyle(spot, cmdList) != VEM_OK) return VEM_ARGRESP;

    lsFirstItem(cmdList, (lsGeneric *) &firstArg, LS_NH);
    if ((firstArg->argType != VEM_POINT_ARG) ||
	(firstArg->length == 0)) {
	vemMessage("format: point [\"cell:view\"] : instance\n",
		   MSG_NOLOG|MSG_DISP);
	return VEM_CORRUPT;
    }

    if (len == 2) {
	/* Specified instance by name */
	lsLastItem(cmdList, (lsGeneric *) &lastArg, LS_NH);
    }

    /* Get the specification for the instance */
    if (len == 2) {
	spotFacet.objectId = spot->theFct;
	if (octGetById(&spotFacet) == OCT_OK) {
	    newInstance.contents.instance.master = spotFacet.contents.facet.cell;
	    newInstance.contents.instance.view = spotFacet.contents.facet.view;
	    newInstance.contents.instance.facet = spotFacet.contents.facet.facet;
	}
	if (vuParseInst(lastArg->argData.vemTextArg, &newInstance, &str) != VEM_OK) {
	    vemMsg(MSG_A, "Cannot parse master specification: `%s'\n",
		   lastArg->argData.vemTextArg);
	    return VEM_CORRUPT;
	}
    } else {
	/* Specified by spot */
	if (vuFindSpot(spot, &newInstance, OCT_INSTANCE_MASK) != VEM_OK) {
	    vemMessage("no instance under spot\n", MSG_NOLOG|MSG_DISP);
	    return VEM_CORRUPT;
	}
    }
    newInstance.type = OCT_INSTANCE;
    newInstance.contents.instance.name = "";
    newInstance.contents.instance.version = OCT_CURRENT_VERSION;
    newInstance.contents.instance.transform.transformType = OCT_NO_TRANSFORM;

    /* Ready to place instances */
    for (i = 0;  i < firstArg->length;  i++) {
	newInstance.contents.instance.transform.translation.x =
	  firstArg->argData.points[i].thePoint.x;
	newInstance.contents.instance.transform.translation.y =
	  firstArg->argData.points[i].thePoint.y;
	instFacet.objectId = firstArg->argData.points[i].theFct;
	if (octGetById(&instFacet) == OCT_OK) {
	    if (octCreate(&instFacet, &newInstance) == OCT_OK) {
#ifdef OLD_DISPLAY
		/* Redisplay region */
		if (octBB(&newInstance, &bb) == OCT_OK) {
		    wnQRegion(firstArg->argData.points[i].theFct, &bb);
		} else {
		    (void) sprintf(errMsgArea, "warning: instance has no size:\n  %s\n",
			    octErrorString());
		    vemMessage(errMsgArea, MSG_DISP);
		}
#endif
	    } else {
		(void) sprintf(errMsgArea, "warning: unable to place instance:\n  %s\n",
			octErrorString());
		vemMessage(errMsgArea, MSG_DISP);
	    }

	    bufMarkActive( instFacet.objectId );

	    
	} else {
	    (void) sprintf(errMsgArea, "warning: unable to open an instance's facet:\n  %s\n",
		    octErrorString());
	    vemMessage(errMsgArea, MSG_DISP);
	}
    }
    return VEM_OK;
}



vemStatus phyLabelCmd(spot, cmdList)
vemPoint *spot;
lsList cmdList;
/*
 * Creates or edits labels.  The command has two forms:
 *   point "LAYER" : edit-label
 *   objects : edit-label
 * The first form creates a new label at the specified
 * point.  If the layer isn't given,  it will be determined
 * by the spot.  The second form edits the specified labels.
 * In both cases,  the command spawns a modeless dialog for
 * editing the label.
 */
{
    vemOneArg *thisArg;
    octObject spotFacet, layer, bag, label, pntFacet;
    octGenerator gen, lyr_gen;
    int i;

    spotFacet.objectId = spot->theFct;
    if (octGetById(&spotFacet) != OCT_OK) {
	vemMsg(MSG_A, "Spot buffer is inaccessible:\n  \n",
	       octErrorString());
	return VEM_CORRUPT;
    }
    if (vuParseLayer(spot, cmdList, &spotFacet, &layer) != VEM_OK) {
	layer.contents.layer.name = "NO LAYER";
    }
    if (lsLength(cmdList) != 1) {
	vemMsg(MSG_C, "format: [label-loc] {\"layer\"} : edit-label\n");
	vemMsg(MSG_C, "        [objects] : edit-label\n");
	return VEM_ARGRESP;
    }
    (void) lsFirstItem(cmdList, (lsGeneric *) &thisArg, LS_NH);
    if (thisArg->argType == VEM_POINT_ARG) {
	/* Create new label */
	label.type = OCT_LABEL;
	label.objectId = oct_null_id;
	label.contents.label.label = "";
	label.contents.label.textHeight = 100;
	label.contents.label.vertJust = OCT_JUST_CENTER;
	label.contents.label.horizJust = OCT_JUST_CENTER;
	label.contents.label.lineJust = OCT_JUST_CENTER;
	for (i = 0;  i < thisArg->length;  i++) {
	    if (!initTarget(thisArg->argData.points[i].theFct,
			    &pntFacet, &layer))
	      continue;
	    label.contents.label.region.lowerLeft =
	      thisArg->argData.points[i].thePoint;
	    label.contents.label.region.upperRight =
	      thisArg->argData.points[i].thePoint;
	    phyLESpawn(thisArg->argData.points[i].theWin,
		       thisArg->argData.points[i].theFct, &layer, &label);
	}
    } else if (thisArg->argType == VEM_OBJ_ARG) {
	/* Edit existing label */
	bag.objectId = thisArg->argData.vemObjArg.theBag;
	VEM_OCTCKRVAL(octGetById(&bag), VEM_CORRUPT);
	VEM_OCTCKRVAL(octInitGenContents(&bag, OCT_LABEL_MASK, &gen),
		      VEM_CORRUPT);
	while (octGenerate(&gen, &label) == OCT_OK) {
	    VEM_OCTCKRVAL(octInitGenContainers(&label, OCT_LAYER_MASK, &lyr_gen),
			  VEM_CORRUPT);
	    layer.objectId = oct_null_id;
	    while (octGenerate(&lyr_gen, &layer) == OCT_OK) {
		/* Null body */
	    }
	    phyLESpawn(spot->theWin,
		       thisArg->argData.vemObjArg.theFct, &layer, &label);
	}
    } else {
	/* Bad arguments */
	vemMsg(MSG_C, "format: [label-loc] {\"layer\"} : edit-label\n");
	vemMsg(MSG_C, "        [objects] : edit-label\n");
	return VEM_ARGRESP;
    }
    return VEM_OK;
}
