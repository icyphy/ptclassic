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
 * VEM Utility Routines
 * Oct Assistance
 *
 * David Harrison
 * University of California,  Berkeley
 * 1987
 *
 * This file contains routines which help VEM deal with Oct.
 * They include routines for helping to parse argument lists
 * and disambiguate selection.
 */

#include "general.h"
#include <X11/Xlib.h>
#include "oct.h"
#include "region.h"
#include "vemDM.h"
#include "vemUtil.h"
#include "message.h"
#include "commands.h"
#include "list.h"
#include "da.h"
#include "st.h"
#include "windows.h"
#include "defaults.h"
#include "argsel.h"
#include "hilite.h"
#include "selset.h"
#include "selection.h"

/* Not done elsewhere */
extern void qsort();

/* Forward */
static int traceBag();

/* Used by vuSubArgFromBag */
typedef struct vu_sub_lyrbag {
    st_table *off_layers;
    octObject *selBag;
} vuSubLyrBag;

/* List of layer names for vuFindLayer */
#define INITLRSIZE	10
static int realLrSize = 0;
static dmWhichItem *layerList = (dmWhichItem *) 0;



octStatus vuCreateTemporary(container, object)
octObject *container;		/* Containing object */
octObject *object;		/* New object        */
/*
 * This routine creates a new object using octCreate()
 * and then marks it as temporary using octMarkTemporary().
 */
{
    octStatus ret_value;

    if ((ret_value = octCreate(container, object)) >= OCT_OK) {
	octMarkTemporary(object);
    }
    return ret_value;
}

octStatus vuTemporary(fct, object)
octObject *fct;			/* What facet */
octObject *object;		/* New object */
/*
 * This routine is similar to vuCreateTemporary except that
 * the object is created on a known bag attached to the facet
 * of the object.  The name of the bag is given by VEMTEMPBAG.
 * If this bag does not exist,  it is created.  The last facet
 * is maintainted for efficiency in finding the bag.
 */
{
    octObject bag;
    static octId lastFctId;
    static octId lastVemBagId;
    octStatus stat;

    if (fct->objectId == lastFctId) {
	bag.objectId = lastVemBagId;
	if ((stat = octGetById(&bag)) != OCT_OK) return stat;
    } else {
	bag.type = OCT_BAG;
	bag.contents.bag.name = VEMTEMPBAG;
	if (octGetByName(fct, &bag) != OCT_OK) {
	    if ((stat = octCreate(fct, &bag)) != OCT_OK) return stat;
	    octMarkTemporary(&bag);
	}
	lastFctId = fct->objectId;
	lastVemBagId = bag.objectId;
    }
    return vuCreateTemporary(&bag, object);
}



int lyrcmp(a, b)
char *a, *b;
/* Comparison function for qsort and uniq */
{
    return STRCOMP(((dmWhichItem *) a)->itemName,
		  ((dmWhichItem *) b)->itemName);
}

vemStatus vuFindLayer(theFacet, spot, layerName)
octObject *theFacet;		/* Facet to look in        */
struct octPoint *spot;		/* Place to look for layer */
char *layerName;		/* Returned layer name     */
/*
 * This routine finds the layer of the object under 'spot' and
 * copies it into 'layerName'.  'layerName' should be user
 * allocated space.  If the spot is ambiguous,  it will use a
 * dialog box to ask which layer.  NOTE:  this routine produces messages
 * to the console window if things go wrong!
 */
{
    struct octObject newLayer, spotObj, tempLayer;
    struct octBox spotBox;
    regObjGen theGen;
    octGenerator octGen;
    octStatus retCode;
    vemStatus utReturn;
    int idx;

    if (realLrSize == 0) {
	realLrSize = INITLRSIZE;
	layerList = VEMARRAYALLOC(dmWhichItem, realLrSize);
    }

    /* Try to find using spot -- use region package to find it */

    newLayer.contents.layer.name = (char *) 0;
    spotBox.lowerLeft = *spot;
    spotBox.upperRight = *spot;
    utReturn = regObjStart(theFacet, &spotBox, OCT_GEO_MASK, &theGen, 0);
    if (utReturn == REG_NOMORE) {
	return VEM_CANTFIND;
    } else if (utReturn != REG_OK) {
	vemMessage("vuFindLayer:  Unable to generate objects under spot\n",
		   0);
	return VEM_CORRUPT;
    }
    idx = 0;
    while (regObjNext(theGen, &spotObj) == REG_OK) {
	/* We have an object - try to get the layer */
	if (octInitGenContainers(&spotObj,OCT_LAYER_MASK,&octGen) != OCT_OK)
	  {
	      vemMsg(MSG_C, "Can't generate layer from spot object:\n  %s\n",
		     octErrorString());
	      regObjFinish(theGen);
	      return VEM_CORRUPT;
	  }
	tempLayer.contents.layer.name = "";
	while ((retCode = octGenerate(&octGen, &tempLayer)) == OCT_OK) {
	    /* Do nothing:  use last layer found */
	}
	if (retCode != OCT_GEN_DONE) {
	    vemMsg(MSG_C, "Error while getting layer of spot object:\n  %s\n",
		   octErrorString());
	    regObjFinish(theGen);
	    return VEM_CORRUPT;
	}
	if (idx >= realLrSize) {
	    /* Make it bigger */
	    realLrSize *= 2;
	    layerList = VEMREALLOC(dmWhichItem, layerList, realLrSize);
	}
	layerList[idx++].itemName = tempLayer.contents.layer.name;
    }
    regObjFinish(theGen);
    if (idx == 0) {
	return VEM_CANTFIND;
    } else {
	/* Sort and remove duplicates */
	qsort((char *) layerList, idx, sizeof(dmWhichItem), lyrcmp);
	idx = uniq((char *) layerList, idx, sizeof(dmWhichItem), lyrcmp);
	if (idx == 1) {
	    STRMOVE(layerName, layerList[0].itemName);
	    return VEM_OK;
	} else {
	    int item = 0;
	    char help_buf[VEMMAXSTR*5];

	    /* Ask which one */
	    (void) sprintf(help_buf, "%s\n%s\n%s\n%s\n%s",
		    "Since you did not explicitly specify",
		    "a layer,  I looked for a unique layer",
		    "under the cursor.  Unfortunately,  I",
		    "found several suitable layers.  Please",
		    "choose the one you want.");
	    if (dmWhichOne("Ambiguous Layer", idx,
			   layerList, &item, (int (*)()) 0, help_buf) == VEM_OK)
	      {
		  STRMOVE(layerName, layerList[item].itemName);
		  return VEM_OK;
	      } else {
		  return VEM_NOTUNIQ;
	      }
	}
    }
}


/*
 * When vuArgTemplate encounters a point with more than one object under
 * it,  it asks the user to choose zero or more of them.  The definitions
 * on this page support this interface.
 */

/* Dynamic array of dmWhichItem */
#define INITPNTSIZE	10
static dary realPnts = (dary) 0;
static vemOneArg *pntSet = (vemOneArg *) 0;

static int pntInit(fctId)
octId fctId;			/* Facet of item selection  */
/*
 * Initializes the state of the item selection interface.
 */
{
    if (!realPnts) realPnts = daAlloc(dmWhichItem, INITPNTSIZE);
    else daReset(realPnts);
    if (vemGetHiLite(fctId, &pntSet) != VEM_OK) {
	return 0;
    } else {
	return 1;
    }
}

/*ARGSUSED*/
static void addPntObj(theObj, data)
octObject *theObj;		/* Object to add to dialog */
Pointer data;			/* Not used in this case   */
/*
 * Adds an object to the list of objects to be displayed in the
 * dialog.  The user data for the object is the object id.  This
 * will be used to highlight the item.  
 */
{
    int i = daLen(realPnts);

    daSet(dmWhichItem, realPnts, i)->itemName = vuMakeName(theObj);
    daSet(dmWhichItem, realPnts, i)->userData = (Pointer) theObj->objectId;
    daSet(dmWhichItem, realPnts, i)->flag = 0;
}

static void pntEnd()
/*
 * This routine is called when the point selection of items
 * is finished.  It frees up the highlight set and deallocates
 * the display strings for the items.
 */
{
    int idx;

    vemRelHiLite();
    for (idx = 0;  idx < daLen(realPnts);  idx++) {
	VEMFREE(daGet(dmWhichItem, realPnts, idx)->itemName);
    }
}

/*ARGSUSED*/
static int showPntItem(item_idx, item_value, user_data)
int item_idx;			/* Which item is selected */
int item_value;			/* Whether on or off      */
char *user_data;		/* Associated data        */
/*
 * This routine is called by dmMultiWhich when it detects
 * a change in state of one of the posted items.  This routine
 * adds the item to the highlight set if its on and removes
 * it if it is off.
 */
{
    if (item_value) {
	vemAddSelSet(pntSet, (octId) user_data);
    } else {
	vemDelSelSet(pntSet, (octId) user_data);
    }
    return 1;
}

static void attFunc(obj, data)
octObject *obj;			/* Object to add to set */
Pointer data;			/* Pointer to bag       */
/*
 * This routine is called by the region acceptance function
 * passed to vuArgTemplate if the object passes all requirements.
 * This routine adds it to the bag passed as data.
 */
{
    octObject *bag = (octObject *) data;

    VEM_OCTCKRET(octAttach(bag, obj));
}



