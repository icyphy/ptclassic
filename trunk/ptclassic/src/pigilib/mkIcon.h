/* mkIcon.h  edg
Version identification:
$Id$
*/

/*
Copyright (c) 1989 The Regents of the University of California.
			All Rights Reserved.
*/

#define TERM_ARR_MAX 14  /* max # I/O terms = max # positions */

struct Term_s {
    char *name;
    boolean multiple;
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
