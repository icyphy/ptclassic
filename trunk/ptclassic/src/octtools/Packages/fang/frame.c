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
#include "ansi.h"
#include "fang.h"
#include "fa_internal.h"

/*LINTLIBRARY*/

void fa_grow_xy
	ARGS((fa_geometry *geo, int amount, int direction, fa_geometry *outgeo));

void fa_frame(geo, grow, shrink, outgeo)
fa_geometry *geo;
int grow, shrink;
fa_geometry *outgeo;
{
    fa_geometry ngeo;

    fa_grow(geo, grow, &ngeo);
    fa_grow(&ngeo, -shrink, outgeo);
    fa_free_geo(&ngeo);

    if (fa_debug) {
	(void) fprintf(stderr, "after grow :\n");
	fa_dump_vertices(outgeo->head);
    }
}
void fa_frame_xy(geo, grow, shrink, direction, outgeo)
fa_geometry *geo;
int grow, shrink;
int direction;
fa_geometry *outgeo;
{
    fa_geometry ngeo;

    fa_grow_xy(geo, grow, direction, &ngeo);
    fa_grow_xy(&ngeo, -shrink, direction, outgeo);
    fa_free_geo(&ngeo);

    if (fa_debug) {
	(void) fprintf(stderr, "after grow :\n");
	fa_dump_vertices(outgeo->head);
    }
}