static vemStatus vuArgTemplate(theFacet, theBag, arg, regFunc, objArg)
octObject *theFacet;		/* Facet for arguments     */
octObject *theBag;		/* Bag to add items to     */
vemOneArg *arg;			/* Argument for adding     */
vemStatus (*regFunc)();		/* Region handler          */
Pointer objArg;			/* Argument to region handler */
/*
 * This routine is the template function used by all parsing
 * functions like vuAddArgToBag.  It processes each argument
 * building up boxes for search purposes.  For each box discovered,
 * it calls `regFunc' with the following arguments:
 *   vemStatus regFunc(facet, box, inF, func, data, objArg)
 *   octObject *facet;
 *   struct octBox *box;
 *   int inF;
 *   void (*func)();
 *   Pointer data;
 *   Pointer objArg;
 * This function is expected to examine the items in `box' and
 * determine which objects should be added to the selected set.
 * If `inF' is set,  only objects strictly in the region should be 
 * considered. Those objects that meet the function's criteria,  
 * should be passed to `func' that has the following form:
 *   void func(obj, data)
 *   octObject *obj;
 *   Pointer data;
 * Where `data' is what was passed in to the region function as data.
 * For point arguments,  all objects accepted are grouped into
 * an array.  If after the argument has been processed,  there
 * is more than one object,  a dialog will be used to ask which
 * of the items should be added to the bag.  For line and box
 * arguments,  all accepted objects are added to the bag.
 */
{
    struct octObject spotObj;
    struct octBox spotBox;
    octCoord temp;
    int idx, idx2, dontAttach;
    char help_buf[1024];

    switch (arg->argType) {
    case VEM_POINT_ARG:
	/* For all points in the argument: */
	for (idx = 0;  idx < arg->length;  idx++) {
	    VEM_CKRVAL(pntInit(theFacet->objectId),
		       "Failed pntInit", VEM_CORRUPT);
	    if (arg->argData.points[idx].theFct != theFacet->objectId)
	      continue;   /* Not in the same buffer */
	    /* Now build region for search */
	    spotBox.lowerLeft = arg->argData.points[idx].thePoint;
	    spotBox.upperRight = arg->argData.points[idx].thePoint;
	    /* Call region search routine */
	    VEM_CKRVAL((*regFunc)(theFacet, &spotBox, 0, addPntObj,
				  (Pointer) 0, objArg) == VEM_OK,
		       "Region template function failed", VEM_CORRUPT);
#ifdef NOTDEF
	    if (regObjStart(theFacet, &spotBox,
			    OCT_GEO_MASK|OCT_INSTANCE_MASK,
			    &theGen, 0) != REG_OK)
	      continue;		/* No objects to generate */
	    diagIdx = 0;
	    while (regObjNext(theGen, &spotObj) == REG_OK) {
		if ((*objFunc)(theFacet, &spotObj, &r_objs, objArg) == VEM_OK) {
		    for (oc = 0;  oc < daLen(r_objs);  oc++) {
			retObj = daGet(octObject, r_objs, oc);
			addPntObj(&retObj, diagIdx++);
		    }
		}
	    }
	    regObjFinish(theGen);
#endif
	    dontAttach = 0;
	    if (daLen(realPnts) > 1) {
		/* Show a dialog box */
		(void) sprintf(help_buf, "%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s",
			"There are several objects under the",
			"cursor suitable for selection.  Choose",
			"one or more of them by clicking on their",
			"description in the dialog box with the",
			"mouse (the object will become highlighted).",
			"To reverse a choice,  click on the description",
			"again.  Press Ok and those highlighted",
			"will be added to the selected set.");

		if (dmMultiWhich("Which Objects?", daLen(realPnts),
				 daData(dmWhichItem, realPnts), showPntItem,
				 help_buf) == VEM_NOSELECT)
		  {
		      dontAttach = 1;
		  }
	    } else if (daLen(realPnts) == 1) {
		daGet(dmWhichItem, realPnts, 0)->flag = 1;
	    }
	    /* Add the selected ones to the bag */
	    for (idx2 = 0;  idx2 < daLen(realPnts);  idx2++) {
		if (daGet(dmWhichItem, realPnts, idx2)->flag && !dontAttach) {
		    spotObj.objectId =
		      (octId) daGet(dmWhichItem, realPnts, idx2)->userData;
		    octGetById(&spotObj);
		    if (octAttach(theBag, &spotObj) != OCT_OK) {
			vemMsg(MSG_C,
			       "vuAddArgToBag: octAttach failed:\n  %s\n",
			       octErrorString());
		    }
		}
	    }
	    pntEnd();
	}
	break;
    case VEM_LINE_ARG:
	/* For all lines in the argument: */
	for (idx = 0;  idx < arg->length-1;  idx++) {
	    if ((arg->argData.points[idx].theFct != theFacet->objectId) ||
		(arg->argData.points[idx+1].theFct != theFacet->objectId))
	      continue;  /* Not in same buffer */
	    /* 
	     * Now find all objects which intersect a box formed
	     * by the line (the line is made one unit wide if necessary).
	     */
	    spotBox.lowerLeft = arg->argData.points[idx].thePoint;
	    spotBox.upperRight = arg->argData.points[idx+1].thePoint;
	    VEMBOXNORM(spotBox, temp);
	    if (spotBox.lowerLeft.x == spotBox.upperRight.x)
	      spotBox.upperRight.x += 1;
	    if (spotBox.lowerLeft.y == spotBox.upperRight.y)
	      spotBox.upperRight.y += 1;
	    /* Call region search routine */
	    VEM_CKRVAL((*regFunc)(theFacet, &spotBox, 0, attFunc,
				  (Pointer) theBag, objArg) == VEM_OK,
		       "Region template function failed", VEM_CORRUPT);
#ifdef NOTDEF
	    if (regObjStart(theFacet, &spotBox,
			    OCT_GEO_MASK|OCT_INSTANCE_MASK,
			    &theGen, 0) != REG_OK)
	      continue;  /* No geometry */
	    while (regObjNext(theGen, &spotObj) == REG_OK) {
		if ((*objFunc)(theFacet, &spotObj, &r_objs, objArg) == VEM_OK) {
		    for (oc = 0;  oc < daLen(r_objs);  oc++) {
			retObj = daGet(octObject, r_objs, oc);
			if (octAttach(theBag, &retObj) != OCT_OK) {
			    vemMsg(MSG_L, "vuAddArgToBag: octAttach failed:\n  %s\n",
				   octErrorString());
			}
		    }
		} 
	    }
	    regObjFinish(theGen);
#endif
	}
	break;
    case VEM_BOX_ARG:
	/* For all boxes in the argument: */
	for (idx = 0;  idx < arg->length;  idx++) {
	    if (arg->argData.points[idx*2].theFct != theFacet->objectId)
	      continue;  /* Not in the same buffer */
	    /* Now find all objects INSIDE the box and add to bag */
	    spotBox.lowerLeft = arg->argData.points[idx*2].thePoint;
	    spotBox.upperRight = arg->argData.points[idx*2+1].thePoint;
	    /* Normalize the box */
	    VEMBOXNORM(spotBox, temp);
	    /* Call region function */
	    VEM_CKRVAL((*regFunc)(theFacet, &spotBox, 1, attFunc,
				  (Pointer) theBag, objArg) == VEM_OK,
		       "Region template function failed", VEM_CORRUPT);
	}
	break;
    default:
	/* Bogus argument type - ignored */
	break;
    }
    return VEM_OK;
}


/*ARGSUSED*/
static vemStatus argObjHandler(fct, box, inF, func, data, arg)
octObject *fct;			/* Facet of object    */
struct octBox *box;		/* Search region      */
int inF;			/* Inside flag        */
void (*func)();			/* Function to call   */
Pointer data;			/* Data to function   */
Pointer arg;			/* Arbitrary argument */
/*
 * This routine is passed to vuArgTemplate from vuAddArgToBag.
 * It will examine the objects in `box' and call `func' for
 * each one found that is on a layer in the hash table
 * of layers passed as `arg'.
 */
{
    st_table *off_layers = (st_table *) arg;
    octObject theLayer, obj;
    regObjGen theGen;

    if (regObjStart(fct, box, OCT_GEO_MASK|OCT_INSTANCE_MASK,
		    &theGen, inF) != REG_OK)
      return VEM_OK;	/* No geometry */
    while (regObjNext(theGen, &obj) == REG_OK) {
	/* Call function on object if on st_table */
	if (octGenFirstContainer(&obj, OCT_LAYER_MASK, &theLayer) == OCT_OK) {
	    if (!off_layers ||
		(!st_is_member(off_layers, theLayer.contents.layer.name))) {
		(*func)(&obj, data);
	    }
	} else {
	    /* If no layer, select it anyway */
	    (*func)(&obj, data);
	}
    }
    regObjFinish(theGen);
    return VEM_OK;
}

/*ARGSUSED*/
vemStatus vuAddArgToBag(fct, theBag, arg, set_bag, off_layers)
octObject *fct;			/* Facet for arguments     */
octObject *theBag;		/* Bag to add items to     */
vemOneArg *arg;			/* Argument for adding     */
octObject *set_bag;		/* Bag of selected items   */
st_table *off_layers;		/* Layers to omit          */
/*
 * This routine uses vuArgTemplate to add raw geometry to the
 * bag `theBag'.  If `off_layers' is provided,  only layers
 * with names which ARE NOT in the table will be selected.
 * `set_bag' is provided only for reference (see vuSubArgFromBag
 * for how this can be used). See vuArgTemplate for details on 
 * the user interface.
 */
{
    return vuArgTemplate(fct, theBag, arg, argObjHandler, (Pointer) off_layers);
}


/*ARGSUSED*/
static vemStatus argUnselHandler(fct, box, inF, func, dm, arg)
octObject *fct;			/* Facet of object  */
struct octBox *box;		/* Region to search */
int inF;			/* Inside flag      */
void (*func)();			/* Function to call */
Pointer dm;			/* Data to func     */
Pointer arg;			/* Arbitrary arg    */
/*
 * This routine is passed to vuArgTemplate from vuSubArgFromBag.
 * `arg' is a pointer to vuSubLyrBag structure.  The routine
 * will call `func' with `data' for each object found in
 * the routine on a layer in the hash table passed in as
 * arg and on the bag passed in as arg.
 */
{
    vuSubLyrBag *data = (vuSubLyrBag *) arg;
    octObject theLayer, obj;
    regObjGen theGen;

    if (regObjStart(fct, box, OCT_GEO_MASK|OCT_INSTANCE_MASK,
		    &theGen, inF) != REG_OK)
      return VEM_OK;		/* No geometry */
    while (regObjNext(theGen, &obj) == REG_OK) {
	/* Apply tests to object */
	if (octGenFirstContainer(&obj, OCT_LAYER_MASK, &theLayer) == OCT_OK) {
	    if (!data->off_layers ||
		(!st_is_member(data->off_layers,
			       theLayer.contents.layer.name))) {
		/* We know is not on an invisible layer */
		/* check to see if its on the bag       */
		if (data->selBag &&
		    (octIsAttached(data->selBag, &obj) == OCT_OK)) {
		    (*func)(&obj, dm);
		}
	    }
	} else if ((data->selBag) &&
		   (octIsAttached(data->selBag, &obj) == OCT_OK)) {
	    (*func)(&obj, dm);
	}
    }
    regObjFinish(theGen);
    return VEM_OK;
}

/*ARGSUSED*/
vemStatus vuSubArgFromBag(fct, theBag, arg, set_bag, off_layers)
octObject *fct;			/* Facet for arguments  */
octObject *theBag;		/* Bag to add items to  */
vemOneArg *arg;			/* Argument for adding  */
octObject *set_bag;		/* Selected items       */
st_table *off_layers;		/* Layers not displayed */
/*
 * This routine uses vuArgTemplate to add raw geometry to the
 * bag `theBag'.  Only items attached to layers which are
 * NOT in `off_layers' AND attached to `set_bag' will be
 * added to `theBag'. vuArgTemplate specifies the user 
 * interface for the routine.
 */
{
    vuSubLyrBag data;

    data.off_layers = off_layers;
    data.selBag = set_bag;

    return vuArgTemplate(fct, theBag, arg, argUnselHandler, (Pointer) &data);
}



