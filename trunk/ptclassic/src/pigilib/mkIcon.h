/* mkIcon.h  edg
Version identification:
$Id$
*/

/*
Copyright (c) 1989 The Regents of the University of California.
			All Rights Reserved.
*/

#define MAX_NUM_TERMS 65  /* max total # input and output terms */

#ifndef __cplusplus
#ifndef const
#define const /* nothing */
#endif
#endif

struct Term_s {
    const char *name;		/* terminal name */
    const char *type;		/* terminal datatype */
    boolean multiple;		/* true if multiporthole */
};
typedef struct Term_s Term;

struct TermList_s {
    Term in[MAX_NUM_TERMS];
    int in_n;
    Term out[MAX_NUM_TERMS];
    int out_n;
};
typedef struct TermList_s TermList;

extern boolean MkPalIcon();
extern boolean MkUnivIcon();
extern boolean MkGalIcon();
extern boolean MkStarIcon();
