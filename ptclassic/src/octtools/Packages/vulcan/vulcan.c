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
#include <varargs.h>
#include "oct.h"
#include "oh.h"
#include "fang.h"
#include "harpoon.h"
#include "st.h"
#include "tap.h"
#include "ansi.h"
#include "vulcan.h"

/*
 *	Assertion macros for the packages used
 */
#undef OCT_ASSERT
#define OCT_ASSERT(exp)		\
    if ((exp) < OCT_OK) {						\
	errRaise(VULCAN_PKG_NAME, 0, "oct assertion failure (%s:%d) %s",\
			__FILE__, __LINE__, octErrorString());		\
    }
#define ASSERT(exp, mess)	\
    if ( ! (exp)) {							\
	errRaise(VULCAN_PKG_NAME, 0, "assertion failure (%s:%d) %s",	\
			__FILE__, __LINE__, mess);			\
    }

#define VULCAN_START(name)	if (vulcanDepth++ == 0) {		\
				    vulcanRoutine = (name);		\
				    errPushHandler(vulcanHandler);	\
				}
#define VULCAN_END()		if (--vulcanDepth == 0) {		\
				    errPopHandler();			\
				}
char vulcanPkgName[] = "vulcan";
static int vulcanDepth = 0;
static char *vulcanRoutine = "<no routine>";


static void vulcanHandler();

static st_table *framedFacets;		/* table of all completed frames */
					    /* key:  interface facet id */
					    /* value:  (frameDesc *) */

static void defaultMessageHandler();
static void (*messageHandler)() = defaultMessageHandler;

static vulcanMessageType worstMessage;	/* this'll be returned to the user */

/*typedef enum { VUL_FALSE, VUL_TRUE } boolean;*/

extern int octIdCmp(), octIdHash();

typedef struct termDesc {
    struct termDesc *next;		/* forward link */
    char *name;				/* name of terminal */
    harList implementation;		/* implementing geometry */
} termDesc;

typedef struct frameDesc {
    octObject inFacet;			/* unframed facet */
    octObject outFacet;			/* framed facet */
    harList frameGeos;			/* protection frames (excl. terms) */
    harList cutGeos;			/* all term geo (to cut from frame) */
    termDesc *terminals;		/* per-terminal frame geo */
    st_table *masters;			/* table of instance masters */
					    /* key:  an instance pointer */
					    /* value:  (masterEntry *) */
    st_table *instances;		/* table of intsances */
					    /* key:  instance id */
					    /* value:  (instTrans *) */

    int frameGeoType;			/* harpoon geo type for frames */
    octObjectMask frameGeoMask;		/* gen mask for frame geo to delete */
    int flagWord;			/* bunch of flag bits */
    double fracGrowth;			/* fraction of cell size to grow */
    vulcanDebugType debug;		/* type of debugging output if any */
} frameDesc;

typedef struct masterEntry {
    frameDesc *master;			/* description of master */
    struct instTrans *transList;	/* list of transforms for instances */
} masterEntry;

typedef struct instTrans {
    struct instTrans *next;		/* forward link */
    masterEntry *masterEntPtr;		/* this instance's master */
    struct octTransform transform;	/* transform of an instance */
    boolean includedUngrown;		/* included in frame before growing
					 * (because it implemented a formal)
					 */
} instTrans;

#define BIG_JOINED_NAME		"JOINEDTERMS"
#define JOINED_NAME		"JOINED"
#define ALIASED_NAME		"CONNECTED"		/* XXX Not ratified */
#define ALIASED_NET_NAME	"CONNECTEDNETS"		/* XXX Not ratified */

/* WRONG!!! should be technology-dependent XXX */
#define GLOBAL_LAYER_NAME	"PLACE"

#define OCT_PHYSGEO_MASK	(OCT_BOX_MASK | OCT_POLYGON_MASK | \
					OCT_CIRCLE_MASK | OCT_PATH_MASK)

/* Forward references */
static void doFrame
	ARGS((frameDesc *framePtr));
static frameDesc *getMasterFrame
	ARGS((frameDesc *parentFramePtr, octObject *instPtr));
static void doFrameMaster
	ARGS((char *userStr, frameDesc *framePtr, octObject *instPtr, frameDesc *parentFramePtr));
static void getEquivTerm
	ARGS((octObject *oldTermPtr, octObject *newTermPtr, octObject *facetPtr));
static void handleTermConnectivity
	ARGS((octObject *oldTermPtr, octObject *newTermPtr, octObject *outputFacetPtr));
static void handlePromotedTerminal
	ARGS((octObject *actualPtr, termDesc *termDescPtr, frameDesc *framePtr));
static void handleNetGroup
	ARGS((octObject *netBagPtr, octObject *facetPtr));
static int handleNet
	ARGS((octObject *netPtr, octObject *facetPtr, octObject *objPtr));

static void handleFormalTerminal
	ARGS((octObject *termPtr, frameDesc *framePtr));
static void collectTerminalGeo
	ARGS((octObject *termPtr, termDesc *termDescPtr, frameDesc *framePtr));

static void buildSubcellTable
	ARGS((frameDesc *framePtr));
static void setProps
	ARGS((frameDesc *framePtr));
static void collectFormalTerminals
	ARGS((frameDesc *framePtr));
static void doProtectionFrames
	ARGS((frameDesc *framePtr));
static int getSize
	ARGS((octObject *objPtr));
static int getTechBasedGrowth
	ARGS((frameDesc *framePtr, octObject *techFacetPtr, char *layerName));
static int makeLayerFrame
	ARGS((frameDesc *framePtr, harList geoList, int grow, fa_geometry *newGeoPtr));

static int cutbackMakeLayerFrame
	ARGS((frameDesc *framePtr, harList geoList, int grow, int techGrow, fa_geometry *newGeoPtr));
static void reportUnreachableTerm
	ARGS((frameDesc *framePtr, char *layerName, fa_geometry *frameGeoPtr));
static fa_geometry *getExistingNamedGeo
	ARGS((char *name, harList *geoListPtr));
static void makeGlobalFrame
	ARGS((frameDesc *framePtr));
static void buildFrames
	ARGS((frameDesc *framePtr));
static void freeFrame
	ARGS((frameDesc *framePtr));
static void debugDumpFrame
	ARGS((frameDesc *framePtr));
static void debugDumpGeos
	ARGS((harList geoList));
int masterCmp
	ARGS((CONST char *a, CONST char *b));
static int masterHash
	ARGS((char *val, int mod));
static void spillExcessError();
static void errAddString
	ARGS((char *str));
static void errAddNumber
	ARGS((int num));
static void addTransformedGeoList
	ARGS((harList *geoListPtr, struct octTransform *transformPtr, harList *newGeoListPtr));
static void andNotGeoList
	ARGS((harList *geoListPtr, harList *cutGeoListPtr));


/*
 *	This stuff is basically a small subset of the nonexistent
 *  ``snprintf'' function with a facet-printing primitive.
 */
#define ERR_BUF_SIZE	1000
static char errorBuffer[ERR_BUF_SIZE + 20];	/* enough extra for any %d */
static char *errBufPtr = errorBuffer;


