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
/*LINTLIBRARY*/
/*
 * VEM Buffer Management
 * Tracking Changes
 *
 * David Harrison
 * University of California,  Berkeley
 * 1988
 *
 * This file contains routines for tracking changes that occur to 
 * buffers using the Oct change list mechanism.  The call bufChanges()
 * examines all changes to all buffers and calls certain routines
 * in response to these changes.  Right now,  this module simply
 * monitors layer changes.
 */

#include "bufinternal.h"	/* Internal view of buffer module */
#include "message.h"		/* Message handling               */
#include "vemUtil.h"		/* VEM utilities                  */
#include "defaults.h"		/* Default handling               */
#include "windows.h"		/* Window management functions    */

#ifndef OLD_DISPLAY
/*
 * The new change monitoring for buffers performs automatic display updates.
 */

/*
 * Forward declarations
 */

static int layer_updates();
static octId new_layer_changes();
static int inst_updates();
static gu_marker new_inst_changes();
static enum st_retval lyr_update();
static void handle_autosave();
static void new_layer();

typedef struct lyr_args_defn {
    octObject *fct;
    long *change_count;
} lyr_args;

/* This is cheating .. */
extern void vemPrompt();



void _bufExChange(fct, buf)
octObject *fct;			/* Facet associated with buffer   */
intern_buffer *buf;		/* Locally maintained information */
/*
 * This routine examines the changes associated with `buf'.  Certain
 * update routines are called and then the routine resets the change
 * list related information for the buffer.  Steps involved:
 *   1. Examine the list of changes to layers.  If layers have
 *      been created, then a special routine is called to update
 *      the display for all objects currently on that layer.
 *      If a layer has been deleted, its geometric update marker
 *      is removed.  In all cases, the buffer layer table is
 *      generated anew.
 *   2. Changes to instances are generated.  If instances have been
 *      created or deleted, an update to window layer table is called.
 *      In all cases, display updates are done based on the change
 *      to the instance.
 *   3. Changes under each layer in the layer table are generated.
 *      Automatic display updates are done for all significant chagnes.
 *   4. A count of all significant changes is kept.  If this count
 *      is exceeded, an auto save of the buffer is done (if autosave
 *      is turned on).
 * This routine touches the layer_changes, inst_changes, and buf_changes
 * fields of the main structure and touches the lyr_changes field in
 * the lyr_tbl for the buffer.
 */
{
    int layerChanges = 0;
    int instanceChanges = 0;
    lyr_args args;

    /* This is a very expensive routine. */
    /* We need a function to exit quickly from here if there are no changes */

    /* 1. Handle creation and deletion of layers */
    if (!(buf->layer_changes) ||
	(layerChanges = layer_updates(fct, buf->layer_changes, buf))) {
	buf->layer_changes = new_layer_changes(fct);
    }
    buf->change_count += layerChanges;

    /* 2. Handle instance changes */
    if (buf->inst_changes) {
	instanceChanges = inst_updates(fct, buf->inst_changes);
    }
    buf->inst_changes = new_inst_changes(fct); /* This starts a new mark. */

    buf->change_count += instanceChanges;

    /* 3. Handle changes to individual layers */
    if (buf->lyr_tbl) {
	args.fct = fct;
	args.change_count = &(buf->change_count);
	st_foreach(buf->lyr_tbl, lyr_update, (char *) &args);
    }

    /* 4. Autosave */
    handle_autosave(fct, buf);
}


static int layer_updates(fct, layer_change_id, buf)
octObject *fct;			/* Facet for changes    */
octId layer_change_id;		/* Layer change list id */
intern_buffer *buf;		/* Internal buffer structure */
/*
 * This routine handles creation and deletion of layers.  When a
 * new layer is created, the buffer layer table is remade and
 * a special routine is called to update the screen for all items
 * on that layer.  Currently, nothing specific is done on delete.
 * This is because the layer should be reclaimed when the buffer
 * table is remade.  Note this leaves a screen update vulnerability 
 * when a layer is deleted (regions of items attached to the dead
 * layer won't be queued for redisplay).  The change list itself is deleted.
 */
{
    octObject change_list, change;
    octGenerator gen;
    int regenerate_p;
    int cnt = 0;

    change_list.objectId = layer_change_id;
    VEM_OCTCK(octGetById(&change_list));
    VEM_OCTCK(octInitGenContents(&change_list, OCT_CHANGE_RECORD_MASK, &gen));
    regenerate_p = 0;
    while (octGenerate(&gen, &change) == OCT_OK) {
	regenerate_p = 1;
	switch (change.contents.changeRecord.changeType) {
	case OCT_CREATE:
	    new_layer(fct, change.contents.changeRecord.objectExternalId);
	    break;
	case OCT_DELETE:
	    /* Nothing specific -- see above */
	    break;
	}
	cnt++;
    }
    if (regenerate_p) {
	_bufMakeLayers(fct->objectId, buf);
	buf->bufBits &= (~LYR_UPDATE);
	wnLyrChange(fct->objectId);
    }
    if (cnt) {
	VEM_OCTCK(octDelete(&change_list));
    }
    return cnt;
}


