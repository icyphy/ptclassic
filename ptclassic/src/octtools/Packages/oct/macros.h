/* Version Identification:
 * $Id$
 */
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
#ifndef OCT_MACROS_H
#define OCT_MACROS_H

#ifndef UTILITY_H
#include "utility.h"
#endif

#define oct_put_props() oct_put_chain(NIL(struct chain))
#define oct_put_contents(chain) oct_put_chain(chain)
#define oct_get_props(ptr) oct_get_chain(ptr,&((ptr)->contents))
#define oct_get_contents(ptr) oct_get_chain(ptr,&((ptr)->contents))

#define oct_detach_content(container, prop)\
    oct_detach_chain(&(container)->contents,container,prop)
#define oct_attach_content(container, prop, only_once)\
    oct_attach_chain(container,&(container)->contents,prop,only_once)

#define ILLEGAL_OFFSET -1

#define VALID_BB(box) ((box)->lowerLeft.x <= (box)->upperRight.x)

#define MAKE_BB_INVALID(box) ((box)->lowerLeft.x = 1, (box)->upperRight.x = 0)

/* compute the byte offset of 'field' in a struct of type 'type' */
#define FIELD_OFFSET(type, field) \
    ( ((char *) &(((type *) 0)->field)) - ((char *) ((type *) 0)))

#define IN_MASK(type, mask) (1<<(type)&(mask))

     /*
      * the delete and attach functions for a circular doubly linked list (DLL)
      * As an end of list marker, head->next->last == nil.
      */

#define DLL_DETACH(head, obj, type, next, last)\
{\
    if ((obj)->next == (obj)) {\
	(head) = NIL(type);\
    } else {\
	if ((obj)->last == NIL(type)) {\
	    (head)->next = (obj)->next;\
	} else {\
	    (obj)->last->next = (obj)->next;\
	}\
	(obj)->next->last = (obj)->last;\
	if ((head) == (obj)) {\
	    (head) = (obj)->last;\
	    (head)->next->last = NIL(type);\
	}\
    }\
}

#define DLL_ATTACH(head, obj, type, next, last)\
{\
    if (head == NIL(type)) {\
	(head) = (obj)->next = (obj); (obj)->last = NIL(type);\
    } else {\
	(obj)->next = (head)->next;\
	(head)->next->last = (obj);\
	(head)->next  = (obj);\
	(obj)->last = NIL(type);\
    }\
}

#define oct_detach_generic_list(head, object)\
    DLL_DETACH(head, (generic *) object, generic, next, last)
	
#define oct_attach_generic_list(head, object)\
    DLL_ATTACH(head, (generic *) object, generic, next, last)

#define NEED_TO_RECORD(obj, function)\
  ((obj)->facet->object_lists[OCT_CHANGE_LIST] != NIL(generic) ? oct_must_record(obj, function) : 0)

#define RECORD_CHANGE(obj, func, cxid)\
    (NEED_TO_RECORD(obj, func) ?\
     oct_record_change(obj, (obj)->flags.type, func,  cxid, NIL(octObject), 0, NIL(struct octPoint), NIL(struct octBox)) :\
     OCT_OK)

#define RECORD_DETACH(container, contents, retval)\
{\
    (retval) = OCT_OK;\
    if (NEED_TO_RECORD(container, OCT_DETACH_CONTAINER)) {\
	(retval) = \
	  oct_record_change(container, container->flags.type, OCT_DETACH_CONTAINER, contents->externalId, NIL(octObject),\
			    0, NIL(struct octPoint), NIL(struct octBox));\
    }\
    if ((retval) >= OCT_OK && NEED_TO_RECORD(contents, OCT_DETACH_CONTENT)) {\
	(retval) =\
	  oct_record_change(container, contents->flags.type, OCT_DETACH_CONTENT, contents->externalId, NIL(octObject),\
			    0, NIL(struct octPoint), NIL(struct octBox));\
    }\
}

#define RECORD_ATTACH(container, contents, retval)\
{\
    (retval) = OCT_OK;\
    if (NEED_TO_RECORD(container, OCT_ATTACH_CONTAINER)) {\
	(retval) = \
	  oct_record_change(container, container->flags.type, OCT_ATTACH_CONTAINER, contents->externalId, NIL(octObject),\
			    0, NIL(struct octPoint), NIL(struct octBox));\
    }\
    if ((retval) >= OCT_OK && NEED_TO_RECORD(contents, OCT_ATTACH_CONTENT)) {\
	(retval) =\
	  oct_record_change(container, contents->flags.type, OCT_ATTACH_CONTENT, contents->externalId, NIL(octObject),\
			    0, NIL(struct octPoint), NIL(struct octBox));\
    }\
}

/* string equality, taking into account nil pointers and defining
 * (char *) 0 == "" */

#define STR_EQ(p1, p2) (p1 ? (p2 ? !strcmp(p1,p2) : *p1 == '\0'):\
			     (p2 ? *p2 == '\0'    : 1))

/*
 * update an old string field with a new one, freeing the old and
 * strsave'ing the new if necessary
 */

#define MODIFY_NAME(old_name, new_name) \
    if (old_name != NIL(char)) {\
	if (new_name != NIL(char)) {\
	    if (strcmp(new_name, old_name) != 0) {\
		oct_str_free(old_name);\
		new_name = oct_str_intern(new_name);\
	    } else {\
		new_name = old_name;\
	    }\
	} else {\
	    oct_str_free(old_name);\
	}\
    } else if (new_name != NIL(char)) {\
	    new_name = oct_str_intern(new_name);\
    }

#endif /* OCT_MACROS_H */