/*VARARGS*/
static void message(va_alist)
va_dcl
{
    vulcanMessageType type;
    char *format;
    va_list ap;

    va_start(ap);
    type = va_arg(ap, vulcanMessageType);
    if ((int) type > (int) worstMessage) worstMessage = type;
    format = va_arg(ap, char *);

    while (*format != '\0') {
	spillExcessError();

	if (*format != '%') {
	    *errBufPtr++ = *format++;
	} else {
	    format++;
	    switch (*format++) {
		case 's':		/* string argument */
		    errAddString(va_arg(ap, char *));
		    break;
		case 'd':		/* decimal number argument */
		    errAddNumber(va_arg(ap, int));
		    break;
		case 'f':		/* facet pointer */
		    {
			octObject *facetPtr = va_arg(ap, octObject *);

			*errBufPtr++ = '`';
			errAddString(facetPtr->contents.facet.cell);
			*errBufPtr++ = ':';
			errAddString(facetPtr->contents.facet.view);
			*errBufPtr++ = ':';
			errAddString(facetPtr->contents.facet.facet);
			*errBufPtr++ = ':';
			errAddString(facetPtr->contents.facet.version);
			*errBufPtr++ = '\'';
		    }
		    break;
		default:		/* shouldn't happen */
		    ASSERT(0, "***bad format directive in message"); /* SUPPRESS 275 */
	    }
	}
    }
    *errBufPtr = '\0';
    va_end(ap);

    if (type != VULCAN_PARTIAL) {
	(*messageHandler)(type, errorBuffer);
	errBufPtr = errorBuffer;
    }
}

/*
 *	vulcanParameterized -- full form of interface generator
 */
vulcanMessageType
vulcanParameterized(inputFacetPtr, outputFacetPtr, flags, minFeature, dbg)
octObject *inputFacetPtr;	/* facet from which frame is made */   
octObject *outputFacetPtr;	/* framed facet */
int flags;			/* miscellaneous flag bits */
double minFeature;		/* percent size of smallest feature to keep */
vulcanDebugType dbg;		/* type of debugging information to give */
{
    frameDesc *framePtr;	/* frame structure for desired cell */
    st_generator *gen;		/* runs through framedFacets, to free all */
    char *key, *value;		/* generated values */
    octObject facet;		/* interface of a framed facet, to be closed */

    VULCAN_START("vulcanParameterized");

    framedFacets = st_init_table(octIdCmp, octIdHash);
    worstMessage = VULCAN_INFORMATION;

    framePtr = ALLOC(frameDesc, 1);
    framePtr->inFacet = *inputFacetPtr;
    framePtr->outFacet = *outputFacetPtr;
    framePtr->fracGrowth = minFeature / 200.0;
    framePtr->debug = dbg;
    framePtr->flagWord = flags;

    if (framePtr->flagWord & VULCAN_BOX_FRAMES) {
	if (framePtr->flagWord & VULCAN_UNFILL_FRAMES) {
	    framePtr->frameGeoType = HAR_UNFILLED_BOX;
	} else {
	    framePtr->frameGeoType = HAR_FILLED_BOX;
	}
	framePtr->frameGeoMask = OCT_BOX_MASK;
    } else {
	if (framePtr->flagWord & VULCAN_UNFILL_FRAMES) {
	    framePtr->frameGeoType = HAR_UNFILLED_POLYGON;
	} else {
	    framePtr->frameGeoType = HAR_FILLED_POLYGON;
	}
	framePtr->frameGeoMask = OCT_POLYGON_MASK;
    }

    fa_set_auto_merge(1000, 1.5);

    doFrame(framePtr);
    freeFrame(framePtr);
    gen = st_init_gen(framedFacets);
    while (st_gen(gen, &key, &value)) {
	facet.objectId = *(octId *) key;
	OCT_ASSERT(octGetById(&facet));
	OCT_ASSERT(octFreeFacet(&facet));
	freeFrame((frameDesc *) value);
        FREE(key);
    }
    st_free_gen(gen);
    st_free_table(framedFacets);

    VULCAN_END();
    return(worstMessage);
}

/*
 *	vulcan -- simple form of interface generator with default parameters
 */
vulcanMessageType vulcan(inputFacetPtr, outputFacetPtr)
octObject *inputFacetPtr;	/* facet from which frame is made */   
octObject *outputFacetPtr;	/* framed facet */
{
    VULCAN_START("vulcan");
    (void) vulcanParameterized(inputFacetPtr, outputFacetPtr,
		VULCAN_DFLT_FLAGS, VULCAN_DFLT_MIN_FEATURE, VULCAN_NO_DEBUG);
    VULCAN_END();
    return(worstMessage);
}

/*
 *	vulcanMessageHandler -- register a message handling function
 */
void vulcanMessageHandler(handler)
void (*handler)();
{
    if (handler) {
	messageHandler = handler;
    } else {
	messageHandler = defaultMessageHandler;
    }
}

static void doFrame(framePtr)
    frameDesc *framePtr;		/* frame structure to be filled in */
{
    framePtr->frameGeos = HAR_EMPTY_LIST;
    framePtr->cutGeos = HAR_EMPTY_LIST;
    framePtr->terminals = NIL(termDesc);
    framePtr->masters = NIL(st_table);
    framePtr->instances = NIL(st_table);

    {
	struct octFacet *inFacetPtr = &framePtr->inFacet.contents.facet;

	inFacetPtr->cell = util_strsav(inFacetPtr->cell);
	inFacetPtr->view = util_strsav(inFacetPtr->view);
	inFacetPtr->facet = util_strsav(inFacetPtr->facet);
	inFacetPtr->version = util_strsav(inFacetPtr->version);
    }

    buildSubcellTable(framePtr);	/* make master and instance tables */

    if (!octIdIsNull(framePtr->outFacet.objectId)) {
	setProps(framePtr);		/* put required props on output facet */
    }

    collectFormalTerminals(framePtr);	/* collect terminal implementations */

    doProtectionFrames(framePtr);	/* collect frames; free subcells */

    if ( framePtr->flagWord & VULCAN_GLOBAL_FRAME ) {
	makeGlobalFrame(framePtr);		/* make frame on PLACE layer */
    }

    if (framePtr->flagWord & VULCAN_CLOSE_INPUT) {
	OCT_ASSERT(octFreeFacet(&framePtr->inFacet));
    }

    if (!octIdIsNull(framePtr->outFacet.objectId)) {
	buildFrames(framePtr);		/* put protection frames in output */
	OCT_ASSERT(octFlushFacet(&framePtr->outFacet));
    }
}