static void new_layer(fct, lyr_xid)
octObject *fct;			/* Facet for changes */
octId lyr_xid;			/* Layer external id */
/*
 * This routine handles newly created layers.  Basically, this
 * routine causes updates to the screen for all geometry attached
 * to the layer.  This is done because the changes that would normally
 * cause screen updates haven't been captured since the layer is
 * new.
 */
{
    octObject layer, obj;
    octGenerator gen;
    struct octBox bb;

    if (octGetByExternalId(fct, lyr_xid, &layer) >= OCT_OK) {
	VEM_OCTCK(octInitGenContents(&layer, OCT_GEO_MASK, &gen));
	while (octGenerate(&gen, &obj) == OCT_OK) {
	    if (octBB(&obj, &bb) >= OCT_OK) {
		wnQRedisplay(fct->objectId, &bb);
	    }
	}
    }
}


static octId new_layer_changes(fct)
octObject *fct;
/*
 * This routine creates a new change list attached to `fct' for 
 * detecting the creation and deletion of layers.
 */
{
    octObject new_list;

    new_list.type = OCT_CHANGE_LIST;
    new_list.contents.changeList.objectMask =
      OCT_LAYER_MASK;
    new_list.contents.changeList.functionMask =
      OCT_CREATE_MASK | OCT_DELETE_MASK;
    VEM_OCTCK(vuTemporary(fct, &new_list));
    return new_list.objectId;
}


static int inst_updates(fct, marker)
octObject *fct;			/* Facet for changes */
gu_marker marker;		/* Instance changes  */
/*
 * This routine generates changes to instances.  If instances have
 * been created or deleted, the layer table for all windows looking
 * at this buffer are marked for update.  Display updates are
 * done for all geometric changes to instances.  The change marker
 * is destroyed after use.
 */
{
    gu_gen gen;
    gu_change change;
    octObject inst;
    struct octBox bb, *bb_ptr;
    int cnt = 0;

    gen = guStart(marker);
    while ( (change = guNext(gen)) ) {
	switch (guOp(change)) {
	case GU_ADD:
	case GU_REMOVE:
	    wnLyrChange(fct->objectId);
	    /* Intentional fall-through */
	default:
	    /* Queue up old location */
	    if ( (bb_ptr = guOldSize(change)) ) {
		wnQRedisplay(fct->objectId, bb_ptr);
	    }
	    /* Queue up new location */
	    if ((octGetByExternalId(fct, guXid(change), &inst) == OCT_OK) &&
		(octBB(&inst, &bb) == OCT_OK)) {
		wnQRedisplay(fct->objectId, &bb);
	    }
	    break;
	}
	cnt++;
    }
    guEnd(gen);
    guUnmark(marker);		/* Always remove the mark to avoid long change lists of select/unselect */
    return cnt;
}


static gu_marker new_inst_changes(fct)
octObject *fct;			/* Facet for change */
/*
 * This routine creates a new geometric update marker for tracking
 * changes to instances.
 */
{
    return guMark(fct, OCT_INSTANCE_MASK);
}



/*ARGSUSED*/
static enum st_retval lyr_update(key, value, arg)
char *key, *value, *arg;
/*
 * This routien is passed to st_foreach by bufExChange() to examine
 * changes to each layer in a buffer.  These changes are used to
 * generate automatic display updates.
 */
{
    buf_lyr *lyr = (buf_lyr *) value;
    lyr_args *args = (lyr_args *) arg;
    octObject layer;
    gu_gen gen;
    gu_change change;
    struct octBox bb, *bb_ptr;
    octObject obj;
    int cnt = 0;

    layer.objectId = lyr->lyr_id;
    if (octGetById(&layer)) {
	if (lyr->lyr_changes) {
	    gen = guStart(lyr->lyr_changes);
	    while ( (change = guNext(gen))) {
		/* No case analysis is required -- just the region change */
		if ( (bb_ptr = guOldSize(change)) ) {
		    wnQRedisplay(args->fct->objectId, bb_ptr);
		}
		if ((octGetByExternalId(args->fct, guXid(change), &obj) == OCT_OK) &&
		    (octBB(&obj, &bb) == OCT_OK)) {
		    wnQRedisplay(args->fct->objectId, &bb);
		}
		cnt++;
	    }
	    guEnd(gen);

	    /* Always unmark, even if nothing happened. The change list may have
	       thousands of useless records deriving from select/unselect actions */
	    guUnmark(lyr->lyr_changes);
	    lyr->lyr_changes = (gu_marker) 0; 
	}

	lyr->lyr_changes = guMark(&layer, OCT_GEO_MASK); /* Restart the mark. */
    }
    *(args->change_count) += cnt;
    return ST_CONTINUE;
}



