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
#include "oct.h"
#include "hackterm.h"

static void convertLayer
     ARGS((octObject *facetPtr, octObject *layerPtr));
static int implementTerminal
	ARGS((octObject *facetPtr, octObject *terminalPtr, octObject *labelPtr, octObject *layerPtr));
static int createTerminal
	ARGS((octObject *facetPtr, octObject *terminalPtr, octObject *geoPtr));

static int geoAlreadyImplementsTerm
	ARGS((octObject *geoPtr, char *safeName));

static int differentBaseName
	ARGS((char *termName, char *match));

static void bagTerminals
	ARGS((octObject *facetPtr, octObject *origTermPtr, octObject *newTermPtr));

static int makeBags;

void
hackTerm(facetPtr, joinFlag)
octObject *facetPtr;
int joinFlag;
{
    octGenerator layerGen;	/* generator for running through the layers */
    octObject layer;		/* layer currently being considered */
    int genStatus;		/* return status from generator */

    makeBags = joinFlag;

#if 0
    facetPtr->contents.facet.mode = "a";
    if (octOpenFacet(facetPtr) < OCT_OK) {
	char buffer[100];
	(void) sprintf(buffer, "Can't open cell \"%s:%s:%s\" for appending",
		facetPtr->contents.facet.cell,
		facetPtr->contents.facet.view,
		facetPtr->contents.facet.facet);
	octError(buffer);
	exit(1);
    }
#endif

    /* Generate through the layers, converting labels to terminals */
    if (octInitGenContents(facetPtr, OCT_LAYER_MASK, &layerGen) != OCT_OK) {
	octError("Couldn't start the layer generator");
	exit(1);
    }
    while ((genStatus = octGenerate(&layerGen, &layer)) == OCT_OK) {
	convertLayer(facetPtr, &layer);
    }
    if (genStatus != OCT_GEN_DONE) {
	octError("generating layers");
	exit(1);
    }

#if 0
    if (octCloseFacet(facetPtr) != OCT_OK) {
	char buffer[100];
	(void) sprintf(buffer, "Can't write out cell \"%s:%s:%s\"",
		facetPtr->contents.facet.cell,
		facetPtr->contents.facet.view,
		facetPtr->contents.facet.facet);
	octError(buffer);
	exit(1);
    }
#endif
}

static void convertLayer(facetPtr, layerPtr)
octObject *facetPtr;	/* facet being worked on */
octObject *layerPtr;	/* layer we're looking for labels on */
{
    octGenerator labelGen;	/* generator for labels on this layer */
    octObject label;		/* label currently being considered */
    int genStatus;		/* return status from generator */
    octObject terminal;		/* newly created formal terminal */

    if (octInitGenContents(layerPtr, OCT_LABEL_MASK, &labelGen) != OCT_OK) {
	octError("Couldn't start the label generator");
	exit(1);
    }
    while ((genStatus = octGenerate(&labelGen, &label)) == OCT_OK) {
	terminal.type = OCT_TERM;
	terminal.contents.term.name = label.contents.label.label;
	terminal.contents.term.instanceId = oct_null_id;
	terminal.contents.term.width = 1;
	if (implementTerminal(facetPtr, &terminal, &label, layerPtr)) {
	    if (octDelete(&label) != OCT_OK) {
		octError("deleting label");
		exit(1);
	    }
	} else {
	    (void) fprintf(stderr,
			"Can't find implementation for %s -- retaining label\n",
			label.contents.label.label);
	}
    }
    if (genStatus != OCT_GEN_DONE) {
	octError("generating labels");
	exit(1);
    }
}

static int
implementTerminal(facetPtr, terminalPtr, labelPtr, layerPtr)
octObject *facetPtr;		/* facet we're working on */
octObject *terminalPtr;		/* new formal terminal */
octObject *labelPtr;		/* label that we're replacing */
octObject *layerPtr;		/* layer the label was on */
{
    octGenerator boxGen;	/* generator for running through the boxes */
    octObject box;		/* box currently being examined */
    int genStatus;		/* return status from generator */
    octCoord labelX, labelY;	/* label coordinates */

    /* XXX HACK */
    labelX = labelPtr->contents.label.region.lowerLeft.x;
    labelY = labelPtr->contents.label.region.lowerLeft.y;
    /* Generate through the boxes, checking for one containing the terminal */
    if (octInitGenContents(layerPtr, OCT_BOX_MASK, &boxGen) != OCT_OK) {
	octError("Couldn't start generating boxes looking for terminal");
	exit(1);
    }
    while ((genStatus = octGenerate(&boxGen, &box)) == OCT_OK) {
	if (	box.contents.box.lowerLeft.x <= labelX &&
		box.contents.box.upperRight.x >= labelX &&
		box.contents.box.lowerLeft.y <= labelY &&
		box.contents.box.upperRight.y >= labelY) {
	    return(createTerminal(facetPtr, terminalPtr, &box));
	}
    }
    if (genStatus != OCT_GEN_DONE) {
	octError("generating boxes");
	exit(1);
    }
    return(OCTMM_FALSE);
}


