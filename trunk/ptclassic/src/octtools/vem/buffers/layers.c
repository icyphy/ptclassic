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
 * Layer Table Maintenence
 *
 * David Harrison
 * University of California,  Berkeley
 * 1988
 *
 * This file contains routines that update a buffers layer table.  The
 * layer table in the buffer is used to quickly map from a tkLayer
 * pointer into a buffer specific layer id.  Also,  input path widths
 * are stored in this table.  Also,  a change in a layer affects
 * the much more extensive layer table kept on a window-by-window
 * basis.  The routines here also help to keep those tables up
 * to date.
 */

#include "bufinternal.h"	/* Internal view of buffer module */
#include "windows.h"		/* Window module definitions      */



void _bufMrkLayers(fctId, buf)
octId fctId;			/* What facet     */
intern_buffer *buf;		/* Buffer to mark */
/*
 * This routine marks the layer table as out of date.  The
 * next time it is referenced,  it will be rebuilt.  It
 * also causes the windows module to mark its layer
 * tables as out of date.
 */
{
    buf->bufBits |= LYR_UPDATE;
    wnLyrChange(fctId);
}



void _bufMakeLayers(fctId, buf)
octId fctId;			/* Facet to examine          */
intern_buffer *buf;		/* Internal buffer structure */
/*
 * This routine returns a newly created hash table of
 * layers indexed by tkLayer handles.  Data in each
 * slot is (buf_lyr *).  The routine also
 * finds the highest priority layer encountered
 * and places it in the hi_lyr field of `buf'.
 */
{
    octObject facet, layer;
    octGenerator gen;
    buf_lyr *new_lyr, *old_lyr;
    tkLayer attr;
    int hi_prio;
    st_table *old_table;

    old_table = buf->lyr_tbl;
    buf->lyr_tbl = st_init_table(st_ptrcmp, st_ptrhash);
    facet.objectId = fctId;
    VEM_OCTCKRET(octGetById(&facet));
    VEM_OCTCKRET(octInitGenContents(&facet, OCT_LAYER_MASK, &gen));
    hi_prio = VEMMININT;
    while (octGenerate(&gen, &layer) == OCT_OK) {
	new_lyr = VEMALLOC(buf_lyr);
	new_lyr->lyr_id = layer.objectId;
	attr = tkLyr(buf->tech, &layer);
	if (old_table &&
	    st_delete(old_table, (char **) &attr, (char **) &old_lyr)) {
	    new_lyr->path_width = old_lyr->path_width;
#ifndef OLD_DISPLAY
	    new_lyr->lyr_changes = old_lyr->lyr_changes;
#endif
	    VEMFREE(old_lyr);
	} else {
	    if (tkMinWidth(buf->tech, layer.contents.layer.name,
			   &(new_lyr->path_width)) != VEM_OK) {
		new_lyr->path_width = 0;
	    }
#ifndef OLD_DISPLAY
	    new_lyr->lyr_changes = (gu_marker) 0;
#endif
	}
	st_insert(buf->lyr_tbl, (char *) attr, (char *) new_lyr);
	if (attr->priority > hi_prio) {
	    buf->hi_lyr = layer.objectId;
	    hi_prio = attr->priority;
	}
    }
    if (old_table) _bufFreeLayers(old_table);
    buf->bufBits &= (~LYR_UPDATE);
}



/*ARGSUSED*/
static enum st_retval free_layer(key, value, arg)
char *key, *value, *arg;
/*
 * Passed to st_foreach to free the layer structures
 * associated with a buffer.
 */
{
#ifndef OLD_DISPLAY
    _bufLCFree((buf_lyr *) value);
#endif
    VEMFREE(value);
    return ST_CONTINUE;
}


void _bufFreeLayers(lyr_tbl)
st_table *lyr_tbl;
/*
 * Frees up all resources consumed by `lyr_tbl'.
 */
{
    st_foreach(lyr_tbl, free_layer, (char *) 0);
    st_free_table(lyr_tbl);
}


