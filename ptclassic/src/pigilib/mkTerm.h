/* mkTerm.h  edg
Version identification:
$Id$
*/

/*
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY
*/

/* position of terminals around icon box */
enum Position_e {
    l3u = 0, l2u, l1u, l0, l1d, l2d, l3d,
    r3u, r2u, r1u, r0, r1d, r2d, r3d
};
#define POSITION_MAX 14  /* max number of positions */

extern boolean MkTermInit();
extern boolean MkTerm();

#define FLOAT_COLOR "blue"
#define INT_COLOR   "yellow"
#define COMPLEX_COLOR "white"
#define ANYTYPE_COLOR "red"

#define SHAPE_MAX 100  /* max number of points in a Shape */

struct Shape_s {
    int type;  /* OCT_PATH, OCT_POLYGON, OCT_BOX */
    struct octPoint *points;
    int points_n;
};
typedef struct Shape_s Shape;
