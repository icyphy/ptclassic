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
/*LINTLIBRARY*/
/*
 * Object Selection Module
 *
 * David Harrison
 * University of California,  Berkeley
 * 
 * This module supports routines for hilighting Oct objects in VEM
 * windows.  Once highlighted,  the objects will remain highlighted
 * until explicitly unhighlighted (across both exposure events
 * and general editing).
 *
 * Selection of objects in VEM is implemented in two layers.  The
 * lowest layer (implemented here) works on a collection of objects
 * stored in the `selBag' of a vemObjects structure.  This is 
 * a non-hierarchical bag which stores the actual geometry that
 * is highlighted.  The module exports an interface for adding
 * to and subtracting from this set as well as changing the
 * transform applied to the objects (this is also stored in
 * the vemObjects structure).  This replaces an earilier effort
 * implemented inside the octWin.c module.
 *
 * The next layer up implements selection at an abstract level
 * using the primary bag in the vemObjects structure, `theBag'.
 * It is at this level that all other modules of VEM interact.
 * See selset.c for the implementation of this layer.
 *
 * Note that the module interacts very tightly with the windows
 * module.  It makes heavy use of the internal window structures
 * and mapping functions.
 */

#include "wininternal.h"	/* Internal window definitions */
#include "display.h"		/* Drawing primitives          */
#include "list.h"		/* List management             */
#include "message.h"		/* Message handling            */

#define INITPNTCOUNT	20
#define PNTPAD		5

#define OCT_DISP_MASK	(OCT_GEO_MASK|OCT_INSTANCE_MASK)

/*
 * The following list maintains a collection of vemObjects structures
 * which are currently `turned on' via objOn().  The items in the
 * set will remain highlighted until the items are removed with
 * objSub or all highlighting is turned off via objOff().
 */
static lsList objList = (lsList) 0;

/*
 * The following structure is passed to oneDraw when drawing single
 * objects on a object set.
 */
typedef struct one_draw_defn {
    octId oneFct;		/* Facet  of object */
    octObject *theObj;		/* Object itself    */
} one_draw;

/*
 * Forward Declarations
 */
static enum st_retval multiDraw();
static enum st_retval oneDraw();
static void selDraw();



void objOn(objset)
vemObjects *objset;		/* Set to turn on */
/*
 * This routine turns on the highlighting of the objects attached to
 * 'selBag' in `objset'.  The objects will be highlighted
 * using the attributes specified in `objset'.  Before highlighting,
 * the objects will be transformed as specified by the transformation
 * specification inside `objset'.  The routine sets the current
 * attribute to the one specified by `objset'.  
 */
{
    if (!objList) {
	objList = lsCreate();
    }

    /* Set the attribute */
    (void) atrSet(objset->attr);

    /* Push on transformation */
    tr_push(_wnTranStack);
    tr_identity(_wnTranStack);
    tr_add_transform(_wnTranStack, &(objset->tran), 0);

    _wnForeach(WN_ACTIVE | WN_INTR, multiDraw, (char *) objset);

    /* Get rid of that transform */
    tr_pop(_wnTranStack);

    /* Add to list of active object sets */
    lsNewBegin(objList, (lsGeneric) objset, LS_NH);
}