vemStatus bufLyrId(fct_id, tkl, lyr_id)
octId fct_id;			/* What facet to examine */
tkLayer tkl;			/* What layer to look up */
octId *lyr_id;			/* Returned layer id     */
/*
 * This routine looks up a layer by technology layer
 * pointer and returns its Oct identifier.  If the buffer
 * or layer can't be found,  the routine returns VEM_CANTFIND.
 */
{
    intern_buffer *buf;
    buf_lyr *lyr;

    if ( (buf = _bufFindBuffer(fct_id)) ) {
	if (buf->bufBits & LYR_UPDATE) _bufMakeLayers(fct_id, buf);
	if (st_lookup(buf->lyr_tbl, (char *) tkl, (char **) &lyr)) {
	    *lyr_id = lyr->lyr_id;
	    return VEM_OK;
	} else {
	    return VEM_CANTFIND;
	}
    } else {
	return VEM_CANTFIND;
    }
}

tkLayer bufHighLayer(fct_id)
octId fct_id;			/* What facet to examine */
/*
 * Returns the layer in the facet specified by `fct_id' with
 * the highest drawing priority.
 */
{
    intern_buffer *buf;
    octObject lyr;

    if ( (buf = _bufFindBuffer(fct_id)) ) {
	if (buf->bufBits & LYR_UPDATE) _bufMakeLayers(fct_id, buf);
	lyr.objectId = buf->hi_lyr;
	if (octGetById(&lyr) == OCT_OK) {
	    return tkLyr(buf->tech, &lyr);
	} else {
	    return (tkLayer) 0;
	}
    } else {
	return (tkLayer) 0;
    }
}


/*
 * Path width management
 */

static buf_lyr *findLayer(fctId, realbuf, lyr)
octId fctId;			/* Facet to examine  */
intern_buffer *realbuf;		/* Associated buffer */
octObject *lyr;			/* Target layer      */
/*
 * Tries to find the layer `lyr' in the list of layers for the buffer.
 * Returns (buf_lyr *) 0 if it can't find it.
 */
{
    buf_lyr *result;
    tkLayer tkl;

    if (!realbuf || !lyr) return (buf_lyr *) 0;
    tkl = tkLyr(realbuf->tech, lyr);
    if (realbuf->bufBits & LYR_UPDATE) _bufMakeLayers(fctId, realbuf);
    if (st_lookup(realbuf->lyr_tbl, (char *) tkl, (char **) &result)) {
	return result;
    } else {
	return (buf_lyr *) 0;
    }
}

octCoord bufPathWidth(fctId, lyr)
octId fctId;			/* Facet of buffer to examine */
octObject *lyr;			/* Layer object              */
/*
 * This routine returns the currently stored input path width of
 * the layer `lyr' in the buffer associated with `fctId'.  If no 
 * width is stored for the layer,   it returns zero.
 */
{
    intern_buffer *realBuf;
    buf_lyr *item;

    if ( (realBuf = _bufFindBuffer(fctId)) ) {
	item = findLayer(fctId, realBuf, lyr);
	if (item) {
	    return item->path_width;
	} else {
	    octCoord ret_width;
	    
	    if (realBuf->tech && (tkMinWidth(realBuf->tech,
					     lyr->contents.layer.name,
					     &ret_width) == VEM_OK)) {
		return ret_width;
	    } else {
		return (octCoord) 0;
	    }
	}
    } else return (octCoord) 0;
}



vemStatus bufSetWidth(fctId, lyr, width)
octId fctId;			/* Facet of associated buffer */
octObject *lyr;			/* Layer of buffer           */
octCoord width;			/* New path width            */
/*
 * This routine sets the current path width of layer `lyr' in the buffer
 * associated with `fctId' to `width'.  If `width' is less than zero,  
 * the width will be set to the minimum design rule width for the 
 * technology of buffer `buf' (zero if no such rule exists).  If no 
 * such layer exists,  the routine will return VEM_CANTFIND.  If the 
 * path width is less than the design rule minimum,  the routine will 
 * set the width but return VEM_TOOSMALL.
 */
{
    intern_buffer *realBuf;
    buf_lyr *item;
    octCoord min_width;

    if ( (realBuf = _bufFindBuffer(fctId)) ) {
	item = findLayer(fctId, realBuf, lyr);
	if (item) {
	    min_width = 0;
	    tkMinWidth(realBuf->tech, lyr->contents.layer.name, &min_width);
	    if (width < 0) {
		item->path_width = min_width;
		return VEM_OK;
	    } else {
		item->path_width = width;
		if (width < min_width) 
		  return VEM_TOOSMALL;
		else
		  return VEM_OK;
	    }
	} else {
	    return VEM_CANTFIND;
	}
    } else {
	return VEM_CORRUPT;
    }
}
