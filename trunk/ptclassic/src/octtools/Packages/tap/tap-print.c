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
#include "errtrap.h"
#include "ansi.h"
#include "oct.h"
#include "oh.h"
#include "tap.h"
#include "options.h"

#define TAP_DEBUG

#ifdef TAP_DEBUG	/* for debugging tap itself */
#include "tap_int.h"
#endif

optionStruct optionList[] = {
    /* Make sure -T is the first one -- the message is changed dynamically */
    { "T",	"tech_root",	"root directory (default "		},
    { "a",	0,		"print all layer properties"		},
    { "d",	0,		"print design rule information"		},
    { "l",	0,		"print layer looks information"		},
    { "p",	0,		"print summary of known palettes"	},
#ifdef TAP_DEBUG
    { "c",	0,		"print list of connectors"		},
#endif
    { "q",	0,		"interactively query connectors"	},
    { OPT_RARG,	"technology",	"name of technology"			},
    { OPT_RARG,	"viewtype",	"view type for this technology"		},
    { 0,	0,		0					}
};

#define DISPLAY_BAG_NAME	"DISPLAY-LOOKS"

#define DO_LAYER_PROPS	0x001
#define DO_DES_RULES	0x002
#define DO_LAYER_LOOKS	0x004
#define DO_PALATTES	0x008
#define DO_CONNECTORS	0x010
#define DO_QUERY_CONN	0x020

void printProp
	ARGS((octObject *propPtr));
void printRuleValue
	ARGS((octCoord val, octCoord lambda));
int countObjects
	ARGS((octObject *containerPtr, octObjectMask mask));
int eatSpace();
void eatAll();
void getString
	ARGS((char *str));
void getNumber
	ARGS((int *numPtr));


int parseOptions(argc, argv)
int argc;
char *argv[];
{
    int allOptions = 0;
    int option;
    char *rootDir, *usageT;
    extern char *optarg;

    /* fix usage message for -T option */
    rootDir = tapGetDefaultRoot();
    usageT = ALLOC(char, strlen(optionList[0].description)+strlen(rootDir)+2);
    (void) sprintf(usageT, "%s%s)", optionList[0].description, rootDir);
    optionList[0].description = usageT;

    while ((option = optGetOption(argc, argv)) != EOF) {
	switch (option) {
	    case 'T':		/* change the technology root directory */
		(void) tapRootDirectory(optarg);
		break;
	    case 'a':		/* print layer properties */
		allOptions |= DO_LAYER_PROPS;
		break;
	    case 'd':		/* print design rule information */
		allOptions |= DO_DES_RULES;
		break;
	    case 'l':		/* print layer looks information */
		allOptions |= DO_LAYER_LOOKS;
		break;
	    case 'p':		/* print palette instances */
		allOptions |= DO_PALATTES;
		break;
	    case 'c':		/* print list of connectors */
		allOptions |= DO_CONNECTORS;
		break;
	    case 'q':		/* interactively query connectors */
		allOptions |= DO_QUERY_CONN;
		break;
	}
    }

    return(allOptions);
}

void
printGeneralInfo(facetPtr)
octObject *facetPtr;
{
    char *str=(char *)NULL, *pkg, *msg;
    int code;
    octGenerator gen;
    octStatus status;
    octObject bag;
    octObject prop;

    (void) printf("Technology root directory:  ");
    ERR_IGNORE(str = tapRootDirectory(NIL(char)));
    if (errStatus(&pkg, &code, &msg)) {
	(void) printf(">>>%s\n", msg);
    } else {
	(void) puts(str);
    }

    (void) printf("Specific technology directory:  ");
    ERR_IGNORE(str = tapGetDirectory(facetPtr));
    if (errStatus(&pkg, &code, &msg)) {
	(void) printf(">>>%s\n", msg);
    } else {
	(void) puts(str);
    }

    (void) printf("Technology properties:");
    OH_ASSERT(ohGetOrCreateBag(facetPtr, &bag, TECH_PROP_BAG_NAME));
    OH_ASSERT(octInitGenContents(&bag, OCT_PROP_MASK, &gen));
    while ((status = octGenerate(&gen, &prop)) == OCT_OK) {
	(void) printf("\n\t");
	printProp(&prop);
    }
    OH_ASSERT(status);
    (void) printf("\n\n");
}