/*ARGSUSED*/
static vemStatus argTermHandler(fct, box, inF, func, data, arg)
octObject *fct;			/* Facet of object  */
struct octBox *box;		/* Box to search    */
int inF;			/* Inside flag      */
void (*func)();			/* Function to call */
Pointer data;			/* Data to function */
Pointer arg;			/* Arbitrary arg    */
/*
 * This routine is passed to vuArgTemplate from vuAddTermToBag.
 * It examines all objects in `box' and calls `func' for all
 * actual and formal terminals found in the region.
 */
{
    octGenerator gen;
    octObject theTerm, obj;
    st_table *the_table = (st_table *) arg;
    regObjGen theGen;
    regTermInfo *term_ary;
    int found, i;

    if (regObjStart(fct, box, OCT_GEO_MASK|OCT_INSTANCE_MASK,
		    &theGen, inF) != REG_OK)
      return VEM_OK;		/* No objects */
    /* First geometric objects */
    while (regObjNext(theGen, &obj) == REG_OK) {
	/* Apply tests to object */
	VEM_OCTCKRVAL(octInitGenContainers(&obj,OCT_TERM_MASK, &gen), VEM_FALSE);
	while (octGenerate(&gen, &theTerm) == OCT_OK) {
	    /* Found a terminal - see if its already added */
	    if (!st_is_member(the_table, (char *) theTerm.objectId)) {
		/* Its not - add and return */
		st_insert(the_table, (char *) theTerm.objectId, (char *) 0);
		(*func)(&obj, data);
	    }
	}
    }
    regObjFinish(theGen);
    /* Then using the region package */
    if (regMultiActual(fct, box, &found, &term_ary) == REG_OK) {
	for (i = 0;  i < found;  i++) {
	    if (!st_is_member(the_table, (char *) term_ary[i].term.objectId)) {
		st_insert(the_table, (char *) term_ary[i].term.objectId, (char *) 0);
		(*func)(&(term_ary[i].term), data);
	    }
	    VEM_OCTCKRVAL(octInitGenContainers(&(term_ary[i].term),
					       OCT_TERM_MASK, &gen),
			  VEM_FALSE);
	    while (octGenerate(&gen, &theTerm) == OCT_OK) {
		if (!st_is_member(the_table, (char *) theTerm.objectId)) {
		    st_insert(the_table, (char *) theTerm.objectId, (char *) 0);
		    (*func)(&theTerm, data);
		}
	    }
	}
	VEMFREE(term_ary);
    }
    return VEM_OK;
}

/*ARGSUSED*/
vemStatus vuAddTermToBag(fct, theBag, arg, set_bag, extra)
octObject *fct;			/* Facet for arguments     */
octObject *theBag;		/* Bag to add items to     */
vemOneArg *arg;			/* Argument for adding     */
octObject *set_bag;		/* Objects already added   */
Pointer extra;			/* Additional info         */
/*
 * The terminals whose implementations are those found by
 * parsing `arg' are added to the bag `theBag'.  See vuArgTemplate
 * for details on the user interface.
 */
{
    st_table *the_table;
    vemStatus ret_code;

    the_table = st_init_table(st_numcmp, st_numhash);
    ret_code = vuArgTemplate(fct, theBag, arg, argTermHandler,
			     (Pointer) the_table);
    st_free_table(the_table);
    return ret_code;
}


/*ARGSUSED*/
static vemStatus argLyrHandler(fct, box, inF, func, data, arg)
octObject *fct;			/* Facet of object    */
struct octBox *box;		/* Region to search   */
int inF;			/* Inside flag        */
void (*func)();			/* Function to call   */
Pointer data;			/* Data to function   */
Pointer arg;			/* Arbitrary argument */
/*
 * This routine is passed to vuArgTemplate from vuAddLyrToBag.
 * It examines all data in `box' and calls `func' for those
 * on the layer whose name is passed in `arg'.
 */
{
    octObject theLayer, obj;
    char *condLayerName = (char *) arg;
    regObjGen theGen;

    if (regObjStart(fct, box, OCT_GEO_MASK|OCT_INSTANCE_MASK,
		    &theGen, inF) != REG_OK)
      return VEM_OK;	/* No geometry */
    while (regObjNext(theGen, &obj) == REG_OK) {
	/* Call function on object if on layer */
	if (octGenFirstContainer(&obj, OCT_LAYER_MASK, &theLayer) == OCT_OK) {
	    if (STRCOMP(theLayer.contents.layer.name, condLayerName) == 0) {
		(*func)(&obj, data);
	    }
	}
    }
    regObjFinish(theGen);
    return VEM_OK;
}


/*ARGSUSED*/
vemStatus vuAddLyrToBag(fct, theBag, arg, set_bag, lyr_name)
octObject *fct;			/* Facet for arguments     */
octObject *theBag;		/* Bag to add items to     */
vemOneArg *arg;			/* Argument for adding     */
octObject *set_bag;		/* Objects already added   */
char *lyr_name;			/* Additional info         */
/*
 * Uses vuArgTemplate to add raw geometry to the bag `theBag'.
 * If `lyr_name' is provided,  only geometry on that layer will
 * be added to the bag.  `set_bag' is provided only for reference
 * only.  See vuArgTemplate for details on the user interface.
 */
{
    return vuArgTemplate(fct, theBag, arg, argLyrHandler, (Pointer) lyr_name);
}



/*
 * vuFindSpot uses a interface similar to vuAddArgToBag for individual points.
 * Thus,  it also uses the routines pntInit, addPntObj, and pntEnd to
 * build the list of items.  However,  it uses its own hilighting routine
 * to make sure only one item is highlighted.
 */

/*ARGSUSED*/
static int fndShowItem(item_idx, user_data)
int item_idx;			/* Which item is selected */
char *user_data;		/* Associated data        */
/*
 * This routine is called by dmWhichOne when a new blender
 * choice is made.  The routine clears the highlight set
 * and adds the new object to it.
 */
{
    octId theId = (octId) user_data;
    octObject theObj;

    vemClearSelSet(pntSet);
    theObj.objectId = theId;
    if (octGetById(&theObj) == OCT_OK) {
	if (theObj.type != OCT_TERM) {
	    vemAddSelSet(pntSet, theId);
	}
    }
    return 1;
}

vemStatus vuFindSpot(spot, rtnObj, mask)
vemPoint *spot;			/* Place to look        */
octObject *rtnObj;		/* Returned object      */
octObjectMask mask;		/* Mask			*/
/*
 * This routine looks for geometry under the given spot.  If
 * there is a unique object there,  it is returned in 'rtnObj'.
 * If it is ambiguous,  the user is queried using a dialog
 * box to choose one.  If the object is an instance,  it also
 * looks to see if the point is over an actual or formal terminal.
 * If so,  the terminals are added to the list of items presented
 * to the user.  If there is no object under the spot,  it returns
 * VEM_CANTFIND and rtnObj is set to the spot facet.  If no selection
 * is made,  it returns VEM_NOSELECT and rtnObj is unmodified.
 */
{
    struct octObject theFacet, spotObj, termObj, subTermObj;
    struct octBox spotBox, implBB;
    octObjectMask local_mask;
    regObjGen theGen;
    octGenerator octGen;
    int finalItem;

    theFacet.objectId = spot->theFct;
    if (octGetById(&theFacet) != OCT_OK) {
	vemMsg(MSG_A, "vuFindSpot: Can't get facet:\n  %s\n",
	       octErrorString());
	return VEM_CORRUPT;
    }
    /* Set up the loop to find objects */
    spotBox.upperRight = spotBox.lowerLeft = spot->thePoint;
    local_mask = mask;
    if (mask & OCT_TERM_MASK) {
	local_mask = mask | (OCT_INSTANCE_MASK|OCT_GEO_MASK);
    }
    if (regObjStart(&theFacet, &spotBox,
		    (local_mask & (OCT_GEO_MASK|OCT_INSTANCE_MASK)),
		    &theGen, 0) != REG_OK)
      {
	  /* No objects to generate */
	  *rtnObj = theFacet;
	  return VEM_CANTFIND;
      }
    VEM_CKRVAL(pntInit(spot->theFct), "Failed pntInit", VEM_CORRUPT);
    while (regObjNext(theGen, &spotObj) == REG_OK) {
	/* Add object to list */
	if (spotObj.type == OCT_INSTANCE) {
	    if (mask & OCT_TERM_MASK) {
		/* Add the instance if specified */
		if (mask & OCT_INSTANCE_MASK) addPntObj(&spotObj, (Pointer) 0);
		/* Find the formal and actual terminals (if any) */
		if (regFindActual(&theFacet, &(spot->thePoint),
				  &termObj, &implBB) == REG_OK) {
		    /* Add actual to list */
		    addPntObj(&termObj, (Pointer) 0);
		    /* Actual may implement a formal */
		    if (octInitGenContainers(&termObj, OCT_TERM_MASK,
					       &octGen) != OCT_OK)
			continue;
		    while (octGenerate(&octGen, &subTermObj) == OCT_OK) {
			/* Add to list */
			addPntObj(&subTermObj, (Pointer) 0);
		    }
		}
	    } else {
		/* Always add instance */
		addPntObj(&spotObj, (Pointer) 0);
	    }
	} else {
	    if (mask & OCT_TERM_MASK) {
		/* Add geometry if specified */
		if ((1<<spotObj.type) & mask) {
		    addPntObj(&spotObj, (Pointer) 0);
		}
		/* Must be geometry.  See if there are formal terminals */
		if (octInitGenContainers(&spotObj, OCT_TERM_MASK, &octGen) != OCT_OK)
		    continue;
		while (octGenerate(&octGen, &termObj) == OCT_OK) {
		    /* Add to list */
		    addPntObj(&termObj, (Pointer) 0);
		}
	    } else {
		addPntObj(&spotObj, (Pointer) 0);
	    }
	}
    }
    regObjFinish(theGen);
    /* Ask about list if more than one */
    finalItem = 0;
    if (daLen(realPnts) == 0) {
	*rtnObj = theFacet;
	pntEnd();
	return VEM_CANTFIND;
    } else if (daLen(realPnts) == 1) {
	rtnObj->objectId = (octId) daGet(dmWhichItem, realPnts, 0)->userData;
	octGetById(rtnObj);
	pntEnd();
	return VEM_OK;
    } else if (daLen(realPnts) > 1) {
	char help_buf[VEMMAXSTR*5];

	/* Show the dialog */
	(void) sprintf(help_buf, "%s\n%s\n%s",
		"You pointed at several objects but",
		"only one is needed.  Please choose",
		"the one you want.");
	if (dmWhichOne("What Object?", daLen(realPnts),
		       daData(dmWhichItem, realPnts), &finalItem,
		       fndShowItem, help_buf) == VEM_NOSELECT) 
	  {
	      pntEnd();
	      return VEM_NOSELECT;
	  } else {
	      rtnObj->objectId =
		(octId) daGet(dmWhichItem, realPnts, finalItem)->userData;
	      octGetById(rtnObj);
	      pntEnd();
	      return VEM_OK;
	  }
    } else {
	pntEnd();
	return VEM_NOSELECT;
    }
}


vemStatus vuFindBag(spot, rtnBag)
vemPoint *spot;			/* Point over an object */
octObject *rtnBag;		/* Returned bag         */
/*
 * This routine looks for a bag connected to an object under
 * the given spot.  The routine uses vuFindSpot to uniquely
 * discover the object,  then calls a routine which interactively
 * allows the user to trace the bag heirarchy from that point.
 * When he finally specifies a bag,  it is returned in 'rtnBag'.
 * If he aborts,  the routine returns VEM_NOSELECT.  May return
 * VEM_CORRUPT if something went wrong or VEM_CANTFIND if there
 * aren't any bags.
 */
{
    octObject spotObj;

    if (vuFindSpot(spot, &spotObj, OCT_ALL_MASK) == VEM_NOSELECT) {
	return VEM_NOSELECT;
    }
    /* Call the interactive heirarchy tracer */
    return traceBag(spot->theFct, &spotObj, rtnBag);
}

