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
 * Move and Copy
 *
 * David Harrison
 * University of California, Berkeley
 * 1988, 1990
 *
 * This file contains the implementation for symbolic move and copy.
 * New implementation is much better but still requires patching into
 * new area.
 *
 * Work to be done:
 *  - This should go into VEM specific directory
 *  - Should integrate in the drag code
 *  - Should have versions that do destination patching.
 */

#include "general.h"
#include "symbolic.h"
#include "oct.h"
#include "windows.h"
#include "commands.h"
#include "list.h"
#include "message.h"
#include "selset.h"
#include "region.h"
#include "defaults.h"
#include "vemDM.h"
#include "vemUtil.h"
#include "se.h"

#define COPY_RETURN_NAME	";;; Copied Objects ;;;"
#define DEST_OBJS_NAME		";;; Destination Objects ;;;"

/* Stolen from physical */
extern vemStatus MCParse();

typedef enum command_type_defn { MOVE, COPY, DRAG } command_type;

static vemStatus parse_verify();
static vemStatus dest_check();
static void do_reconnects();

#ifdef OLD_DISPLAY
/*ARGSUSED*/
static void move_item(item, after_p, data)
octObject *item;		/* Moved item             */
int after_p;			/* Non-zero if after move */
char *data;			/* User specific data     */
/*
 * Called before and after actual move.  Right now,  this
 * queues up regions for redisplay.  It won't be necessary
 * after the automatic redisplay stuff is operational.
 */
{
    octObject facet;
    struct octBox bb;

    octGetFacet(item, &facet);
    if (octBB(item, &bb) == OCT_OK) {
	wnQRegion(facet.objectId, &bb);
    }
}
#endif

vemStatus seBaseMove(spot, cmdList, cf)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
int cf;				/* Connect flag             */
/*
 * Has the same form as physical move or copy.  Moves objects
 * using new move operator.  If `cf' is non-zero, it will
 * attempt to connect items in the destination region.
 */
{
#ifdef OLD_DISPLAY
    static symMoveItemFunc callback = { move_item, (char *) 0 };
#endif
    octObject facet;
    octObject set;
    octObject reconnect_set;
    struct octTransform *trans;
    vemStatus stat;

    vemExpert( spot );
    seInit();

    stat = parse_verify(spot, cmdList, MOVE, &facet, &set,
			&trans, (vemOneArg **) 0, cf, &reconnect_set);
    if ((stat != VEM_FALSE) && (stat != VEM_ARGRESP)) {
#ifndef OLD_DISPLAY
	symMoveObjects(&set, trans, (symMoveItemFunc *) 0);
#else
	symMoveObjects(&set, trans, &callback);
#endif
	if (stat == VEM_OK) {
	    do_reconnects(&facet, &set, &reconnect_set);
	}
	VEM_OCTCKRVAL(octDelete(&reconnect_set), VEM_CORRUPT);
	trans->translation.x = 0;
	trans->translation.y = 0;
	trans->transformType = OCT_NO_TRANSFORM;
	return VEM_ARGRESP;
    } else {
	if (stat != VEM_ARGRESP) {
	    VEM_OCTCKRVAL(octDelete(&reconnect_set), VEM_CORRUPT);
	}
	return stat;
    }
}