void
printLayerProps(facetPtr)
octObject *facetPtr;
{
    octObject layer1, layer2;
    octObject prop;
    octGenerator layerGen, propGen, propLyrGen;
    octStatus stat;

    (void) printf("Layer properties:");

    OH_ASSERT(octInitGenContents(facetPtr, OCT_LAYER_MASK, &layerGen));
    while ((stat = octGenerate(&layerGen, &layer1)) == OCT_OK) {
	(void) printf("\n    %s:", layer1.contents.layer.name);

	OH_ASSERT(octInitGenContents(&layer1, OCT_PROP_MASK, &propGen));
	while ((stat = octGenerate(&propGen, &prop)) == OCT_OK) {
	    (void) printf("\n\t");

	    OH_ASSERT(octInitGenContainers(&prop, OCT_LAYER_MASK, &propLyrGen));
	    while ((stat = octGenerate(&propLyrGen, &layer2)) == OCT_OK) {
		(void) printf("%s ", layer2.contents.layer.name);
	    }
	    OH_ASSERT(stat);

	    printProp(&prop);
	}
	OH_ASSERT(stat);
    }
    OH_ASSERT(stat);

    (void) printf("\n\n");
}

void
printProp(propPtr)
octObject *propPtr;
{
    (void) printf("%s", propPtr->contents.prop.name);

    switch (propPtr->contents.prop.type) {
	case OCT_NULL:
	    (void) printf("(null)");
	    break;
	case OCT_INTEGER:
	    (void) printf("(int):  %ld",
			(long) propPtr->contents.prop.value.integer);
	    break;
	case OCT_REAL:
	    (void) printf("(real):  %g", propPtr->contents.prop.value.real);
	    break;
	case OCT_STRING:
	    (void) printf("(string):  %s", propPtr->contents.prop.value.string);
	    break;
	case OCT_ID:
	    (void) printf("(oct id):  %s",
                          octFormatId(propPtr->contents.prop.value.id));
	    break;
	case OCT_STRANGER:
	    (void) printf("(stranger):  length=%ld",
			(long) propPtr->contents.prop.value.stranger.length);
	    break;
	case OCT_REAL_ARRAY:
	    (void) printf("(real-array):  length=%ld",
			(long) propPtr->contents.prop.value.real_array.length);
	    break;
	case OCT_INTEGER_ARRAY:
	    (void) printf("(int-array):  length=%ld",
		    (long) propPtr->contents.prop.value.integer_array.length);
	    break;
	default:
	    (void) printf("(unknown type %d)",
			(int) propPtr->contents.prop.type);
    }
}