void objOff(objset)
vemObjects *objset;
/*
 * This routine turns off the highlighting of all objects in the
 * set specified by `objset'.  The module releases any hold it
 * had on the set.  IMPORTANT:  removing an object from an object
 * set bag should be done with objSub.  If it isn't,  an object
 * may not be unselected properly.  The objects ARE NOT detached
 * from the underlying bag.  This means the set can be passed
 * back to objOn() if re-highlighting is desired.
 */
{
    lsGen theGen;
    vemObjects *theObj;
    octObject mainBag, nextObj;
    octGenerator octGen;
    struct octBox bb;
    int plane_flag;

    if (!objList) {
	objList = lsCreate();
    }

    mainBag.objectId = objset->selBag;
    if (octGetById(&mainBag) == OCT_OK) {
	/* Undraw all objects */
	plane_flag = (atrIsPlane(objset->attr) == VEM_OK);
	tr_push(_wnTranStack);
	tr_identity(_wnTranStack);
	tr_add_transform(_wnTranStack, &(objset->tran), 0);

	VEM_OCTCK(octInitGenContents(&mainBag, OCT_DISP_MASK, &octGen));

	while (octGenerate(&octGen, &nextObj) == OCT_OK) {
	    /* All objects must have a bounding box */
	    VEM_OCTCK(octBB(&nextObj, &bb));
	    if (!tr_is_identity(_wnTranStack)) {
		octCoord temp;

		/* Transform bounding box */
		tr_transform(_wnTranStack, &(bb.lowerLeft.x), &(bb.lowerLeft.y));
		tr_transform(_wnTranStack, &(bb.upperRight.x), &(bb.upperRight.y));
		VEMBOXNORM(bb, temp);
	    }
	    if (plane_flag) {
		wnQSelect(objset, &bb);
	    } else {
#ifndef OLD_DISPLAY
		wnQRedisplay(objset->theFct, &bb);
#else
		wnQRegion(objset->theFct, &bb);
#endif
	    }
	}
	tr_pop(_wnTranStack);
    }

    /* Remove from list of active object sets */
    theGen = lsStart(objList);
    while (lsNext(theGen, (lsGeneric *) &theObj, LS_NH) == VEM_OK) {
	if (theObj == objset) {
	    lsDelBefore(theGen, (lsGeneric *) &theObj);
	    break;
	}
    }
    lsFinish(theGen);
}



void objAdd(objset, obj)
vemObjects *objset;		/* Highlight set        */
octObject *obj;			/* Object to add to set */
/*
 * This routine adds a given object to the 'selBag' in `objset'.
 * As a side-effect,  the object is highlighted
 * on the screen where appropriate.  The routine (possibly
 * redundantly) checks to make sure the item has not
 * already been added to the set.
 */
{
    one_draw the_draw;
    octObject theBag;
    
    theBag.objectId = objset->selBag;
    VEM_OCTCK(octGetById(&theBag));

    if (octIsAttached(&theBag, obj) == OCT_NOT_ATTACHED) {
	/* Attach to bag */
	VEM_OCTCK(octAttach(&theBag, obj));

	/* Set the attribute */
	(void) atrSet(objset->attr);

	/* Push on transformation */
	tr_push(_wnTranStack);
	tr_identity(_wnTranStack);
	tr_add_transform(_wnTranStack, &(objset->tran), 0);

	/* Set the drawing record and draw */
	the_draw.oneFct = objset->theFct;
	the_draw.theObj = obj;
	_wnForeach(WN_ACTIVE | WN_INTR, oneDraw, (char *) &the_draw);
	
	/* Pop the transform */
	tr_pop(_wnTranStack);
    }
}


void objSub(objset, obj)
vemObjects *objset;		/* Highlight set        */
octObject *obj;			/* Object to add to set */
/*
 * This routine removes a given objects from `selBag' in `objset'.
 * The object is unhighlighted as a side-effect.  If the object is 
 * not attached to the bag,  no unhighlighting is done.  
 */
{
    octObject theBag;
    struct octBox bb;
    
    theBag.objectId = objset->selBag;
    VEM_OCTCK(octGetById(&theBag));

    /* Detach from bag */
    VEM_OCTCK(octDetach(&theBag, obj));

    /* Handle transform */
    tr_push(_wnTranStack);
    tr_identity(_wnTranStack);
    tr_add_transform(_wnTranStack, &(objset->tran), 0);

    /* Requeue region - if applicable */
    if (octBB(obj, &bb) == OCT_OK) {
	if (!tr_is_identity(_wnTranStack)) {
	    octCoord temp;

	    /* Transform bounding box */
	    tr_transform(_wnTranStack, &(bb.lowerLeft.x), &(bb.lowerLeft.y));
	    tr_transform(_wnTranStack, &(bb.upperRight.x), &(bb.upperRight.y));
	    VEMBOXNORM(bb, temp);
	}
	if (atrIsPlane(objset->attr) == VEM_OK) {
	    wnQSelect(objset, &bb);
	} else {
#ifndef OLD_DISPLAY
	    wnQRedisplay(objset->theFct, &bb);
#else
	    wnQRegion(objset->theFct, &bb);
#endif
	}
    }
    tr_pop(_wnTranStack);
}



