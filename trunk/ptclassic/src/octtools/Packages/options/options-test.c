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
#include "copyright.h"
#include "port.h"
#include "options.h"

optionStruct optionList[] = {
    { "x",	"",		"flag option"			},
    { "y",	"",		"another flag option"		},
    { "bk",	"frammitz_size",
	"frammitz size in 512-byte blocks (-b) or kilobytes (-k)"	},
    { "d",	"debug_type",	"set debug mode: (debug_type: action)"	},
    { OPT_CONT,	0,		"\n    1:  flood the user with junk"	},
    { OPT_CONT,	0,		"\n    2:  flood the world with junk"	},
    { OPT_CONT,	0,		"\n    3:  flood the galaxy with junk"	},
    { OPT_RARG,	"file_name",	"file to be processed"		},
    { OPT_ELLIP, 0,		0				},
    { OPT_OARG,	"host:display",
	"the display to use for the gratuitous menus "		},
    { OPT_CONT,	0,		
	"(defaults to the value of the environment variable $DISPLAY)"	},
    { OPT_DESC,	0,
	"Accomplishes nothing useful at all with each file it processes" },
    { 0,	0,		0				}
};

main(argc, argv)
int argc;
char *argv[];
{
    int option;			/* option letter from optGetOption */
    int xFlag = 0;		/* the x flag */
    int yFlag = 0;		/* the y flag */
    int framSize = 0;		/* the frammitz size */
    int debugType = 0;		/* quantity of debugging info */
    char *display = "";		/* the X display */

    while ((option = optGetOption(argc, argv)) != EOF) {
	switch (option) {
	    case 'x':		/* set x mode */
		xFlag = 1;
		break;
	    case 'y':		/* set y mode */
		yFlag = 1;
		break;
	    case 'b':		/* frammitz size in blocks */
		framSize = atoi(optarg) * 512;
		break;
	    case 'k':		/* frammitz size in K */
		framSize = atoi(optarg) * 1024;
		break;
	    case 'd':		/* set debugging type */
		debugType = atoi(optarg);
		break;
	    default:
		optUsage();
	}
    }

    if (strchr(argv[argc-1], ':')) {
	display = argv[--argc];
    }

    if (octoptind >= argc) {
	(void) fprintf(stderr, "%s: you must specify at least one file\n",
			optProgName);
	optUsage();
    }

    while (octoptind < argc) {
	processFile(argv[octoptind], xFlag, yFlag, framSize,
		    debugType, display); 
	octoptind++;
    }

    exit(0);
}

/*ARGSUSED*/
processFile(fName, xFlag, yFlag, framSize, debugType, display)
char *fName;
int xFlag, yFlag, framSize, debugType;
char *display;
{
}
