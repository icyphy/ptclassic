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
#include "ref.h"

static int gen_fix();

/* 
 * To avoid dangling pointers in generators, each object keeps a 
 * reference count of generators that current point toward it.  When 
 * an object with a non-zero reference count is deleted or detached, a
 * list of all active generators in the object's facet is searched for generators 
 * that point to that object.  Those generators are moved past the 
 * offending object so no dangling references are left.  The reference 
 * count lets us search that list of active generators only for 
 * objects that are actually being referenced. Since it is
 * unlikely that several generators will be simultaneously pointing to 
 * the same object, the ref_count field is only 2 bits deep.  If the 
 * count overflows that field (i.e. becomes larger than REF_MANY), it 
 * is set permanently to REF_MANY.  In this rare case, there will be a 
 * perhaps un-neccessary search of the active generators when that 
 * object is deleted  or detached
 */

#ifdef notdef  

#define REF_MANY 3

oct_ref_increment(obj)
generic *obj;
{
    int val = obj->flags.ref_count;

    if (++val > REF_MANY) {
	val = REF_MANY;
    }

    obj->flags.ref_count = val;
    return 1;
}

oct_ref_decrement(obj)
generic *obj;
{
    int val = obj->flags.ref_count;

    if (val == REF_MANY) {
	return 1;
    }
    
    if (--val < 0) {
	oct_error("Panic: negative generator reference count");
	/* XXX sometimes it is allowed to go negative! */
	/* errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString()); */
	return 0;
    }
    
    obj->flags.ref_count = val;
    return 1;
}
#endif /* notdef */

/* 
 * Fix all the generators that reference `obj' through `link'. If 
 * `link' is nil, then fix any generator that references obj through 
 * any link (this is the case where obj is being deleted rather than 
 * just detached)
 */

int
oct_fix_generators(link, obj)
struct chain *link;
generic *obj;
{
    struct generator *ptr;

    ptr = obj->facet->generators;

    while (ptr != NIL(struct generator)) {
	if (!gen_fix(ptr, link, obj)) {
	    return 0;
	}
	ptr = ptr->last_gen;
    }
    return 1;
}


#define DO_GEN_FIX(generator) (*generator->fix_func)(generator)

static int gen_fix(generator, link, obj)
struct generator *generator;
struct chain *link;
generic *obj;
{
    if (link == NIL(struct chain)) {
	while (generator->next && (generator->next->object == obj ||
				   generator->next->container == obj)) {
	    DO_GEN_FIX(generator);
	}
    } else while (generator->next == link) {
	DO_GEN_FIX(generator);
    }
    return 1;
}

