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
#ifndef FANG_H
#define FANG_H

extern char faPkgName[];
#define FA_PKG_NAME	faPkgName

#include "ansi.h"

#ifndef OCT_H
#include "oct.h"
#endif

typedef int fa_coord;

typedef struct fa_box fa_box;

struct fa_box {
    fa_coord left, right, bottom, top;
    fa_box *next;
};

typedef struct fa_point fa_point;

struct fa_point {
    fa_coord x,y;
    fa_point *next;
};

typedef struct fa_region fa_region;

struct fa_region {
    int count;
    fa_point *head, *tail;
    fa_region *interior_regions;
    fa_region *next;
};

typedef struct fa_poly fa_poly;

struct fa_poly {
    int count;
    fa_point *list;
    fa_poly *next;
};

	/* The eight different types of simple vertices. The shape
	 * is described by the position of a equivelently shaped
	 * corner in a square, while the sense is POS if vertex is
	 * convex (as in a square) or NEG if concave.
	 */

typedef enum {
    NO_VERTEX = 0,
    UPPER_RIGHT_NEG,
    UPPER_LEFT_POS,
    LOWER_RIGHT_POS,
    LOWER_LEFT_NEG,
    UPPER_RIGHT_POS,
    LOWER_LEFT_POS,
    UPPER_LEFT_NEG,
    LOWER_RIGHT_NEG,
    LEFT_DIAGONAL,	/* UPPER_LEFT_POS and LOWER_RIGHT_POS */
    RIGHT_DIAGONAL	/* UPPER_RIGHT_POS and LOWER_LEFT_POS */
} fa_vertex_type;

typedef struct fa_vertex fa_vertex;

struct fa_vertex {
    fa_coord x,y;
    fa_vertex_type type;
    fa_vertex *next;
};

typedef struct fa_geometry fa_geometry;

struct fa_geometry {
    int count;			/* number of vertices */
    int merge_threshold;	/* number of vertices before remerging */
    int status;			/* indicates whether the geometry is sorted,
							    merged, etc. */
    fa_box bb;  		/* the bounding box */
    fa_vertex *head, *tail;	/* pointers to the vertex list */
};

typedef struct fa_type_struct fa_type_struct;

struct fa_type_struct {
    unsigned int edge : 2;		/* The side of the edge on which
					 * the interior lies.
				         * either FA_LEFT, FA_RIGHT,
					 * FA_UP, or FA_DOWN
					 */
    unsigned int low_corner : 1;	/* FA_CONCAVE or FA_CONVEX */
    unsigned int high_corner : 1;	/* FA_CONCAVE or FA_CONVEX */
    unsigned int silent : 1;		/* Do we emit this edge? */
};

typedef struct fa_edge fa_edge;

/*
 * for vertical edges (FA_LEFT and FA_RIGHT), the coordinates of the end
 * points of the edge are (center, start) and (center, end).  For horizontal
 * edges (FA_UP and FA_DOWN), the coordinates are (start, center) and
 * (end, center)
 */

struct fa_edge {
    fa_coord start, end;
    fa_coord center;
    fa_type_struct type;
    fa_edge *next;
};

#define FA_LEFT 0	/* interior lies to the left (x < center) */
#define FA_RIGHT 1	/* interior lies to the right (x > center) */
#define FA_UP 2		/* interior lies above (y > center) */
#define FA_DOWN 3	/* interior lies below (y < center) */
#define FA_CONCAVE 0
#define FA_CONVEX 1

#define   FA_VERTICAL   1
#define   FA_HORIZONTAL 3

/*	Basic operations on the fang `geo'	*/
EXTERN void fa_init
  ARGS((fa_geometry *geo));
EXTERN void fa_copy
  ARGS((fa_geometry *geo, fa_geometry *outgeo));
EXTERN void fa_add_geo
  ARGS((fa_geometry *geo1, fa_geometry *geo2));
EXTERN void fa_bb
  ARGS((fa_geometry *geo, fa_box *bbox));

EXTERN void fa_dump_geo
  ARGS((fa_geometry *geo));
EXTERN void fa_free_geo
  ARGS((fa_geometry *geo));
EXTERN void fa_put_vertex
  ARGS((fa_geometry *outgeo, fa_coord x, fa_coord y, fa_vertex_type type));