void
printDesignRules(facetPtr)
octObject *facetPtr;
{
    octGenerator gen, gen2;
    octStatus status;
    octObject layer, layer2;
    octCoord rule = (octCoord)0;
    int err = 0;
    char *pkg, *msg;
    int code;
    int lambda = 20;
    if ( tapGetProp(facetPtr, TAP_LAMBDA) ) {
	lambda = TAP_LAMBDA->value.integer;
    } else {
	fprintf(stderr, "No lambda value: 20 assumed\n" );
    }
    (void) printf("Width rules\n");
    (void) printf("       min  max\n");
    OH_ASSERT(octInitGenContents(facetPtr, OCT_LAYER_MASK, &gen));
    while ((status = octGenerate(&gen, &layer)) == OCT_OK) {
	(void) printf("%-5s ", layer.contents.layer.name);
	ERR_IGNORE(rule = tapGetMinWidth(&layer));
	printRuleValue(rule, lambda);
	err = errStatus(&pkg, &code, &msg);
	(void) printf(" ");
	ERR_IGNORE(rule = tapGetMaxWidth(&layer));
	printRuleValue(rule, lambda);
	(void) printf("\n");
	if (errStatus(&pkg, &code, &msg) || err) {
	    (void) printf(">>>%s\n", msg);
	}
    }
    OH_ASSERT(status);

    (void) printf("\nMinimum spacing rules\n        ");
    OH_ASSERT(octInitGenContents(facetPtr, OCT_LAYER_MASK, &gen));
    while ((status = octGenerate(&gen, &layer)) == OCT_OK) {
	(void) printf("%-5s ", layer.contents.layer.name);
    }
    OH_ASSERT(status);
    (void) printf("\n");
    OH_ASSERT(octInitGenContents(facetPtr, OCT_LAYER_MASK, &gen));
    while ((status = octGenerate(&gen, &layer)) == OCT_OK) {
	(void) printf("%-5s ", layer.contents.layer.name);
	err = 0;
	OH_ASSERT(octInitGenContents(facetPtr, OCT_LAYER_MASK, &gen2));
	while ((status = octGenerate(&gen2, &layer2)) == OCT_OK) {
	    (void) printf("  ");
	    ERR_IGNORE(rule = tapGetMinSpacing(&layer, &layer2));
	    printRuleValue(rule, lambda);
	    err |= errStatus(&pkg, &code, &msg);
	}
	OH_ASSERT(status);
	(void) printf("\n");
    }
    OH_ASSERT(status);
    if (err) {
	(void) printf(">>>%s\n", msg);
    }

    (void) printf("\nMinimum overlap rules\n        ");
    OH_ASSERT(octInitGenContents(facetPtr, OCT_LAYER_MASK, &gen));
    while ((status = octGenerate(&gen, &layer)) == OCT_OK) {
	(void) printf("%-5s ", layer.contents.layer.name);
    }
    OH_ASSERT(status);
    (void) printf("\n");
    OH_ASSERT(octInitGenContents(facetPtr, OCT_LAYER_MASK, &gen));
    while ((status = octGenerate(&gen, &layer)) == OCT_OK) {
	(void) printf("%-5s ", layer.contents.layer.name);
	err = 0;
	OH_ASSERT(octInitGenContents(facetPtr, OCT_LAYER_MASK, &gen2));
	while ((status = octGenerate(&gen2, &layer2)) == OCT_OK) {
	    (void) printf("  ");
	    ERR_IGNORE(rule = tapGetMinOverlap(&layer, &layer2));
	    printRuleValue(rule, lambda);
	    err |= errStatus(&pkg, &code, &msg);
	}
	OH_ASSERT(status);
	(void) printf("\n");
    }
    OH_ASSERT(status);
    if (err) {
	(void) printf(">>>%s\n", msg);
    }

    (void) printf("\n");
}

void
printRuleValue(val, lambda)
octCoord val;
octCoord lambda;
{
    char *pkg, *msg;
    int code;

    if (errStatus(&pkg, &code, &msg)) {
	(void) printf(" err");
    } else if (val == TAP_NO_RULE) {
	(void) printf("none");
    } else {
	(void) printf("%4.1f", (double)val / (double)lambda );
    }
}

#define STIPPLE_TYPE(flg)	((flg) == TAP_EMPTY ? "empty" : \
				 (flg) == TAP_STIPPLED ? "stippled" : \
				 (flg) == TAP_SOLID ? "solid" : "???")
