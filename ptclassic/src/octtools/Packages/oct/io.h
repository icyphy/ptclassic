/* Version Identification:
 * $Id$
 */
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
#ifndef OCT_IO_H
#define OCT_IO_H

/* 
 * Read a disk object id, translate it to an in-memory id.  If the 
 * corresponding object doesnt exist, create it.  return a ptr to
 * the object in either case
 */
#define GET_ID_AND_PTR(id, ptr, type) \
    if (!oct_get_32(&id)) {\
	return 0;\
    }\
    id += oct_id_offset;\
    ptr = oct_id_to_ptr(id);\
    if (ptr == NIL(generic)) {\
	ptr = (struct generic *) malloc((unsigned) oct_object_descs[type].internal_size);\
	ptr->contents = ptr->containers = NIL(struct chain);\
	CLEAR_FLAGS(ptr);\
	if (id != oct_new_id(ptr)) {\
	    oct_error("Panic: Corrupted facet: id's out of sequence");\
	    return 0;\
	}\
    }
/* 
 * lookup the disk id for `ptr', assigning it one if neccessary 
 */
#define PTR_TO_DUMP_ID(ptr, id)\
{\
   if (!oct_lookup_mark((generic *) ptr, &id)) {\
   	id = oct_next_dump_id++;\
	oct_mark_ptr((generic *) ptr, id);\
   }\
}


extern octId oct_id_offset, oct_next_dump_id;

int oct_dump
	ARGS((struct facet *desc, char *file));
int oct_restore
	ARGS((char *file, struct facet *desc));
int oct_restore_info
	ARGS((char *file, struct facet *desc));

#endif /* OCT_IO_H */

