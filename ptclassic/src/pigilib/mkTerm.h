#ifndef _MKTERM_H
#define _MKTERM_H 1

/* mkTerm.h  edg
Version identification:
@(#)mkTerm.h	1.8	4/26/96
*/

/*
Copyright (c) 1990-1996 The Regents of the University of California.
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

/* position of terminals around icon box */
enum Position_e {
    l3u = 0, l2u, l1u, l0, l1d, l2d, l3d,
    r3u, r2u, r1u, r0, r1d, r2d, r3d
};
#define POSITION_MAX 14  /* max number of positions */

#define FLOAT_COLOR	"blue"
#define INT_COLOR	"yellow"
#define COMPLEX_COLOR	"white"
#define ANYTYPE_COLOR	"red"

#define SHAPE_MAX 100  /* max number of points in a Shape */

struct Shape_s {
    int type;			/* OCT_PATH, OCT_POLYGON, OCT_BOX */
    struct octPoint *points;
    int points_n;
};
typedef struct Shape_s Shape;

/* Do the right thing for sol2 boolean defs.  compat.h must be included
 * first so sys/types.h is included correctly.
 */
#include "sol2compat.h"

/* Define octObject data structures */
#include "oct.h"

extern boolean MkTermInit ARGS((octObject *CurrentFacetPtr));
extern boolean PutShape ARGS((octObject *containPtr, octObject *objPtr,
			Shape *shapePtr, struct octPoint *translatePtr,
			boolean thick));
extern boolean MkTerm ARGS((char *name, boolean input, const char *type,
			    boolean multiple, int position, int totalNumber));

#endif  /* _MKTERM_H */
