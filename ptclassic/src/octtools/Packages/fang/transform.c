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
 * transform.c
 *
 * Copyright (c) 1987 Wayne A. Christopher, U. C. Berkeley CAD Group 
 *	 faustus@cad.berkeley.edu, ucbvax!faustus
 * Permission is granted to modify and re-distribute this code in any manner
 * as long as this notice is preserved.  All standard disclaimers apply.
 *
 * Transform a fang geometry according to an oct transform type.
 *
 * 1987 Brian Lee
 * Modified to transform fang geometry by an oct transform instead of just
 * and oct transform type.  Filled out rotation transformation arrays.
 */

#include "copyright.h"
#include "port.h"
#include "fang.h"
#include "oct.h"
#include "fa_internal.h"	/* Pick up decl for fa_error() */

/*	transformed		original	*/

static fa_vertex_type mx_verts[] = {
	NO_VERTEX,		/* NO_VERTEX */
	UPPER_LEFT_NEG,		/* UPPER_RIGHT_NEG */
	UPPER_RIGHT_POS,	/* UPPER_LEFT_POS */
	LOWER_LEFT_POS,		/* LOWER_RIGHT_POS */
	LOWER_RIGHT_NEG,	/* LOWER_LEFT_NEG */
	UPPER_LEFT_POS,		/* UPPER_RIGHT_POS */
	LOWER_RIGHT_POS,	/* LOWER_LEFT_POS */
	UPPER_RIGHT_NEG,	/* UPPER_LEFT_NEG */
	LOWER_LEFT_NEG,		/* LOWER_RIGHT_NEG */
	RIGHT_DIAGONAL,		/* LEFT_DIAGONAL */
	LEFT_DIAGONAL		/* RIGHT_DIAGONAL */
} ;

static fa_vertex_type my_verts[] = {
	NO_VERTEX,		/* NO_VERTEX */
	LOWER_RIGHT_NEG,	/* UPPER_RIGHT_NEG */
	LOWER_LEFT_POS,		/* UPPER_LEFT_POS */
	UPPER_RIGHT_POS,	/* LOWER_RIGHT_POS */
	UPPER_LEFT_NEG,		/* LOWER_LEFT_NEG */
	LOWER_RIGHT_POS,	/* UPPER_RIGHT_POS */
	UPPER_LEFT_POS,		/* LOWER_LEFT_POS */
	LOWER_LEFT_NEG,		/* UPPER_LEFT_NEG */
	UPPER_RIGHT_NEG,	/* LOWER_RIGHT_NEG */
	RIGHT_DIAGONAL,		/* LEFT_DIAGONAL */
	LEFT_DIAGONAL		/* RIGHT_DIAGONAL */
} ;

static fa_vertex_type r90_verts[] = {
	NO_VERTEX,		/* NO_VERTEX */
	UPPER_LEFT_NEG,		/* UPPER_RIGHT_NEG */
	LOWER_LEFT_POS,		/* UPPER_LEFT_POS */
	UPPER_RIGHT_POS,	/* LOWER_RIGHT_POS */
	LOWER_RIGHT_NEG,	/* LOWER_LEFT_NEG */
	UPPER_LEFT_POS,		/* UPPER_RIGHT_POS */
	LOWER_RIGHT_POS,	/* LOWER_LEFT_POS */
	LOWER_LEFT_NEG,		/* UPPER_LEFT_NEG */
	UPPER_RIGHT_NEG,	/* LOWER_RIGHT_NEG */
	RIGHT_DIAGONAL,		/* LEFT_DIAGONAL */
	LEFT_DIAGONAL		/* RIGHT_DIAGONAL */
} ;

static fa_vertex_type r180_verts[] = {
	NO_VERTEX,		/* NO_VERTEX */
	LOWER_LEFT_NEG,		/* UPPER_RIGHT_NEG */
	LOWER_RIGHT_POS,	/* UPPER_LEFT_POS */
	UPPER_LEFT_POS,		/* LOWER_RIGHT_POS */
	UPPER_RIGHT_NEG,	/* LOWER_LEFT_NEG */
	LOWER_LEFT_POS,		/* UPPER_RIGHT_POS */
	UPPER_RIGHT_POS,	/* LOWER_LEFT_POS */
	LOWER_RIGHT_NEG,	/* UPPER_LEFT_NEG */
	UPPER_LEFT_NEG,		/* LOWER_RIGHT_NEG */
	LEFT_DIAGONAL,		/* LEFT_DIAGONAL */
	RIGHT_DIAGONAL		/* RIGHT_DIAGONAL */
} ;