EXTERN int fa_count
  ARGS((fa_geometry *geo));
EXTERN int fa_intersect_p
  ARGS((fa_geometry *geo1, fa_geometry *geo2));

/*	Geometrical operations		*/
EXTERN void fa_and
  ARGS((fa_geometry *geo1, fa_geometry *geo2, fa_geometry *outgeo));
EXTERN void fa_or
  ARGS((fa_geometry *geo1, fa_geometry *geo2, fa_geometry *outgeo));
EXTERN void fa_andnot
  ARGS((fa_geometry *geo1, fa_geometry *geo2, fa_geometry *outgeo));
EXTERN void fa_invert
  ARGS((fa_geometry *geo, fa_box *bbox, fa_geometry *outgeo));

EXTERN void fa_grow
  ARGS((fa_geometry *geo, int amount, fa_geometry *outgeo));
EXTERN void fa_smooth
  ARGS((fa_geometry *geo, int min, fa_geometry *outgeo));
EXTERN void fa_frame
  ARGS((fa_geometry *geo, int grow, int shrink, fa_geometry *outgeo));
EXTERN void fa_scale
  ARGS((fa_geometry *geo, double scale_factor, fa_geometry *outgeo));
EXTERN void fa_scale1
  ARGS((fa_geometry *geo, double scale_factor));
EXTERN void fa_fill
  ARGS((fa_geometry *geo, fa_geometry *outgeo));

/*	Functions using the fa_box structure	*/
EXTERN void fa_to_boxes
  ARGS((fa_geometry *geo, fa_box **outbox));
EXTERN void fa_add_box
  ARGS((fa_geometry *geo, fa_coord left, fa_coord bottom, fa_coord right, fa_coord top));

EXTERN void fa_print_boxes
  ARGS((fa_box *boxes, int scale, FILE *stream));
EXTERN void fa_dump_boxes
  ARGS((fa_box *boxes));
EXTERN void fa_free_boxes
  ARGS((fa_box *box));

/*	Functions using the `fa_region' structure	*/
EXTERN void fa_to_regions
  ARGS((fa_geometry *geo, fa_region **outregion));
EXTERN void fa_add_regions
  ARGS((fa_geometry *geo, fa_region *region));
EXTERN void fa_regions_to_boxes
  ARGS((fa_region *region, fa_box **outbox));

EXTERN void fa_fill_regions1
  ARGS((fa_region *region));

EXTERN void fa_print_region
  ARGS((fa_region *region, FILE *stream));
EXTERN void fa_print_regions
  ARGS((fa_region *ptr, FILE *stream));

EXTERN void fa_dump_region
  ARGS((fa_region *region));
EXTERN void fa_dump_regions
  ARGS((fa_region *list));
EXTERN void fa_free_regions
  ARGS((fa_region *region));

/*	Functions using the `fa_edge' structure		*/
EXTERN void fa_to_edges
  ARGS((fa_geometry *geo, fa_edge **horiz_edges, fa_edge **vert_edges));
EXTERN void fa_dump_edge
  ARGS((fa_edge *edge));
EXTERN void fa_dump_edges
  ARGS((fa_edge *list));
EXTERN void fa_free_edges
  ARGS((fa_edge *list));

/*	Functions not to be used by casual users	*/
EXTERN void fa_sort
  ARGS((fa_geometry *geo));
EXTERN void fa_set_auto_merge
  ARGS((int threshold, double factor));
EXTERN void fa_merge
  ARGS((fa_geometry *geo, fa_geometry *outgeo));
EXTERN void fa_merge1
  ARGS((fa_geometry *geo));

/* Brian Lee's functions */
EXTERN int fa_equal
	ARGS((fa_geometry *geo1, fa_geometry *geo2));
EXTERN void fa_oct_transform
	ARGS((fa_geometry *geo, octTransform *transform, fa_geometry *outgeo));
EXTERN int fa_print_vertices
	ARGS((fa_vertex *vertices, FILE *stream));
EXTERN int fa_print_geo
	ARGS((fa_geometry *geo, FILE *stream));

extern char fa_err_string[];	/* String containing error messages
				 * after a fang routine dies
				 */
extern int fa_debug;		/* a global debugging flag */

#endif /*FANG_H*/

