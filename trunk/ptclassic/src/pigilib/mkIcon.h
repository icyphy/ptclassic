/* mkIcon.h  edg
Version identification:
$Id$
*/

/*
Copyright (c) 1989 The Regents of the University of California.
			All Rights Reserved.
*/

#define TERM_ARR_MAX 14  /* max # I/O terms = max # positions */

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
    Term in[TERM_ARR_MAX];
    int in_n;
    Term out[TERM_ARR_MAX];
    int out_n;
};
typedef struct TermList_s TermList;

extern boolean MkPalIcon();
extern boolean MkUnivIcon();
extern boolean MkGalIcon();
extern boolean MkStarIcon();