static void buildSubcellTable(framePtr)
frameDesc *framePtr;		/* frame structure to be filled in */
{
    octGenerator instanceGen;	/* generator for running through instances */
    octObject *instPtr;		/* instance whose master should go in table */
    int genStatus;		/* return status from generator */
    masterEntry *masterEntPtr;	/* new masterEntry structure */
    char **slotPtr;		/* place to put masterEntPtr in hash table */
    instTrans *transPtr;	/* new instTrans structure */

    framePtr->masters = st_init_table(masterCmp, masterHash) ;
    framePtr->instances = st_init_table(octIdCmp, octIdHash);

    /* Generate through the instances, adding each into masterTable */
    OCT_ASSERT(octInitGenContents(&framePtr->inFacet, OCT_INSTANCE_MASK,
							&instanceGen));
    instPtr = ALLOC(octObject, 1);
    while ((genStatus = octGenerate(&instanceGen, instPtr)) == OCT_OK) {
        octId *id = ALLOC(octId, 1);
	transPtr = ALLOC(instTrans, 1);
	transPtr->transform = instPtr->contents.instance.transform;
	transPtr->includedUngrown = FALSE;
        *id = instPtr->objectId;
	if (st_insert(framePtr->instances, (char *) id, (char *) transPtr)) {
	    message(VULCAN_SEVERE,
			"cell %f has multiply attached instance `%s'\n",
			&framePtr->inFacet, instPtr->contents.instance.name);
	}

	if (st_find_or_add(framePtr->masters, (char *) instPtr, &slotPtr)) {
	    masterEntPtr = (masterEntry *) *slotPtr;
	} else {
	    masterEntPtr = ALLOC(masterEntry, 1);
	    *slotPtr = (char *) masterEntPtr;
	    masterEntPtr->master = getMasterFrame(framePtr, instPtr);
	    masterEntPtr->transList = NIL(instTrans);
	    instPtr = ALLOC(octObject, 1);
	}

	if (framePtr->flagWord & VULCAN_CUT_BACK_FRAMES) {
	    addTransformedGeoList(&masterEntPtr->master->frameGeos,
				&transPtr->transform, &framePtr->frameGeos);
	    addTransformedGeoList(&masterEntPtr->master->cutGeos,
				&transPtr->transform, &framePtr->frameGeos);
	    transPtr->includedUngrown = TRUE;
	}
	transPtr->masterEntPtr = masterEntPtr;
	transPtr->next = masterEntPtr->transList;
	masterEntPtr->transList = transPtr;
    }
    OCT_ASSERT(genStatus);
    FREE(instPtr);
}

static frameDesc *getMasterFrame(parentFramePtr, instPtr)
frameDesc *parentFramePtr;	/* instantiating cell's (partial) frame */
octObject *instPtr;		/* instance we want frame for */
{
    octObject doneMaster;	/* possibly existing interface of master */
    octStatus stat;		/* status of doneMaster open */
    octId doneId;		/* completed frame facet's oct id */
    char *dummyPtr;		/* for st_lookup */
    frameDesc *framePtr;	/* frame for the master of *instPtr */

    doneMaster.type = OCT_FACET;
    doneMaster.contents.facet.facet =
			    parentFramePtr->outFacet.contents.facet.facet;
    doneMaster.contents.facet.mode = "r";
    stat = octOpenMaster(instPtr, &doneMaster);
    if (stat == OCT_ALREADY_OPEN &&
	    st_lookup(framedFacets, (char *) &doneMaster.objectId, &dummyPtr)) {
	OCT_ASSERT(octFreeFacet(&doneMaster));
	return((frameDesc *) dummyPtr);
    }

    framePtr = ALLOC(frameDesc, 1);
    *framePtr = *parentFramePtr;	/* default to copy of parent */
    framePtr->flagWord |= VULCAN_CLOSE_INPUT;

    if (parentFramePtr->flagWord & VULCAN_DO_SUBCELLS) {
	doFrameMaster("Creating %f\n", framePtr, instPtr, parentFramePtr);
	doneId = framePtr->outFacet.objectId;
	/* if (stat >= OCT_OK) OCT_ASSERT(octFreeFacet(&doneMaster)); */
    } else {
	if (stat >= OCT_OK) {
	    framePtr->inFacet = doneMaster;
	    framePtr->outFacet = framePtr->inFacet;	/* for error messages */
	    framePtr->outFacet.objectId = oct_null_id;
	    framePtr->flagWord &= ~VULCAN_CLOSE_INPUT;
	    doFrame(framePtr);
	    doneId = doneMaster.objectId;
	} else {
	    doFrameMaster("Can't find %f -- creating it\n",
					framePtr, instPtr, parentFramePtr);
	    doneId = framePtr->outFacet.objectId;
	}
    }

    if (!octIdIsNull(doneId)) {
	octId *id = ALLOC(octId, 1);
	*id = doneId;
	ASSERT( ! st_insert(framedFacets, (char *) id, (char *) framePtr),
			"redid a frame already completed");
    }

    return(framePtr);
}

static void doFrameMaster(userStr, framePtr, instPtr, parentFramePtr)
char *userStr;			/* user message */
frameDesc *framePtr;		/* frame structure to be filled in */
octObject *instPtr;		/* instance of master to be framed */
frameDesc *parentFramePtr;	/* instantiating cell's frame */
{
    framePtr->frameGeos = HAR_EMPTY_LIST;
    framePtr->cutGeos = HAR_EMPTY_LIST;
    framePtr->terminals = NIL(termDesc);
    framePtr->masters = NIL(st_table);
    framePtr->instances = NIL(st_table);
    framePtr->inFacet.objectId = oct_null_id;
    framePtr->outFacet.objectId = oct_null_id;

    /* tell people we're creating subcell interface facet */
    framePtr->inFacet.contents.facet.cell =
		framePtr->outFacet.contents.facet.cell =
		instPtr->contents.instance.master;
    framePtr->inFacet.contents.facet.view =
		framePtr->outFacet.contents.facet.view =
		instPtr->contents.instance.view;
    framePtr->inFacet.contents.facet.version =
		framePtr->outFacet.contents.facet.version =
		instPtr->contents.instance.version;
    message(VULCAN_INFORMATION, userStr, &framePtr->outFacet);

    framePtr->inFacet.contents.facet.mode = "r";
    if (octOpenMaster(instPtr, &framePtr->inFacet) < OCT_OK) {
	message(VULCAN_SEVERE, "can't find %f, instantiated by %f (%s)\n",
			&framePtr->inFacet, &parentFramePtr->inFacet,
			octErrorString());
    } else {
	framePtr->outFacet.contents.facet.mode =
		    (framePtr->flagWord & VULCAN_DESTROY_OLD) ? "w!" : "a!";
	if (octOpenRelative(&framePtr->inFacet, &framePtr->outFacet, OCT_SIBLING) < OCT_OK) {
/*XXX		     && octOpenFacet(&framePtr->outFacet) < OCT_OK)*/
	    message(VULCAN_WARNING,
			"can't write %f -- other facets should be OK\n",
			&framePtr->outFacet);
	    framePtr->outFacet.objectId = oct_null_id;
	}
	doFrame(framePtr);
    }
}

/*
*	This routine puts the properties required by the symbolic
*  policy into the interface (output) facet.  These properties are:
*
*	VIEWTYPE	(copied from contents)
*	TECHNOLOGY	(copied from contents)
*	CELLCLASS	"MODULE" if contents has instances; else "LEAF"
*	EDITSTYLE	"PHYSICAL"
*/
static void setProps(framePtr)
frameDesc *framePtr;		/* frame structure to be filled in */
{
    octObject prop;		/* the property being added to ouputFacet */

    if (ohGetByPropName(&framePtr->inFacet, &prop, "VIEWTYPE") == OCT_OK) {
	OCT_ASSERT(octCreateOrModify(&framePtr->outFacet, &prop));
    } else {
	message(VULCAN_WARNING, "`%s' property not found in %f\n",
			prop.contents.prop.name, &framePtr->inFacet);
    }

    if (ohGetByPropName(&framePtr->inFacet, &prop, "TECHNOLOGY") == OCT_OK) {
	OCT_ASSERT(octCreateOrModify(&framePtr->outFacet, &prop));
    } else {
	message(VULCAN_WARNING, "`%s' property not found in %f\n",
			prop.contents.prop.name, &framePtr->inFacet);
    }

    OCT_ASSERT(ohCreateOrModifyPropStr(&framePtr->outFacet, &prop,
	    "CELLCLASS", st_count(framePtr->instances) ? "MODULE" : "LEAF"));

    OCT_ASSERT(ohCreateOrModifyPropStr(&framePtr->outFacet, &prop,
	    "EDITSTYLE", "PHYSICAL"));
}

    /* Copy formal terminals to output and collect all terminal geometry */
