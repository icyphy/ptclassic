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
#include "port.h"
#include "utility.h"
#include "fang.h"
#include "fa_internal.h"

/*LINTLIBRARY*/

void fa_andnot(geo1, geo2, outgeo)
fa_geometry *geo1, *geo2;
fa_geometry *outgeo;
{
    fa_geometry ngeo;
    fa_box bb;

    if (geo2->count == 0 || geo1->count == 0) {
	fa_copy(geo1, outgeo);
	return;
    }
	
    bb.left = MIN(geo1->bb.left, geo2->bb.left);
    bb.right = MAX(geo1->bb.right, geo2->bb.right);
    bb.bottom = MIN(geo1->bb.bottom, geo2->bb.bottom);
    bb.top = MAX(geo1->bb.top, geo2->bb.top);

    fa_invert(geo2, &bb, &ngeo);

    if (fa_debug) {
	(void) fprintf(stderr, "fa_andnot : after invert :\n");
	fa_dump_vertices(ngeo.head);
    }

    fa_init(outgeo);

    fa_and(&ngeo, geo1, outgeo);
    
    fa_free_geo(&ngeo);

    if (fa_debug) {
	(void) fprintf(stderr, "after and :\n");
	fa_dump_vertices(outgeo->head);
    }
}