static int traceBag(fctId, thisOne, result)
octId fctId;			/* Facet Id for selection */
octObject *thisOne;		/* Object to examine    */
octObject *result;		/* Final bag            */
/*
 * This routine queries 'thisOne' for bags in the objects contents
 * and containers.  If there are none and the object is a bag,
 * it is returned as 'result'.   If there are some,  and the
 * object is a bag,  the routine asks if the user wants this
 * bag or one of its related bags.  If related bags are requested,
 * it may also ask which ones (contents or containers) if there
 * are both.  Otherwise,  it pops up a list of the bags using
 * dmWhichOne and a selected item interface similar to vuFindSpot.
 * A bag chosen in this fashion is then passed to this routine
 * recursively.
 */
{
    octGenerator theGen;
    octObject bagObj;
    char *thisName, help_buf[VEMMAXSTR*5];
    int aboveFlag, belowFlag, contentsFlag, finalItem = 0;

    /* First,  determine whether there are related bags */
    aboveFlag = belowFlag = 0;
    if (octInitGenContents(thisOne, OCT_BAG_MASK, &theGen) == OCT_OK) {
	while (octGenerate(&theGen, &bagObj) == OCT_OK) {
	    if (!octIsTemporary(&bagObj)) {
		belowFlag = 1;
	    }
	}
    }
    if (octInitGenContainers(thisOne, OCT_BAG_MASK, &theGen) == OCT_OK) {
	while (octGenerate(&theGen, &bagObj) == OCT_OK) {
	    if (!octIsTemporary(&bagObj)) {
		aboveFlag = 1;
	    }
	}
    }

    if (aboveFlag || belowFlag) {
	/* 
	 * If the primary object is a bag,  ask whether the user 
	 * wants to stop here with this bag or browse the related ones.
	 */
	if (thisOne->type == OCT_BAG) {
	    errMsgArea[0] = '\0';
	    STRCONCAT(errMsgArea, "There are other bags attached to\n");
	    STRCONCAT(errMsgArea, "the current bag.  Would you like to\n");
	    STRCONCAT(errMsgArea, "select this one or browse through the\n");
	    STRCONCAT(errMsgArea, "related bags?");
	    if (dmConfirm("Related Bags", errMsgArea,
			  "This One", "Related Bags") == VEM_OK)
	      {
		  /* He selected this one */
		  *result = *thisOne;
		  return VEM_OK;
	      }
	}
	/* If there are both,  ask which one */
	if (aboveFlag && belowFlag) {
	    errMsgArea[0] = '\0';
	    STRCONCAT(errMsgArea, "There are bags attached both\n");
	    STRCONCAT(errMsgArea, "above (containers) and below (contents)\n");
	    STRCONCAT(errMsgArea, "the current bag.  Which ones would you\n");
	    STRCONCAT(errMsgArea, "like to see?");
	    if (dmConfirm("Bag Relation", errMsgArea, "Contents",
			  "Containers") == VEM_OK)
	      {
		  contentsFlag = 1;
	      } else {
		  contentsFlag = 0;
	      }
	} else if (belowFlag) {
	    contentsFlag = 1;
	} else {
	    contentsFlag = 0;
	}
	/* Build up list of bags to show */
	VEM_CKRVAL(pntInit(fctId), "Failed pntInit", VEM_CORRUPT);
	thisName = vuMakeName(thisOne);
	if (contentsFlag) {
	    octInitGenContents(thisOne, OCT_BAG_MASK, &theGen);
	    STRMOVE(errMsgArea, "Contents of ");
	    STRCONCAT(errMsgArea, thisName);
	    (void) sprintf(help_buf, "%s\n%s\n%s",
		    "There are several bags attached to",
		    "this object.  Please choose the one",
		    "you wish to examine.");
	} else {
	    octInitGenContainers(thisOne, OCT_BAG_MASK, &theGen);
	    STRMOVE(errMsgArea, "Containers of ");
	    STRCONCAT(errMsgArea, thisName);
	    (void) sprintf(help_buf, "%s\n%s\n%s",
		    "There are several bags which contain",
		    "this object.  Please choose the one",
		    "you wish to examine.");
	}
	VEMFREE(thisName);
	while (octGenerate(&theGen, &bagObj) == OCT_OK) {
	    if (!octIsTemporary(&bagObj) != VEM_OK) {
		addPntObj(&bagObj, (Pointer) 0);
	    }
	}
	if (dmWhichOne(errMsgArea, daLen(realPnts),
		       daData(dmWhichItem, realPnts), &finalItem,
		       fndShowItem, help_buf) == VEM_NOSELECT)
	  {
	      pntEnd();
	      return VEM_NOSELECT;
	  } else {
	      pntEnd();
	      /* Call recursively */
	      bagObj.objectId =
		(octId) daGet(dmWhichItem, realPnts, finalItem)->userData;
	      octGetById(&bagObj);
	      return traceBag(fctId, &bagObj, result);
	  }
    } else {
	/* Nothing attached to it if its a bag thats it */
	if (thisOne->type == OCT_BAG) {
	    *result = *thisOne;
	    return VEM_OK;
	} else {
	    return VEM_CANTFIND;
	}

    }
}
			   


vemStatus vuAddBag(bag, addBag)
octObject *bag;			/* Bag to add items to   */
octObject *addBag;		/* Bag of objects to add */
/*
 * This routine attaches all objects in 'addBag' not in 'bag' to 'bag'.
 * Only the items added to 'bag' remain in 'addBag'.  Precondition:  items 
 * attached to 'addBag' are in the same facet as 'bag' (note: no check
 * is done!).
 */
{
    octGenerator theGen, subGen;
    octObject bagObject, subObject;

    /* First,  clear out the duplicates */
    if (octInitGenContents(bag, OCT_ALL_MASK, &theGen) != OCT_OK) {
	vemMsg(MSG_L, "vuAddBag:  Unable to get main bag contents:\n  %s\n",
	       octErrorString());
	return VEM_CORRUPT;
    }
    while (octGenerate(&theGen, &bagObject) == OCT_OK) {
	if (octInitGenContents(addBag, OCT_ALL_MASK, &subGen) != OCT_OK) {
	    vemMsg(MSG_L,
		   "vuAddBag:  Can't generate bag contents:\n  %s\n",
		    octErrorString());
	    return VEM_CORRUPT;
	}
	while (octGenerate(&subGen, &subObject) == OCT_OK) {
	    if (bagObject.objectId == subObject.objectId) {
		/* Duplicate -- Remove from addBag and set to zero */
		octDetach(addBag, &subObject);
		subObject.objectId = oct_null_id;
		break;
	    }
	}
    }
    /* Now that the addBag is clean,  attach its items to bag */
    octInitGenContents(addBag, OCT_ALL_MASK, &theGen);
    while (octGenerate(&theGen, &bagObject) == OCT_OK) {
	if (octAttach(bag, &bagObject) != OCT_OK) {
	    vemMsg(MSG_L, "vuAddBag: octAttach failed:\n  %s\n",
		   octErrorString());
	}
    }
    return VEM_OK;
}
		



vemStatus vuSubBag(bag, subBag)
octObject *bag;			/* Bag to subtract items from */
octObject *subBag;		/* Bag of objects to subtract */
/*
 * This routine detaches from both bags all objects in 'subBag' which
 * are attached to both bags.  The effect is to remove from 'bag'
 * any objects in 'subBag' and leave only the items subtracted
 * in 'subBag'.  Assumes items in both bags are in the same facet.
 */
{
    octGenerator bagGen, subBagGen;
    octObject bagObject, testObject;
    octStatus retCode;

    /* Generate each item in subBag and see if its in bag */
    if (octInitGenContents(subBag, OCT_ALL_MASK, &bagGen) != OCT_OK) {
	vemMsg(MSG_C, "vuSubBag: Can't start generation of bag:\n  %s\n",
	       octErrorString());
	return VEM_CORRUPT;
    }
    while (octGenerate(&bagGen, &bagObject) == OCT_OK) {
	/* Is it part of the regular bag? */
	if (octInitGenContents(bag, OCT_ALL_MASK, &subBagGen) != OCT_OK)
	  continue;  /* Can't generate any items */
	while ((retCode = octGenerate(&subBagGen, &testObject)) == OCT_OK) {
	    if (bagObject.objectId == testObject.objectId) {
		/* Found him:  detach from main bag */
		octDetach(bag, &testObject);
		break;
	    }
	}
	if (retCode == OCT_GEN_DONE) {
	    /* Didn't find the item - subtract from subBag */
	    octDetach(subBag, &bagObject);
	}
    }
    return VEM_OK;
}


static int onLayer(obj, layer)
octObject *obj;			/* Object to check  */
octObject *layer;		/* Layer for object */
/*
 * This routine return 1 if the given object is attached
 * to the given layer. 
 */
{
    octGenerator theGen;
    octObject someObj;
    int found;

    octInitGenContainers(obj, OCT_LAYER_MASK, &theGen);
    found = 0;
    while (octGenerate(&theGen, &someObj) == OCT_OK) {
	if (someObj.objectId == layer->objectId) {
	    found = 1;
	}
    }
    return found;
}

vemStatus vuMinusLayer(bag, layer)
struct octObject *bag;		/* Bag to remove items from */
struct octObject *layer;	/* Conditioning layer       */
/*
 * This routine removes all geometries from 'bag' which are
 * not attached to 'layer'.  It is used by commands whose
 * arguments are conditioned on a layer.
 */
{
    octGenerator theGen;
    struct octObject someObject;

    octInitGenContents(bag, OCT_GEO_MASK, &theGen);
    while (octGenerate(&theGen, &someObject) == OCT_OK) {
	if (!onLayer(&someObject, layer)) {
	    octDetach(bag, &someObject); /* Not on right layer */
	}
    }
    return VEM_OK;
}


#define LOCALBAGNAME	";;; vuObjArgParseBag ;;;"