static void collectFormalTerminals(framePtr)
frameDesc *framePtr;		/* frame structure to be filled in */
{
    octObject bag;		/* the JOINEDTERMS bag */
    octGenerator gen;		/* terminal generator */
    int genStatus;		/* status returned by generator */
    octObject terminal;		/* generated terminal */

    /* wipe out old JOINED stuff */
    if (!octIdIsNull(framePtr->outFacet.objectId)) {
	if (ohGetByBagName(&framePtr->outFacet,&bag,BIG_JOINED_NAME)==OCT_OK) {
	    ohRecursiveDelete(&bag, OCT_BAG_MASK|OCT_PROP_MASK);
	}
    }

    /* Generate through the terminals, calling handleFormalTerminal on each */
    OCT_ASSERT(octInitGenContents(&framePtr->inFacet, OCT_TERM_MASK, &gen));
    while ((genStatus = octGenerate(&gen, &terminal)) == OCT_OK) {
	handleFormalTerminal(&terminal, framePtr);
    }
    OCT_ASSERT(genStatus);
}

static void handleFormalTerminal(termPtr, framePtr)
octObject *termPtr;		/* formal terminal under consideration */
frameDesc *framePtr;		/* frame structure to be filled in */
{
    octObject newTerm;		/* new formal terminal for output facet */
    octGenerator implGen;	/* terminal implementation generator */
    int genStatus;		/* status returned by generator */
    octObject impl;		/* generated oct geometric object */
    termDesc *termDescPtr;	/* description of terminal implementation */
    harList termGeo;		/* steps through terminal geo list */
    octObject tempBag;		/* bag holding per-layer implementing boxes */
    octObject layer;		/* layer of implementing geometry */

    termDescPtr = ALLOC(termDesc, 1);
    termDescPtr->next = framePtr->terminals;
    framePtr->terminals = termDescPtr;
    termDescPtr->name = util_strsav(termPtr->contents.term.name);
    termDescPtr->implementation = HAR_EMPTY_LIST;

    /* Collect terminal implementation geometry into *termDescPtr, and
     *   include it in framePtr->cutGeos.
     */
    collectTerminalGeo(termPtr, termDescPtr, framePtr);
    addTransformedGeoList(&termDescPtr->implementation, HAR_NO_TRANSFORM,
							&framePtr->cutGeos);

    if (octIdIsNull(framePtr->outFacet.objectId)) return;

    getEquivTerm(termPtr, &newTerm, &framePtr->outFacet);

    /* Generate through the previous implementations of newTerm, deleting
     *   each.
     */
    OCT_ASSERT(octInitGenContents(&newTerm, OCT_PHYSGEO_MASK, &implGen));
    while ((genStatus = octGenerate(&implGen, &impl)) == OCT_OK) {
	OCT_ASSERT(octDelete(&impl));
    }
    OCT_ASSERT(genStatus);

    /* Boxify the terminal implementation geometry, and attach the boxes
     *   to the appropriate layers and the terminal in the output facet.
     */
    tempBag.type = OCT_BAG;
    tempBag.contents.bag.name = "vulcan-temporary";
    for (termGeo = termDescPtr->implementation; termGeo != HAR_EMPTY_LIST;
				    termGeo = harRestOfList(termGeo)) {
	if (fa_count(harFirstGeo(termGeo)) > 0) {
	    OCT_ASSERT(octCreate(&framePtr->outFacet, &tempBag));
	    harFangToContents(harFirstGeo(termGeo), &tempBag, HAR_UNFILLED_BOX);
	    layer.type = OCT_LAYER;
	    layer.contents.layer.name = harFirstName(termGeo);
	    OCT_ASSERT(octGetOrCreate(&framePtr->outFacet, &layer));
	    OCT_ASSERT(octInitGenContents(&tempBag, OCT_ALL_MASK, &implGen));
	    while ((genStatus = octGenerate(&implGen, &impl)) == OCT_OK) {
		OCT_ASSERT(octAttach(&layer, &impl));
		OCT_ASSERT(octAttach(&newTerm, &impl));
	    }
	    OCT_ASSERT(genStatus);
	    OCT_ASSERT(octDelete(&tempBag));
	}
    }

    handleTermConnectivity(termPtr, &newTerm, &framePtr->outFacet);
}

/*
*	This routine finds the terminal in *facetPtr with the same name
*  as *oldTermPtr, and puts it into *newTermPtr.  It bitches and exits
*  if it can't find the terminal.
*/
static void getEquivTerm(oldTermPtr, newTermPtr, facetPtr)
octObject *oldTermPtr;
octObject *newTermPtr;
octObject *facetPtr;
{
    if (ohGetByTermName(facetPtr, newTermPtr, oldTermPtr->contents.term.name)
								!= OCT_OK) {
	errRaise(VULCAN_PKG_NAME, 0,
"the facet `%s:%s:%s' is missing the terminal `%s'.  It\n\
\tmust be recreated to fix this.  If there are any properties on that\n\
\tfacet you want to save, write them down on paper, rerun vulcan with\n\
\tthe -k option, and add the properties back in by hand.",
		facetPtr->contents.facet.cell,
		facetPtr->contents.facet.view,
		facetPtr->contents.facet.facet,
		newTermPtr->contents.term.name);
    }

}

/*
*	Get all the implementation for *termPtr, and put it into the
*  harGeoList in *termDescPtr.  *framePtr gives us the subcell info we need.
*/
static void collectTerminalGeo(termPtr, termDescPtr, framePtr)
octObject *termPtr;		/* formal terminal in contents facet */
termDesc *termDescPtr;		/* description of terminal implementation */
frameDesc *framePtr;		/* frame structure to be filled in */
{
    octGenerator gen;		/* for implementation geos and aterms */
    octStatus genStatus;	/* generator status */
    octObject implGeo;		/* implementing geo or aterm */
    octObject layer;		/* layer of implementing geo */

    OCT_ASSERT(octInitGenContents(termPtr,OCT_PHYSGEO_MASK|OCT_TERM_MASK,&gen));
    while ((genStatus = octGenerate(&gen, &implGeo)) == OCT_OK) {
	if (implGeo.type == OCT_TERM) {
	    /* extract geometry from subcell's description */
	    handlePromotedTerminal(&implGeo, termDescPtr, framePtr);
	} else {
	    /* include this piece of geometry directly */
	    if (octGenFirstContainer(&implGeo, OCT_LAYER_MASK, &layer)
								== OCT_OK) {
		harAddObjectToFang(&implGeo,
				    harGetNamedGeo(layer.contents.layer.name,
						&termDescPtr->implementation),
				    HAR_NO_TRANSFORM);
	    } else {
		message(VULCAN_SEVERE,
		    "terminal `%s' in %f has implementing geo not on a layer\n",
		    termPtr->contents.term.name, &framePtr->inFacet);
	    }
	}
    }
    OCT_ASSERT(genStatus);
}

