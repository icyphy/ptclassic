#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/* Copyright (c) 1990-1993 The Regents of the University of California.
 * All rights reserved.
 * 
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the above
 * copyright notice and the following two paragraphs appear in all copies
 * of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
 * THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
 * CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.
 * 
 */
/*
 * List package testing program
 *
 * David Harrison
 * University of California, Berkeley
 * 1988
 */

#include "copyright.h"
#include "port.h"
#include "list.h"
#include "errtrap.h"

/* for errtrap */
char *optProgName;

#define MAX_LINE	1024

/*ARGSUSED*/
int
main(argc, argv)
int argc;
char *argv[];
/*
 * Reads a list of names (and or) numbers into a list
 * sorts them and uniquifies them.  The resulting
 * list is output in reverse order.  This excercises
 * most of the package.
 */
{
    lsList list;
    lsGen gen;
    char line_buf[MAX_LINE];
    char *copy_line;

    optProgName = argv[0];
    list = lsCreate();
    while (fgets(line_buf, MAX_LINE, stdin) != (char *) 0) {
	copy_line = malloc((unsigned) ((sizeof(char) * strlen(line_buf)) + 1));
	if (copy_line == (char *) 0) {
	    errRaise(argv[0], 0, "out of memory");
	}
	(void) strcpy(copy_line, line_buf);
	lsNewEnd(list, (lsGeneric) copy_line, LS_NH);
    }
    lsSort(list, strcmp);
    lsUniq(list, strcmp, free);
    gen = lsEnd(list);
    while (lsPrev(gen, (lsGeneric *) &copy_line, LS_NH) == LS_OK) {
	(void) fputs(copy_line, stdout);
    }
    return 0;
}