static void handle_autosave(fct, buf)
octObject *fct;			/* Facet for autosave        */
intern_buffer *buf;		/* Internal buffer structure */
/*
 * This routine handles autosaving for a buffer.  If the current
 * count is larger than the autosave buffer and the buffer is
 * writable,  the buffer is autosaved and the count is modified.
 */
{
    int auto_save;

    dfGetInt("autosave", &auto_save);
    if (auto_save && ((buf->bufBits & WRITE_PROTECT) == 0) &&
	(buf->change_count > auto_save)) {
	vemMsg(MSG_C, "\nAutosaving %s:%s:%s; please wait.\n",
	       fct->contents.facet.cell,
	       fct->contents.facet.view,
	       fct->contents.facet.facet);
	if (bufAutoSave(fct->objectId) == VEM_OK) {
	    vemMsg(MSG_C, "Finished\n");
	    vemPrompt();
	}
	while (buf->change_count > auto_save) {
	    buf->change_count -= auto_save;
	}
    }
}



void _bufChInit(buf)
intern_buffer *buf;
/*
 * Initializes change list related information in `buf'.
 */
{
    buf->layer_changes = (octId) 0;
    buf->inst_changes = (gu_marker) 0;
}

void _bufRCFree(buf)
intern_buffer *buf;		/* Internal buffer structure */
/*
 * Frees the change list related information of `buf'.
 */
{
    octObject list;

    if (buf->layer_changes) {
	list.objectId = buf->layer_changes;
	if (octGetById(&list) >= OCT_OK) {
	    VEM_OCTCK(octDelete(&list));
	}
    }
    if (buf->inst_changes) {
	guUnmark(buf->inst_changes);
    }
}

void _bufLCFree(lyr_info)
buf_lyr *lyr_info;		/* Layer by layer information */
/* 
 * This routine frees change list related resources in the layer
 * by layer information kept by the buffer module.
 */
{
    if (lyr_info->lyr_changes) guUnmark(lyr_info->lyr_changes);
}



vemStatus bufChanges( )
/*
 * This routine examines the changes to all buffers since the last
 * time it was called.  Changes are tracked using Oct change lists.
 * Below is a list of changes and their effects:
 *   1.  Any creation or deletion of a layer, or creation or deletion
 *       of an instance causes markLyrUpdate() to be called.
 * Additional changes may be added later.
 */
{
    st_generator *gen;
    octObject fct;
    intern_buffer *buf;

    st_table* table = _activeBufTable;

    if ( table && st_count(table) ) {
	/* fprintf( stderr, "%d/%d buf to check\n", st_count( table ), st_count(_bufTable)); */
	gen = st_init_gen(table);
	while (st_gen(gen, (char **) &(fct.objectId), (char **) &buf)) {
	    if (octGetById(&fct) >= OCT_OK) {
		_bufExChange(&fct, buf); /* Facet exists, check its changes. */
	    }
	}
	st_free_gen(gen);
    }
    
    return VEM_OK;
}

#else



/*
 * On this page begins the suite of routines for handling buffer
 * change lists.
 */

octId _bufNewChanges(fct)
octObject *fct;			/* Facet of buffer */
/*
 * Creates a new appropriate change list for the buffer attached
 * to a temporary bag VEM uses to place all of its extra goodies.
 * This change list now tracks all creations, deletions, and
 * modifications of objects.  Create and delete of layers and
 * instances causes layer table updates,  all others are counted
 * for the auto-save feature.
 */
{
    octObject new_list;

    new_list.type = OCT_CHANGE_LIST;
    /* ---THIS MUST BE IMPROVED WITH DIRECTED ATTACH AND DETACH MASKS--- */
    new_list.contents.changeList.objectMask =
      OCT_ALL_MASK;
    new_list.contents.changeList.functionMask =
      OCT_CREATE_MASK|OCT_DELETE_MASK|OCT_MODIFY_MASK;
    if (vuTemporary(fct, &new_list) < OCT_OK) {
	vemMsg(MSG_A, "Cannot create buffer change list:\n  %s\n",
	       octErrorString());
	return oct_null_id;
    }
    return new_list.objectId;
}