void
printLayerLooks(facetPtr)
octObject *facetPtr;
{
    octGenerator layerGen, deviceGen;
    octStatus status;
    char *pkg, *msg;
    int code;
    octObject layer, dispBag, deviceBag;
    int prio, nColors, fill, border;
    unsigned short r, g, b;
    int w, h;
    char *bits;
    int i;
    tapStroke *strokes;

    OH_ASSERT(octInitGenContents(facetPtr, OCT_LAYER_MASK, &layerGen));
    while ((status = octGenerate(&layerGen, &layer)) == OCT_OK) {
	(void) printf("Layer ``%s'' looks:\n", layer.contents.layer.name);
	dispBag.type = OCT_BAG;
	dispBag.contents.bag.name = DISPLAY_BAG_NAME;
	if (octGetByName(&layer, &dispBag) != OCT_OK) continue;
	OH_ASSERT(octInitGenContents(&dispBag, OCT_BAG_MASK, &deviceGen));
	while ((status = octGenerate(&deviceGen, &deviceBag)) == OCT_OK) {
	    (void) printf("    For a ``%s'' device:\n",
						deviceBag.contents.bag.name);
	    ERR_IGNORE(
		(tapSetDisplayType(deviceBag.contents.bag.name, TAP_COLOR),
		 tapGetDisplayInfo(&layer, &prio, &nColors, &fill, &border)));
	    if (errStatus(&pkg, &code, &msg)) {
		(void) printf(">>>%s\n", msg);
		continue;
	    }

	    (void) printf(
			"\tPriority: %d\tnColors: %d\tFill: %s\tBorder: %s\n",
			prio, nColors,
			STIPPLE_TYPE(fill), STIPPLE_TYPE(border));

	    (void) printf("\tColors:");
	    for (i = 0; i < nColors; i++) {
		ERR_IGNORE(tapGetDisplayColor(&layer, i, &r, &g, &b));
		if (errStatus(&pkg, &code, &msg)) {
		    (void) printf(">>>%s\n", msg);
		    break;
		}
		(void) printf(" (%u,%u,%u)", r, g, b);
	    }
	    (void) printf("\n");

	    ERR_IGNORE(tapGetDisplayPattern(&layer, TAP_FILL_PATTERN, &w, &h, &bits));
	    if (errStatus(&pkg, &code, &msg)) {
		(void) printf(">>>%s\n", msg);
		continue;
	    }
	    
	    {
		/* Print pattern nicely */
		int i = 0;	/* Count rows */
		int j = 0;	/* Count cols */
		int n = 0;
		char* buf = (char*)malloc( sizeof(char) * w * h * 2 ); /* Rather large buffer */
		
		while ( i < h ) {
		    buf[n++] = '\n';
		    buf[n++] = '\t';
		    j = 0;
		    while ( j < w ) {
			buf[n++] = bits[ i * w + j ];
			j++;
		    } 
		    i++;
		} 
		buf[n] = '\0';	/* Terminate buffer. */
		(void) printf("\tFill pattern: %dx%d  %s\n", w, h, buf);
		free( buf );
		
	    }
	    ERR_IGNORE(tapGetDisplayStrokes(&layer, &w, &strokes));
	    if (errStatus(&pkg, &code, &msg)) {
		(void) printf(">>>%s\n", msg);
		continue;
	    }
	    (void) printf("\t\tstrokes: ");
	    for (i = 0; i < w; i++) {
		(void) printf(" (%.1f - %d)",
				strokes[i].angle / 10.0, strokes[i].spacing);
	    }
	    (void) printf("\n");

	    ERR_IGNORE(tapGetDisplayPattern(&layer, TAP_BORDER_PATTERN, &w, &h, &bits));
	    if (errStatus(&pkg, &code, &msg)) {
		(void) printf(">>>%s\n", msg);
		continue;
	    }
	    (void) printf("\tBorder pattern: %dx%d  \"%s\"\n\n", w, h, bits);
	}
	OH_ASSERT(status);
    }
    OH_ASSERT(status);
}

void
printPalettes(facetPtr)
octObject *facetPtr;
{
    octObject palette;
    int numPalettes, i;
    char **paletteNames;
    int stat = 0;
    char *pkg, *msg;
    int code;

    ERR_IGNORE(tapListPalettes(facetPtr, &numPalettes, &paletteNames));
    if (errStatus(&pkg, &code, &msg)) {
	(void) printf(">>>%s\n", msg);
    } else {
	(void) printf("%d palette%s: ", numPalettes,
					(numPalettes == 1 ? "" : "s"));
	for (i = 0; i < numPalettes; i++) {
	    (void) printf(" %s", paletteNames[i]);
	}
	(void) printf("\n");
	FREE(paletteNames);
    }

    ERR_IGNORE(stat = tapOpenPalette(facetPtr, TAP_LAYER_PALETTE_NAME, &palette, "r"));
    if (errStatus(&pkg, &code, &msg)) {
	(void) printf("    >>>%s\n", msg);
    } else if (stat == 0) {
	(void) printf("    >>>couldn't find ``%s'' palette\n",
			TAP_LAYER_PALETTE_NAME);
    } else {
	(void) printf("    Layer Palette:\t``%s:%s:%s:%s''\n",
			palette.contents.facet.cell,
			palette.contents.facet.view,
			palette.contents.facet.facet,
			palette.contents.facet.version);
	(void) printf("\t%d Layers\n",
			countObjects(&palette, OCT_LAYER_MASK));
    }

    if ( strcmp( "physical", facetPtr->contents.facet.view ) ) {
	ERR_IGNORE(stat = tapOpenPalette(facetPtr, TAP_CONNECTOR_PALETTE_NAME, &palette, "r"));
	if (errStatus(&pkg, &code, &msg)) {
	    (void) printf("    >>>%s\n", msg);
	} else if (stat == 0) {
	    (void) printf("    >>>couldn't find ``%s'' palette\n",
			  TAP_CONNECTOR_PALETTE_NAME);
	} else {
	    (void) printf("    Connector Pal.:\t``%s:%s:%s:%s''\n",
			  palette.contents.facet.cell,
			  palette.contents.facet.view,
			  palette.contents.facet.facet,
			  palette.contents.facet.version);
	}
    }

    (void) printf("\n");
}

