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
    octObject net1, net2;
    octObject term;
    octObject facet;

    errProgramName(argv[0]);
    
    if((argc < 2) || (argc > 3)) {
	fprintf(stderr, "Usage:  %s  net1 [net2] | term\n", argv[0]);
	fprintf(stderr, "With one argument:  capacitance to ground.\n");
	fprintf(stderr, " (if the argument is not a net, it must be a term)\n");
	fprintf(stderr, "With two arguments: coupling capacitance.\n");
	exit(-1);
    }
    ohOpenFacet(&facet, "mytest", "symbolic", "contents", "r");
    thTechnology(&facet);
    
    if(argc == 3) {
	OH_ASSERT(ohGetByNetName(&facet, &net1, argv[1]));
	OH_ASSERT(ohGetByNetName(&facet, &net2, argv[2]));
	printf("Capacitance between %s and %s: %sF\n", argv[1], argv[2],
	       util_pretty_print(pepCrossCapacitance(&net1, &net2)));
    } else {
	if(ohGetByNetName(&facet, &net1, argv[1]) == OCT_OK) {
	    printf("Capacitance of net %s to ground: %sF\n", argv[1],
		   util_pretty_print(pepCapacitance(&net1)));
	} else if(ohGetByTermName(&facet, &term, argv[1]) == OCT_OK) {
	    printf("Capacitance of term %s to ground: %sF\n", argv[1],
		   util_pretty_print(pepTermCap( &term )));
	}
    }
    octCloseFacet(&facet);
    return 0;
}
