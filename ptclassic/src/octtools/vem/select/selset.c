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
 * VEM Selection Set Routines
 *
 * David Harrison
 * University of California,  Berkeley
 * 1986, 1987
 *
 * This file contains routines for the creation and maintenence
 * of an arbitrary number of selected sets.  This facility is
 * used by remote applications which might want to display selected
 * items using different colors for aesthetic purposes.
 *
 * This is a new version of this module (May, 1987).  The new
 * selection set paridigm used by VEM consists of two layers:
 * a low level object based highlighting subsystem (implemented
 * in objsel.c),  and a high-level interface (implemented here).
 * This module exports an interface which works on an entire
 * vemOneArg structure which is assumed to be of type VEM_OBJ_ARG.
 *
 * Unlike objsel.c,  this package deals with the primary bag
 * in this structure (`theBag').  These items may have multiple
 * objects actually highlighted (nets and bags for example).
 * Multiple objects are highlighted by adding these objects
 * to the highlight bag using objsel.c.
 */

#include "general.h"		/* General definitions       */
#include <X11/Xlib.h>		/* X library interface       */
#include "oct.h"		/* OCT interface definitions */
#include "attributes.h"		/* Attribute handler         */
#include "buffer.h"		/* Buffer managment          */
#include "windows.h"		/* Window management         */
#include "objsel.h"		/* Low-level object highlighting */
#include "commands.h"		/* Argument types            */
#include "region.h"		/* Region package            */
#include "selset.h"		/* Self declaration          */
#include "vemUtil.h"		/* General VEM utilities     */
#ifdef OLDBAGS
#include "bags.h"		/* VEM bag name manager      */
#endif

#define NORMBAGNAME	";;; SelSetBag ;;;"
#define HIGHBAGNAME	";;; HiLiteBag ;;;"
#define ARTBAGNAME	";;; ArtGeoBag ;;;"

static int findGeometry();
static int cleanArt();



vemOneArg *vemAttrSelSet(setFacet, someAttr)
octId setFacet;			/* Facet of selected set      */
atrHandle someAttr;		/* Attributes of selected set */
/*
 * This routine creates a new selected set given a predefined
 * VEM attribute specification.  It returns a newly created
 * vemOneArg structure which is set to the type VEM_OBJ_ARG.
 * This pointer should be used when calling other routines
 * in this package.  The routine will return (vemOneArg *) 0
 * if there were problems.
 */
{
    octObject theFacet, oneBag;
    vemOneArg *newSet;
    vemObjects *objset;

    newSet = VEMALLOC(vemOneArg);
    newSet->argType = VEM_OBJ_ARG;
    newSet->length = newSet->alloc_size = 0;
    objset = &(newSet->argData.vemObjArg);

    objset->theFct = setFacet;
    theFacet.objectId = setFacet;
    if (octGetById(&theFacet) != OCT_OK) {
	VEMFREE(newSet);
	return (vemOneArg *) 0;
    }

    /* Create a new bags to contain the selected items */
	
    oneBag.type = OCT_BAG;
    oneBag.contents.bag.name = NORMBAGNAME;
    if (vuTemporary(&theFacet, &oneBag) != OCT_OK) {
	VEMFREE(newSet);
	return (vemOneArg *) 0;
    }
    objset->theBag = oneBag.objectId;

    oneBag.type = OCT_BAG;
    oneBag.contents.bag.name = HIGHBAGNAME;
    if (vuTemporary(&theFacet, &oneBag) != OCT_OK) {
	oneBag.objectId = objset->theBag;
	if (octGetById(&oneBag)) octDelete(&oneBag);
	VEMFREE(newSet);
	return (vemOneArg *) 0;
    }
    objset->selBag = oneBag.objectId;

    /* Set appropriate fields in vemObjects structure */
    objset->tran.translation.x = 0;
    objset->tran.translation.y = 0;
    objset->tran.transformType = OCT_NO_TRANSFORM;
    objset->attr = someAttr;

    /* Inform the objsel module we have a new selection set */
    objOn(objset);

    /* Register the package bag names */
#ifdef OLDBAGS
    vemNewBagName(NORMBAGNAME);
    vemNewBagName(HIGHBAGNAME);
    vemNewBagName(ARTBAGNAME);
#endif

    return newSet;
}


vemOneArg *vemNewSelSet(setFacet, red, green, blue, len, lineStyle,
			width, height, pat)
octId setFacet;			/* Facet of selected set                 */
unsigned short red, green, blue;
				/* Selection color (normalized to 65535) */
int len, lineStyle;		/* Length and bits of line style pattern */
int width, height;		/* Size of fill pattern                  */
char *pat;			/* Pattern itself                        */
/*
 * This routine creates a new selected set and returns its handle.  This
 * handle should be used when calling all other routines in this package.
 * A selected set is specified by a facet (all objects selected must
 * belong to this facet) and some display attributes.  Note the color
 * specification is useful only on color displays.  If no fill pattern
 * is desired,  either width or height should be zero.  The routine returns
 * 0 if there were problems.  The new normalization of colors is 0-65535.
 */
{
    atrHandle newAttr;
    XColor theColor;
    flPattern fill, lpat;
    unsigned long mask;

    theColor.flags = DoRed|DoGreen|DoBlue;
    theColor.red = (unsigned short) red;
    theColor.green = (unsigned short) green;
    theColor.blue = (unsigned short) blue;
    
    /* Build line style from old line style */
    if (len > 0) {
	char buf[32];
	int i;

	for (i = len-1;  i >= 0;  i--) {
	    if (lineStyle & 0x01) buf[i] = '1';
	    else buf[i] = '0';
	    lineStyle >>= 1;
	}
	lpat = flNewPattern(len, 1, buf);
    } else {
	lpat = (flPattern) 0;
    }

    if ((width > 0) && (height > 0)) {
	fill = flNewPattern(width, height, pat);
    } else {
	fill = (flPattern) 0;
    }

    /* Define the selected set attributes */
    if (atrSelect(&newAttr, &mask, &theColor, fill, lpat, 1) < VEM_OK) {
	return (vemOneArg *) 0;
    }

    return vemAttrSelSet(setFacet, newAttr);
}


int vemFreeAttrSet(selset, selAttr)
vemOneArg *selset;		/* Selection set to free */
atrHandle *selAttr;		/* Returned attributes   */
/*
 * This routine clears the specified selected set and frees
 * all resources except the display attributes associated
 * with it.  The display attributes are returned in 'selAttr'.
 * This is meant to be used with vemAttrSelSet.
 */
{
    vemObjects *objset = &(selset->argData.vemObjArg);
    octObject oneBag;

    /* Turn off selection */
    vemClearSelSet(selset);

    /* Set up to return attributes */
    *selAttr = objset->attr;

    /* Turn off object selection */
    objOff(objset);

    /* Destroy the bags */
    oneBag.objectId = objset->theBag;
    if (octGetById(&oneBag) == OCT_OK) octDelete(&oneBag);
    oneBag.objectId = objset->selBag;
    if (octGetById(&oneBag) == OCT_OK) octDelete(&oneBag);

    /* Free the record */
    VEMFREE(selset);

    return 1;
}

int vemFreeSelSet(selset)
vemOneArg *selset;		/* Selection set to free */
/*
 * Clears the specified selected set and frees all resources (including
 * any display attributes) associated with it.  The specified selection 
 * set should not be referenced again.  This is meant to be used with
 * vemNewSelSet.
 */
{
    atrHandle theAttrs;

    vemFreeAttrSet(selset, &theAttrs);
    /* Free the attributes */
    atrRelease(theAttrs);
    return 1;
}



int vemClearSelSet(selset)
vemOneArg *selset;		/* Selection set to clear */
/*
 * Turns off highlighting on all objects in the specified selected
 * set and removes all objects from the set.  The set itself
 * remains intact and can be used again.  Returns 0 if there
 * were problems.
 */
{
    vemObjects *objset = &(selset->argData.vemObjArg);
    octGenerator octGen;
    octObject oneObject, mainBag;

    /* Initialize generator through all selected objects */
    mainBag.objectId = objset->theBag;
    if (octGetById(&mainBag) < OCT_OK) return 0;
    if (octInitGenContents(&mainBag, OCT_ALL_MASK, &octGen) != OCT_OK)
      return 0;
    while (octGenerate(&octGen, &oneObject) == OCT_OK) {
	switch (oneObject.type) {
	case OCT_BAG:
	case OCT_NET:
	case OCT_TERM:
	    if (oneObject.type != OCT_TERM) {
		/* De-select all of the normal geometries */
		findGeometry(objset, &oneObject, (octObject *) 0, 0);
	    }
	    /* Clean out special artifical geometries */
	    cleanArt(objset, &oneObject);
	    break;
	case OCT_INSTANCE:
	case OCT_POLYGON:
	case OCT_BOX:
	case OCT_CIRCLE:
	case OCT_PATH:
	case OCT_LABEL:
	    /* Simply remove the item from the selected set */
	    objSub(objset, &oneObject);
	    break;
	default:
	    /* This shouldn't be possible - simply ignore */
	    break;
	}
	/* Detach the item from the select set bag */
	octDetach(&mainBag, &oneObject);
    }
    selset->length = 0;
    /* Force updates */
    vemSyncEvents();
    return 1;
}



int vemAddSelSet(selset, objectId)
vemOneArg *selset;		/* Selected set to add item to */
octId objectId;			/* OCT id of object            */
/*
 * This routine adds the specified item to the selected set and
 * highlights it immediately.  For all objects with masks in
 * OCT_GEO_MASK,  there is a one-to-one coorespondence between
 * the item and the highlighted geometry.  For OCT_INSTANCE objects,
 * the bounding box of the instance is highlighted.  See findNet
 * for the highlighting done for nets.  For OCT_BAG objects,
 * all objects in the bag are highlighted according to the rules
 * above.  OCT_TERM objects are highlighted by creating artifical
 * geometry and selecting that.  The following object types ARE NOT supported:
 * OCT_FACET, OCT_PROP, OCT_LAYER, OCT_POINT, and OCT_EDGE.
 * Objects of these types are ignored.
 */
{
    vemObjects *objset = &(selset->argData.vemObjArg);
    octObject theObject, objFacet, selFacet, artBag, termImpl, mainBag;
    int attachFlag;

    theObject.objectId = objectId;
    if (octGetById(&theObject) != OCT_OK) return 0;
    selFacet.objectId = objset->theFct;
    if (octGetById(&selFacet) != OCT_OK) return 0;
    octGetFacet(&theObject, &objFacet);
    if (selFacet.objectId != objFacet.objectId) {
	return 0;
    }
    mainBag.objectId = objset->theBag;
    if (octGetById(&mainBag) < OCT_OK) return 0;
    if (octIsAttached(&mainBag, &theObject) == OCT_OK) {
	/* Already on the set */
	return 1;
    }
    switch (theObject.type) {
    case OCT_NET:
    case OCT_BAG:
    case OCT_TERM:
	/* Creates artifical bag for funny geometry */
	artBag.type = OCT_BAG;
	artBag.objectId = oct_null_id;
	artBag.contents.bag.name = ARTBAGNAME;
	if (vuTemporary(&selFacet, &artBag) != OCT_OK) {
	    return 0;
	}
	/* Attach the object to the bag */
	if (octAttach(&artBag, &theObject) != OCT_OK) {
	    octDelete(&artBag);
	    return 0;
	}
	/* Attach the main bag to the object */
	if (octAttach(&artBag, &mainBag) != OCT_OK) {
	    octDelete(&artBag);
	    return 0;
	}
	if (theObject.type == OCT_TERM) {
	    /* Create artificial geometry in artifical bag */
	    termImpl.type = OCT_BOX;
	    termImpl.objectId = oct_null_id;
	    if (regFindImpl(&theObject, &(termImpl.contents.box)) == REG_OK) {
		octCreate(&artBag, &termImpl);
		/* Select the newly created box */
		objAdd(objset, &termImpl);
		attachFlag = 1;
	    } else {
		attachFlag = 0;
	    }
	} else {
	    /* Find the highlightable stuff and hilight it */
	    findGeometry(objset, &theObject, &artBag, 1);
	    attachFlag = 1;
	}
	break;
    case OCT_INSTANCE:
    case OCT_POLYGON:
    case OCT_BOX:
    case OCT_CIRCLE:
    case OCT_PATH:
    case OCT_LABEL:
	/* Simply add the item to the selected set */
	objAdd(objset, &theObject);
	attachFlag = 1;
	break;
    default:
	/* Error:  ignore the item */
	attachFlag = 0;
	break;
    }
    /* If successfully added,  attach to bag */
    if (attachFlag) {
	if (octAttach(&mainBag, &theObject) != OCT_OK) {
	    /* Bad news - can't attach it        */
	    /* Some clean up should be done here */
	    return 0;
	} else {
	    selset->length += 1;
	    return 1;
	}
    } else return 0;
}



int vemDelSelSet(selset, objectId)
vemOneArg *selset;		/* Selected set to remove item from */
octId objectId;			/* OCT id of object to remove       */
/*
 * This routine removes an item from a selected set.  It is assumed
 * the object was placed on the selected set using 'vemAddSelSet'.
 * Note removing objects with many highlighted objects causes all
 * of its highlighted objects to be unhighlighted (OCT_BAG for
 * example).
 */
{
    vemObjects *objset = &(selset->argData.vemObjArg);
    octObject theObject, mainBag;

    theObject.objectId = objectId;
    if (octGetById(&theObject) != OCT_OK) return 0;
    mainBag.objectId = objset->theBag;
    if (octGetById(&mainBag) != OCT_OK) return 0;
    /* Check to make sure it is attached */
    if (octDetach(&mainBag, &theObject) != OCT_OK) return 0;
    switch (theObject.type) {
    case OCT_NET:
    case OCT_BAG:
    case OCT_TERM:
	if (theObject.type != OCT_TERM) {
	    /* Recursively traverse heirarchy unselecting geometry */
	    findGeometry(objset, &theObject, (octObject *) 0, 0);
	}
	/* Clean up artificial geometry */
	cleanArt(objset, &theObject);
	break;
    case OCT_INSTANCE:
    case OCT_POLYGON:
    case OCT_BOX:
    case OCT_CIRCLE:
    case OCT_PATH:
    case OCT_LABEL:
	/* Simply remove the item from the selected set */
	objSub(objset, &theObject);
	break;
    default:
	/* Error:  ignore the item */
	break;
    }
    selset->length -= 1;
    /* Force an update */   /* But why? */
    /* vemSyncEvents(); */  /* This is too slow here!! */
    return 1;
}


static int findGeometry(objset, oneObject, artBag, flag)
vemObjects *objset;		/* objsel.c set          */
octObject *oneObject;		/* OCT object            */
octObject *artBag;		/* Artificial geo bag    */
int flag;			/* If on,  selects       */
/*
 * This routine generates the contents of objects of type
 * OCT_NET and OCT_BAG and calls itself recursively on each
 * object generated.  If the object is some form of geometry
 * (OCT_GEO|OCT_INSTANCE),  the object is either selected
 * or unselected based on 'flag'.  If the object is a terminal,
 * and `flag' is set,  a temporary box is created and attached to 
 * `artBag'.  That box is then highlighted.
 */
{
    octGenerator octGen;
    octObject genObject, termImpl;
    int returnValue = 0, localRtn;

    switch (oneObject->type) {
    case OCT_BAG:
    case OCT_NET:
	/* Generate through the contents and call recursively */
	if (octInitGenContents(oneObject, OCT_ALL_MASK, &octGen) != OCT_OK)
	  return 0;
	while (octGenerate(&octGen, &genObject) == OCT_OK) {
	    localRtn = findGeometry(objset, &genObject, artBag, flag);
	    returnValue = VEMMAX(returnValue, localRtn);
	}
	return returnValue;
    case OCT_TERM:
	/* Create some artifical geometry to show terminal */
	if (flag) {
	    termImpl.type = OCT_BOX;
	    termImpl.objectId = oct_null_id;
	    if (regFindImpl(oneObject, &(termImpl.contents.box)) == REG_OK) {
		octCreate(artBag, &termImpl);
		/* Select the newly created box */
		objAdd(objset, &termImpl);
	    }
	}
	return 1;
    case OCT_INSTANCE:
    case OCT_POLYGON:
    case OCT_BOX:
    case OCT_CIRCLE:
    case OCT_PATH:
    case OCT_LABEL:
	if (flag)
	  objAdd(objset, oneObject);
	else
	  objSub(objset, oneObject);
	return 1;
    default:
	/* Unknown type:  return 0 */
	return 0;
    }
}


static int cleanArt(objset, someObj)
vemObjects *objset;		/* Low level handle      */
octObject *someObj;		/* Net, bag, or terminal */
/*
 * Nets, bags, and terminals have an artificial geometry bag
 * as one of their containers.  This routine finds this bag
 * and deletes all of the geometry attached to it and then 
 * deletes the bag itself.  The geometries were created to
 * hilight things not necessarily in the same facet as the
 * main hilight set.
 */
{
    octGenerator theGen;
    octObject genObj, artBag, mainBag;

    mainBag.objectId = objset->theBag;
    if (octGetById(&mainBag) != OCT_OK) return 0;

    /* Find the artifical bag */
    if (octInitGenContainers(someObj, OCT_BAG_MASK, &theGen) != OCT_OK) {
	return 0;
    }
    artBag.objectId = oct_null_id;
    while (octGenerate(&theGen, &genObj) == OCT_OK) {
	if ((STRCOMP(genObj.contents.bag.name, ARTBAGNAME) == 0) &&
	    (octIsAttached(&genObj, &mainBag) == OCT_OK)) {
	    artBag = genObj;
	}
    }
    if (artBag.objectId == oct_null_id) {
	return 0;
    }

    /* Detach object and mainBag from the bag */
    octDetach(&artBag, someObj);
    octDetach(&artBag, &mainBag);

    /* Get rid of the temporary objects */
    if (octInitGenContents(&artBag, OCT_ALL_MASK, &theGen) != OCT_OK) {
	return 0;
    }
    while (octGenerate(&theGen, &genObj) == OCT_OK) {
	/* De-select it then get rid of it */
	objSub(objset, &genObj);
	octDelete(&genObj);
    }
    /* Destroy the artificial bag itself */
    octDelete(&artBag);
    return 1;
}


int vemRCSelSet(selset, oldObjId, newObjId)
vemOneArg *selset;		/* Selected set for replacement   */
octId oldObjId, newObjId;	/* Old object and its replacement */
/*
 * This is a special purpose routine that replaces `oldObjId' with
 * `newObjId' assuming that the highlighting DOES NOT change.  Thus,
 * no change in the display of the selected set is carried out.  This
 * is most often used by commands that copy items where the copy
 * should replace the original in the highlight set.  In general,
 * only geometry (not nets, bags, or terminals) should be replaced 
 * in this fashion.
 */
{
    vemObjects *objset = &(selset->argData.vemObjArg);
    struct octObject oldObject, newObject, mainBag;

    oldObject.objectId = oldObjId;
    if (octGetById(&oldObject) < OCT_OK) return 0;
    newObject.objectId = newObjId;
    if (octGetById(&newObject) < OCT_OK) return 0;
    if (oldObject.type != newObject.type) return 0;
    mainBag.objectId = objset->theBag;
    if (octGetById(&mainBag) < OCT_OK) return 0;
    
    switch (oldObject.type) {
    case OCT_INSTANCE:
    case OCT_POLYGON:
    case OCT_BOX:
    case OCT_CIRCLE:
    case OCT_PATH:
    case OCT_LABEL:
	/* Detach old from bag,  attach new to bag */
	if (octDetach(&mainBag, &oldObject) < OCT_OK) return 0;
	if (octAttach(&mainBag, &newObject) < OCT_OK) return 0;
	/* Handle actual highlighting */
	objRepl(objset, &oldObject, &newObject);
	break;
    default:
	/* Can't handle it */
	return 0;
    }
    return 1;
}
    


int vemZoomSelSet(selset, win, scale)
vemOneArg *selset;		/* Selected set to zoom to */
Window win;			/* Window to zoom in       */
double scale;			/* Expansion factor        */
/*
 * This routine computes the bounding box of all selected
 * items in 'selset' and zooms 'win' to that extent.  The
 * bounding box displayed is actually the bounding box of
 * the selected items multiplied by the factor _scale_.  This
 * can be used to specify variable amounts of padding around
 * the highlighted items.
 */
{
    struct octBox bb;
    int extraWidth, extraHeight;

    if (objBB(&(selset->argData.vemObjArg), &bb) != VEM_OK) {
	return 0;
    }

    /* Adds slack */
    extraWidth = (int) ((bb.upperRight.x - bb.lowerLeft.x) * scale + 0.5);
    extraHeight = (int) ((bb.upperRight.y - bb.lowerLeft.y) * scale + 0.5);
    bb.lowerLeft.x -= extraWidth;
    bb.lowerLeft.y -= extraHeight;
    bb.upperRight.x += extraWidth;
    bb.upperRight.y += extraHeight;

    /* Zooms window */
    wnZoom(win, &bb);
    wnQWindow(win);
    return 1;
}