int
countObjects(containerPtr, mask)
octObject *containerPtr;
octObjectMask mask;
{
    octGenerator gen;
    octStatus stat;
    octObject dummyObj;
    int count = 0;

    OH_ASSERT(octInitGenContents(containerPtr, mask, &gen));
    while ((stat = octGenerate(&gen, &dummyObj)) == OCT_OK) {
	count++;
    }
    OH_ASSERT(stat);
    return(count);
}

void
printConnectors(facetPtr)
octObject *facetPtr;
{
#ifdef TAP_DEBUG
    struct tapLayerListElement layerList[1];
    octObject instance;
    struct tapTech *techPtr;
    struct tapConnector *connPtr;
    int i;
    int stat;
    char *pkg, *msg;
    int code;
    int lambda = 20;
    int lambda2;

    if ( tapGetProp(facetPtr, TAP_LAMBDA) ) {
	lambda = TAP_LAMBDA->value.integer;
    } else {
	fprintf(stderr, "No lambda value: 20 assumed\n" );
    }
    lambda2 = lambda * lambda;

    /* force tap to initialize its connector list (by asking for one) */
    if (octGenFirstContent(facetPtr, OCT_LAYER_MASK, &layerList[0].layer) != OCT_OK) {
	(void) fprintf(stderr, "technology facet has no layers\n");
	exit(1);
    }
    layerList[0].width = 0;
    layerList[0].direction = 0;
    ERR_IGNORE(stat = tapGetConnector(1, layerList, &instance));
    if (errStatus(&pkg, &code, &msg)) {
	(void) printf(">>>%s\n", msg);
	return;
    }

    
/*
    if (stat == 0) {
	(void) printf(">>>(couldn't find any connector)\n");
	return;
    }
*/

    /* run through connectors in technology */
    techPtr = tapInternTech(facetPtr);
    for (connPtr = techPtr->connList; connPtr; connPtr = connPtr->next) {
	(void) printf("Connector ``%s:%s''\t%2d layers\t%g area\n",
			connPtr->instance.contents.instance.master,
			connPtr->instance.contents.instance.view,
			connPtr->nLayers, connPtr->area / lambda2);
	for (i = 0; i < techPtr->layerCount; i++) {
	    if (connPtr->layerDescList[i].isUsed) {
		(void) printf("%15s r(%2ld) t(%2ld) l(%2ld) b(%2ld)\n",
		    techPtr->layerList[i].techLayer.contents.layer.name,
		      (long)connPtr->layerDescList[i].widthArray[0]/lambda,
		      (long)connPtr->layerDescList[i].widthArray[1]/lambda,
		      (long)connPtr->layerDescList[i].widthArray[2]/lambda,
		      (long)connPtr->layerDescList[i].widthArray[3]/lambda);
	    }
	}
    }
#endif

    (void) printf("\n");
}

char *transformName[] = {
	"no transform",		"mirror X",		"mirror Y",
	"rotate 90",		"rotate 180",		"rotate 270",
	"mirror X, rot 90",	"mirror Y, rot 90",	"full transform" };

