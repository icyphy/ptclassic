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
 * Implementations of the the oct functions for boxes.
 */

#include "copyright.h"
#include "port.h"
#include "internal.h"
#include "geo.h"
#include "io.h"
#include "io_procs.h"

#include "box.h"

static int box_write_fields(), box_read_fields(), box_fix_fields();

extern struct object_desc oct_geo_desc;
static struct object_desc *super = &oct_geo_desc;

void
oct_box_desc_set(object_desc)
struct object_desc *object_desc;
{
    *object_desc = *super;
    object_desc->internal_size = sizeof(struct box);
    object_desc->external_size = sizeof(struct octBox);
    object_desc->flags |= HAS_BB_FIELD;
    object_desc->contain_mask = (octObjectMask) OCT_PROP_MASK|OCT_BAG_MASK;
    object_desc->user_offset = FIELD_OFFSET(struct box, user_box);
    object_desc->bb_offset = FIELD_OFFSET(struct box, user_box);
    object_desc->write_fields_func = box_write_fields;
    object_desc->read_fields_func = box_read_fields;
    object_desc->fix_fields_func = box_fix_fields;
}
  
static octStatus
box_fix_fields(ptr)
generic *ptr;
{
    struct octBox *box = &((struct box *) ptr)->user_box;
    octCoord tmp;

    if (box->lowerLeft.x > box->upperRight.x) {
	tmp = box->upperRight.x;
	box->upperRight.x = box->lowerLeft.x;
	box->lowerLeft.x = tmp;
    }
    if (box->lowerLeft.y > box->upperRight.y) {
	tmp = box->upperRight.y;
	box->upperRight.y = box->lowerLeft.y;
	box->lowerLeft.y = tmp;
    }
    
    return 1;
}

static octStatus 
box_read_fields(ptr)
generic *ptr;
{
    struct box *box = (struct box *) ptr;
    return (*super->read_fields_func)(ptr) && oct_get_box(&box->user_box);
}

static octStatus
box_write_fields(ptr)
generic *ptr;
{
    struct box *box = (struct box *) ptr;
    return (*super->write_fields_func)(ptr) && oct_put_box(box->user_box);
}
