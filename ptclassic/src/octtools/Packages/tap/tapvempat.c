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
#include "utility.h"
#include "errtrap.h"
#include "oct.h"
#include "oh.h"
#include "tap.h"
#include "options.h"

#define TAP_DEBUG

#ifdef TAP_DEBUG	/* for debugging tap itself */
#include "tap_int.h"
#endif

void printLayerLooks();

char *Type;
char *Tech;
int Chrom;

optionStruct optionList[] = {
    { "t",	"display",	"display type"				},
    { "b",	0,		"black and white"			},
    { OPT_RARG,	"technology",	"name of technology"			},
    { OPT_RARG,	"viewtype",	"view type for this technology"		},
    { 0,	0,		0					}
};


void parseOptions(argc, argv)
int argc;
char *argv[];
{
    int option;
    extern char *optarg;

    Chrom = TAP_COLOR;
    while ((option = optGetOption(argc, argv)) != EOF) {
	switch (option) {
	    case 't':
		Type = util_strsav(optarg);
		break;
	    case 'b':
		Chrom = TAP_BW;
		break;
	}
    }

    return;
}

#define STIPPLE_TYPE(flg)	((flg) == TAP_EMPTY ? "empty" : \
				 (flg) == TAP_STIPPLED ? "stippled" : \
				 (flg) == TAP_SOLID ? "solid" : "???")
void
printLayerLooks(facetPtr, type)
octObject *facetPtr;
char *type;
{
    octGenerator layerGen;
    octStatus status;
    octObject layer;
    int prio, nColors, fill, border;
    unsigned short r, g, b;
    int w, h;
    char *bits;

    printf("(technology %s\n", Tech);

    OH_ASSERT(octInitGenContents(facetPtr, OCT_LAYER_MASK, &layerGen));
    while ((status = octGenerate(&layerGen, &layer)) == OCT_OK) {
	printf("  (figureGroupDefault %s\n", layer.contents.layer.name);
	tapSetDisplayType(type, Chrom);
	tapGetDisplayInfo(&layer, &prio, &nColors, &fill, &border);
        printf("      (userData priority %d)\n", prio);
        printf("      (width %ld)\n", (long)tapGetMinWidth(&layer));
        if (Chrom == TAP_COLOR) {
	    if (nColors >= 0) {
	        tapGetDisplayColor(&layer, 0, &r, &g, &b);
	        r = (r * 1000) / 65535;
	        g = (g * 1000) / 65535;
	        b = (b * 1000) / 65535;
                printf("      (color %d %d %d)\n", r, g, b);
            }
        }
	tapGetDisplayPattern(&layer, TAP_FILL_PATTERN, &w, &h, &bits);
	printf("      (fillPattern %d %d \"%s\")\n", w, h, bits);
        if (border != TAP_EMPTY) {
            tapGetDisplayPattern(&layer, TAP_BORDER_PATTERN, &w, &h, &bits);
            printf("      (borderPattern %d \"%s\")\n", w, bits);
        }
        printf("  )\n");
    }
    printf(")\n");
    OH_ASSERT(status);
    return;
}

int
main(argc, argv)
int argc;
char *argv[];
{
    octObject facet;
    extern int optind;

    parseOptions(argc, argv);

    if (argc != optind+2) optUsage();

    octBegin();

    /* get the technology facet directly */
    Tech = argv[optind+1];
    tapOpenTechFacet(argv[optind], argv[optind+1], &facet, "r");

    /* keep the facet open even if tap flushes its caches and closes it */
    facet.contents.facet.mode = "r";
    if (octOpenFacet(&facet) != OCT_ALREADY_OPEN) {
	(void) fprintf(stderr,
		    "can't reopen technology facet %s:%s:%s???\n",
		    facet.contents.facet.cell, facet.contents.facet.view,
		    facet.contents.facet.facet);
	exit(1);
    }

    printLayerLooks(&facet, Type);

    octEnd();

    return 0;
}
