/* mkTerm.h  edg
Version identification:
$Id$
*/

/*
Copyright (c) 1989 The Regents of the University of California.
			All Rights Reserved.
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