static void handlePromotedTerminal(actualPtr, termDescPtr, framePtr)
octObject *actualPtr;		/* actual terminal that implements formal */
termDesc *termDescPtr;		/* formal terminal being implemented */
frameDesc *framePtr;		/* frame structure to be filled in */
{
    char *dummyPtr;		/* dummy pointer for st_lookup */
    instTrans *transPtr;	/* transform of implementing instance */
    frameDesc *masterFramePtr;	/* frame struct of implementing master */
    termDesc *actTerm;		/* master's terminal implementation */

    /* find the instance transform structure for the actual terminal */
    if ( ! st_lookup(framePtr->instances,
		    (char *) &actualPtr->contents.term.instanceId, &dummyPtr)) {
	message(VULCAN_SEVERE,
	    "instance implementing terminal `%s' in %f not attached to facet\n",
	    actualPtr->contents.term.name, &framePtr->inFacet);
	return;
    }
    transPtr = (instTrans *) dummyPtr;
    masterFramePtr = transPtr->masterEntPtr->master;

    /* find the implementation of this terminal from the master */
    for (actTerm=masterFramePtr->terminals; actTerm; actTerm = actTerm->next) {
	if (strcmp(actTerm->name, actualPtr->contents.term.name) == 0) break;
    }
    if (actTerm == NIL(termDesc)) {
	message(VULCAN_SEVERE,
"an instance of cell %f in %f has terminal `%s', but the master doesn't\n",
		&masterFramePtr->inFacet, &framePtr->inFacet, actTerm->name);
	return;
    }

    addTransformedGeoList(&actTerm->implementation, &transPtr->transform,
						&termDescPtr->implementation);
    
    if ( ! transPtr->includedUngrown) {
	addTransformedGeoList(&masterFramePtr->frameGeos, &transPtr->transform,
							&framePtr->frameGeos);
	addTransformedGeoList(&masterFramePtr->cutGeos, &transPtr->transform,
							&framePtr->frameGeos);
	transPtr->includedUngrown = TRUE;
    }
}

static void handleTermConnectivity(oldTermPtr, newTermPtr, outputFacetPtr)
octObject *oldTermPtr;
octObject *newTermPtr;
octObject *outputFacetPtr;
{
    octObject bag;	/* to look for containing bags */
    octGenerator gen;
    octStatus status;
    octObject net;	/* net that contains oldTermPtr */

    /* check if this terminal's connectivity has been noticed already */
    OCT_ASSERT(octInitGenContainers(newTermPtr, OCT_BAG_MASK, &gen));
    while ((status = octGenerate(&gen, &bag)) == OCT_OK) {
	if (strcmp(bag.contents.bag.name, JOINED_NAME) == 0 ||
		    strcmp(bag.contents.bag.name, ALIASED_NAME) == 0) {
	    /* this terminal has been handled already */
	    OCT_ASSERT(octFreeGenerator(&gen));
	    return;
	}
    }
    OCT_ASSERT(status);

    /* collect connectivity info for this terminal's net or connected group */
    if (octGenFirstContainer(oldTermPtr, OCT_NET_MASK, &net) == OCT_OK) {
	OCT_ASSERT(octInitGenContainers(&net, OCT_BAG_MASK, &gen));
	while ((status = octGenerate(&gen, &bag)) == OCT_OK) {
	    if (strcmp(bag.contents.bag.name, ALIASED_NET_NAME) == 0) {
		OCT_ASSERT(octFreeGenerator(&gen));
		handleNetGroup(&bag, outputFacetPtr);
		return;
	    }
	}
	OCT_ASSERT(status);
	(void) handleNet(&net, outputFacetPtr, NIL(octObject));
    }
}

static void handleNetGroup(netBagPtr, facetPtr)
octObject *netBagPtr;		/* bag of connected nets */
octObject *facetPtr;		/* output facet to put connectivity info */
{
    octObject bigBag;		/* facetPtr's JOINEDTERMS bag */
    octObject smallBag;		/* CONNECTED bag for these terminals */
    octObject net;		/* a net in group of connected nets */
    octObject conObj, oConObj;	/* objects that go into CONNECTED bag */
    int aliasCount;		/* number of nets with formal terms in group */
    octGenerator gen;
    octStatus status;

    aliasCount = 0;
    OCT_ASSERT(octInitGenContents(netBagPtr, OCT_NET_MASK, &gen));
    while ((status = octGenerate(&gen, &net)) == OCT_OK) {
	if (handleNet(&net, facetPtr, &conObj) > 0) {
	    switch (++aliasCount) {
		case 1:		/* save until we have a second CONNECTED obj */
		    oConObj = conObj;
		    break;
		case 2:		/* we have 2 -- make bags and attach */
		    bigBag.type = OCT_BAG;
		    bigBag.contents.bag.name = BIG_JOINED_NAME;
		    OCT_ASSERT(octGetOrCreate(facetPtr, &bigBag));

		    smallBag.type = OCT_BAG;
		    smallBag.contents.bag.name = ALIASED_NAME;
		    OCT_ASSERT(octCreate(&bigBag, &smallBag));

		    OCT_ASSERT(octAttach(&smallBag, &oConObj));
		    /* fall through */
		default:	/* attach additional CONNECTED object */
		    OCT_ASSERT(octAttach(&smallBag, &conObj));
	    }
	}
    }
    OCT_ASSERT(status);
}

static int handleNet(netPtr, facetPtr, objPtr)
octObject *netPtr;		/* net in input facet */
octObject *facetPtr;		/* output facet to put connectivity info */
octObject *objPtr;		/* NIL or place to copy term or JOINED bag */
{
    octObject bigBag;		/* facetPtr's JOINEDTERMS bag */
    octObject smallBag;		/* JOINED bag for these terminals */
    octObject oldTerm, newTerm;	/* a terminal in the input and output facets */
    int joinCount;		/* number of formal terms in netPtr */
    octGenerator gen;
    octStatus status;

    joinCount = 0;
    OCT_ASSERT(octInitGenContents(netPtr, OCT_TERM_MASK, &gen));
    while ((status = octGenerate(&gen, &oldTerm)) == OCT_OK) {
	if (octIdIsNull(oldTerm.contents.term.instanceId)) {
	    /* it's a formal terminal */
	    switch (++joinCount) {
		case 1:		/* save until we have second JOINED terminal */
		    getEquivTerm(&oldTerm, &newTerm, facetPtr);
		    break;
		case 2:		/* we have 2 terms -- make bags and attach */
		    bigBag.type = OCT_BAG;
		    bigBag.contents.bag.name = BIG_JOINED_NAME;
		    OCT_ASSERT(octGetOrCreate(facetPtr, &bigBag));

		    smallBag.type = OCT_BAG;
		    smallBag.contents.bag.name = JOINED_NAME;
		    OCT_ASSERT(octCreate(&bigBag, &smallBag));

		    OCT_ASSERT(octAttach(&smallBag, &newTerm));

		    /* fall through */
		default:	/* attach additional JOINED terminal */
		    getEquivTerm(&oldTerm, &newTerm, facetPtr);
		    OCT_ASSERT(octAttach(&smallBag, &newTerm));
	    }
	}
    }
    OCT_ASSERT(status);

    if (objPtr != NIL(octObject)) {
	if (joinCount == 1) {
	    *objPtr = newTerm;
	} else if (joinCount > 1) {
	    *objPtr = smallBag;
	}
    }

    return(joinCount);
}

