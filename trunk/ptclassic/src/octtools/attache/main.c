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
#include "copyright.h"
#include "port.h"
#include "utility.h"
#include "options.h"
#include "oct.h"
#include "oh.h"
#include "st.h"
#include "errtrap.h"
#include "io.h"
#include "internal.h"
#include "edit.h"
#include "attache.h"

/* This flag is necessary to allow inspectFacet to be callable from 
 * the attache executable or from another program that attache has been
 * linked into.  */
extern attacheCalledFromMainP;

optionStruct optionList[] = {
    { OPT_RARG,	"cell:view[:facet]",
	    "the oct facet to be edited (defaults to contents facet)"	},
    { 0,	0,	0						}
};

int
main(argc, argv)
int argc;
char *argv[];
{
    int option;
    octObject facet;
    struct state outerState;
    SIGNAL_FN die();


    while ((option = optGetOption(argc, argv)) != EOF) {
	switch (option) {
	}
    }

    if (optind != argc-1) optUsage();

    ohUnpackDefaults(&facet, "r", "::contents");
    if (ohUnpackFacetName(&facet, argv[optind]) != OCT_OK) {
	optUsage();
    }

    nameTable = st_init_table(strcmp, st_strhash);
    outerState.genMask = OCT_ALL_MASK;
    currentState = &outerState;
    attacheCalledFromMainP = 1;

    (void) signal(SIGINT, die);

    inspectFacet(&facet);

    octEnd();

    return RET_OK;
}




SIGNAL_FN die()
{
    IOend();
    (void) printf("Interrupt -- really kill program? ");
    if (getchar() == 'y') exit(RET_FAIL);

    while(getchar() != '\n');
    IOinit();
    errRaise(optProgName, 0, "return from interrupt");
}