void objRepl(objset, oldobj, newobj)
vemObjects *objset;		/* Highlight set  */
octObject *oldobj;		/* Old object     */
octObject *newobj;		/* New object     */
/*
 * This routine replaces the object `oldobj' with object `newobj'
 * silently (without effecting the screen).  It is used exclusively
 * by the vemRCSelSet call.
 */
{
    octObject theBag;

    theBag.objectId = objset->selBag;
    VEM_OCTCK(octGetById(&theBag));

    /* Detach old one from bag */
    VEM_OCTCK(octDetach(&theBag, oldobj));

    /* Attach new one to bag */
    VEM_OCTCK(octAttach(&theBag, newobj));
}



void objTran(objset, newtran)
vemObjects *objset;		/* Highlight set      */
struct octTransform *newtran;	/* New transformation */
/*
 * This routine changes the transformation of the highlight
 * set `objset' to `newtran'.  As a side effect,  the highlighted
 * geometry is redrawn to reflect the change.  The current drawing
 * attributes are set to the attribute specified by `objset'. This
 * calls wnFlush to effect the necessary changes.
 */
{
    octObject mainBag, nextObj;
    octGenerator theGen;
    struct octBox bb;
    int plane_flag;

    (void) atrSet(objset->attr);

    /* Turn off the display of all objects */
    plane_flag = (atrIsPlane(objset->attr) == VEM_OK);

    mainBag.objectId = objset->selBag;
    VEM_OCTCK(octGetById(&mainBag));
    VEM_OCTCK(octInitGenContents(&mainBag, OCT_DISP_MASK, &theGen));

    tr_push(_wnTranStack);
    tr_identity(_wnTranStack);
    tr_add_transform(_wnTranStack, &(objset->tran), 0);

    /* Queue up the old regions */
    while (octGenerate(&theGen, &nextObj) == OCT_OK) {
	VEM_OCTCK(octBB(&nextObj, &bb));
	if (!tr_is_identity(_wnTranStack)) {
	    octCoord temp;

	    /* Transform bounding box */
	    tr_transform(_wnTranStack, &(bb.lowerLeft.x), &(bb.lowerLeft.y));
	    tr_transform(_wnTranStack, &(bb.upperRight.x), &(bb.upperRight.y));
	    VEMBOXNORM(bb, temp);
	}
	if (plane_flag) {
	    wnQSelect(objset, &bb);
	} else {
#ifndef OLD_DISPLAY
	    wnQRedisplay(objset->theFct, &bb);
#else
	    wnQRegion(objset->theFct, &bb);
#endif
	}
    }
    /* Make the change and queue up the new ones */
    objset->tran = *newtran;
    tr_identity(_wnTranStack);
    tr_add_transform(_wnTranStack, &(objset->tran), 0);

    VEM_OCTCK(octInitGenContents(&mainBag, OCT_DISP_MASK, &theGen));
    while (octGenerate(&theGen, &nextObj) == OCT_OK) {
	VEM_OCTCK(octBB(&nextObj, &bb));
	if (!tr_is_identity(_wnTranStack)) {
	    octCoord temp;

	    /* Transform bounding box */
	    tr_transform(_wnTranStack, &(bb.lowerLeft.x), &(bb.lowerLeft.y));
	    tr_transform(_wnTranStack, &(bb.upperRight.x), &(bb.upperRight.y));
	    VEMBOXNORM(bb, temp);
	}
	if (plane_flag) {
	    wnQSelect(objset, &bb);
	} else {
#ifndef OLD_DISPLAY
	    wnQRedisplay(objset->theFct, &bb);
#else
	    wnQRegion(objset->theFct, &bb);
#endif
	}
    }
    /* wnFlush(); */   /* Wrong call!! this is too expensive !! */
    wnQuickFlush();
    tr_pop(_wnTranStack);
} 



/*ARGSUSED*/
vemStatus objMove(objset, xoff, yoff)
vemObjects *objset;		/* Highlight set  */
octCoord xoff, yoff;		/* Offset to move */
/*
 * This routine attempts to move an object set as a unit.  This
 * is used for real-time move operations.  Currently,  only
 * selection on its own plane allows this operation.  If it
 * can't do it,  it will return VEM_NOIMPL.
 */
{
    struct octTransform tran;

    tran = objset->tran;
    tran.translation.x += xoff;
    tran.translation.y += yoff;
    objTran(objset, &tran);
    return VEM_OK;
}


