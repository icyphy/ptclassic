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
 * Implementations of the the oct functions for labels.
 */

#include "copyright.h"
#include "port.h"
#include "internal.h"
#include "geo.h"
#include "io.h"
#include "io_procs.h"
#include "label.h"
static octStatus label_read_fields(), label_write_fields();
static octStatus label_copy_fields(), label_free_fields();
extern struct object_desc oct_geo_desc;
static struct object_desc *super = &oct_geo_desc;

void
oct_label_desc_set(object_desc)
struct object_desc *object_desc;
{
    *object_desc = *super;
    object_desc->internal_size = sizeof(struct label);
    object_desc->external_size = sizeof(struct octLabel);
    object_desc->flags |= HAS_BB_FIELD|HAS_NAME;
    object_desc->bb_offset = FIELD_OFFSET(struct label, user_label.region);
    object_desc->contain_mask = (octObjectMask) (OCT_ALL_MASK & ~OCT_FACET_MASK);
    object_desc->user_offset = FIELD_OFFSET(struct label, user_label);
    object_desc->name_offset = FIELD_OFFSET(struct octLabel, label);
    object_desc->free_fields_func = label_free_fields;
    object_desc->write_fields_func = label_write_fields;
    object_desc->read_fields_func = label_read_fields;
    object_desc->copy_fields_func = label_copy_fields;
}

/*ARGSUSED*/
static octStatus
label_copy_fields(o_p, n_p, size)
char *o_p, *n_p;
int size;
{
    struct octLabel *new = (struct octLabel *) n_p;
    struct octLabel *old = (struct octLabel *) o_p;

    *new = *old;
    if (new->label != NIL(char)) {
	new->label = oct_str_intern(new->label);
    }
    return OCT_OK;		/* Return OCT_OK because we take the
				   pointer of this func */
}


static int
label_free_fields(ptr)
generic *ptr;
{
    struct label *label = (struct label *) ptr;

    if (label->user_label.label != NIL(char)) {
	oct_str_free(label->user_label.label);
    }
    return OCT_OK;		/* Return OCT_OK because we take the
				   pointer of this func */

}
    
static octStatus
label_read_fields(ptr)
generic *ptr;
{
    int temp, val;
    struct label *label = (struct label *) ptr;

    val = (*super->read_fields_func)(ptr) &&
            oct_get_string(&label->user_label.label) &&
	    oct_get_box(&label->user_label.region) &&
	    oct_get_32(&label->user_label.textHeight) &&
	    oct_get_byte(&temp);
    label->user_label.vertJust = ((temp >> 4) & 0x3);
    label->user_label.horizJust = ((temp >> 2) & 0x3);
    label->user_label.lineJust = (temp & 0x3);
    return val;
}

static octStatus
label_write_fields(ptr)
generic *ptr;
{
    struct label *label = (struct label *) ptr;

    return (*super->write_fields_func)(ptr) &&
            oct_put_string(label->user_label.label) &&
	    oct_put_box(label->user_label.region) &&
	    oct_put_32(label->user_label.textHeight) &&
	    oct_put_byte((int) ((label->user_label.vertJust << 4) +
				(label->user_label.horizJust << 2) +
				label->user_label.lineJust));
}
