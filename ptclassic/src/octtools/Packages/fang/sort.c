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
#include "port.h"
#include "utility.h"
#include "fang.h"
#include "fa_internal.h"

/* LINTLIBRARY */

#define TYPE		fa_vertex
#define NEXT		next
#define SORT		fa_list_sort
#include "lsort.h"

#define ARG_MIN(a,b,c) ((a) > (b) ? 1 : ((a) < (b) ? -1 : (c)))

int fa_compar_vertex(a, b)
fa_vertex *a, *b;
{
    return ARG_MIN(a->y, b->y, ARG_MIN(a->x, b->x, 0));
}


int fa_rev_compar_vertex(a, b)
fa_vertex *a, *b;
{
    return ARG_MIN(a->x, b->x, ARG_MIN(a->y, b->y, 0));
}


void fa_sort(geo)
fa_geometry *geo;
{
    register fa_vertex *p;
    
    if (! (geo->status & FA_SORTED) && (geo->count > 0)) {
	geo->head = fa_list_sort(geo->head, fa_compar_vertex, geo->count);
	for(p = geo->head; p != 0; p = p->next) {
	    geo->tail = p;
	}
	geo->status |= FA_SORTED;
    }
}


void fa_rev_sort(geo)
fa_geometry *geo;
{
    register fa_vertex *p;
    
    if (geo->count > 0) {
	geo->head = fa_list_sort(geo->head, fa_rev_compar_vertex, geo->count);
	for(p = geo->head; p != 0; p = p->next) {
	    geo->tail = p;
	}
    }
}