vemStatus objBB(objset, bb)
vemObjects *objset;		/* Highlight set         */
struct octBox *bb;		/* Returned bounding box */
/*
 * This routine returns the virtual bounding box of the items
 * in the set `objset'.  If the set has no bounding box,  it
 * will return VEM_CANTFIND.  This bounding box IS NOT transformed.
 * See objTranBB() for the transformed bounding box.
 */
{
    octObject genBag, nextObj;
    octGenerator theGen;
    struct octBox itemBB;
    int validBB;

    genBag.objectId = objset->selBag;
    VEM_OCTCK(octGetById(&genBag));
    
    bb->lowerLeft.x = bb->lowerLeft.y = VEMMAXINT;
    bb->upperRight.x = bb->upperRight.y = VEMMININT;
    VEM_OCTCK(octInitGenContents(&genBag, OCT_DISP_MASK, &theGen));
    validBB = 0;
    while (octGenerate(&theGen, &nextObj) == OCT_OK) {
	if (octBB(&nextObj, &itemBB) == OCT_OK) {
	    bb->lowerLeft.x = VEMMIN(bb->lowerLeft.x, itemBB.lowerLeft.x);
	    bb->lowerLeft.y = VEMMIN(bb->lowerLeft.y, itemBB.lowerLeft.y);
	    bb->upperRight.x = VEMMAX(bb->upperRight.x, itemBB.upperRight.x);
	    bb->upperRight.y = VEMMAX(bb->upperRight.y, itemBB.upperRight.y);
	    validBB = 1;
	}
    }
    if (validBB) {
	return VEM_OK;
    } else {
	return VEM_CANTFIND;
    }
}

vemStatus objTranBB(objset, bb)
vemObjects *objset;		/* Highlight set         */
struct octBox *bb;		/* Returned bounding box */
/*
 * This routine returns the virtual bounding box of the items
 * in the set `objset'.  Unlike objBB,  this routine transforms
 * the bounding box in accordance with the transformation
 * associated with the object set.  If the set has no bounding
 * box,  it will return VEM_CANTFIND.
 */
{
    vemStatus retCode;

    if ((retCode = objBB(objset, bb)) == VEM_OK) {
	octCoord temp;

	tr_push(_wnTranStack);
	tr_identity(_wnTranStack);
	tr_add_transform(_wnTranStack, &(objset->tran), 0);
	tr_transform(_wnTranStack, &(bb->lowerLeft.x), &(bb->lowerLeft.y));
	tr_transform(_wnTranStack, &(bb->upperRight.x), &(bb->upperRight.y));
	VEMBOXNORM(*bb, temp);
	return VEM_OK;
    } else return retCode;
}




/*
 * Hash table tracing routines
 */

/*ARGSUSED*/
static enum st_retval multiDraw(key, value, arg)
char *key, *value, *arg;
/*
 * This routine is passed to st_foreach on the table of active
 * windows to effect the drawing of items on a particular 
 * object selection set.  Assumes the attribute and transformation
 * has been set up appropriately.  Sets up the globals
 * _wnWin and _wnPhysical for _wnObjDisp().
 */
{
    vemObjects *objset = (vemObjects *) arg;
    octGenerator theGen;
    octObject genBag, nextObj;
    struct octBox winbb, bb;
    octCoord temp;
    int t_flag;

    _wnWin = *((internalWin *) value);
    if (objset->theFct == _wnWin.fcts[W_PRI].winFctId) {
	/* Draw all of those that intersect the window */
	genBag.objectId = objset->selBag;
	VEM_OCTCK(octGetById(&genBag));
	VEM_OCTCK(octInitGenContents(&genBag, OCT_DISP_MASK, &theGen));

	dspInit(_wnWin.XWin, 0, 0, _wnWin.width, _wnWin.height);

	/* Compute virtual window bounding box */
	winbb.lowerLeft.x = _wnWin.fcts[W_PRI].x_off;
	winbb.lowerLeft.y = _wnWin.fcts[W_PRI].y_off;
	winbb.upperRight.x = winbb.lowerLeft.x +
	  OCTSCALE(_wnWin.width, _wnWin.scaleFactor);
	winbb.upperRight.y = winbb.lowerLeft.y +
	  OCTSCALE(_wnWin.height, _wnWin.scaleFactor);

	/* Inverse transform it to check against geometry */
	tr_inverse_transform(_wnTranStack, &winbb.lowerLeft.x,
			     &winbb.lowerLeft.y);
	tr_inverse_transform(_wnTranStack, &winbb.upperRight.x,
			     &winbb.upperRight.y);
	VEMBOXNORM(winbb, temp);

	/* Physical region is entire window */
	_wnPhysical.lowerLeft.x = 0;
	_wnPhysical.lowerLeft.y = _wnWin.height;
	_wnPhysical.upperRight.x = _wnWin.width;
	_wnPhysical.upperRight.y = 0;

	t_flag = !tr_is_identity(_wnTranStack);

	while (octGenerate(&theGen, &nextObj) == OCT_OK) {
	    VEM_OCTCK(octBB(&nextObj, &bb));
	    if (VEMBOXINTERSECT(winbb, bb)) {
		selDraw(&nextObj, &bb, t_flag);
	    }
	}
	dspEnd();
    }
    return ST_CONTINUE;
}

