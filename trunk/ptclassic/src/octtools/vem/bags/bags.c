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
 * VEM Bag Management
 *
 * David Harrison
 * University of California,  1986, 1989
 *
 * This file contains routines for editing bags.  The commands defined
 * include:
 *   bagSelect:		Selects a bag
 *   bagSelectCnts:	Selects a bags contents
 *   bagNew:		Create a new bag
 *   bagReplace:	Replace a bags contents
 */

#include "general.h"		/* General VEM definitions   */
#include <X11/Xlib.h>		/* C language interface to X */
#include "list.h"		/* List handling package     */
#include "message.h"		/* Message handling system   */
#include "oct.h"		/* OCT library               */
#include "region.h"		/* Region package            */
#include "buffer.h"		/* VEM buffer management     */
#include "commands.h"		/* Definitions for VEM commands */
#include "st.h"			/* Hash table package        */
#include "vemUtil.h"		/* General VEM utilities     */
#include "selection.h"		/* Selection primitives      */

/* Forward Declarations */
static vemStatus bagParse();
   /* Parses argument list of a bagAttach or bagDetach */


#define INSERT_BAG	0x00
#define INSERT_CONTENTS	0x01

static vemStatus doSelection(spot, cmdList, insFlag)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
int insFlag;			/* bag or contents          */
/*
 * This is the command which actually carries out the selection 
 * of a bag.  It is called by bagSelect and bagSelectCnts.  See
 * these functions for the user interface.
 */
{
    vemOneArg *firstArg, *lastArg;
    octObject bagFacet, theBag;
    int len;

    /* First,  all kinds of validity checking */
    len = lsLength(cmdList);
    if (len > 2) {
	vemMessage("format:  [objects] [\"bag name\"] select-bag\n",
		   MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }
    bagFacet.objectId = spot->theFct;
    VEM_OCTCKRVAL(octGetById(&bagFacet), VEM_CORRUPT);
    /* Check the argument list for correctness */
    if (len > 0) {
	lsFirstItem(cmdList, (lsGeneric *) &firstArg, LS_NH);
	lsLastItem(cmdList, (lsGeneric *) &lastArg, LS_NH);
    }
    switch (len) {
    case 0:
	/* Fine */
	break;
    case 1:
	/* Should be either items or text */
	if ((firstArg->argType != VEM_TEXT_ARG) &&
	    (firstArg->argType != VEM_OBJ_ARG))
	  {
	      vemMessage("format: [objects] [\"bag name\"] select-bag\n",
			 MSG_NOLOG|MSG_DISP);
	      return VEM_ARGRESP;
	  }
	break;
    case 2:
	/* Better be objects followed by text */
	if ((firstArg->argType != VEM_OBJ_ARG) ||
	    (lastArg->argType != VEM_OBJ_ARG))
	  {
	      vemMessage("format: [objects] [\"bag name\"] select-bag\n",
			 MSG_NOLOG|MSG_DISP);
	      return VEM_ARGRESP;
	  }
	break;
    default:
	/* Too many arguments */
	vemMessage("format: [objects] [\"bag name\"] select-bag\n",
		   MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }
    if ((len > 0) && (lastArg->argType == VEM_TEXT_ARG)) {
	/* He has given us a bag name */
	theBag.type = OCT_BAG;
	theBag.contents.bag.name = lastArg->argData.vemTextArg;
	if (octGetByName(&bagFacet, &theBag) != OCT_OK) {
	    vemMsg(MSG_A,  "Unable to find a bag named '%s'\n",
		   theBag.contents.net.name);
	    return VEM_ARGRESP;
	}
	lsDelEnd(cmdList, (lsGeneric *) &lastArg);
	vuOffSelect(lastArg);
    } else {
	/* We must find the bag using spot */
	switch (vuFindBag(spot, &theBag)) {
	case VEM_OK:
	    /* Fine - continue */
	    break;
	case VEM_NOSELECT:
	    /* Casual warning */
	    vemMessage("No bag selected\n", MSG_NOLOG|MSG_DISP);
	    return VEM_ARGRESP;
	case VEM_CANTFIND:
	    vemMessage("There is no bag attached to object\n",
		       MSG_NOLOG|MSG_DISP);
	    return VEM_ARGRESP;
	default:
	    /* Hard warning: */
	    vemMsg(MSG_A, "Horrible error:\n  %s\n", octErrorString());
	    return VEM_CORRUPT;
	}
    }

    /* We now have a valid bag in theBag */
    if (selectNetOrBag(spot->theFct, cmdList, &theBag, insFlag) == VEM_OK) {
	return VEM_ARGRESP;
    } else {
	return VEM_CORRUPT;
    }
}



vemStatus bagSelect(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This command adds a bag to the current argument list and highlights
 * it appropriately.  It takes one text argument:  the bag name.  If
 * the name is not present,  it will use vuFindBag to trace the bags
 * attached to the object under spot.  This command selects the bag
 * as a unit.  bagSelectCnts can be used to select the items attached
 * to the bag.
 */
{
    return doSelection(spot, cmdList, INSERT_BAG);
}


vemStatus bagSelectCnts(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This command adds a bag to the current argument list and highlights
 * it appropriately.  It takes one text argument:  the bag name.  If
 * the name is not present,  it will use vuFindBag to trace the bags
 * attached to the object under spot.  This command places all items
 * attached to the bag on the argument list seperately.  bagSelect
 * can be used to attach the bag as a unit.
 */
{
    return doSelection(spot, cmdList, INSERT_CONTENTS);
}



vemStatus bagNew(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This command creates a new bag with nothing in it.  The command
 * takes two arguments:  a VEM_OBJ_ARG describing the parent objects
 * of the bag,  and a bag name.  If the parent object list is absent,
 * the facet is assumed.  No duplicate name checking is done.  Once
 * the bag is created,  objects can be added to it using bagReplace.
 */
{
    vemOneArg *firstArg, *lastArg;
    octObject newBag, parentBag, primary;
    octGenerator theGen;
    int len, parentCount;

    /* Syntax checking */
    len = lsLength(cmdList);
    if ((len < 1) || (len > 2)) {
	vemMessage("format:  [objects] \"bag name\" create-bag\n",
		   MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }
    lsFirstItem(cmdList, (lsGeneric *) &firstArg, LS_NH);
    lsLastItem(cmdList, (lsGeneric *) &lastArg, LS_NH);
    newBag.type = OCT_BAG;
    newBag.objectId = oct_null_id;
    if (len == 1) {
	if (firstArg->argType != VEM_TEXT_ARG) {
	    vemMessage("format:  [objects] \"bag name\" new-bag\n",
		       MSG_NOLOG|MSG_DISP);
	    return VEM_ARGRESP;
	} else {
	    newBag.contents.bag.name = firstArg->argData.vemTextArg;
	    parentCount = 0;
	}
    } else {
	if ((firstArg->argType!=VEM_OBJ_ARG) ||
	    (lastArg->argType!=VEM_TEXT_ARG))
	  {
	      vemMessage("format:  [objects] \"bag name\" new-bag\n",
			 MSG_NOLOG|MSG_DISP);
	      return VEM_ARGRESP;
	  } else {
	      newBag.contents.bag.name = lastArg->argData.vemTextArg;
	      parentCount = firstArg->length;
	  }
    }

    primary.objectId = oct_null_id;
    if (parentCount) {
	/* We have a bag of items to generate through */
	parentBag.objectId = firstArg->argData.vemObjArg.theBag;
	if (octGetById(&parentBag) == OCT_OK) {
	    if (octInitGenContents(&parentBag,OCT_ALL_MASK,&theGen) == OCT_OK) {
		octGenerate(&theGen, &primary);
	    }
	}
    } else {
	/* The primary parent is the facet */
	primary.objectId = spot->theFct;
	VEM_OCTCKRVAL(octGetById(&primary), VEM_CORRUPT);
    }

    /* Ready to create the bag */
    if (primary.objectId != oct_null_id) {
	if (octCreate(&primary, &newBag) != OCT_OK) {
	    vemMsg(MSG_A, "Unable to create bag:\n  %s\n", octErrorString());
	    return VEM_CORRUPT;
	}
    } else {
	vemMsg(MSG_A, "Problems determining parent:\n  %s\n",
	       octErrorString());
	return VEM_CORRUPT;
    }

    /* Now,  attach other parents if any */
    if (parentCount) {
	while (octGenerate(&theGen, &primary) == OCT_OK) {
	    if (octAttach(&primary, &newBag) != OCT_OK) {
		vemMsg(MSG_A, "Error attaching other parents:\n  %s\n",
		       octErrorString());
	    }
	}
    }
    return VEM_OK;
}
	    


vemStatus bagAttach(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* Argument list            */
/*
 * This command attaches the items on the argument list to
 * a specified bag.  It takes two arguments:  a list of objects
 * and an optional bag name.  If the bag name is missing,
 * it will attempt to find the bag by looking at the object
 * under `spot'.
 */
{
    octObject itemBag, destBag, nextObj;
    octGenerator theGen;
    vemStatus rt_code;
    int attached_count, already_count, tot_count, error_count;

    if ((rt_code = bagParse(spot, cmdList, &itemBag, &destBag, "attach-to-bag"))
	!= VEM_OK)
      return rt_code;

    /* Attach items to the bag */
    VEM_OCTCKRVAL(octInitGenContents(&itemBag, OCT_ALL_MASK, &theGen),
		  VEM_CORRUPT);
    attached_count = already_count = tot_count = error_count = 0;
    while (octGenerate(&theGen, &nextObj) == OCT_OK) {
	tot_count++;
	switch (octAttach(&destBag, &nextObj)) {
	case OCT_OK:
	    attached_count++;
	    break;
	case OCT_ALREADY_ATTACHED:
	    already_count++;
	    break;
	default:
	    vemMsg(MSG_A, "Could not attach object:\n  %s\n",
		   octErrorString());
	    error_count++;
	    break;
	}
    }
    if (attached_count < tot_count) {
	if (tot_count > 1) {
	    vemMsg(MSG_C, "Out of %d objects:\n", tot_count);
	} else {
	    vemMsg(MSG_C, "Out of one object:\n");
	}
	if (attached_count == 0) {
	    vemMsg(MSG_C, "  None were successfully attached\n");
	} else if (attached_count > 1) {
	    vemMsg(MSG_C, "  %d objects were successfully attached\n",
		   attached_count);
	} else {
	    vemMsg(MSG_C, "  One object was successfully attached\n");
	}
	if (already_count > 1) {
	    vemMsg(MSG_C, "  %d objects were already attached\n",
		   already_count);
	} else if (already_count == 1) {
	    vemMsg(MSG_C, "  One object was already attached\n");
	}
	if (error_count > 1) {
	    vemMsg(MSG_C, "  %d objects failed to attach\n",
		   error_count);
	} else if (error_count == 1) {
	    vemMsg(MSG_C, "  One object failed to attach\n");
	}
    }
    return VEM_OK;
}



vemStatus bagDetach(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* Argument list            */
/*
 * This command detaches the items on the argument list from
 * a specified bag.  It takes two arguments:  a list of objects
 * and an optional bag name.  If the bag name is missing,
 * it will attempt to find the bag by looking at the object
 * under `spot'.
 */
{
    octObject itemBag, destBag, nextObj;
    octGenerator theGen;
    vemStatus rt_code;
    int detached_count, not_count, tot_count, error_count;

    if ((rt_code = bagParse(spot, cmdList, &itemBag, &destBag, "attach-to-bag"))
	!= VEM_OK)
      return rt_code;

    /* Detach items to the bag */
    VEM_OCTCKRVAL(octInitGenContents(&itemBag, OCT_ALL_MASK, &theGen),
		  VEM_CORRUPT);
    detached_count = not_count = tot_count = error_count = 0;
    while (octGenerate(&theGen, &nextObj) == OCT_OK) {
	tot_count++;
	switch (octDetach(&destBag, &nextObj)) {
	case OCT_OK:
	    detached_count++;
	    break;
	case OCT_NOT_ATTACHED:
	    not_count++;
	    break;
	default:
	    vemMsg(MSG_A, "Could not detach object:\n  %s\n",
		   octErrorString());
	    error_count++;
	    break;
	}
    }
    if (detached_count < tot_count) {
	if (tot_count > 1) {
	    vemMsg(MSG_C, "Out of %d objects:\n", tot_count);
	} else {
	    vemMsg(MSG_C, "Out of one object:\n");
	}
	if (detached_count == 0) {
	    sprintf(errMsgArea, "  None were successfully detached\n");
	} else if (detached_count > 1) {
	    sprintf(errMsgArea, "  %d objects were successfully detached\n",
		    detached_count);
	} else {
	    sprintf(errMsgArea, "  One object was successfully detached\n");
	}
	vemMessage(errMsgArea, MSG_DISP);
	if (not_count > 1) {
	    sprintf(errMsgArea, "  %d objects were not attached to the bag\n",
		    not_count);
	} else if (not_count == 1) {
	    sprintf(errMsgArea, "  One object was not attached to the bag");
	}
	vemMessage(errMsgArea, MSG_DISP);
	if (error_count > 1) {
	    sprintf(errMsgArea, "  %d objects failed to detach\n",
		    error_count);
	} else if (error_count == 1) {
	    sprintf(errMsgArea, "  One object failed to detach\n");
	}
	vemMessage(errMsgArea, MSG_DISP);
    }
    return VEM_OK;
}


static vemStatus bagParse(spot, cmdList, itemBag, destBag, cmd)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* Argument list            */
octObject *itemBag;		/* List of items            */
octObject *destBag;		/* Destination bag          */
char *cmd;			/* Name of command          */
/*
 * This routine parses the argument list of a bagAttach or
 * bagDetach command.  If successful,  it will return
 * a bag containing items to be added to (or subtracted from)
 * the destination bag,  and the destination bag itself.
 */
{
    vemOneArg *firstArg, *lastArg;
    octObject bagFacet;
    int len;

    /* Syntax checking */
    len = lsLength(cmdList);
    if ((len < 1) || (len > 2)) {
	sprintf(errMsgArea, "format:  objects [\"bag name\"] %s\n", cmd);
	vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }
    if (vuCkStyle(spot, cmdList) != VEM_OK) return VEM_ARGRESP;
    lsFirstItem(cmdList, (lsGeneric *) &firstArg, LS_NH);
    lsLastItem(cmdList, (lsGeneric *) &lastArg, LS_NH);
    bagFacet.objectId = spot->theFct;
    if (octGetById(&bagFacet) != OCT_OK) {
	vemMsg(MSG_A, "Cannot get facet:\n  %s\n", octErrorString());
	return VEM_ARGRESP;
    }
    destBag->type = OCT_BAG;
    destBag->objectId = oct_null_id;
    if (len == 1) {
	if (firstArg->argType != VEM_OBJ_ARG) {
	    sprintf(errMsgArea, "format:  objects [\"bag name\"] %s\n", cmd);
	    vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
	    return VEM_ARGRESP;
	} else {
	    destBag->contents.bag.name = (char *) 0;
	}
    } else {
	if ((firstArg->argType!=VEM_OBJ_ARG) ||
	    (lastArg->argType!=VEM_TEXT_ARG))
	  {
	      sprintf(errMsgArea, "format:  objects [\"bag name\"] %s\n", cmd);
	      vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
	      return VEM_ARGRESP;
	  } else {
	      destBag->contents.bag.name = lastArg->argData.vemTextArg;
	  }
    }

    if (destBag->contents.bag.name) {
	/* We have a bag to look up by name */
	if (octGetByName(&bagFacet, destBag) != OCT_OK) {
	    sprintf(errMsgArea, "Cannot find bag %s\n  %s\n",
		    destBag->contents.bag.name, octErrorString());
	    vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
	    return VEM_ARGRESP;
	}
    } else {
	/* We have to find it using spot */
	switch (vuFindBag(spot, destBag)) {
	case VEM_OK:
	    /* Fine - continue */
	    break;
	case VEM_NOSELECT:
	    /* Casual warning */
	    vemMessage("No bag selected\n", MSG_NOLOG|MSG_DISP);
	    return VEM_ARGRESP;
	case VEM_CANTFIND:
	    vemMessage("There is no bag attached to object\n",
		       MSG_NOLOG|MSG_DISP);
	    return VEM_ARGRESP;
	default:
	    /* Hard warning: */
	    sprintf(errMsgArea, "Horrible error:\n  %s\n", octErrorString());
	    vemMessage(errMsgArea, MSG_DISP);
	    return VEM_CORRUPT;
	}
    }

    /* Get the bag for the objects */
    itemBag->objectId = firstArg->argData.vemObjArg.theBag;
    if (octGetById(itemBag) != OCT_OK) {
	sprintf(errMsgArea, "Unable to get bag argument:\n  %s\n",
		octErrorString());
	vemMessage(errMsgArea, MSG_DISP);
	return VEM_CORRUPT;
    }
    return VEM_OK;
}