/*ARGSUSED*/
vemStatus vuObjArgParse(spot, cmdList, theFctId, finalBag, layerFlag)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* Argument list            */
octId *theFctId;		/* Result facet id of items */
octObject *finalBag;		/* Result bag               */
int layerFlag;			/* Whether it is layer specific */
/*
 * This routine parses the argument list for commands which
 * take a bag of objects possibly conditioned by layer.  The
 * argument list is assumed to be VEM_OBJ_ARG or a collection
 * of points and boxes followed by an optional layer name 
 * specification.  If there is no layer name,  the parsing
 * routine will attempt to find one under 'spot'.  The argument
 * list is left unchanged.  The routine produces its own
 * diagnostics.  The resulting bag is a pure fabrication and
 * should be deleted after use.  This should be updated to
 * allow for incremental additions to an existing argument bag.
 * NOTE: layer specific flag is ignored currently
 */
{
    vemOneArg *firstArg, *thisArg;
    octObject targetFacet, argBag;
    lsGen listGen;
    st_table *lyr_tbl;
    wnOpts opts;

    finalBag->type = OCT_BAG;
    finalBag->objectId = oct_null_id;
    finalBag->contents.bag.name = LOCALBAGNAME;
#ifdef OLDBAGS
    vemNewBagName(LOCALBAGNAME);
#endif
    if (lsFirstItem(cmdList, (Pointer *) &firstArg, LS_NH) == LS_OK) {
	if (firstArg->argType == VEM_OBJ_ARG) {
	    /* He has given us a set -- get the bag */
	    argBag.objectId = firstArg->argData.vemObjArg.theBag;
	    if (octGetById(&argBag) != OCT_OK) {
		vemMessage("Unable to get selected set\n", MSG_NOLOG|MSG_DISP);
		return VEM_CORRUPT;
	    }
	    *theFctId = firstArg->argData.vemObjArg.theFct;
	    targetFacet.objectId = *theFctId;
	    if (octGetById(&targetFacet) != OCT_OK) {
		vemMsg(MSG_A, "Unable to get facet of set:\n  %s\n",
		       octErrorString());
		return VEM_CORRUPT;
	    }
	    if (vuCreateTemporary(&targetFacet, finalBag) != OCT_OK) {
		vemMsg(MSG_C, "Can't create temporary bag:\n  %s\n",
		       octErrorString());
		return VEM_ARGRESP;
	    }
	    vuAddBag(finalBag, &argBag);
	} else if (firstArg->argType != VEM_TEXT_ARG) {
	    /* Points and boxes have been specified.  Get the target buffer. */
	    *theFctId = firstArg->argData.points[0].theFct;
	    targetFacet.objectId = *theFctId;
	    if (octGetById(&targetFacet) != OCT_OK) {
		vemMsg(MSG_A, "Unable to get facet of set:\n  %s\n",
		       octErrorString());
		return VEM_CORRUPT;
	    }
	    
	    if (wnGetInfo(spot->theWin, &opts, (octId *) 0,
			  (octId *) 0, (bdTable *) 0,
			  (struct octBox *) 0) == VEM_OK) {
		lyr_tbl = opts.off_layers;
	    } else {
		lyr_tbl = (st_table *) 0;
	    }
	    /* Alright,  we have layer information.  Build up the finalBag */
	    if (vuCreateTemporary(&targetFacet, finalBag) != OCT_OK) {
		vemMsg(MSG_C, "Can't create temporary bag:\n  %s\n",
		       octErrorString());
		return VEM_ARGRESP;
	    }
	    listGen = lsStart(cmdList);
	    while (lsNext(listGen, (Pointer *) &thisArg, LS_NH) == LS_OK) {
		/* Got an object */
		vuAddArgToBag(&targetFacet, finalBag, thisArg,
			      (octObject *) 0, lyr_tbl);
	    }
	} else {
	    vemMessage("First argument can't be text\n", MSG_NOLOG|MSG_DISP);
	    return VEM_CORRUPT;
	}
    } else {
	vemMessage("Missing points, boxes, lines, or text\n",
		   MSG_NOLOG|MSG_DISP);
	return VEM_CORRUPT;
    }
    return VEM_OK;
}




vemStatus vuOffSelect(arg)
vemOneArg *arg;
/*
 * This routine takes an argument list item (arg) and turns
 * off all selection on the item and releases all preallocated
 * space for the argument.  The argument itself is also freed
 * (using free).  It can then be deleted from an argument list
 * using lsDelete.
 */
{
    static atrHandle selattr = (atrHandle) 0;

    switch (arg->argType) {
    case VEM_POINT_ARG:
    case VEM_BOX_ARG:
    case VEM_LINE_ARG:
	if (!selattr) {
	    dfGetAttr("select", &selattr);
	}
	atrSetXor(selattr);
	argOff(arg);
	VEMFREE(arg->argData.points);
	VEMFREE(arg);
	break;
    case VEM_TEXT_ARG:
	VEMFREE(arg->argData.vemTextArg);
	VEMFREE(arg);
	break;
    case VEM_OBJ_ARG:
	/*
	 * The commands in selection.c are the only ones that know
	 * about the selection attributes for VEM_OBJ_ARG arguments.
	 * Thus,  we call on a special routine in that module for
	 * getting rid of this type of argument.
	 */
	selectWipe(arg);
	break;
    }
    return VEM_OK;
}



vemStatus vuWipeList(someList)
lsList someList;		/* List to wipe clean */
/*
 * Eliminates all items in the specified list without destroying
 * the list itself.  It deselects all points and frees all necessary
 * data.
 */
{
    lsGen generator;
    vemOneArg *oneItem;

    generator = lsStart(someList);
    while (lsNext(generator, (Pointer *) &oneItem, LS_NH) == LS_OK) {
	vuOffSelect(oneItem);
	lsDelBefore(generator, (Pointer *) &oneItem);
    }
    lsFinish(generator);
    return VEM_OK;
}



static int xid(obj)
octObject *obj;
/*
 * Returns the external id of the object.
 */
{
    int32 val;

    octExternalId(obj, &val);
    return (int) val;
}

#define MAX_OBJ_SIZE	4096
#define END(m)	((m)+strlen(m))

static char *disp_inst(obj)
octObject *obj;
/* Returns a textual representation of an instance */
{
    static char ibuf[MAX_OBJ_SIZE];

    (void) sprintf(ibuf, "OCT_INSTANCE_%d[%s:%s:%s", (int) obj->objectId,
		   obj->contents.instance.master,
		   obj->contents.instance.view,
		   obj->contents.instance.facet);
    if (STRLEN(obj->contents.instance.name) > 0) {
	(void) sprintf(END(ibuf), " N=`%s'", obj->contents.instance.name);
    }
    if (STRLEN(obj->contents.instance.version) > 0) {
	(void) sprintf(END(ibuf), " V=`%s'", obj->contents.instance.version);
    }
    switch (obj->contents.instance.transform.transformType) {
    case OCT_NO_TRANSFORM:
	(void) sprintf(END(ibuf), " OCT_NO_TRANSFORM");
	break;
    case OCT_MIRROR_X:
	(void) sprintf(END(ibuf), " OCT_MIRROR_X");
	break;
    case OCT_MIRROR_Y:
	(void) sprintf(END(ibuf), " OCT_MIRROR_Y");
	break;
    case OCT_ROT90:
	(void) sprintf(END(ibuf), " OCT_ROT90");
	break;
    case OCT_ROT180:
	(void) sprintf(END(ibuf), " OCT_ROT180");
	break;
    case OCT_ROT270:
	(void) sprintf(END(ibuf), " OCT_ROT270");
	break;
    case OCT_MX_ROT90:
	(void) sprintf(END(ibuf), " OCT_MX_ROT90");
	break;
    case OCT_MY_ROT90:
	(void) sprintf(END(ibuf), " OCT_MY_ROT90");
	break;
    case OCT_FULL_TRANSFORM:
	(void) sprintf(END(ibuf), " OCT_FULL_TRANSFORM");
	break;
    }
    (void) sprintf(END(ibuf), "  (%ld,%ld)]",
		   (long)obj->contents.instance.transform.translation.x,
		   (long)obj->contents.instance.transform.translation.y);
    return ibuf;
}



#define MAX_PNTS	5
#define MAX_POINTS	1024

static char *disp_points(obj)
octObject *obj;
/* Displays some points of a path or polygon */
{
    static char pntbuf[MAX_OBJ_SIZE];
    struct octPoint points[MAX_POINTS];
    int32 num = MAX_POINTS, i;

    pntbuf[0] = '\0';
    if (octGetPoints(obj, &num, points) != OCT_OK) {
	(void) sprintf(pntbuf, "[too many points]");
	return pntbuf;
    }
    if (num == 0) {
	(void) sprintf(pntbuf, "[no points]");
	return pntbuf;
    }
    pntbuf[0] = '\0';
    if (num > MAX_PNTS) {
	for (i = 0;  i < MAX_PNTS-1;  i++) {
	    (void) sprintf(END(pntbuf), " %ld,%ld",
			   (long)points[i].x, (long)points[i].y);
	}
	(void) sprintf(END(pntbuf), " ... %ld,%ld",
		       (long)points[num-1].x, (long)points[num-1].y);
    } else {
	for (i = 0;  i < num;  i++) {
	    (void) sprintf(END(pntbuf), " %ld,%ld",
			   (long)points[i].x, (long)points[i].y);
	}
    }
    return pntbuf;
}


static char *disp_prop(obj)
octObject *obj;
/* Displays a property */
{
    static char propbuf[MAX_OBJ_SIZE];
    int i;

    (void) sprintf(propbuf, "OCT_PROP_%d[`%s'", (int) obj->objectId,
	    obj->contents.prop.name);
    switch (obj->contents.prop.type) {
    case OCT_NULL:
	(void) sprintf(END(propbuf), " OCT_NULL]");
	break;
    case OCT_INTEGER:
	(void) sprintf(END(propbuf), " OCT_INTEGER=%ld]",
		(long)obj->contents.prop.value.integer);
	break;
    case OCT_REAL:
	(void) sprintf(END(propbuf), " OCT_REAL=%g]",
		obj->contents.prop.value.real);
	break;
    case OCT_STRING:
	(void) sprintf(END(propbuf), " OCT_STRING=`%s']",
		obj->contents.prop.value.string);
	break;
    case OCT_ID:
	(void) sprintf(END(propbuf), " OCT_ID=%d]",
		(int) obj->contents.prop.value.id);
	break;
    case OCT_STRANGER:
	(void) sprintf(END(propbuf), " OCT_STRANGER]");
	break;
    case OCT_REAL_ARRAY:
	(void) sprintf(END(propbuf), " OCT_REAL_ARRAY=<");
	for (i = 0;  i < obj->contents.prop.value.real_array.length;  i++) {
	    (void) sprintf(END(propbuf), " %g",
		    obj->contents.prop.value.real_array.array[i]);
	}
	(void) sprintf(END(propbuf), ">]");
	break;
    case OCT_INTEGER_ARRAY:
	(void) sprintf(END(propbuf), " OCT_INTEGER_ARRAY=<");
	for (i = 0;  i < obj->contents.prop.value.integer_array.length;  i++) {
	    (void) sprintf(END(propbuf), " %ld",
		    (long)obj->contents.prop.value.integer_array.array[i]);
	}
	(void) sprintf(END(propbuf), ">]");
	break;
    }
    return propbuf;
}

static char *disp_omask(mask)
octObjectMask mask;
/* Returns a string representation of an object mask */
{
    static char omask_buf[500];

    omask_buf[0] = '\0';
    if (mask & OCT_FACET_MASK) strcat(omask_buf, "|OCT_FACET_MASK");
    if (mask & OCT_TERM_MASK) strcat(omask_buf, "|OCT_TERM_MASK");
    if (mask & OCT_NET_MASK) strcat(omask_buf, "|OCT_NET_MASK");
    if (mask & OCT_INSTANCE_MASK) strcat(omask_buf, "|OCT_INSTANCE_MASK");
    if (mask & OCT_PROP_MASK) strcat(omask_buf, "|OCT_PROP_MASK");
    if (mask & OCT_BAG_MASK) strcat(omask_buf, "|OCT_BAG_MASK");
    if (mask & OCT_POLYGON_MASK) strcat(omask_buf, "|OCT_POLYGON_MASK");
    if (mask & OCT_BOX_MASK) strcat(omask_buf, "|OCT_BOX_MASK");
    if (mask & OCT_CIRCLE_MASK) strcat(omask_buf, "|OCT_CIRCLE_MASK");
    if (mask & OCT_PATH_MASK) strcat(omask_buf, "|OCT_PATH_MASK");
    if (mask & OCT_LABEL_MASK) strcat(omask_buf, "|OCT_LABEL_MASK");
    if (mask & OCT_LAYER_MASK) strcat(omask_buf, "|OCT_LAYER_MASK");
    if (mask & OCT_POINT_MASK) strcat(omask_buf, "|OCT_POINT_MASK");
    if (mask & OCT_EDGE_MASK) strcat(omask_buf, "|OCT_EDGE_MASK");
    if (mask & OCT_FORMAL_MASK) strcat(omask_buf, "|OCT_FORMAL_MASK");
    if (mask & OCT_CHANGE_LIST_MASK) strcat(omask_buf, "|OCT_CHANGE_LIST_MASK");
    if (mask & OCT_CHANGE_RECORD_MASK) strcat(omask_buf, "|OCT_CHANGE_RECORD_MASK");
    if (mask & OCT_FORMAL_MASK) strcat(omask_buf, "|OCT_FORMAL_MASK");
    if (omask_buf[0] == '|') return &omask_buf[1];
    else return (char *) omask_buf;
}