/*ARGSUSED*/
static enum st_retval oneDraw(key, value, arg)
char *key, *value, *arg;
/*
 * This routine is passed to st_foreach on all active windows
 * to turn on the selection of one single item (as opposed
 * to multiple items in multiDraw.
 */
{
    octId theFct = ((one_draw *) arg)->oneFct;
    octObject theObj;
    struct octBox org_bb;
    octCoord temp;
    int tr_flag;
    
    _wnWin = *((internalWin *) value);
    theObj = *(((one_draw *) arg)->theObj);
    if (theFct == _wnWin.fcts[W_PRI].winFctId) {
	if (octBB(&theObj, &org_bb) != OCT_OK) {
	    vemMsg(MSG_L, "oneDraw: object has no bounding box\n");
	    return ST_CONTINUE;
	}

	/* Transform if required */
	_wnPhysical = org_bb;
	tr_flag = tr_is_identity(_wnTranStack);
	if (!tr_flag) {
	    tr_transform(_wnTranStack,
			 &_wnPhysical.lowerLeft.x, &_wnPhysical.lowerLeft.y);
	    tr_transform(_wnTranStack,
			 &_wnPhysical.upperRight.x, &_wnPhysical.upperRight.y);
	    VEMBOXNORM(_wnPhysical, temp);
	}
	/* Map to physical coordinates */
	TR_X(_wnWin, W_PRI, _wnPhysical.lowerLeft.x);
	TR_Y(_wnWin, W_PRI, _wnPhysical.lowerLeft.y);
	TR_X(_wnWin, W_PRI, _wnPhysical.upperRight.x);
	TR_Y(_wnWin, W_PRI, _wnPhysical.upperRight.y);
	/* Clip to physical window coordinates */
	_wnPhysical.lowerLeft.x = VEMMAX(_wnPhysical.lowerLeft.x, 0);
	_wnPhysical.upperRight.x = VEMMIN(_wnPhysical.upperRight.x,_wnWin.width);
	_wnPhysical.lowerLeft.y = VEMMIN(_wnPhysical.lowerLeft.y, _wnWin.height);
	_wnPhysical.upperRight.y = VEMMAX(_wnPhysical.upperRight.y, 0);
	/* If region is exactly zero, we make it a bit bigger */
        if (_wnPhysical.lowerLeft.x == _wnPhysical.upperRight.x) {
	    (_wnPhysical.lowerLeft.x)--;  (_wnPhysical.upperRight.x)++;
	}
	if (_wnPhysical.lowerLeft.y == _wnPhysical.upperRight.y) {
	    (_wnPhysical.lowerLeft.y)++;  (_wnPhysical.upperRight.y)--;
	}
	/* Check it against the window boundaries */
	if ((_wnPhysical.lowerLeft.x < _wnPhysical.upperRight.x) &&
	    (_wnPhysical.lowerLeft.y > _wnPhysical.upperRight.y))
	  {
	      dspInit(_wnWin.XWin, (int) _wnPhysical.lowerLeft.x,
		      (int) _wnPhysical.upperRight.y,
		      (int) (_wnPhysical.upperRight.x -
			     _wnPhysical.lowerLeft.x + 1),
		      (int) (_wnPhysical.lowerLeft.y -
			     _wnPhysical.upperRight.y + 1));
	      selDraw(&theObj, &org_bb, tr_flag);
	      dspEnd();
	}
    }
    return ST_CONTINUE;
}

