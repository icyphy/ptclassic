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
#include "copyright.h"
#include "port.h"
#include "internal.h"

#include "oct_utils.h"

void 
octMarkTemporary(object)
octObject *object;
{
    generic *ptr = oct_id_to_ptr(object->objectId);

    if (ptr == NIL(generic)) {
	oct_error("octMarkTemporary: The argument's octId has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    if (!ptr->flags.is_temporary) {
	ptr->flags.is_temporary = 1;
	ptr->facet->temp_count++;
    }
    return;
}

void 
octUnmarkTemporary(object)
octObject *object;
{
    generic *ptr = oct_id_to_ptr(object->objectId);

    if (ptr == NIL(generic)) {
	oct_error("octUnmarkTemporary: The argument's octId has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    if (ptr->flags.is_temporary) {
	ptr->flags.is_temporary = 0;
	ptr->facet->temp_count--;
    }
    return;
}

int
octIsTemporary(object)
octObject *object;
{
    generic *ptr = oct_id_to_ptr(object->objectId);

    if (ptr == NIL(generic)) {
	oct_error("octIsTemporary: The argument's octId has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }

    return ptr->flags.is_temporary;
}
