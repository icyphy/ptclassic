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
#include "oct.h"
#include "oh.h"
#include "tap.h"
#include "options.h"
#include "vulcan.h"
#include "vov.h"

optionStruct optionList[] = {
    { "T",	"tech_dir",	"override default technology root directory" },
    { "r",	0,		"update all cells in hierarchy"		},
    { "k",	0,		"destroy old interface facets when updating" },
    { "C",	0,		"cut back frames around terminals (experimental)" },
    { "G",      0,              "do not generate global frame (on layer PLACE)" },
    { "f",	"feature_size",	"minimum feature size (% of cell size)"	},
    { "d",	"debug_type",	"produce tons of debugging information"	},
    { "o",	"framed_facet",	"output facet name (default interface)"	},
    { OPT_RARG,	"cell:view",	"the cell to be framed"			},
    { OPT_ELLIP,0,		0					},
    { 0,	0,		0					}
};

static int flagWord = VULCAN_DFLT_FLAGS | VULCAN_CLOSE_INPUT;
static double minFeature = VULCAN_DFLT_MIN_FEATURE;
static vulcanDebugType debugType = VULCAN_NO_DEBUG;
static char *outputFacetName = "interface";

static void handleOptions
	ARGS((int argc, char *argv[]));
static void messageHandler
	ARGS((vulcanMessageType type, char *message));


int
main(argc, argv)
int argc;
char *argv[];
{
    extern int optind;		/* index of current argument */
    octObject inputFacet;	/* facet to be framed */
    octObject outputFacet;	/* framed facet */
    octStatus status;
    int retCode = 0;
    void messageHandler();

    handleOptions(argc, argv);
    if (optind >= argc) optUsage();

    VOVbegin( argc, argv );

    octBegin();
    vulcanMessageHandler(messageHandler);

    ohUnpackDefaults(&inputFacet, "r", "::contents");

    while (optind < argc) {
	if (ohUnpackFacetName(&inputFacet, argv[optind++]) != OCT_OK) {
	    optUsage();
	}

	inputFacet.contents.facet.mode = "r";

	VOVinputFacet( &inputFacet );

	status = octOpenFacet(&inputFacet) ;
	if ( status < OCT_OK) {
	    (void) fprintf(stderr, "%s: can't open cell ``", optProgName);
	    (void) fprintf(stderr, "%s:%s:%s:%s",
			    inputFacet.contents.facet.cell,
			    inputFacet.contents.facet.view,
			    inputFacet.contents.facet.facet,
			    inputFacet.contents.facet.version);
	    (void) fprintf(stderr, "'' for reading:  %s\n", octErrorString());
	    retCode = 1;
	    continue;
	} else if ( status == OCT_INCONSISTENT ) {
	    (void)fprintf( stderr, "Inconsistent facet" );
	    VOVend( 1 );
	}

	outputFacet = inputFacet;
	outputFacet.contents.facet.facet = outputFacetName;
	outputFacet.contents.facet.mode = (flagWord & VULCAN_DESTROY_OLD) ? "w" : "a";

	VOVoutputFacet( &outputFacet );

	if (octOpenFacet(&outputFacet) < OCT_OK) {
	    (void) fprintf(stderr, "%s: can't open cell ``", optProgName);
	    (void) fprintf(stderr, "%s:%s:%s:%s",
			    outputFacet.contents.facet.cell,
			    outputFacet.contents.facet.view,
			    outputFacet.contents.facet.facet,
			    outputFacet.contents.facet.version);
	    (void) fprintf(stderr, "'' for writing:  %s\n", octErrorString());
	    retCode = 1;
	    continue;
	}

	if (vulcanParameterized(&inputFacet, &outputFacet, flagWord,
			    minFeature, debugType) == VULCAN_SEVERE) {
	    retCode = 1;
	}

	if (octCloseFacet(&outputFacet) != OCT_OK) {
	    (void) fprintf(stderr, "%s: oct error closing framed facet: %s\n",
				optProgName, octErrorString());
	    retCode = 1;
	}
    }

    octEnd();

    VOVend(retCode);
    return retCode;
}

static void handleOptions(argc, argv)
int argc;
char *argv[];
{
    int option;			/* option letter being processed */
    extern char *optarg;	/* argument string for options with arguments */
    double atof();

    flagWord |= VULCAN_GLOBAL_FRAME; 			/* Set this by default. */
    while ((option = optGetOption(argc, argv)) != EOF) {
	switch (option) {
	    case 'T':		/* set technology root directory */
		(void) tapRootDirectory(optarg);
		break;
	    case 'G':
		flagWord &= ~VULCAN_GLOBAL_FRAME;
		break;
	    case 'r':		/* force recursive recreation */
		flagWord |= VULCAN_DO_SUBCELLS;
		break;
	    case 'k':		/* kill old interface facet */
		flagWord |= VULCAN_DESTROY_OLD;
		break;
	    case 'C':		/* cut back frames around terminals */
		flagWord |= VULCAN_CUT_BACK_FRAMES;
		break;
	    case 'd':		/* bury user in debugging info */
		debugType = (vulcanDebugType) atoi(optarg);
		break;
	    case 'f':		/* minimum fractional grow-merge-shrink */
		minFeature = atof(optarg);
		break;
	    case 'o':		/* output facet name */
		outputFacetName = optarg;
		break;
	}
    }
}

static void messageHandler(type, message)
vulcanMessageType type;
char *message;
{
    switch (type) {
	case VULCAN_SEVERE:
	    (void) fprintf(stderr, "%s:  SEVERE: %s", optProgName, message);
	    break;
	case VULCAN_WARNING:
	    (void) fprintf(stderr, "%s:  WARNING: %s", optProgName, message);
	    break;
	case VULCAN_INFORMATION:
	    (void) fprintf(stderr, "%s:  %s", optProgName, message);
	    break;
	case VULCAN_DEBUGGING:
	    (void) fprintf(stderr, "%s:  debug: %s", optProgName, message);
	    break;
	case VULCAN_PARTIAL:
	    (void) fprintf(stderr, "%s:  start of long message\n%s\n",
				optProgName, message);
	    break;
	case VULCAN_FATAL:
	    (void) fprintf(stderr, "%s:  fatal: %s", optProgName, message);
	    break;

    }
    (void) fflush(stderr);
}
