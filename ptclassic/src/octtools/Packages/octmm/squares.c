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
#include "fang.h"

/*
 *    Works like "squares" command in magic's cifoutput specification
 */

void
squares(geo, border, size, sep, outgeo)
fa_geometry    *geo;        /* Input geo passed in */
int        border;        /* Min. border size if possible */
int        size;        /* Square size */
int        sep;        /* Distance between squares */
fa_geometry    *outgeo;    /* Pointer to output geo created */
{
    fa_box        *boxes, *box;
    int        nx, ny;
    int        x, y;
    int        x_start, nxn;

    fa_init(outgeo);
    fa_to_boxes(geo, &boxes);
    
    for (box = boxes; box; box = box->next) {
        if (size > box->right - box->left)
            continue;
        if (size > box->top - box->bottom)
            continue;

        nx = (box->right - box->left - 2 * border + sep) / (size + sep);
        ny = (box->top - box->bottom - 2 * border + sep) / (size + sep);

        x_start = box->left + (box->right - box->left -
                nx * size - (nx-1) * sep)/2;

        y = box->bottom + (box->top - box->bottom -
                ny * size - (ny-1) * sep)/2;
        
        while (ny-- > 0) {
            x = x_start;
            nxn = nx;

            while (nxn-- > 0) {
                fa_add_box(outgeo, x, y, x + size, y + size);
                x += size + sep;
            }
            y += size + sep;
        }
    }
    fa_free_boxes(boxes);
    return;
}