static void doProtectionFrames(framePtr)
frameDesc *framePtr;		/* frame structure to be filled in */
{
    st_generator *gen;		/* generates through master table */
    char *key, *value;		/* dummy pointers for generator */
    harList geoList;		/* grown geometry of a subcell */
    int fracBasedGrow, techBasedGrow, grow;	/* amounts of grow/shrink */
    octObject techFacet;	/* cell's technology facet */
    fa_geometry newGeo;
    char *pkg, *msg;
    int code;

    /* collect geos in input facet, with placeholders for layers in subcells */
    harAddFacetToFangList(&framePtr->inFacet, &framePtr->frameGeos,
							HAR_NO_TRANSFORM);
    if ( ! (framePtr->flagWord & VULCAN_CUT_BACK_FRAMES)) {
	andNotGeoList(&framePtr->frameGeos, &framePtr->cutGeos);
	gen = st_init_gen(framePtr->masters);
	while (st_gen(gen, &key, &value)) {
	    for (	geoList = ((masterEntry *) value)->master->frameGeos;
			geoList != HAR_EMPTY_LIST;
			geoList = harRestOfList(geoList)) {
		(void) harGetNamedGeo(harFirstName(geoList),
					    &framePtr->frameGeos);
	    }
	}
	st_free_gen(gen);
    }

    fracBasedGrow = framePtr->fracGrowth * getSize(&framePtr->inFacet);

    /* get the technology facet */
    ERR_IGNORE(techFacet.objectId = tapGetFacetIdFromObj(&framePtr->inFacet));
    if (errStatus(&pkg, &code, &msg)) {		/* tap can't find tech facet */
	message(VULCAN_WARNING,
	    "no technology for %f (%s:%s)\n\tusing conservative design rules\n",
	    &framePtr->inFacet, pkg, msg);
	techFacet.objectId = oct_null_id;
    } else {
	OCT_ASSERT(octGetById(&techFacet));
    }

    for (	geoList = framePtr->frameGeos;
		geoList != HAR_EMPTY_LIST;
		geoList = harRestOfList(geoList)) {
	techBasedGrow = getTechBasedGrowth(framePtr, &techFacet,
							harFirstName(geoList));
	fa_init(&newGeo);
	for (grow = fracBasedGrow; ; grow /= 2) {
	    if (grow < techBasedGrow) grow = techBasedGrow;
	    if (framePtr->flagWord & VULCAN_CUT_BACK_FRAMES) {
		if (cutbackMakeLayerFrame(framePtr, geoList, grow,
					    techBasedGrow, &newGeo)) break;
	    } else {
		if (makeLayerFrame(framePtr, geoList, grow, &newGeo)) break;
	    }
	    if (grow == techBasedGrow) {
		reportUnreachableTerm(framePtr, harFirstName(geoList), &newGeo);
		break;
	    }
	    fa_free_geo(&newGeo);
	}
	fa_free_geo(harFirstGeo(geoList));
	*harFirstGeo(geoList) = newGeo;
    }

    /* clean master and instance tables */
    gen = st_init_gen(framePtr->masters);
    while (st_gen(gen, &key, &value)) {
	FREE(key);
	FREE(value);
    }
    st_free_table(framePtr->masters);
    framePtr->masters = NIL(st_table);

    gen = st_init_gen(framePtr->instances);
    while (st_gen(gen, &key, &value)) {
	FREE(value);
    }
    st_free_table(framePtr->instances);
    framePtr->instances = NIL(st_table);
}

static int getSize(objPtr)
octObject *objPtr;		/* object whose size is of interest */
{
    struct octBox bb;		/* object's bounding box */
    octStatus status;

    status = octBB(objPtr, &bb);
    if (status == OCT_NO_BB) {
	return(0);
    } else {
	OCT_ASSERT(status);
	return(MAX((bb.upperRight.x - bb.lowerLeft.x),
		    (bb.upperRight.y - bb.lowerLeft.y)));
    }
}

static int getTechBasedGrowth(framePtr, techFacetPtr, layerName)
frameDesc *framePtr;		/* frame structure to be filled in */
octObject *techFacetPtr;	/* technology facet for input facet */
char *layerName;		/* layer whose growth is being determined */
{
    octObject layer;		/* layer in technology facet */
    int width, spacing;		/* design rules for `layer' */

    if (strcmp(layerName, GLOBAL_LAYER_NAME) == 0) {
	return(getSize(&framePtr->inFacet));
    }
    if (octIdIsNull(techFacetPtr->objectId)) return(0);
    layer.type = OCT_LAYER;
    layer.contents.layer.name = layerName;
    if (octGetByName(techFacetPtr, &layer) == OCT_OK) {
	width = tapGetMinWidth(&layer);
	if (width == TAP_NO_RULE) width = 0;
	spacing = tapGetMinSpacing(&layer, &layer);
	if (spacing == TAP_NO_RULE) spacing = 0;
	return((width-1) / 2 + spacing);
    } else {
	message(VULCAN_SEVERE, "layer `%s' not in technology for %f\n",
			    layerName, &framePtr->inFacet);
	return(0);
    }
}

static int makeLayerFrame(framePtr, geoList, grow, newGeoPtr)
frameDesc *framePtr;		/* frame structure to be filled in */
harList geoList;		/* has layer name and some geometry */
int grow;			/* amount to grow/shrink */
fa_geometry *newGeoPtr;		/* place to put frame */
{
    char *layerName;		/* name of this layer */
    st_generator *gen;		/* generates through master table */
    char *key, *value;		/* dummy pointers for generator */
    frameDesc *masterFramePtr;	/* master's frame structure */
    fa_geometry geo1, geo2;	/* temporary geometry */
    fa_geometry *geoPtr;	/* temporary geometry */
    instTrans *transPtr;	/* steps through instance transforms */
    int status;			/* did we succeed? */
    fa_geometry *getExistingNamedGeo();

    layerName = harFirstName(geoList);
    fa_grow(harFirstGeo(geoList), grow, newGeoPtr);

    gen = st_init_gen(framePtr->masters);
    while (st_gen(gen, &key, &value)) {
	masterFramePtr = ((masterEntry *) value)->master;
	fa_init(&geo1);
	if ( (geoPtr = getExistingNamedGeo(layerName, 
					   &masterFramePtr->frameGeos))){
	    fa_add_geo(&geo1, geoPtr);
	}
	if (( geoPtr = getExistingNamedGeo(layerName, 
					   &masterFramePtr->cutGeos))) {
	    fa_add_geo(&geo1, geoPtr);
	}
	if (fa_count(&geo1) > 0) {
	    fa_grow(&geo1, grow, &geo2);
	    for (	transPtr = ((masterEntry *) value)->transList;
			transPtr;
			transPtr = transPtr->next) {
		if ( ! transPtr->includedUngrown) {
		    harAddTransformedFang(&geo2,&transPtr->transform,newGeoPtr);
		}
	    }
	    fa_free_geo(&geo2);
	}
	fa_free_geo(&geo1);
    }
    st_free_gen(gen);

    fa_grow(newGeoPtr, -grow, &geo1);
    fa_free_geo(newGeoPtr);
    if ( (geoPtr = getExistingNamedGeo(layerName, &framePtr->cutGeos)) ) {
	fa_andnot(&geo1, geoPtr, newGeoPtr);
	fa_free_geo(&geo1);

	/* check that none of the terminals are enclosed */
	fa_fill(newGeoPtr, &geo1);
	status = ! fa_intersect_p(&geo1, geoPtr);
	fa_free_geo(&geo1);
	return(status);
    } else {
	*newGeoPtr = geo1;
	return(1);
    }
}

