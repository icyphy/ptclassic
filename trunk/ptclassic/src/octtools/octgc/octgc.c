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
#include "options.h"
#include "oct.h"
#include "oh.h"
#include "octgc.h"
#include "vov.h"

static void gcFacet(octObject *facetPtr);
static void markUsed(struct useFlags *usePtr, octObject *facetPtr);
static void removeUnused(struct useFlags *usePtr, octObject *facetPtr);
static void doMarkUsed(struct useFlags *usePtr, octObject *objPtr, int upFlag);

optionStruct optionList[] = {
    { "p",	0,		"print descriptions of garbage objects"	},
    { "d",	0,		"delete garbage objects"		},
    { "t",	0,		"only touch objects in tree rooted at facet" },
    { OPT_RARG,	"cell:view[:facet]",	"cell(s) to clean up"		},
    { OPT_ELLIP,0,	0						},
    { 0,	0,	0						}
};

int flags = 0;

int main(argc, argv)
    int argc;
    char *argv[];
{
    int option;
    octObject facet;
    /* Ptolemy uses octoptind instead of optind */
    extern int octoptind;

    while ((option = optGetOption(argc, argv)) != EOF) {
	switch (option) {
	case 'p':		/* print trash */
	    flags |= PRINT;
	    break;
	case 'd':		/* delete trash */
	    flags |= UPDATE;
	    break;
	case 't':		/* only reach objects in tree rooted at facet */
	    flags |= STRICT_DOWN;
	    break;
	}
    }
    if (octoptind >= argc) optUsage();

    VOVbegin( argc, argv );
    octBegin();

    ohUnpackDefaults(&facet, (flags & UPDATE) ? "a" : "r", "::contents");


    while (octoptind < argc) {
	if (ohUnpackFacetName(&facet, argv[octoptind++]) != OCT_OK) {
	    optUsage();
	}

	if ( flags & UPDATE ) {
	    VOVinputFacet( &facet );
	    VOVoutputFacet( &facet );
	}

	gcFacet(&facet);
    }

    octEnd();
    
    VOVend(0);
    return 0;
}

void gcFacet(facetPtr)
    octObject *facetPtr;
{
    octObject dumbObj;
    struct useFlags use;
    long i;

    if (octOpenFacet(facetPtr) < OCT_OK) {
	(void) fprintf(stderr, "%s: couldn't open facet (%s)\n",
		       optProgName, octErrorString());
	return;
    }

    dumbObj.type = OCT_BAG;
    dumbObj.contents.bag.name = "";

    OCT_ASSERT(octCreate(facetPtr, &dumbObj));
    use.lowId = oct_null_id + 1; /* XXX */
    use.highId = dumbObj.objectId;
    OCT_ASSERT(octDelete(&dumbObj));

    use.usedArray = ALLOC(char, use.highId - use.lowId);
    for (i = 0; i < (long) (use.highId - use.lowId); i++) {
	use.usedArray[i] = 0;
    }

    markUsed(&use, facetPtr);

    removeUnused(&use, facetPtr);

    FREE(use.usedArray);

    OCT_ASSERT(octFreeFacet(facetPtr));
}

/*
*	markUsed -- mark all objects that are reachable from facet
*
*   To avoid stack overflow, this routine first follows only the down
*   links from the facet (calling doMarkUsed with upFlag = 0).  Then,
*   for each object that has been reached, it generates all containing
*   objects and calls doMarkUsed with upFlag = 1, to follow all links
*   from that object.  For strict-down mode, skip the second pass.
*/
void markUsed(usePtr, facetPtr)
struct useFlags *usePtr;
octObject *facetPtr;
{
    long i;
    octObject object;
    octGenerator gen;
    octStatus stat;
    octObject genObj;

    doMarkUsed(usePtr, facetPtr, 0);

    if (flags & STRICT_DOWN) return;

    for (i = 0; i < (long) (usePtr->highId - usePtr->lowId); i++) {
	if (usePtr->usedArray[i]) {
	    object.objectId = (octId) i + usePtr->lowId;
	    OCT_ASSERT(octGetById(&object));
	    if (object.type != OCT_FACET) {
		OCT_ASSERT(octInitGenContainers(&object, OCT_ALL_MASK, &gen));
		while ((stat = octGenerate(&gen, &genObj)) == OCT_OK) {
		    doMarkUsed(usePtr, &genObj, 1);
		}
		OCT_ASSERT(stat);
	    }
	}
    }
}

void doMarkUsed(usePtr, objPtr, upFlag)
struct useFlags *usePtr;
octObject *objPtr;
int upFlag;
{
    octId id;
    octGenerator gen;
    octStatus stat;
    octObject genObj;

    id = objPtr->objectId;
    if (id < usePtr->lowId || id >= usePtr->highId) {
	PANIC("object Id out of range");
    }

    if (usePtr->usedArray[id - usePtr->lowId]) return;

    usePtr->usedArray[id - usePtr->lowId] = 1;

    OCT_ASSERT(octInitGenContents(objPtr, OCT_ALL_MASK, &gen));
    while ((stat = octGenerate(&gen, &genObj)) == OCT_OK) {
	doMarkUsed(usePtr, &genObj, upFlag);
    }
    OCT_ASSERT(stat);

    if (upFlag && objPtr->type != OCT_FACET) {
	OCT_ASSERT(octInitGenContainers(objPtr, OCT_ALL_MASK, &gen));
	while ((stat = octGenerate(&gen, &genObj)) == OCT_OK) {
	    doMarkUsed(usePtr, &genObj, upFlag);
	}
	OCT_ASSERT(stat);
    }
}

void removeUnused(usePtr, facetPtr)
struct useFlags *usePtr;
octObject *facetPtr;
{
    long i;
    long nGoodObj = 0, nBadObj = 0;
    octObject unusedObj;
    octObject objFacet;

    for (i = 0; i < (long) (usePtr->highId - usePtr->lowId); i++) {
	if (usePtr->usedArray[i]) {
	    nGoodObj++;
	} else {
	    unusedObj.objectId = (octId) i + usePtr->lowId;
	    if (octGetById(&unusedObj) == OCT_OK) {
		octGetFacet(&unusedObj, &objFacet);
		if (objFacet.objectId == facetPtr->objectId) {
		    nBadObj++;
		} else {
		    usePtr->usedArray[i] = 1;
		}
	    } else {
		usePtr->usedArray[i] = 1;
	    }
	}
    }

    (void) printf("\n\t%s:%s:%s:%s\t%5ld objects %5ld garbage\n",
		    facetPtr->contents.facet.cell,
		    facetPtr->contents.facet.view,
		    facetPtr->contents.facet.facet,
		    facetPtr->contents.facet.version,
		    nGoodObj + nBadObj, nBadObj);

    for (i = 0; i < (long) (usePtr->highId - usePtr->lowId); i++) {
	if ( ! usePtr->usedArray[i]) {
	    unusedObj.objectId = (octId) i + usePtr->lowId;
	    OCT_ASSERT(octGetById(&unusedObj));
	    /* XXX -- should assert, but it fails for facets (???) */
	    if (flags & PRINT) (void) (octPrintObject(stdout, &unusedObj, 0));
	    if (flags & UPDATE) OCT_ASSERT(octDelete(&unusedObj));
	}
    }

    if (nBadObj > 0 && (flags & UPDATE)) OCT_ASSERT(octFlushFacet(facetPtr));
}