static char *disp_fmask(mask)
octFunctionMask mask;
/* Returns a string representation of a function mask */
{
    static char fmask_buf[500];

    fmask_buf[0] = '\0';
    if (mask & OCT_CREATE_MASK) strcat(fmask_buf, "|OCT_CREATE_MASK");
    if (mask & OCT_DELETE_MASK) strcat(fmask_buf, "|OCT_DELETE_MASK");
    if (mask & OCT_MODIFY_MASK) strcat(fmask_buf, "|OCT_MODIFY_MASK");
    if (mask & OCT_PUT_POINTS_MASK) strcat(fmask_buf, "|OCT_PUT_POINTS_MASK");
    if (mask & OCT_DETACH_CONTENT_MASK) strcat(fmask_buf, "|OCT_DETACH_CONTENT_MASK");
    if (mask & OCT_DETACH_CONTAINER_MASK) strcat(fmask_buf, "|OCT_DETACH_CONTAINER_MASK");
    if (mask & OCT_ATTACH_CONTENT_MASK) strcat(fmask_buf, "|OCT_ATTACH_CONTENT_MASK");
    if (mask & OCT_ATTACH_CONTAINER_MASK) strcat(fmask_buf, "|OCT_ATTACH_CONTAINER_MASK");
    if (fmask_buf[0] == '|') return &fmask_buf[1];
    else return (char *) fmask_buf;
}


char *disp_crec(obj)
octObject *obj;
/* Displays a change record */
{
    static char crecbuf[MAX_OBJ_SIZE];

    (void) sprintf(crecbuf, "OCT_CHANGE_RECORD_%d[", (int) obj->objectId);
    switch (obj->contents.changeRecord.changeType) {
    case OCT_CREATE:
	(void) sprintf(END(crecbuf), "OCT_CREATE");
	break;
    case OCT_DELETE:
	(void) sprintf(END(crecbuf), "OCT_DELETE");
	break;
    case OCT_MODIFY:
	(void) sprintf(END(crecbuf), "OCT_MODIFY");
	break;
    case OCT_PUT_POINTS:
	(void) sprintf(END(crecbuf), "OCT_PUT_POINTS");
	break;
    case OCT_ATTACH_CONTENT:
	(void) sprintf(END(crecbuf), "OCT_ATTACH_CONTENT");
	break;
    case OCT_ATTACH_CONTAINER:
	(void) sprintf(END(crecbuf), "OCT_ATTACH_CONTAINER");
	break;
    case OCT_DETACH_CONTENT:
	(void) sprintf(END(crecbuf), "OCT_DETACH_CONTENT");
	break;
    case OCT_DETACH_CONTAINER:
	(void) sprintf(END(crecbuf), "OCT_DETACH_CONTAINER");
	break;
    }
    (void) sprintf(END(crecbuf), ", %d, %d]",
		   (int) obj->contents.changeRecord.objectExternalId,
		   (int) obj->contents.changeRecord.contentsExternalId);
    return crecbuf;
}



extern char *vuDispObject(obj)
octObject *obj;
/* Returns a string suitable for display given an object */
{
    static char final[MAX_OBJ_SIZE];
    octObject layerObj;

    layerObj.contents.layer.name = "No Layer";
    final[0] = '\0';
    switch (obj->type) {
    case OCT_UNDEFINED_OBJECT:
	(void) sprintf(END(final), "OCT_UNDEFINED");
	break;
    case OCT_FACET:
	(void) sprintf(END(final), "OCT_FACET_%d[%s:%s:%s]", xid(obj),
		obj->contents.facet.cell,
		obj->contents.facet.view,
		obj->contents.facet.facet);
	break;
    case OCT_TERM:
	(void) sprintf(END(final), "OCT_TERMINAL_%d[`%s']", xid(obj),
		obj->contents.term.name);
	break;
    case OCT_NET:
	(void) sprintf(END(final), "OCT_NET_%d[`%s']", xid(obj),
		(obj->contents.net.name ? obj->contents.net.name
		                        : "Machine generated"));
	break;
    case OCT_INSTANCE:
	(void) sprintf(END(final), "%s", disp_inst(obj));
	break;
    case OCT_POLYGON:
	octGenFirstContainer(obj, OCT_LAYER_MASK, &layerObj);
	(void) sprintf(END(final), "OCT_POLYGON_%d[%s]:%s", xid(obj),
		layerObj.contents.layer.name, disp_points(obj));
	break;
    case OCT_BOX:
	octGenFirstContainer(obj, OCT_LAYER_MASK, &layerObj);
	(void) sprintf(END(final), "OCT_BOX_%d[%s; %ld,%ld %ld,%ld]", xid(obj),
		       layerObj.contents.layer.name,
		       (long)obj->contents.box.lowerLeft.x,
		       (long)obj->contents.box.lowerLeft.y,
		       (long)obj->contents.box.upperRight.x,
		       (long)obj->contents.box.upperRight.y);
	break;
    case OCT_CIRCLE:
	octGenFirstContainer(obj, OCT_LAYER_MASK, &layerObj);
	(void) sprintf(END(final),
		       "OCT_CIRCLE_%d[%s; %ld,%ld R=%ld,%ld A=%ld,%ld]",
		       xid(obj),
		       layerObj.contents.layer.name,
		       (long)obj->contents.circle.center.x,
		       (long)obj->contents.circle.center.y,
		       (long)obj->contents.circle.innerRadius,
		       (long)obj->contents.circle.outerRadius,
		       (long)obj->contents.circle.startingAngle,
		       (long)obj->contents.circle.endingAngle);
	break;
    case OCT_PATH:
	octGenFirstContainer(obj, OCT_LAYER_MASK, &layerObj);
	(void) sprintf(END(final), "OCT_PATH_%d[%s; %ld]:%s", xid(obj),
		       layerObj.contents.layer.name,
		       (long)obj->contents.path.width, disp_points(obj));
	break;
    case OCT_LABEL:
	octGenFirstContainer(obj, OCT_LAYER_MASK, &layerObj);
	(void) sprintf(END(final), "OCT_LABEL_%d[%s; %ld,%ld `%s']", xid(obj),
		       layerObj.contents.layer.name,
		       (long)obj->contents.label.region.lowerLeft.x,
		       (long)obj->contents.label.region.lowerLeft.y,
		       obj->contents.label.label);
	break;
    case OCT_PROP:
	(void) sprintf(END(final), "%s", disp_prop(obj));
	break;
    case OCT_BAG:
	(void) sprintf(END(final), "OCT_BAG_%d[`%s']", xid(obj),
		obj->contents.bag.name);
	break;
    case OCT_LAYER:
	(void) sprintf(END(final), "OCT_LAYER_%d[`%s']", xid(obj),
		obj->contents.layer.name);
	break;
    case OCT_POINT:
	(void) sprintf(END(final), "OCT_POINT_%d[%ld,%ld]", xid(obj),
		       (long)obj->contents.point.x,
		       (long)obj->contents.point.y);
	break;
    case OCT_EDGE:
	(void) sprintf(END(final), "OCT_EDGE_%d[%ld,%ld to %ld,%ld]", xid(obj),
		       obj->contents.edge.start.x,
		       obj->contents.edge.start.y,
		       obj->contents.edge.end.x,
		       obj->contents.edge.end.y);
	break;
    case OCT_CHANGE_LIST:
	(void) sprintf(END(final), "OCT_CHANGE_LIST_%d[%s, %s]",
		       xid(obj),
		       disp_omask(obj->contents.changeList.objectMask),
		       disp_fmask(obj->contents.changeList.functionMask));
	break;
    case OCT_CHANGE_RECORD:
	(void) sprintf(END(final), "%s", disp_crec(obj));
	break;
    default:
	(void) sprintf(END(final), "OCT_UNKNOWN_%d[%d]", xid(obj), obj->type);
	break;
    }
    return final;
}


#ifdef PRT
/* For debugging purposes */

#define OC(call) \
if ((call) < OCT_OK) abort();

int prt_type(ptr, val)
char *ptr;
octId val;
/* Prints out string representation of object type */
{
    char *start = ptr;

    switch (val) {
    case OCT_UNDEFINED_OBJECT:
	ptr += prt_str(ptr, "OCT_UNDEFINED_OBJECT");
	break;
    case OCT_FACET:
	ptr += prt_str(ptr, "OCT_FACET");
	break;
    case OCT_TERM:
	ptr += prt_str(ptr, "OCT_TERM");
	break;
    case OCT_NET:
	ptr += prt_str(ptr, "OCT_NET");
	break;
    case OCT_INSTANCE:
	ptr += prt_str(ptr, "OCT_INSTANCE");
	break;
    case OCT_POLYGON:
	ptr += prt_str(ptr, "OCT_POLYGON");
	break;
    case OCT_BOX:
	ptr += prt_str(ptr, "OCT_BOX");
	break;
    case OCT_CIRCLE:
	ptr += prt_str(ptr, "OCT_CIRCLE");
	break;
    case OCT_PATH:
	ptr += prt_str(ptr, "OCT_PATH");
	break;
    case OCT_LABEL:
	ptr += prt_str(ptr, "OCT_LABEL");
	break;
    case OCT_PROP:
	ptr += prt_str(ptr, "OCT_PROP");
	break;
    case OCT_BAG:
	ptr += prt_str(ptr, "OCT_BAG");
	break;
    case OCT_LAYER:
	ptr += prt_str(ptr, "OCT_LAYER");
	break;
    case OCT_POINT:
	ptr += prt_str(ptr, "OCT_POINT");
	break;
    case OCT_EDGE:
	ptr += prt_str(ptr, "OCT_EDGE");
	break;
    case OCT_FORMAL:
	ptr += prt_str(ptr, "OCT_FORMAL");
	break;
    case OCT_MASTER:
	ptr += prt_str(ptr, "OCT_MASTER");
	break;
    case OCT_CHANGE_LIST:
	ptr += prt_str(ptr, "OCT_CHANGE_LIST");
	break;
    case OCT_CHANGE_RECORD:
	ptr += prt_str(ptr, "OCT_CHANGE_RECORD");
	break;
    default:
	ptr += prt_str(ptr, "UNKNOWN OBJECT");
	break;
    }
    return (int) (ptr - start);
}

int prt_facet(ptr, fct)
char *ptr;
struct octFacet *fct;
{
    char *start = ptr;