static int cutbackMakeLayerFrame(framePtr, geoList, grow, techGrow, newGeoPtr)
frameDesc *framePtr;		/* frame structure to be filled in */
harList geoList;		/* has layer name and some geometry */
int grow;			/* amount to grow/shrink */
int techGrow;			/* amount to do precisely around terminals */
fa_geometry *newGeoPtr;		/* place to put frame */
{
    fa_geometry *termGeoPtr;	/* terminal geometry */
    fa_geometry grownTermGeo;	/* terminal geometry grown by `techGrow' */
    fa_geometry geo1, geo2;	/* temporary geometry */
    int status;			/* did we succeed? */

    termGeoPtr = getExistingNamedGeo(harFirstName(geoList), &framePtr->cutGeos);
    if (termGeoPtr) {
	/* build basic frame */
	fa_grow(termGeoPtr, techGrow, &grownTermGeo);
	fa_andnot(harFirstGeo(geoList), &grownTermGeo, &geo1);
	fa_frame(&geo1, grow, grow, &geo2);
	fa_free_geo(&geo1);

	/* cut out around terminals */
	fa_andnot(&geo2, &grownTermGeo, &geo1);
	fa_free_geo(&geo2);
	fa_free_geo(&grownTermGeo);

	/* add in exact geometry around terminals */
	fa_add_geo(&geo1, harFirstGeo(geoList));
	fa_andnot(&geo1, termGeoPtr, newGeoPtr);
	fa_free_geo(&geo1);

	/* check that none of the terminals are enclosed */
	fa_fill(newGeoPtr, &geo1);
	status = ! fa_intersect_p(&geo1, termGeoPtr);
	fa_free_geo(&geo1);
	return(status);
    } else {
	fa_frame(harFirstGeo(geoList), grow, grow, newGeoPtr);
	return(1);
    }
}

static void reportUnreachableTerm(framePtr, layerName, frameGeoPtr)
frameDesc *framePtr;		/* frame structure we're working on */
char *layerName;		/* layer that unreachable term is on */
fa_geometry *frameGeoPtr;	/* protection frame for layer */
{
    fa_geometry geo1, geo2, *termGeoPtr;
    termDesc *termPtr;

    fa_fill(frameGeoPtr, &geo1);
    fa_and(&geo1, getExistingNamedGeo(layerName, &framePtr->cutGeos), &geo2);
    message(VULCAN_WARNING,
	"some terminal implementation(s) on layer `%s' of %f are unreachable\n",
	layerName, &framePtr->outFacet);
    for (termPtr = framePtr->terminals; termPtr; termPtr = termPtr->next) {
	termGeoPtr = getExistingNamedGeo(layerName, &termPtr->implementation);
	if (termGeoPtr && fa_intersect_p(termGeoPtr, &geo2)) {
	    message(VULCAN_WARNING,
		"terminal `%s' of %f is unreachable on layer `%s'\n",
		termPtr->name, &framePtr->inFacet, layerName);
	}
    }
}

static fa_geometry *getExistingNamedGeo(name, geoListPtr)
char *name;
harList *geoListPtr;
{
    harList geoList;
    
    for (	geoList = *geoListPtr;
		geoList != HAR_EMPTY_LIST;
		geoList = harRestOfList(geoList)) {
	if (strcmp(harFirstName(geoList), name) == 0) {
	    return(harFirstGeo(geoList));
	}
    }

    return(NIL(fa_geometry));
}

static void addTransformedGeoList(geoListPtr, transformPtr, newGeoListPtr)
harList *geoListPtr;
struct octTransform *transformPtr;
harList *newGeoListPtr;
{
    harList geoList = *geoListPtr;

    while (geoList != HAR_EMPTY_LIST) {
	if (fa_count(harFirstGeo(geoList)) > 0) {
	    harAddTransformedFang(harFirstGeo(geoList),
			transformPtr,
			harGetNamedGeo(harFirstName(geoList), newGeoListPtr));
	}
	geoList = harRestOfList(geoList);
    }
}

static void andNotGeoList(geoListPtr, cutGeoListPtr)
harList *geoListPtr, *cutGeoListPtr;
{
    harList geoList = *cutGeoListPtr;
    fa_geometry *geoPtr, tempGeo;

    while (geoList != HAR_EMPTY_LIST) {
	geoPtr = harGetNamedGeo(harFirstName(geoList), geoListPtr);
	fa_andnot(geoPtr, harFirstGeo(geoList), &tempGeo);
	fa_free_geo(geoPtr);
	*geoPtr = tempGeo;
	geoList = harRestOfList(geoList);
    }
}

    /* Create global frame on PLACE layer */
static void makeGlobalFrame(framePtr)
frameDesc *framePtr;		/* frame structure to be filled in */
{
    harList frameList;		/* steps through frame geo list */
    fa_geometry *globalFramePtr;/* pointer to collection of all geometry */
    fa_geometry tempGlobal;	/* temporary geo for collecting global frame */
    struct octBox boundingBox;	/* facet's bounding box */
    int maxDim;			/* longest dimension of facet's bounding box */
    octStatus status;

    globalFramePtr = harGetNamedGeo(GLOBAL_LAYER_NAME, &framePtr->frameGeos);

    /* add all layers' protection frames */
    for (	frameList = framePtr->frameGeos;
   		frameList != HAR_EMPTY_LIST;
		frameList = harRestOfList(frameList)) {
	if (strcmp(harFirstName(frameList), GLOBAL_LAYER_NAME) != 0) {
	    fa_add_geo(globalFramePtr, harFirstGeo(frameList));
	}
    }

    /* add all layers' terminal frames */
    for (	frameList = framePtr->cutGeos;
   		frameList != HAR_EMPTY_LIST;
		frameList = harRestOfList(frameList)) {
	fa_add_geo(globalFramePtr, harFirstGeo(frameList));
    }

    status = octBB(&framePtr->inFacet, &boundingBox);
    if (status != OCT_NO_BB) {
	OCT_ASSERT(status);
	maxDim = MAX((boundingBox.upperRight.x - boundingBox.lowerLeft.x),
		     (boundingBox.upperRight.y - boundingBox.lowerLeft.y));
	fa_frame(globalFramePtr, maxDim, maxDim, &tempGlobal);
	fa_free_geo(globalFramePtr);
	*globalFramePtr = tempGlobal;
    }

    debugDumpFrame(framePtr);
}