static vemStatus parse_verify(spot, cmdList, type, fct, set, trans, arg, cf, di)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
command_type type;		/* Either MOVE or COPY      */
octObject *fct;			/* Returned facet           */
octObject *set;			/* Set of objects to move   */
struct octTransform **trans;	/* Transform for objects    */
vemOneArg **arg;		/* Actual argument          */
int cf;				/* Connection flag          */
octObject *di;			/* Destination instances    */
/*
 * This routine parses the argument list for move and copy.  It
 * returns the facet, object set, and transformation for the
 * move or copy.
 */
{
    vemOneArg *theArg;
    octId destFctId;
    vemStatus r_code;
    char *command_name = (char *)NULL;

    switch (type) {
    case MOVE:
	command_name = "move";
	break;
    case COPY:
	command_name = "copy";
	break;
    case DRAG:
	command_name = "drag";
	break;
    }

    if ((r_code = MCParse(spot, cmdList, &theArg, 0, &destFctId)) != VEM_OK) {
	return r_code;
    }
    if (destFctId) {
	vemMsg(MSG_C, "Cannot %s between facets yet\n", command_name);
	return VEM_ARGRESP;
    } else {
	destFctId = theArg->argData.vemObjArg.theFct;
    }
    fct->objectId = destFctId;
    VEM_OCTCKRVAL(octGetById(fct), VEM_CORRUPT);
    if (bufWritable(destFctId) != VEM_OK) {
	vemMsg(MSG_C, "Buffer is read only.\n");
	return VEM_ARGRESP;
    }
    set->objectId = theArg->argData.vemObjArg.theBag;
    VEM_OCTCKRVAL(octGetById(set), VEM_CORRUPT);

    *trans = &(theArg->argData.vemObjArg.tran);
    if (arg) *arg = theArg;
    di->type = OCT_BAG;
    di->contents.bag.name = DEST_OBJS_NAME;
    VEM_OCTCKRVAL(vuTemporary(fct, di), VEM_FALSE);
    return dest_check(fct, set, *trans, command_name, type, cf, di);
}

#define LOW(val, try)	if ((try) < (val)) (val) = (try)
#define HI(val, try)	if ((try) > (val)) (val) = (try)

static vemStatus dest_check(facet, set, trans, cn, type, connect, dest_insts)
octObject *facet;		/* Facet containing objects       */
octObject *set;			/* Set of objects to move or copy */
struct octTransform *trans;	/* Transformation for objects     */
char *cn;			/* Command name                   */
int type;			/* What type of command           */
int connect;			/* Whether to connect destination */
octObject *dest_insts;		/* Instances in destination reg.  */
/*
 * This routine checks for objects in the destination region of
 * a move or copy.  The instances found in this area are attached
 * to the bag `dest_objs' for possible reconnection.  If the default 
 * symbolic.reconnect is non-zero,  no further action is taken.
 * Otherwise, a warning is produced and the user is given an option
 * to abort if expert mode is not on. There are three possible return values:
 *   VEM_OK	Objects found but go ahead
 *   VEM_NOMORE No objects found in destination region
 *   VEM_FALSE  Objects found but user decided to abort.
 */
{
    octObject geo, bb;
    octObject otherMovingPathsSet;
    octGenerator gen;
    struct octBox geo_bb;
    regObjGen reg_gen;
    int count = 0;

    otherMovingPathsSet.type = OCT_BAG;
    otherMovingPathsSet.contents.bag.name = ";;;TMP MOVING PATHS SET;;;";
    octCreate( facet, &otherMovingPathsSet );

    /* Determine bounding box of moved or copied objects */
    bb.type = OCT_BOX;
    bb.contents.box.lowerLeft.x = bb.contents.box.lowerLeft.y = VEMMAXINT;
    bb.contents.box.upperRight.x = bb.contents.box.upperRight.y = VEMMININT;
    VEM_OCTCKRVAL(octInitGenContents(set, OCT_PATH_MASK|OCT_INSTANCE_MASK, &gen), VEM_CORRUPT);
    while (octGenerate(&gen, &geo) == OCT_OK) {
	if (octBB(&geo, &geo_bb)) {
	    LOW(bb.contents.box.lowerLeft.x, geo_bb.lowerLeft.x);
	    LOW(bb.contents.box.lowerLeft.y, geo_bb.lowerLeft.y);
	    HI(bb.contents.box.upperRight.x, geo_bb.upperRight.x);
	    HI(bb.contents.box.upperRight.y, geo_bb.upperRight.y);
	}
	/* Also determine what other segments are about to be moved */
	{
	    octGenerator genTerm,genPath;
	    octObject path, aterm;
	    VEM_OCTCKRVAL( octInitGenContents( &geo, OCT_TERM_MASK, &genTerm ), VEM_CORRUPT );
	    while ( octGenerate( &genTerm, &aterm ) == OCT_OK ) {
		VEM_OCTCKRVAL(octInitGenContainers( &aterm, OCT_PATH_MASK, &genPath ), VEM_CORRUPT );
		while ( octGenerate( &genPath, &path ) == OCT_OK ) {
		    octAttachOnce( &otherMovingPathsSet, &path );
		}
	    }
	}
    }
    octTransformGeo(&bb, trans);
    /* Look for objects and put into a temporary bag */
    if (regObjStart(facet, &(bb.contents.box), OCT_PATH_MASK|OCT_INSTANCE_MASK,
		    &reg_gen, 0) == REG_OK) {
	count = 0;
	while (regObjNext(reg_gen, &geo) == REG_OK) {
	    if (octIsAttached(set,&geo)!=OCT_OK && octIsAttached(&otherMovingPathsSet,&geo)!=OCT_OK){
		/* Attach to bag if an instance */
		if (geo.type == OCT_INSTANCE) {
		    VEM_OCTCKRVAL(octAttach(dest_insts, &geo), VEM_FALSE);
		}
		count++;
	    }
	}
	regObjFinish(reg_gen);
	octDelete( &otherMovingPathsSet );
	if (count > 0) {
	    /* Warning */
	    if (!connect) {
		/* Produce warning */
		vemMsg(MSG_C, "WARNING: there are objects in the destination region\n");
		vemMsg(MSG_C, "         for your %s operation  No connections will be\n", cn);
		vemMsg(MSG_C, "         will be made to these objects.\n");
	    }
	}
    }
    if (count && connect) return VEM_OK;
    else return VEM_NOMORE;
}
    


