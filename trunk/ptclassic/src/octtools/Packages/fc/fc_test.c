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
 * File completion testing program.
 */

#include "port.h"
#include "fc.h"

char optProgName[] = "fc_test";

#define Fprintf		(void) fprintf
#define Printf		(void) printf

int
main(argc, argv)
int argc;
char *argv[];
/*
 * Format: fc_test file
 * Prints possible completions to standard output.
 */
{
    char **list;
    int count, start, span;

    if (argc != 2) {
	Fprintf(stderr, "format: %s file\n", argv[0]);
	exit(1);
    }
    list = fc_complete(argv[1], &start, &span);
    if (list) {
	count = 0;
	while (*list) {
	    Printf("%s\n", *list);
	    list++;
	    count++;
	}
	Printf("%d completions (start = %d, span = %d).\n", count, start, span);
    } else {
	Printf("no completions.\n");
    }
    return 0;
}
