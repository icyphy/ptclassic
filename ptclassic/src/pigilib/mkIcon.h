/* mkIcon.h  edg
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

#define MAX_NUM_TERMS 85  /* max total # input and output terms */

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