static void do_reconnects(fct, set, di)
octObject *fct;			/* Facet to examine               */
octObject *set;			/* Set of objects moved or copied */
octObject *di;			/* Instances in dest region       */
/*
 * This routine calls symReconnect on instances in the move or copy set
 * and on instances in the destination region of the move and copy.
 * This has the effect of connecting the objects together.  This
 * can be expensive so it is an option.
 */
{
    octGenerator gen;
    octObject inst;
    symDelItemFunc df;

    df.func = seDelFunc;
    df.data = (char *) fct->objectId;
    VEM_OCTCKRET(octInitGenContents(set, OCT_INSTANCE_MASK, &gen));
    while (octGenerate(&gen, &inst) == OCT_OK) {
	symReconnect(&inst, symDefConFunc, &df);
    }
    VEM_OCTCKRET(octInitGenContents(di, OCT_INSTANCE_MASK, &gen));
    while (octGenerate(&gen, &inst) == OCT_OK) {
	symReconnect(&inst, symDefConFunc, &df);
    }
}



typedef struct cp_item_defn {
    octId old_id;
    octId new_id;
} cp_item;

static void copy_item(old, new, data)
octObject *old;			/* Old object */
octObject *new;			/* New copy   */
char *data;			/* User data  */
/*
 * This routine is called when an item is actually copied
 * by symCopyObjects().  This handles updating the selected
 * set information and also redisplay.  Automatic redisplay
 * will make the latter unnecessary.
 */
{
    lsList item_list = (lsList) data;
    cp_item *new_item;

    /* Conditions for adding items */
    if ((old && (old->type == OCT_INSTANCE || old->type == OCT_PATH)) ||
	(new && (new->type == OCT_INSTANCE || new->type == OCT_PATH))) {
	new_item = ALLOC(cp_item, 1);
	if (old) new_item->old_id = old->objectId;
	else new_item->old_id = oct_null_id;
	if (new) new_item->new_id = new->objectId;
	else new_item->new_id = oct_null_id;
	lsNewEnd(item_list, (lsGeneric) new_item, LS_NH);
    }
#ifdef OLD_DISPLAY
    if (old) {
	octGetFacet(old, &fct);
	if (octBB(old, &bb) == OCT_OK) {
	    wnQRegion(fct.objectId, &bb);
	}
    }
    octGetFacet(new, &fct);
    if (octBB(new, &bb) == OCT_OK) {
	wnQRegion(fct.objectId, &bb);
    }	
#endif
}

static void fix_selected_set(arg, item_list)
vemOneArg *arg;
lsList item_list;
/*
 * Transfers copied items to the selected set.
 */
{
    lsGen gen = lsStart(item_list);
    lsGeneric data;
    cp_item *item;
    octObject old, new;

    while (lsNext(gen, &data, LS_NH) == LS_OK) {
	item = (cp_item *) data;
	old.objectId = item->old_id;
	if (!octIdIsNull(item->old_id) && (octGetById(&old) == OCT_OK)) {
	    new.objectId = item->new_id;
	    if (!octIdIsNull(item->new_id) && (octGetById(&new) == OCT_OK)) {
		/* Transfer */
		vemRCSelSet(arg, item->old_id, item->new_id);
	    } else {
		/* Delete */
		vemDelSelSet(arg, item->old_id);
	    }
	} else {
	    /* No old object */
	    new.objectId = item->new_id;
	    if (!octIdIsNull(item->new_id) && (octGetById(&new) == OCT_OK)) {
		/* Brand new item */
		vemAddSelSet(arg, item->new_id);
	    }
	}
    }
}