static void buildFrames(framePtr)
frameDesc *framePtr;		/* frame structure to be filled in */
{
    octGenerator layerGen;	/* layer generator */
    int layerGenStatus;		/* status returned by layer generator */
    octObject layer;		/* generated layer */
    octGenerator geoGen;	/* geometry generator */
    int geoGenStatus;		/* status returned by geometry generator */
    octObject geo;		/* generated geometry */
    octObject terminal;		/* generated terminal */
    int nGlobalGeos = 0;	/* number of geos in global protection frame */

    /* Remove previous protection frames */
    OCT_ASSERT(octInitGenContents(&framePtr->outFacet, OCT_GEO_MASK, &geoGen));
    while ((geoGenStatus = octGenerate(&geoGen, &geo)) == OCT_OK) {
	OCT_ASSERT(octDelete(&geo));
    }
    layer.type = OCT_LAYER;
    layer.contents.layer.name = GLOBAL_LAYER_NAME;
    if (octGetByName(&framePtr->outFacet, &layer) == OCT_OK) {
	OCT_ASSERT(octInitGenContents(&layer, OCT_GEO_MASK, &geoGen));
	while ((geoGenStatus = octGenerate(&geoGen, &geo)) == OCT_OK) {
	    OCT_ASSERT(octDelete(&geo));
	}
    }

    /* Build frame geometry in facet */
    harFangListToFacet(&framePtr->frameGeos, &framePtr->outFacet,
						    framePtr->frameGeoType);

    /* Generate the layers, looking for protection frame geometry */
    OCT_ASSERT(octInitGenContents(&framePtr->outFacet, OCT_LAYER_MASK,
								&layerGen));
    while ((layerGenStatus = octGenerate(&layerGen, &layer)) == OCT_OK) {
	/* Generate the geos on layer, looking for ones not attached to terms */
	OCT_ASSERT(octInitGenContents(&layer, framePtr->frameGeoMask, &geoGen));
	while ((geoGenStatus = octGenerate(&geoGen, &geo)) == OCT_OK) {
	    /* If geo not attached to a term, it's a frame -- attach to facet */
	    if (octGenFirstContainer(&geo, OCT_TERM_MASK, &terminal) != OCT_OK){
		if (strcmp(GLOBAL_LAYER_NAME, layer.contents.layer.name)) {
		    OCT_ASSERT(octAttach(&framePtr->outFacet, &geo));
		} else {
		    nGlobalGeos++;
		}
	    }
	}
	OCT_ASSERT(geoGenStatus);
    }
    OCT_ASSERT(layerGenStatus);

    if (nGlobalGeos > 1) {
	message(VULCAN_WARNING, "%d figures on %s frame for cell %f\n",
		    nGlobalGeos, GLOBAL_LAYER_NAME, &framePtr->outFacet);
    }
}

static void freeFrame(framePtr)
frameDesc *framePtr;		/* frame to be freed */
{
    termDesc *termPtr, *nextTermPtr;

    FREE(framePtr->inFacet.contents.facet.cell);
    FREE(framePtr->inFacet.contents.facet.view);
    FREE(framePtr->inFacet.contents.facet.facet);
    FREE(framePtr->inFacet.contents.facet.version);
    harFreeList(&framePtr->frameGeos);
    harFreeList(&framePtr->cutGeos);
    for (termPtr = framePtr->terminals; termPtr; termPtr = nextTermPtr) {
	FREE(termPtr->name);
	harFreeList(&termPtr->implementation);
	nextTermPtr = termPtr->next;
	FREE(termPtr);
    }
}

static void debugDumpFrame(framePtr)
frameDesc *framePtr;		/* frame structure to be filled in */
{
    termDesc *termPtr;		/* walks list of terminal implementations */

    if (framePtr->debug == VULCAN_NO_DEBUG) return;

    message(VULCAN_PARTIAL, "\n%f\n   Frame Geos: ", &framePtr->inFacet);
    debugDumpGeos(framePtr->frameGeos);

    for (termPtr = framePtr->terminals; termPtr; termPtr = termPtr->next) {
	message(VULCAN_PARTIAL, "\n   %s Implementation: ", termPtr->name);
	debugDumpGeos(termPtr->implementation);
    }

    message(VULCAN_DEBUGGING, "\n");
}

static void debugDumpGeos(geoList)
harList geoList;		/* list of named geometries */
{
    while (geoList != HAR_EMPTY_LIST) {
	message(VULCAN_PARTIAL, " %s(%d)", harFirstName(geoList),
					    fa_count(harFirstGeo(geoList)));
	geoList = harRestOfList(geoList);
    }
}

int masterCmp(a,b)
     CONST char *a;
     CONST char *b;
{
  CONST struct octInstance *one = &((CONST octObject *) a)->contents.instance,
 	  	           *two = &((CONST octObject *) b)->contents.instance;


  int retval=0;

	/* SUPPRESS 255 */
    (void)((retval = strcmp(one->master, two->master)) ||
	   (retval = strcmp(one->view, two->view)) ||
	   (retval = strcmp(one->version, two->version)));

    return(retval);
}

static int masterHash(val, mod)
    char *val;
    int mod;
{
    octObject *instPtr = (octObject *) val;

    /* save time -- only hash off the field likely to change */
    return(st_strhash(instPtr->contents.instance.master, mod));
}


static void spillExcessError()
{
    if (errBufPtr >= &errorBuffer[ERR_BUF_SIZE]) {
	(*messageHandler)(VULCAN_PARTIAL, errorBuffer);
	errBufPtr = errorBuffer;
    }
}

static void errAddString(str)
char *str;
{
    while (*str != '\0') {
	spillExcessError();
	*errBufPtr++ = *str++;
    }
}

static void errAddNumber(num)
int num;
{
    (void) sprintf(errBufPtr, "%d", num);
    errBufPtr += strlen(errBufPtr);
}

static void defaultMessageHandler(type, message)
vulcanMessageType type;
char *message;
{
    switch (type) {
	case VULCAN_FATAL:
	    (void) fprintf(stderr, "vulcan library FATAL ERROR: %s", message);
	    break;
	case VULCAN_SEVERE:
	    (void) fprintf(stderr, "vulcan library SEVERE ERROR: %s", message);
	    break;
	case VULCAN_WARNING:
	    (void) fprintf(stderr, "vulcan library WARNING: %s", message);
	    break;
	case VULCAN_INFORMATION:
	    (void) fprintf(stderr, "vulcan library: %s", message);
	    break;
	case VULCAN_DEBUGGING:
	    (void) fprintf(stderr, "vulcan library debug: %s", message);
	    break;
	case VULCAN_PARTIAL:
	    (void) fprintf(stderr, "vulcan library ***LONG MESSAGE***\n%s\n",
					message);
	    break;
    }
    (void) fflush(stderr);
}

/*ARGSUSED*/
static void vulcanHandler(pkgName, code, message)
char *pkgName;
int code;
char *message;
{
    vulcanDepth = 0;
    errPopHandler();
    if (pkgName == VULCAN_PKG_NAME) pkgName = "internal";
    errPass("(function %s) unexpected %s error: %s",
	    vulcanRoutine, pkgName, message);
}
