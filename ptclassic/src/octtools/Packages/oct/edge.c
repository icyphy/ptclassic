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
 * Implementations of the the oct functions for edgees.
 */

#include "copyright.h"
#include "port.h"
#include "internal.h"
#include "geo.h"
#include "io.h"
#include "io_procs.h"

#include "edge.h"

static octStatus edge_read_fields(), edge_write_fields(), edge_bb();

extern struct object_desc oct_geo_desc;
static struct object_desc *super = &oct_geo_desc;

void
oct_edge_desc_set(object_desc)
struct object_desc *object_desc;
{
    *object_desc = *super;
    
    object_desc->internal_size = sizeof(struct edge);
    object_desc->external_size = sizeof(struct octEdge);
    object_desc->flags |= HAS_BB_FUNC;
    object_desc->contain_mask = (octObjectMask) OCT_PROP_MASK|OCT_BAG_MASK;
    object_desc->user_offset = FIELD_OFFSET(struct edge, user_edge);
    object_desc->bb_func = edge_bb;
    object_desc->write_fields_func = edge_write_fields;
    object_desc->read_fields_func = edge_read_fields;
}
  

static octStatus
edge_bb(eptr, bbox)
generic *eptr;
struct octBox *bbox;
{
    struct edge *edge = (struct edge *) eptr;

    if (bbox == NIL(struct octBox)) {
	return OCT_OK;
    }
    
    bbox->lowerLeft.x = MIN(edge->user_edge.start.x,edge->user_edge.end.x);
    bbox->lowerLeft.y = MIN(edge->user_edge.start.y,edge->user_edge.end.y);
    bbox->upperRight.x = MAX(edge->user_edge.start.x,edge->user_edge.end.x);
    bbox->upperRight.y = MAX(edge->user_edge.start.y,edge->user_edge.end.y);
    return OCT_OK;
}

static int
edge_read_fields(eptr)
generic *eptr;
{
    struct edge *edge = (struct edge *) eptr;
    
    return (*super->read_fields_func)(eptr) &&
            oct_get_point(&edge->user_edge.start) &&
            oct_get_point(&edge->user_edge.end);
}

static int
edge_write_fields(eptr)
generic *eptr;
{
    struct edge *edge = (struct edge *) eptr;
    
    return (*super->write_fields_func)(eptr) &&
            oct_put_point(edge->user_edge.start) &&
	    oct_put_point(edge->user_edge.end);
}