vemStatus seBaseCopy(spot, cmdList, cf, confun)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
int cf;				/* Connection flag          */
symConFunc *confun;		/* Connector function       */
/*
 * Has the same form as physical move or copy.  Copies objects
 * using new copy operator.  If `cf' is non-zero, it will
 * attempt to connect items in the destination region.
 */
{
    static symCopyItemFunc callback = { copy_item, (char *) 0 };
    octObject facet;
    octObject set;
    octObject copy_set;
    octObject reconnect_set;
    struct octTransform *trans;
    lsList item_list;
    vemStatus stat;
    vemOneArg *arg;

    (void)vemExpert( spot );
    seInit();

    stat = parse_verify(spot, cmdList, COPY, &facet, &set,
			&trans, &arg, cf, &reconnect_set);
    if ((stat != VEM_FALSE) && (stat != VEM_ARGRESP)) {
	copy_set.type = OCT_BAG;  copy_set.contents.bag.name = COPY_RETURN_NAME;
	VEM_OCTCKRVAL(octCreate(&facet, &copy_set), VEM_CORRUPT);
	octMarkTemporary(&copy_set);
	item_list = lsCreate();
	callback.data = (char *) item_list;
	symCopyObjects(&facet, &set, trans, &copy_set, confun, &callback);
	if (stat == VEM_OK) {
	    do_reconnects(&facet, &copy_set, &reconnect_set);
	}
	VEM_OCTCKRVAL(octDelete(&reconnect_set), VEM_CORRUPT);
	fix_selected_set(arg, item_list);
	lsDestroy(item_list, VEMFREENAME);
	trans->translation.x = 0;
	trans->translation.y = 0;
	trans->transformType = OCT_NO_TRANSFORM;
	VEM_OCTCKRVAL(octDelete(&copy_set), VEM_CORRUPT);
	return VEM_ARGRESP;
    } else {
	if (stat != VEM_ARGRESP) {
	    VEM_OCTCKRVAL(octDelete(&reconnect_set), VEM_CORRUPT);
	}
	return stat;
    }
}


vemStatus seMove(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This is a wrapper function around seBaseMove.  It determines
 * reconnect status by examining the symbolic.reconnect option.
 */
{
    int connect;

    dfGetInt("symbolic.reconnect", &connect);
    return seBaseMove(spot, cmdList, connect);
}

vemStatus seCopy(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This is a wrapper function around seBaseCopy.  It determines
 * reconnect status by examining the symbolic.reconnect option.
 */
{
    int connect;

    dfGetInt("symbolic.reconnect", &connect);
    return seBaseCopy(spot, cmdList, connect, symDefConFunc);
}

#ifdef NOTDEF
vemStatus seDrag(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * Has the same form as physical move or copy.  Drags the objects
 * (preserving connectivity) using the drag symbolic policy routine.
 */
{
    octObject facet;
    octObject set;
    octObject reconnect_set;
    struct octTransform *trans;
    vemStatus stat;

    seInit();

    stat = parse_verify(spot, cmdList, DRAG, &facet, &set,
			&trans, (vemOneArg **) 0, 0, &reconnect_set);
    if ((stat != VEM_FALSE) && (stat != VEM_ARGRESP)) {
	symDragObjects(&set, trans, (symMoveItemFunc *) 0);
	trans->translation.x = 0;
	trans->translation.y = 0;
	trans->transformType = OCT_NO_TRANSFORM;
	VEM_OCTCKRVAL(octDelete(&reconnect_set), VEM_CORRUPT);
	return VEM_ARGRESP;
    } else {
	if (stat != VEM_ARGRESP) {
	    VEM_OCTCKRVAL(octDelete(&reconnect_set), VEM_CORRUPT);
	}
	return stat;
    }
}
#endif