    ptr += prt_str(ptr, "[(octFacet) cell = `");
    ptr += prt_str(ptr, fct->cell);
    ptr += prt_str(ptr, "', view = `");
    ptr += prt_str(ptr, fct->view);
    ptr += prt_str(ptr, "', facet = `");
    ptr += prt_str(ptr, fct->facet);
    ptr += prt_str(ptr, "', version = ");
    if (fct->version && (strlen(fct->version) > 0)) {
	ptr += prt_str(ptr, "`");
	ptr += prt_str(ptr, fct->version);
	ptr += prt_str(ptr, "'");
    } else {
	ptr += prt_str(ptr, "<no version>");
    }
    ptr += prt_str(ptr, ", mode = ");
    ptr += prt_str(ptr, fct->mode);
    ptr += prt_str(ptr, "]");
    return (int) (ptr - start);
}

int prt_term(ptr, term)
char *ptr;
struct octTerm *term;
{
    char *start = ptr;

    ptr += prt_str(ptr, "[(octTerm) name = `");
    ptr += prt_str(ptr, term->name);
    ptr += prt_str(ptr, "', instanceId = ");
    ptr += prt_int(ptr, (int) term->instanceId);
    ptr += prt_str(ptr, ", formalExternalId = ");
    ptr += prt_int(ptr, (int) term->formalExternalId);
    ptr += prt_str(ptr, "]");
    return (int) (ptr - start);
}

#define MAX_NAME	50

int prt_name(ptr, nm)
char *ptr;
char *nm;
/*
 * Used to output various objects that have only a name field
 * (which may be garbage).
 */
{
    char nm_buf[MAX_NAME+5];
    char *start = ptr;

    if (nm) {
	strncpy(nm_buf, nm, MAX_NAME);
	nm_buf[MAX_NAME] = '\0';
	if (strlen(nm_buf) >= MAX_NAME) {
	    strcat(nm_buf, " ...");
	}
	ptr += prt_str(ptr, nm_buf);
    } else {
	ptr += prt_str(ptr, "<none>");
    }
    return (int) (ptr - start);
}

int prt_net(ptr, net)
char *ptr;
struct octNet *net;
{
    char *start = ptr;

    ptr += prt_str(ptr, "[(octNet) name = `");
    ptr += prt_name(ptr, net->name);
    ptr += prt_str(ptr, "']");
    return (int) (ptr - start);
}

int prt_point(ptr, point)
char *ptr;
struct octPoint *point;
{
    char *start = ptr;

    ptr += prt_str(ptr, "[(octPoint) x = ");
    ptr += prt_int(ptr, point->x);
    ptr += prt_str(ptr, ", y = ");
    ptr += prt_int(ptr, point->y);
    ptr += prt_str(ptr, "]");
    return (int) (ptr - start);
}

int prt_transtype(ptr, transtype)
char *ptr;
octTransformType transtype;
{
    char *start = ptr;

    switch (transtype) {
    case OCT_NO_TRANSFORM:
	ptr += prt_str(ptr, "OCT_NO_TRANSFORM");
	break;
    case OCT_MIRROR_X:
	ptr += prt_str(ptr, "OCT_MIRROR_X");
	break;
    case OCT_MIRROR_Y:
	ptr += prt_str(ptr, "OCT_MIRROR_Y");
	break;
    case OCT_ROT90:
	ptr += prt_str(ptr, "OCT_ROT90");
	break;
    case OCT_ROT180:
	ptr += prt_str(ptr, "OCT_ROT180");
	break;
    case OCT_ROT270:
	ptr += prt_str(ptr, "OCT_ROT270");
	break;
    case OCT_MX_ROT90:
	ptr += prt_str(ptr, "OCT_MX_ROT90");
	break;
    case OCT_MY_ROT90:
	ptr += prt_str(ptr, "OCT_MY_ROT90");
	break;
    case OCT_FULL_TRANSFORM:
	ptr += prt_str(ptr, "OCT_FULL_TRANSFORM");
	break;
    default:
	ptr += prt_str(ptr, "<unknown>");
	break;
    }
    return (int) (ptr - start);
}

int prt_gtrans(ptr, gtrans)
char *ptr;
double gtrans[2][2];
{
    char *start = ptr;
    int row, col;

    ptr += prt_str(ptr, "{");
    for (row = 0;  row < 2;  row++) {
	ptr += prt_str(ptr, " {");
	for (col = 0;  col < 2; col++) {
	    if (col > 0) ptr += prt_str(ptr, ", ");
	    ptr += prt_dbl(ptr, gtrans[row][col]);
	}
	ptr += prt_str(ptr, "} ");
    }
    ptr += prt_str(ptr, "}");
    return (int) (ptr - start);
}

int prt_transform(ptr, trans)
char *ptr;
struct octTransform *trans;
{
    char *start = ptr;

    ptr += prt_str(ptr, "[(octTransform) translation = ");
    ptr += prt_point(ptr, &(trans->translation));
    ptr += prt_str(ptr, ", transformType = ");
    ptr += prt_transtype(ptr, trans->transformType);
    if (trans->transformType == OCT_FULL_TRANSFORM) {
	ptr += prt_str(ptr, ", generalTransform = ");
	ptr += prt_gtrans(ptr, trans->generalTransform);
	ptr += prt_str(ptr, "]");
    } else {
	ptr += prt_str(ptr, ", generalTransform = <none>]");
    }
    return (int) (ptr - start);
}

int prt_inst(ptr, inst)
char *ptr;
struct octInstance *inst;
{
    char *start = ptr;

    ptr += prt_str(ptr, "[(octInstance) name = `");
    ptr += prt_name(ptr, inst->name);
    ptr += prt_str(ptr, "', master = `");
    ptr += prt_str(ptr, inst->master);
    ptr += prt_str(ptr, "', view = `");
    ptr += prt_str(ptr, inst->view);
    ptr += prt_str(ptr, "', facet = `");
    ptr += prt_str(ptr, inst->facet);
    ptr += prt_str(ptr, "', version = `");
    ptr += prt_str(ptr, inst->version);
    ptr += prt_str(ptr, "', transform = ");
    ptr += prt_transform(ptr, &(inst->transform));
    ptr += prt_str(ptr, "]");
    return (int) (ptr - start);
}

int prt_pnts(ptr, obj)
char *ptr;
octObject *obj;
{
    char *start = ptr;
    struct octPoint *pnts = (struct octPoint *) 0;
    int np = 0;
    int i;

    ptr += prt_str(ptr, "{");
    octGetPoints(obj, &np, pnts);
    if (np > 0) {
	pnts = VEMARRAYALLOC(struct octPoint, np+1);
	if (octGetPoints(obj, &np, pnts) == OCT_OK) {
	    /* Got the points */
	    for (i = 0;  i < np;  i++) {
		ptr += prt_str(ptr, " (");
		ptr += prt_int(ptr, pnts[i].x);
		ptr += prt_str(ptr, ",");
		ptr += prt_int(ptr, pnts[i].y);
		ptr += prt_str(ptr, ")");
	    }
	} else {
	    ptr += prt_str(ptr, " none");
	}
	VEMFREE(pnts);
    } else {
	ptr += prt_str(ptr, " none");
    }
    ptr += prt_str(ptr, "}");
    return (int) (ptr - start);
}

int prt_poly(ptr, poly_obj)
char *ptr;
octObject *poly_obj;
/* Must have the object to get the points */
{
    char *start = ptr;

    ptr += prt_str(ptr, "[(octPolygon) points = ");
    ptr += prt_pnts(ptr, poly_obj);
    ptr += prt_str(ptr, "]");
    return (int) (ptr - start);
}

int prt_box(ptr, box)
char *ptr; 
struct octBox *box;
{
    char *start = ptr;

    ptr += prt_str(ptr, "[(octBox) lowerLeft = ");
    ptr += prt_point(ptr, &(box->lowerLeft));
    ptr += prt_str(ptr, ", upperRight = ");
    ptr += prt_point(ptr, &(box->upperRight));
    ptr += prt_str(ptr, "]");
    return (int) (ptr - start);
}

int prt_circle(ptr, circle)
char *ptr;
struct octCircle *circle;
{
    char *start = ptr;

    ptr += prt_str(ptr, "[(octCircle) startingAngle = ");
    ptr += prt_int(ptr, circle->startingAngle);
    ptr += prt_str(ptr, ", endingAngle = ");
    ptr += prt_int(ptr, circle->endingAngle);
    ptr += prt_str(ptr, ", innerRadius = ");
    ptr += prt_int(ptr, circle->innerRadius);
    ptr += prt_str(ptr, ", outerRadius = ");
    ptr += prt_int(ptr, circle->outerRadius);
    ptr += prt_str(ptr, ", center = ");
    ptr += prt_point(ptr, &(circle->center));
    ptr += prt_str(ptr, "]");
    return (int) (ptr - start);
}

int prt_path(ptr, path)
char *ptr;
octObject *path;
{
    char *start = ptr;

    ptr += prt_str(ptr, "[(octPath) width = ");
    ptr += prt_int(ptr, path->contents.path.width);
    ptr += prt_str(ptr, ", points = ");
    ptr += prt_pnts(ptr, path);
    ptr += prt_str(ptr, "]");
    return (int) (ptr - start);
}

int prt_label(ptr, lbl)
char *ptr;
struct octLabel *lbl;
{
    char *start = ptr;

    ptr += prt_str(ptr, "[(octLabel) label = `");
    ptr += prt_str(ptr, lbl->label);
    ptr += prt_str(ptr, "', region = ");
    ptr += prt_box(ptr, &(lbl->region));
    ptr += prt_str(ptr, ", vertJust = ");
    switch (lbl->vertJust) {
    case OCT_JUST_BOTTOM:
	ptr += prt_str(ptr, "OCT_JUST_BOTTOM");
	break;
    case OCT_JUST_CENTER:
	ptr += prt_str(ptr, "OCT_JUST_CENTER");
	break;
    case OCT_JUST_TOP:
	ptr += prt_str(ptr, "OCT_JUST_TOP");
	break;
    default:
	ptr += prt_str(ptr, "<unknown>");
	break;
    }
    ptr += prt_str(ptr, ", horizJust = ");
    switch (lbl->horizJust) {
    case OCT_JUST_LEFT:
	ptr += prt_str(ptr, "OCT_JUST_LEFT");
	break;
    case OCT_JUST_CENTER:
	ptr += prt_str(ptr, "OCT_JUST_CENTER");
	break;
    case OCT_JUST_RIGHT:
	ptr += prt_str(ptr, "OCT_JUST_RIGHT");
	break;
    default:
	ptr += prt_str(ptr, "<unknown>");
	break;
    }
    ptr += prt_str(ptr, ", lineJust = ");
    switch (lbl->horizJust) {
    case OCT_JUST_LEFT:
	ptr += prt_str(ptr, "OCT_JUST_LEFT");
	break;
    case OCT_JUST_CENTER:
	ptr += prt_str(ptr, "OCT_JUST_CENTER");
	break;
    case OCT_JUST_RIGHT:
	ptr += prt_str(ptr, "OCT_JUST_RIGHT");
	break;
    default:
	ptr += prt_str(ptr, "<unknown>");
	break;
    } 
    ptr += prt_str(ptr, "]");
    return (int) (ptr - start);
}

int prt_stranger(ptr, stranger)
char *ptr;
struct octUserValue *stranger;
{
    char *start = ptr;

    ptr += prt_str(ptr, "[(octUserValue) length = ");
    ptr += prt_int(ptr, stranger->length);
    (void) sprintf(ptr, ", contents = 0x%x]", (int) stranger->contents);
    ptr += strlen(ptr);
    return (int) (ptr - start);
}