static void selDraw(obj, bb, t_flag)
octObject *obj;			/* Object to draw            */
struct octBox *bb;		/* Object bounding box       */
int t_flag;			/* Transform or not          */
/*
 * This routine is called to draw an item that has been selected.
 * It assumes that the attribute, transformation, and display drawing 
 * sequence has been set up properly. `t_flag' is set if the transformation
 * involved is not the identity transform.  Implicitly passes globals
 * _wnWin and _wnPhysical to _wnObjDisp().
 */
{
    static struct octPoint *pnts = (struct octPoint *) 0;
    static int pntcount = 0;
    int32 realcount;

    switch (obj->type) {
    case OCT_INSTANCE:
	/* Display bounding box of instance */
	obj->type = OCT_BOX;
	obj->contents.box = *bb;
	_wnObjDisp(obj, 0, (struct octPoint *) 0, t_flag,
		   tr_is_manhattan(_wnTranStack), (octObject *) 0);
	break;
    case OCT_POLYGON:
    case OCT_PATH:
	/* Get the points of the object */
	if (pntcount == 0) {
	    pntcount = INITPNTCOUNT;
	    pnts = VEMARRAYALLOC(struct octPoint, pntcount);
	}
	realcount = 0;
	octGetPoints(obj, &realcount, (struct octPoint *) 0);
	if (realcount >= pntcount-1) {
	    /* Make the array bigger */
	    pntcount = realcount + PNTPAD;
	    pnts = VEMREALLOC(struct octPoint, pnts, pntcount);
	}
	realcount = pntcount;
	VEM_OCTCKRET(octGetPoints(obj, &realcount, pnts));
	_wnObjDisp(obj, realcount, pnts, t_flag,
		   tr_is_manhattan(_wnTranStack),
		   (octObject *) 0);
	break;
    case OCT_BOX:
    case OCT_CIRCLE:
    case OCT_LABEL:
	/* Display object normally */
	_wnObjDisp(obj, 0, (struct octPoint *) 0, t_flag,
		   tr_is_manhattan(_wnTranStack), (octObject *) 0);
	break;
    case OCT_FACET:
    case OCT_PROP:
    case OCT_NET:
    case OCT_BAG:
    case OCT_LAYER:
    case OCT_POINT:
    case OCT_EDGE:
    case OCT_TERM:
	/* Unsupported types - ignore */
	break;
    }
}



void objRedraw()
/*
 * This routine is called by the window module to update object
 * selection in a particular region of a window.  It assumes
 * the drawing region has been set up properly before the
 * routine is called.  All of the drawing parameters _wnWin,
 * _wnPhysical, and _wnVirtual must be set.
 */
{
    lsGen theGen;
    vemObjects *anObjSet;
    octGenerator objGen;
    octObject theBag, nextObj;
    struct octBox invVirtual, bb;
    octCoord temp;
    int t_flag;

    if (!objList) {
	objList = lsCreate();
    }

    /* Push any existing transform */
    tr_push(_wnTranStack);

    theGen = lsStart(objList);
    while (lsNext(theGen, (lsGeneric *) &anObjSet, LS_NH) == VEM_OK) {
	if (anObjSet->theFct == _wnWin.fcts[W_PRI].winFctId) {
	    /* Clear out the transform and add object set transform */
	    tr_identity(_wnTranStack);
	    tr_add_transform(_wnTranStack, &(anObjSet->tran), 0);

	    /* Inverse transform virtual region to check against geometry */
	    invVirtual = _wnVirtual;
	    tr_inverse_transform(_wnTranStack, &invVirtual.lowerLeft.x,
				 &invVirtual.lowerLeft.y);
	    tr_inverse_transform(_wnTranStack, &invVirtual.upperRight.x,
				 &invVirtual.upperRight.y);
	    VEMBOXNORM(invVirtual, temp);

	    t_flag = !tr_is_identity(_wnTranStack);
	    atrSet(anObjSet->attr);
	    theBag.objectId = anObjSet->selBag;
	    if (octGetById(&theBag) != OCT_OK) {
		/* Bag is bad -- turn off highlighting */
		objOff(anObjSet);
		return;
	    }
	    VEM_OCTCKRET(octGetById(&theBag));
	    VEM_OCTCKRET(octInitGenContents(&theBag, OCT_DISP_MASK, &objGen));
	    while (octGenerate(&objGen, &nextObj) == OCT_OK) {
		/* See if it intersects the virtual region */
		if (octBB(&nextObj, &bb) == OCT_OK) {
		    if (VEMBOXINTERSECT(invVirtual, bb)) {
			selDraw(&nextObj, &bb, t_flag);
		    }
		} else {
		    vemMsg(MSG_L, "objRedraw: object has no bounding box\n");
		}
	    }
	}
    }
    lsFinish(theGen);
    /* Pop off the temporary transform */
    tr_pop(_wnTranStack);
}



