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
/*
 *  'Generic' linked-list sorting 'package'
 *
 *  Use:
 *	#define TYPE		the linked-list type (usually a struct)
 *	#define NEXT		'next' field name in the linked-list structure
 *	#define SORT		sorting routine (see below)
 *
 *  Optional:
 *	#define DECL_SORT	'static' or undefined
 *	#define DECL_SORT1	'static' or undefined
 *	#define SORT1		sorting routine (see below)
 *	#define FIELD		select a subfield of the structure for the
 *				   compare function (default is to pass
 *				   a pointer to the structure)
 *	#include "lsort.h"
 *
 *  This defines up to two routines:
 *	SORT1(TYPE *list, int (*compare)(TYPE *x, TYPE *y))
 *	    sort the linked list 'list' according to the compare function
 *	    'compare'
 *
 *	SORT(TYPE *list, int (*compare)(TYPE *x, TYPE *y), int length)
 *	    sort the linked list 'list' according to the compare function
 *	    'compare'.  length is the length of the linked list.
 *
 *  Both routines gracefully handle length == 0 (in which case, list == 0 
 *  is also allowed).
 *
 *  By default, both routines are declared 'static'.  This can be changed
 *  using '#define DECL_SORT' or '#define DECL_SORT1'.
 *
 *  If field is used, then a pointer to the particular field is passed
 *  to the comparison function (rather than a TYPE *).
 */


#ifndef DECL_SORT1
#define DECL_SORT1 static
#endif

#ifndef DECL_SORT
#define DECL_SORT static
#endif

DECL_SORT TYPE *SORT();


#ifdef SORT1

DECL_SORT1 TYPE *SORT1(list_in, compare)
TYPE *list_in;
int (*compare)();
{
    register long cnt;
    register TYPE *p;

    /* Find the length of the list */
    for(p = list_in, cnt = 0; p != 0; p = p->NEXT, cnt++)
	;
    return SORT(list_in, compare, cnt);
}

#endif

DECL_SORT TYPE *SORT(list_in, compare, cnt)
TYPE *list_in;
int (*compare)();
long cnt;
{
    register TYPE *p, **plast, *list1, *list2;
    register long i;

    if (cnt > 1) {
	/* break the list in half */
	for(p = list_in, i = cnt/2-1; i > 0; p = p->NEXT, i--)
	    ;
	list1 = list_in;
	list2 = p->NEXT;
	p->NEXT = 0;

	/* Recursively sort the sub-lists (unless only 1 element) */
	if ((i = cnt/2) > 1) {
	    list1 = SORT(list1, compare, i);
	}
	if ((i = cnt - i) > 1) {
	    list2 = SORT(list2, compare, i);
	}

	/* Merge the two sorted sub-lists */
	plast = &list_in;
	for(;;) {
#ifdef FIELD
	    if ((*compare)(&list1->FIELD, &list2->FIELD) <= 0) {
#else
	    if ((*compare)(list1, list2) <= 0) {
#endif
		*plast = list1;
		plast = &(list1->NEXT);
		if ((list1 = list1->NEXT) == 0) {
		    *plast = list2;
		    break;
		}
	    } else {
		*plast = list2;
		plast = &(list2->NEXT);
		if ((list2 = list2->NEXT) == 0) {
		    *plast = list1;
		    break;
		}
	    }
	}
    }

    return list_in;
}