static fa_vertex_type r270_verts[] = {
	NO_VERTEX,		/* NO_VERTEX */
	LOWER_RIGHT_NEG,	/* UPPER_RIGHT_NEG */
	UPPER_RIGHT_POS,	/* UPPER_LEFT_POS */
	LOWER_LEFT_POS,		/* LOWER_RIGHT_POS */
	UPPER_LEFT_NEG,		/* LOWER_LEFT_NEG */
	LOWER_RIGHT_POS,	/* UPPER_RIGHT_POS */
	UPPER_LEFT_POS,		/* LOWER_LEFT_POS */
	UPPER_RIGHT_NEG,	/* UPPER_LEFT_NEG */
	LOWER_LEFT_NEG,		/* LOWER_RIGHT_NEG */
	RIGHT_DIAGONAL,		/* LEFT_DIAGONAL */
	LEFT_DIAGONAL		/* RIGHT_DIAGONAL */
} ;

static fa_vertex_type mx_r90_verts[] = {
	NO_VERTEX,		/* NO_VERTEX */
	LOWER_LEFT_NEG,		/* UPPER_RIGHT_NEG */
	UPPER_LEFT_POS,		/* UPPER_LEFT_POS */
	LOWER_RIGHT_POS,	/* LOWER_RIGHT_POS */
	UPPER_RIGHT_NEG,	/* LOWER_LEFT_NEG */
	LOWER_LEFT_POS,		/* UPPER_RIGHT_POS */
	UPPER_RIGHT_POS,	/* LOWER_LEFT_POS */
	UPPER_LEFT_NEG,		/* UPPER_LEFT_NEG */
	LOWER_RIGHT_NEG,	/* LOWER_RIGHT_NEG */
	LEFT_DIAGONAL,		/* LEFT_DIAGONAL */
	RIGHT_DIAGONAL		/* RIGHT_DIAGONAL */
} ;

static fa_vertex_type my_r90_verts[] = {
	NO_VERTEX,		/* NO_VERTEX */
	UPPER_RIGHT_NEG,	/* UPPER_RIGHT_NEG */
	LOWER_RIGHT_POS,	/* UPPER_LEFT_POS */
	UPPER_LEFT_POS,		/* LOWER_RIGHT_POS */
	LOWER_LEFT_NEG,		/* LOWER_LEFT_NEG */
	UPPER_RIGHT_POS,	/* UPPER_RIGHT_POS */
	LOWER_LEFT_POS,		/* LOWER_LEFT_POS */
	LOWER_RIGHT_NEG,	/* UPPER_LEFT_NEG */
	UPPER_LEFT_NEG,		/* LOWER_RIGHT_NEG */
	LEFT_DIAGONAL,		/* LEFT_DIAGONAL */
	RIGHT_DIAGONAL		/* RIGHT_DIAGONAL */
} ;

void
fa_oct_transform(geo, transform, outgeo)
fa_geometry *geo;
struct octTransform *transform;
fa_geometry *outgeo;
{
    fa_vertex *ptr;
    int tx, ty;
    octTransformType type;

    fa_init(outgeo);

    fa_merge1(geo);	/* merge first => less vertices to transform */

    tx = transform->translation.x;
    ty = transform->translation.y;
    type = transform->transformType;
    for (ptr = geo->head; ptr; ptr = ptr->next) {
	switch (type) {
	    case OCT_NO_TRANSFORM:
		fa_put_vertex(outgeo, ptr->x + tx, ptr->y + ty, ptr->type);
		break;
	    case OCT_MIRROR_X:
		fa_put_vertex(outgeo, (- ptr->x) + tx, ptr->y + ty,
				mx_verts[(int) ptr->type]);
		break;
	    case OCT_MIRROR_Y:
		fa_put_vertex(outgeo, ptr->x + tx, (- ptr->y) + ty,
				my_verts[(int) ptr->type]);
		break;
	    case OCT_ROT90:
		fa_put_vertex(outgeo, (- ptr->y) + tx, ptr->x + ty,
				r90_verts[(int) ptr->type]);
		break;
	    case OCT_ROT180:
		fa_put_vertex(outgeo, (- ptr->x) + tx, (- ptr->y) + ty,
				r180_verts[(int) ptr->type]);
		break;
	    case OCT_ROT270:
		fa_put_vertex(outgeo, ptr->y + tx, (- ptr->x) + ty,
				r270_verts[(int) ptr->type]);
		break;
	    case OCT_MX_ROT90:
		fa_put_vertex(outgeo, (- ptr->y) + tx, (- ptr->x) + ty,
				mx_r90_verts[(int) ptr->type]);
		break;
	    case OCT_MY_ROT90:
		fa_put_vertex(outgeo, ptr->y + tx, ptr->x + ty,
				my_r90_verts[(int) ptr->type]);
		break;
	    case OCT_FULL_TRANSFORM:
		fa_error("OCT_FULL_TRANSFORM not implemented");
	    default:
		fa_error("fa_transform: bad type %d\n", type);
	}
    }
}
