#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
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
/*
 * Simple example: reads integer float pairs into memory and sorts them
 */

#include "port.h"
#include "da.h"

#define EXPECTED_PAIRS	10
typedef struct pair { int i; float f; } Pair;

/* For errtrap */
char *optProgName;

int cmpPair(a, b)
char *a, *b;
/* Compares based on first number */
{
    return ((Pair *) a)->i - ((Pair *) b)->i;
}

/*ARGSUSED */
int
main(argc, argv)
int argc;
char *argv[];
{
    dary items;
    Pair val;
    int j;

    optProgName = argv[0];
    items = daAlloc(Pair, EXPECTED_PAIRS);
    while (scanf("%d %f", &(val.i), &(val.f)) != EOF) {
	*daSetLast(Pair, items) = val;
    }
      
    daSort(items, cmpPair);
    for (j = 0;  j < daLen(items);  j++) {
	(void) printf("%d %.4f\n",
		      daGet(Pair, items, j)->i,
		      daGet(Pair, items, j)->f);
    }
    return 0;
}
