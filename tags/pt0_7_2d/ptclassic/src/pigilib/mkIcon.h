#ifndef _MKICON_H
#define _MKICON_H 1

/* mkIcon.h  edg
Version identification:
@(#)mkIcon.h	1.13	4/26/96
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

/* Do the right thing for sol2 boolean defs.  compat.h must be included
 * first so sys/types.h is included correctly.
 */
#include "sol2compat.h"

#define MAX_NUM_TERMS 85  /* max total # input and output terms */

struct Term_s {
    char *name;			/* terminal name, sometimes dynamic memory */
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

#include "oct.h"

extern boolean MkPalIcon ARGS((octObject *facetPtr,
			       octObject *iconFacetPtr));
extern boolean MkUnivIcon ARGS((octObject *facetPtr,
				octObject *iconFacetPtr));
extern boolean MkGalIcon ARGS((octObject *galFacetPtr,
			       octObject *iconFacetPtr));
extern boolean MkStarIcon ARGS((char *name, char *dir, char* domain,
				octObject *iconFacetPtr));
extern boolean MkGetTerms ARGS((const char* name, TermList* terms));

#endif  /* _MKICON_H */
