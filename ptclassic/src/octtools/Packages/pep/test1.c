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
#include "port.h"
#include "copyright.h"
#include "utility.h"
#include "errtrap.h"
#include "oct.h"
#include "region.h"
#include "oh.h"
#include "fang.h"
#include "harpoon.h"
#include "tap.h"
#include "th.h"
#include "pepint.h"


int
main(argc, argv)
int argc;
char **argv;
{

    pepRes *res; 
    int numberOfBranches; 
    octObject facet, net1;
    octGenerator netGen;
    int i;

    errProgramName(argv[0]);

    if((argc < 2) || (argc > 2)) {
	fprintf(stderr, "Usage:  %s  net1\n", argv[0]);
	exit(-1);
    }
    ohOpenFacet(&facet, "prova1", "CDabut.routed", "contents", "r");
    thTechnology(&facet);
    
    if (strcmp (argv[1], "all") == 0) {
	OH_ASSERT(octInitGenContents(&facet, OCT_NET_MASK, &netGen));
	while (octGenerate(&netGen, &net1) == OCT_OK) {
	    printf("NET----%s\n", net1.contents.net.name);
	    res = pepBranchResistance(&net1, &numberOfBranches);
	    for (i = 0; i < numberOfBranches; i++) {
		printf("resistance between point (%s) and point (%s) : ", 
		       res[i].node1, res[i].node2);
		printf("%s Ohm\n", util_pretty_print(res[i].value));
	    }
	}
	octFreeGenerator(&netGen);
    } else {
	OH_ASSERT(ohGetByNetName(&facet, &net1, argv[1]));
	res = pepBranchResistance( &net1, &numberOfBranches);
	for (i = 0; i < numberOfBranches; i++) {
	    printf("resistance between point (%s) and point (%s) : ", 
		   res[i].node1, res[i].node2);
	    printf("%s Ohm\n", util_pretty_print(res[i].value));
	}
    }
    return 0;
}

