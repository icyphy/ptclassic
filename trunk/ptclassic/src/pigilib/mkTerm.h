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