int prt_rary(ptr, len, ary)
char *ptr;
int len;
double *ary;
{
    char *start = ptr;
    int i;

    ptr += prt_str(ptr, "[(real_array) length = ");
    ptr += prt_int(ptr, len);
    ptr += prt_str(ptr, ", array = {");
    for (i = 0;  i < len;  i++) {
	sprintf(ptr, " %g", ary[i]);
	ptr += strlen(ptr);
    }
    ptr += prt_str(ptr, " }]");
    return (int) (ptr - start);
}

int prt_iary(ptr, len, ary)
char *ptr;
int len;
int *ary;
{
    char *start = ptr;
    int i;

    ptr += prt_str(ptr, "[(integer_array) length = ");
    ptr += prt_int(ptr, len);
    ptr += prt_str(ptr, ", array = {");
    for (i = 0;  i < len;  i++) {
	sprintf(ptr, " %d", ary[i]);
	ptr += strlen(ptr);
    }
    ptr += prt_str(ptr, " }]");
    return (int) (ptr - start);
}


int prt_prop(ptr, prop)
char *ptr;
struct octProp *prop;
{
    char *start = ptr;

    ptr += prt_str(ptr, "[(octProp) name = `");
    ptr += prt_name(ptr, prop->name);
    ptr += prt_str(ptr, "', type = ");
    switch (prop->type) {
    case OCT_NULL:
	ptr += prt_str(ptr, "OCT_NULL");
	break;
    case OCT_INTEGER:
	ptr += prt_str(ptr, "OCT_INTEGER, value = ");
	ptr += prt_int(ptr, prop->value.integer);
	break;
    case OCT_REAL:
	ptr += prt_str(ptr, "OCT_REAL, value = ");
	ptr += prt_dbl(ptr, prop->value.real);
	break;
    case OCT_STRING:
	ptr += prt_str(ptr, "OCT_STRING, value = `");
	ptr += prt_name(ptr, prop->value.string);
	break;
    case OCT_ID:
	ptr += prt_str(ptr, "OCT_ID, value = ");
	ptr += prt_int(ptr, prop->value.integer);
	break;
    case OCT_STRANGER:
	ptr += prt_str(ptr, "OCT_STRANGER, value = ");
	ptr += prt_stranger(ptr, &(prop->value.stranger));
	break;
    case OCT_REAL_ARRAY:
	ptr += prt_str(ptr, "OCT_REAL_ARRAY, value = ");
	ptr += prt_rary(ptr, prop->value.real_array.length,
			prop->value.real_array.array);
	break;
    case OCT_INTEGER_ARRAY:
	ptr += prt_str(ptr, "OCT_INTEGER_ARRAY, value = ");
	ptr += prt_iary(ptr, prop->value.integer_array.length,
			prop->value.integer_array.array);
	break;
    default:
	ptr += prt_str(ptr, "<unknown>");
	break;
    }
    ptr += prt_str(ptr, "]");
    return (int) (ptr - start);
}

int prt_bag(ptr, bag)
char *ptr;
struct octBag *bag;
{
    char *start = ptr;

    ptr += prt_str(ptr, "[(octBag) name = `");
    ptr += prt_name(ptr, bag->name);
    ptr += prt_str(ptr, "']");
    return (int) (ptr - start);
}

int prt_layer(ptr, layer)
char *ptr;
struct octLayer *layer;
{
    char *start = ptr;

    ptr += prt_str(ptr, "[(octLayer) name = `");
    ptr += prt_name(ptr, layer->name);
    ptr += prt_str(ptr, "']");
    return (int) (ptr - start);
}

int prt_edge(ptr, edge)
char *ptr;
struct octEdge *edge;
{
    char *start = ptr;

    ptr += prt_str(ptr, "[(octEdge) start = ");
    ptr += prt_point(ptr, &(edge->start));
    ptr += prt_str(ptr, ", end = ");
    ptr += prt_point(ptr, &(edge->end));
    ptr += prt_str(ptr, "]");
    return (int) (ptr - start);
}

int prt_cl(ptr, cl)
char *ptr;
struct octChangeList *cl;
{
    char *start = ptr;

    ptr += prt_str(ptr, "[(octChangeList) objectMask = ");
    ptr += prt_str(ptr, disp_omask(cl->objectMask));
    ptr += prt_str(ptr, ", functionMask = ");
    ptr += prt_str(ptr, disp_fmask(cl->functionMask));
    ptr += prt_str(ptr, "]");
    return (int) (ptr - start);
}

int prt_cr(ptr, cr)
char *ptr;
struct octChangeRecord *cr;
{
    char *start = ptr;

    ptr += prt_str(ptr, "[(octChangeRecord) changeType = ");
    switch (cr->changeType) {
    case OCT_CREATE:
	ptr += prt_str(ptr, "OCT_CREATE");
	break;
    case OCT_DELETE:
	ptr += prt_str(ptr, "OCT_DELETE");
	break;
    case OCT_MODIFY:
	ptr += prt_str(ptr, "OCT_MODIFY");
	break;
    case OCT_PUT_POINTS:
	ptr += prt_str(ptr, "OCT_PUT_POINTS");
	break;
    case OCT_ATTACH_CONTENT:
	ptr += prt_str(ptr, "OCT_ATTACH_CONTENT");
	break;
    case OCT_ATTACH_CONTAINER:
	ptr += prt_str(ptr, "OCT_ATTACH_CONTAINER");
	break;
    case OCT_DETACH_CONTENT:
	ptr += prt_str(ptr, "OCT_DETACH_CONTENT");
	break;
    case OCT_DETACH_CONTAINER:
	ptr += prt_str(ptr, "OCT_DETACH_CONTAINER");
	break;
    default:
	ptr += prt_str(ptr, "<unknown>");
	break;
    }
    ptr += prt_str(ptr, ", objectExternalId = ");
    ptr += prt_int(ptr, (int) cr->objectExternalId);
    ptr += prt_str(ptr, ", contentsExternalId = ");
    ptr += prt_int(ptr, (int) cr->contentsExternalId);
    ptr += prt_str(ptr, "]");
    return (int) (ptr - start);
}

int prt_contents(ptr, val)
char *ptr;
octObject *val;
/* Prints out the contents of the object */
{
    char *start = ptr;

    switch (val->type) {
    case OCT_UNDEFINED_OBJECT:
	ptr += prt_str(ptr, "<no contents>");
	break;
    case OCT_FACET:
	ptr += prt_facet(ptr, &(val->contents.facet));
	break;
    case OCT_TERM:
	ptr += prt_term(ptr, &(val->contents.term));
	break;
    case OCT_NET:
	ptr += prt_net(ptr, &(val->contents.net));
	break;
    case OCT_INSTANCE:
	ptr += prt_inst(ptr, &(val->contents.instance));
	break;
    case OCT_POLYGON:
	ptr += prt_poly(ptr, val);
	break;
    case OCT_BOX:
	ptr += prt_box(ptr, &(val->contents.box));
	break;
    case OCT_CIRCLE:
	ptr += prt_circle(ptr, &(val->contents.circle));
	break;
    case OCT_PATH:
	ptr += prt_path(ptr, val);
	break;
    case OCT_LABEL:
	ptr += prt_label(ptr, &(val->contents.label));
	break;
    case OCT_PROP:
	ptr += prt_prop(ptr, &(val->contents.prop));
	break;
    case OCT_BAG:
	ptr += prt_bag(ptr, &(val->contents.bag));
	break;
    case OCT_LAYER:
	ptr += prt_layer(ptr, &(val->contents.layer));
	break;
    case OCT_POINT:
	ptr += prt_point(ptr, &(val->contents.point));
	break;
    case OCT_EDGE:
	ptr += prt_edge(ptr, &(val->contents.edge));
	break;
    case OCT_FORMAL:
	ptr += prt_str(ptr, "<unknown>");
	break;
    case OCT_MASTER:
	ptr += prt_str(ptr, "<unknown>");
	break;
    case OCT_CHANGE_LIST:
	ptr += prt_cl(ptr, &(val->contents.changeList));
	break;
    case OCT_CHANGE_RECORD:
	ptr += prt_cr(ptr, &(val->contents.changeRecord));
	break;
    default:
	break;
    }
    return (int) (ptr - start);
}

int prt_oct(ptr, val)
char *ptr;
char *val;
/* Generates a compatible string output of the oct object */
{
    octObject *obj = (octObject *) val;
    char *start = ptr;

    ptr += prt_str(ptr, "[(octObject) type = ");
    ptr += prt_type(ptr, obj->type);
    ptr += prt_str(ptr, ", objectId = ");
    ptr += prt_int(ptr, (int) obj->objectId);
    ptr += prt_str(ptr, ", contents = ");
    ptr += prt_contents(ptr, obj);
    ptr += prt_str(ptr, "]");
    return (int) (ptr - start);
}

int prt_octid(ptr, id)
char *ptr;
int id;
{
    char *start = ptr;
    octObject obj;

    obj.objectId = id;
    if (octGetById(&obj) == OCT_OK) {
	return prt_oct(ptr, &obj);
    } else {
	ptr += prt_str(ptr, "Cannot get object: ");
	ptr += prt_str(ptr, octErrorString());
	return (int) (ptr - start);
    }
}

int prt_octxid(ptr, xid, id)
char *ptr;
int xid, id;
/*
 * Attempts to get object with external id `xid' in facet
 * containing object `id' and print it.
 */
{
    char *start = ptr;
    octObject obj, fct;

    obj.objectId = id;
    if (octGetById(&obj) == OCT_OK) {
	octGetFacet(&obj, &fct);
	if (octGetByExternalId(&fct, xid, &obj) == OCT_OK) {
	    return prt_oct(ptr, (char *) &obj);
	} else {
	    ptr += prt_str(ptr, "Cannot get external object: ");
	    ptr += prt_str(ptr, octErrorString());
	}
    } else {
	ptr += prt_str(ptr, "Cannot get object: ");
	ptr += prt_str(ptr, octErrorString());
    }
    return (int) (ptr - start);
}

int prt_down(ptr, val)
char *ptr;
char *val;
/* Generates all objects connected to `val' and prints those */
{
    octObject *obj = (octObject *) val;
    octObject sub;
    octGenerator gen;
    char *start = ptr;
    int space = 0;

    OC(octInitGenContents(obj, OCT_ALL_MASK, &gen));
    ptr += prt_str(ptr, "[(Contents) ");
    while (octGenerate(&gen, &sub) == OCT_OK) {
	if (space) ptr += prt_str(ptr, " ");
	else space = 1;
	ptr += prt_oct(ptr, &sub);
    }
    ptr += prt_str(ptr, "]");
    return (int) (ptr - start);
}

int prt_up(ptr, val)
char *ptr;
char *val;
/* Generates all objects connected to `val' and prints those */
{
    octObject *obj = (octObject *) val;
    octObject sub;
    octGenerator gen;
    char *start = ptr;
    int space = 0;

    OC(octInitGenContainers(obj, OCT_ALL_MASK, &gen));
    ptr += prt_str(ptr, "[(Containers) ");
    while (octGenerate(&gen, &sub) == OCT_OK) {
	if (space) ptr += prt_str(ptr, " ");
	else space = 1;
	ptr += prt_oct(ptr, &sub);
    }
    ptr += prt_str(ptr, "]");
    return (int) (ptr - start);
}


#endif