void _bufDelChanges(changeId)
octId changeId;			/* Change list id to remove */
/*
 * Causes the specified change list to be removed.
 */
{
    octObject change_obj;

    change_obj.objectId = changeId;
    if (octGetById(&change_obj) == OCT_OK) {
	octDelete(&change_obj);
    }
}



static int find_type(fct, cr)
octObject *fct;			/* Facet for change */
octObject *cr;			/* Change record    */
/*
 * Attempts to find the type of the primary object of `cr'.
 * This may fail.  In that case,  it returns OCT_UNDEFINED_OBJECT.
 */
{
    octObject obj;

    if (octGetByExternalId(fct, cr->contents.changeRecord.objectExternalId,
			   &obj) == OCT_OK) {
	/* Easy case */
	return obj.type;
    } else if (octGenFirstContent(cr, OCT_ALL_MASK, &obj) == OCT_OK) {
	return obj.type;
    } else {
	return OCT_UNDEFINED_OBJECT;
    }
}



/*ARGSUSED*/
static enum st_retval ex_changes(key, value, arg)
char *key, *value, *arg;
/*
 * This routine is passed to st_foreach by bufChanges() to
 * examine the changes for each buffer.  Right now,  only
 * layer changes have any effect.
 */
{
    intern_buffer *buf = (intern_buffer *) value;
    octObject cl, fct, change;
    octGenerator gen;
    int count;
    int type;
    int auto_save;

    fct.objectId = (octId) key;
    if (octGetById(&fct) != OCT_OK) {
	vemMsg(MSG_C, "Cannot get facet:\n  %s\n", octErrorString());
	return ST_CONTINUE;
    }
    if (buf->buf_changes != oct_null_id) {
	cl.objectId = buf->buf_changes;
	if (octGetById(&cl) < OCT_OK) {
	    vemMsg(MSG_A, "Cannot get change list for buffer:\n  %s\n",
		   octErrorString());
	    return ST_CONTINUE;
	}
	if (octInitGenContents(&cl, OCT_CHANGE_RECORD_MASK, &gen) < OCT_OK) {
	    vemMsg(MSG_A, "Cannot generate changes on buffer:\n  %s\n",
		   octErrorString());
	    return ST_CONTINUE;
	}
	count = 0;
	while (octGenerate(&gen, &change) == OCT_OK) {
	    if ((change.contents.changeRecord.changeType == OCT_CREATE) ||
		(change.contents.changeRecord.changeType == OCT_DELETE)) {
		type = find_type(&fct, &change);
		if ((type == OCT_LAYER) || (type == OCT_INSTANCE)) {
		    _bufMrkLayers(fct.objectId, buf);
		}
	    }
	    count++;
	}
	if (count > 0) {
	    _bufDelChanges(buf->buf_changes);
	    buf->buf_changes = _bufNewChanges(&fct);
	    buf->change_count += count;
	    dfGetInt("autosave", &auto_save);
	    if (auto_save && ((buf->bufBits & WRITE_PROTECT) == 0) &&
		(buf->change_count > auto_save)) {
		vemMsg(MSG_C, "\nAutosaving %s:%s:%s; please wait.\n",
		       fct.contents.facet.cell,
		       fct.contents.facet.view,
		       fct.contents.facet.facet);
		if (bufAutoSave(fct.objectId) == VEM_OK) {
		    vemMsg(MSG_C, "Finished\n");
		    vemPrompt();
		}
		while (buf->change_count > auto_save) {
		    buf->change_count -= auto_save;
		}
	    }
	}
    } else {
	buf->buf_changes = _bufNewChanges(&fct);
    }
    return ST_CONTINUE;
}

vemStatus bufChanges()
/*
 * This routine examines the changes to all buffers since the last
 * time it was called.  Changes are tracked using Oct change lists.
 * Below is a list of changes and their effects:
 *   1.  Any creation or deletion of a layer, or creation or deletion
 *       of an instance causes markLyrUpdate() to be called.
 * Additional changes may be added later.
 */
{
    st_foreach(_bufTable, ex_changes, (char *) 0);
    return VEM_OK;
}

#endif /* OLD_DISPLAY */