static int createTerminal(facetPtr, terminalPtr, geoPtr)
octObject *facetPtr;	/* facet having terminal added */
octObject *terminalPtr;	/* terminal to be added */
octObject *geoPtr;	/* implementation found for terminal */
{
    octObject origTerm;
    char *origName;
    char *newName;
    int i = 2;

    switch (geoAlreadyImplementsTerm(geoPtr, terminalPtr->contents.term.name)) {
	case OCTMM_TRUE:		/* already done -- tell caller to kill label */
	    return(OCTMM_TRUE);
	case ERROR:		/* gross DRC violation -- retain label */
	    return(OCTMM_FALSE);
    }

    if (octGetByName(facetPtr, terminalPtr) == OCT_OK) {
	origTerm = *terminalPtr;

	origName = terminalPtr->contents.term.name;
	newName = ALLOC(char, strlen(origName)+20);

	/* I can't believe I really do this */
	do {
	    (void) sprintf(newName, "%s%s%d%s",
				origName, SUFFIX_OPEN, i++, SUFFIX_CLOSE);
	    terminalPtr->contents.term.name = newName;
	} while (octGetByName(facetPtr, terminalPtr) == OCT_OK);

	if (octCreate(facetPtr, terminalPtr) != OCT_OK) {
	    octError("creating terminal");
	    exit(1);
	}

	bagTerminals(facetPtr, &origTerm, terminalPtr);
    } else {
	if (octCreate(facetPtr, terminalPtr) != OCT_OK) {
	    octError("creating terminal");
	    exit(1);
	}
    }

    if (octAttach(terminalPtr, geoPtr) != OCT_OK) {
	octError("attaching terminal implementation");
	exit(1);
    }

    return(OCTMM_TRUE);
}

static int geoAlreadyImplementsTerm(geoPtr, safeName)
octObject *geoPtr;	/* geo we're going to check for containing terms */
char *safeName;		/* name that's OK for containing term */
{
    octGenerator gen;		/* for terminals containing *geoPtr */
    octStatus genStatus;	/* returned status from generator */
    octObject terminal;		/* generated terminal */

    if (octInitGenContainers(geoPtr, OCT_TERM_MASK, &gen) != OCT_OK) {
	octError("starting containing term generator");
	exit(1);
    }
    if ((genStatus = octGenerate(&gen, &terminal)) == OCT_GEN_DONE) {
	/* *geoPtr implements no terminals */
	return(OCTMM_FALSE);
    }

    while (genStatus == OCT_OK) {
	if (differentBaseName(terminal.contents.term.name, safeName)) {
	    (void) fprintf(stderr,
			"Terminals %s and %s need same implementation --\n",
			terminal.contents.term.name, safeName);
	    return(ERROR);
	}
	genStatus = octGenerate(&gen, &terminal);
    }
    if (genStatus != OCT_GEN_DONE) {
	octError("generating terminals containing geo");
	exit(1);
    }

    return(OCTMM_TRUE);
}

static int differentBaseName(termName, match)
char *termName;
char *match;
{
    while (*match) {
	if (*termName++ != *match++) return(OCTMM_TRUE);
    }
    if (*termName == '\0') return(OCTMM_FALSE);
    match = SUFFIX_OPEN;
    while (*match) {
	if (*termName++ != *match++) return(OCTMM_TRUE);
    }
    while (isdigit(*termName)) {
	termName++;
    }
    match = SUFFIX_CLOSE;
    while (*match) {
	if (*termName++ != *match++) return(OCTMM_TRUE);
    }
    return(*termName != '\0');
}

static void bagTerminals(facetPtr, origTermPtr, newTermPtr)
octObject *facetPtr;		/* facet whose terminals are being munged */
octObject *origTermPtr;		/* original terminal (may already have bag) */
octObject *newTermPtr;		/* new terminal (to put into bag) */
{
    octObject bag;		/* bag that should hold terminals */
    octObject topBag;		/* top-level bag that should hold above bag */
    octGenerator gen;		/* for bags now containing *origTermPtr */
    octStatus genStatus;	/* returned status from generator */
    octObject oldBag;		/* generated bag */

    bag.type = OCT_BAG;
    topBag.type = OCT_BAG;

    /* figure out what type of bag (if any) we need to have */
    switch (makeBags) {
	case NONE:
	    return;
	case JOINED:
	    bag.contents.bag.name = JOINED_NAME;
	    topBag.contents.bag.name = JOINED_TOP_NAME;
	    break;
	case MUSTJOIN:
	    bag.contents.bag.name = MUSTJOIN_NAME;
	    topBag.contents.bag.name = MUSTJOIN_TOP_NAME;
	    break;
	default:
	    (void) fprintf(stderr,
			"inferTerminals:  impossible error -- makeBags = %d\n",
			makeBags);
	    exit(1);
    }

    /* look for existing bag on *origTermPtr */
    if (octInitGenContainers(origTermPtr, OCT_BAG_MASK, &gen) != OCT_OK) {
	octError("starting containing bag generator");
	exit(1);
    }
    while ((genStatus = octGenerate(&gen, &oldBag)) == OCT_OK) {
	if (strcmp(oldBag.contents.bag.name, bag.contents.bag.name) == 0) {
	    if (octAttach(&oldBag, newTermPtr) != OCT_OK) {
		octError("attaching terminal to bag");
		exit(1);
	    }
	    return;
	}
    }
    if (genStatus != OCT_GEN_DONE) {
	octError("generating bags containing terminal");
	exit(1);
    }

    /* we need to create a new bag for these terminals */
    if (octGetOrCreate(facetPtr, &topBag) != OCT_OK) {
	octError("getting top-level terminal bag");
	exit(1);
    }
    if (octCreate(&topBag, &bag) != OCT_OK) {
	octError("creating individual terminal bag");
	exit(1);
    }
    if (octAttach(&bag, origTermPtr) != OCT_OK) {
	octError("attaching terminal to bag");
	exit(1);
    }
    if (octAttach(&bag, newTermPtr) != OCT_OK) {
	octError("attaching terminal to bag");
	exit(1);
    }
}
