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
 * equal.c
 *
 * Copyright -C- 1988	Brian D. N. Lee
 * University of California, Berkeley
 *
 * Comapare two fang geometries for equality.
 */

#include "copyright.h"
#include "port.h"
#include "fang.h"

int
fa_equal(geo1, geo2)
fa_geometry *geo1, *geo2;
{
    /* Returns
     *      1  if the two geometries have identical points
     *      0  otherwise
     */
    fa_vertex *vtx1, *vtx2;

    fa_merge1(geo1);
    fa_merge1(geo2);

    if ( fa_count(geo1) != fa_count(geo2) ) return 0;

    for ( vtx1 = geo1->head, vtx2 = geo2->head; vtx1 != 0;
	  vtx1 = vtx1->next, vtx2 = vtx2->next ) {
	if ( vtx1->x != vtx2->x || vtx1->y != vtx2->y ) return 0;
    }
    return 1;
}