void
queryConnectors(facetPtr)
octObject *facetPtr;
{
    struct tapLayerListElement layerList[30];
    char layerNames[30][50];
    int numLayers;
    octObject instance;
    char tempString[50];
    int stat = 0;
    char *pkg, *msg;
    int code;
    int temp;

    (void) printf("Layer-name width direction[ahvlrtb] ...\n");

    while (1) {
	numLayers = 0;
	while (eatSpace()) {
	    getString(layerNames[numLayers]);
	    layerList[numLayers].layer.type = OCT_LAYER;
	    layerList[numLayers].layer.contents.layer.name =
						layerNames[numLayers];
	    if (octGetByName(facetPtr, &layerList[numLayers].layer) != OCT_OK) {
		(void) printf("Layer ``%s'' not in technology\n",
				layerNames[numLayers]);
		numLayers = 0;
		eatAll();
		break;
	    }
	    if (! eatSpace()) {
		(void) printf("Bad input line\n");
		numLayers = 0;
		break;
	    }
	    getNumber(&temp);
	    layerList[numLayers].width = temp;

	    layerList[numLayers].width *= 20;
	    if (! eatSpace()) {
		(void) printf("Bad input line\n");
		numLayers = 0;
		break;
	    }
	    getString(tempString);
	    if (strlen(tempString) != 1) {
		(void) printf("Bad input line\n");
		numLayers = 0;
		eatAll();
		break;
	    }
	    switch (tempString[0]) {
		case 'a':
		    layerList[numLayers].direction =
					    TAP_HORIZONTAL | TAP_VERTICAL;
		    break;
		case 'h':
		    layerList[numLayers].direction = TAP_HORIZONTAL;
		    break;
		case 'v':
		    layerList[numLayers].direction = TAP_VERTICAL;
		    break;
		case 'r':
		    layerList[numLayers].direction = TAP_RIGHT;
		    break;
		case 't':
		    layerList[numLayers].direction = TAP_TOP;
		    break;
		case 'l':
		    layerList[numLayers].direction = TAP_LEFT;
		    break;
		case 'b':
		    layerList[numLayers].direction = TAP_BOTTOM;
		    break;
	    }

	    numLayers++;
	}

	if (numLayers > 0) {
	    ERR_IGNORE(stat = tapGetConnector(numLayers, layerList, &instance));
	    if (errStatus(&pkg, &code, &msg)) {
		    (void) printf(">>>%s\n", msg);
	    } else if (stat == 0) {
		    (void) printf("can't find adequate connector\n");
	    } else {
		(void) printf("Connector: ``%s:%s''\n\tTransform: %s\n",
			instance.contents.instance.master,
			instance.contents.instance.view,
			transformName[(int)
			instance.contents.instance.transform.transformType]);
	    }
	}
    }
}

int
eatSpace()
{
    int ch;

    while (isspace(ch = getchar())) {
	if (ch == '\n') return(0);
    }
    if (ch == EOF) exit(0);
    (void) ungetc(ch, stdin);
    return(1);
}

void
eatAll()
{
    while (getchar() != '\n') ;
}

void
getString(str)
char *str;
{
    int ch;

    while (! isspace(ch = getchar())) {
	*str++ = ch;
    }
    *str = '\0';
    (void) ungetc(ch, stdin);
}

void getNumber(numPtr)
int *numPtr;
{
    char string[100];

    getString(string);
    *numPtr = atoi(string);
}

int
main(argc, argv)
int argc;
char *argv[];
{
    octObject facet;
    int options;
    char *pkg, *msg;
    int code;
    extern int optind;

    options = parseOptions(argc, argv);

    if (argc != optind+2) optUsage();

    octBegin();

    /* get the technology facet directly */
    ERR_IGNORE(tapOpenTechFacet(argv[optind], argv[optind+1], &facet, "r"));
    if (errStatus(&pkg, &code, &msg)) {
	(void) fprintf(stderr,
		    "can't find facet for ``%s'' technology, viewtype ``%s''\n",
		    argv[optind], argv[optind+1]);
	(void) fprintf(stderr, "%s\n", msg);
	exit(1);
    }

    /* keep the facet open even if tap flushes its caches and closes it */
    facet.contents.facet.mode = "r";
    if (octOpenFacet(&facet) != OCT_ALREADY_OPEN) {
	(void) fprintf(stderr,
		    "can't reopen technology facet %s:%s:%s???\n",
		    facet.contents.facet.cell, facet.contents.facet.view,
		    facet.contents.facet.facet);
	exit(1);
    }

    printGeneralInfo(&facet);

    if (options & DO_LAYER_PROPS) printLayerProps(&facet);

    if (options & DO_DES_RULES) printDesignRules(&facet);

    if (options & DO_LAYER_LOOKS) printLayerLooks(&facet);

    if (options & DO_PALATTES) printPalettes(&facet);

    if (options & DO_CONNECTORS) printConnectors(&facet);

    if (options & DO_QUERY_CONN) queryConnectors(&facet);

    octEnd();

    return 0;
}