void objSetRedraw(objattr, objfct, realWin, virtual, physical)
atrHandle objattr;		/* Handle of sets to redraw */
octId objfct;			/* Facet  of sets to redraw */
internalWin *realWin;		/* Window to draw in */
struct octBox *virtual;		/* Virtual region    */
struct octBox *physical;	/* Physical region   */
/*
 * This routine is similar to objRedraw except only objects
 * in the sets with the attribute `objattr' and the buffer
 * `objfct' are drawn.   The routine sets up the
 * drawing region and attributes so no other state preparation
 * is required.  It only draws selected sets which have their
 * own plane allocated.
 */
{
    vemObjects *objset;
    struct octBox invVirtual, bb;
    octObject theBag, nextObj;
    octCoord temp;
    octGenerator objGen;
    lsGen theGen;
    int t_flag;

    if (!objList) {
	objList = lsCreate();
    }

    /* Set up globals for _wnObjDisp() */
    _wnWin = *realWin;
    _wnVirtual = *virtual;
    _wnPhysical = *physical;
    
    /* Set up drawing region */
    dspInit(_wnWin.XWin, (int) _wnPhysical.lowerLeft.x,
	    (int) _wnPhysical.upperRight.y,
	    (int) (_wnPhysical.upperRight.x - _wnPhysical.lowerLeft.x),
	    (int) (_wnPhysical.lowerLeft.y - _wnPhysical.upperRight.y));

    /* Set the attribute */
    atrSet(objattr);
    
    /* Clear only that plane */
    dspClear(0);

    /* Do the necessary drawing */
    theGen = lsStart(objList);
    while (lsNext(theGen, (lsGeneric *) &objset, LS_NH) == LS_OK) {
	if ((objset->theFct == objfct) && (objset->attr == objattr)) {
	    /* Set up the transformation */
	    tr_push(_wnTranStack);
	    tr_identity(_wnTranStack);
	    tr_add_transform(_wnTranStack, &(objset->tran), 0);
    
	    /* Invert transform virtual region for geometry check */
	    invVirtual = _wnVirtual;
	    tr_inverse_transform(_wnTranStack, &invVirtual.lowerLeft.x,
				 &invVirtual.lowerLeft.y);
	    tr_inverse_transform(_wnTranStack, &invVirtual.upperRight.x,
				 &invVirtual.upperRight.y);
	    VEMBOXNORM(invVirtual, temp);

	    t_flag = !tr_is_identity(_wnTranStack);

	    /* Do the drawing */
	    theBag.objectId = objset->selBag;
	    VEM_OCTCKRET(octGetById(&theBag));
	    VEM_OCTCKRET(octInitGenContents(&theBag, OCT_DISP_MASK, &objGen));
	    while (octGenerate(&objGen, &nextObj) == OCT_OK) {
		/* See if it intersects the virtual region */
		VEM_OCTCKRET(octBB(&nextObj, &bb));
		if (VEMBOXINTERSECT(invVirtual, bb)) {
		    selDraw(&nextObj, &bb, t_flag);
		}
	    }
	    /* Pop the transform */
	    tr_pop(_wnTranStack);
	}
    }
    lsFinish(theGen);

    /* Stop the display */
    dspEnd();
}
