/* mkIcon.c  edg
Version identification:
@(#)mkIcon.c	1.33	01/28/99
*/

/*
Copyright (c) 1990-1999 The Regents of the University of California.
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

/* Standard includes */
#include "local.h"
#include <stdio.h>
#include <sys/file.h>

/* Octtools includes */
#include "oct.h"		/* define octObject */
#include "list.h"
#include "rpc.h"
#include "oh.h"

/* Pigilib includes */
#include "mkIcon.h"
#include "err.h"
#include "mkTerm.h"
#include "octIfc.h"
#include "octMacros.h"
#include "util.h"
#include "vemInterface.h"
#include "compile.h"
#include "kernelCalls.h"	/* define callParseClass and KcCheckTerms */

#define ERRBUF_MAX 1000

static octObject starBoxLayer = {OCT_UNDEFINED_OBJECT, 0},
	         galBoxLayer = {OCT_UNDEFINED_OBJECT, 0},
		 iconBackgrLayer = {OCT_UNDEFINED_OBJECT, 0};

/*
MkGetTerms
4/25/96
Split off from KcGetTerms from kernelCalls.cc in order to make
kernelCalls independent of oct.
*/
boolean
MkGetTerms(const char* name, TermList* terms) {
	int i, newNameCount = 0, numOrdPorts = 0;
	char *newNames[MAX_NUM_TERMS];
	const char *newTypes[MAX_NUM_TERMS];
	int newIsOut[MAX_NUM_TERMS];

	if ( !KcCheckTerms(name, (const char **)newNames, newTypes,
			   newIsOut, &numOrdPorts, &newNameCount) ) {
	    return FALSE;
	}

	terms->in_n = 0;
	terms->out_n = 0;
	for (i = 0; i < newNameCount; i++) {
	    if (newIsOut[i]) {
		terms->out[terms->out_n].name = newNames[i];
		terms->out[terms->out_n].type = newTypes[i];
		terms->out[terms->out_n++].multiple = (i >= numOrdPorts);
	    }
	    else {
		terms->in[terms->in_n].name = newNames[i];
		terms->in[terms->in_n].type = newTypes[i];
		terms->in[terms->in_n++].multiple = (i >= numOrdPorts);
	    }
	}
	return TRUE;
}

/* 8/24/89
Check to see if icon already exists.
*/
static boolean
AskAboutIcon(iconCellName)
char *iconCellName;
{
    octObject iconFacet = {OCT_UNDEFINED_OBJECT, 0};

    iconFacet.type = OCT_FACET;
    iconFacet.contents.facet.cell = iconCellName;
    iconFacet.contents.facet.view = "schematic";
    iconFacet.contents.facet.facet = "interface";
    iconFacet.contents.facet.version = OCT_CURRENT_VERSION;
    iconFacet.contents.facet.mode = "r";
    if (octOpenFacet(&iconFacet) >= OCT_OK) {
	CK_OCT(octCloseFacet(&iconFacet));
	if (dmConfirm("Icon already exists, overwrite?", "", "OK", "Abort")
	    == VEM_FALSE) {
	    PrintCon("AskAboutIcon: aborted");
	    return FALSE;
	}
    }
    return (TRUE);
}

/* 8/25/89
Recursively copy all contents of srcPtr into destPtr.
Caveats: Assumes srcPtr forms a tree.
*/
static boolean
DeepCopyObjs(destPtr, srcPtr)
octObject *destPtr, *srcPtr;
{
    octGenerator gen;
    octObject srcItem = {OCT_UNDEFINED_OBJECT, 0},
	      destItem = {OCT_UNDEFINED_OBJECT, 0};
    octStatus status;
    int32 num;

    octInitGenContentsSpecial(srcPtr, OCT_ALL_MASK, &gen);
    while ((status = octGenerate(&gen, &srcItem)) == OCT_OK) {
	destItem = srcItem;
	if (octCreate(destPtr, &destItem) < OCT_OK) {
	    octFreeGenerator(&gen);
	    ErrAdd(octErrorString());
	    return FALSE;
	}
	/* copy points of paths and polygons... */
	if (srcItem.type == OCT_PATH || srcItem.type == OCT_POLYGON) {

/* This temporary fix is limited to 32 points: 10/18/89 */
	    octPoint points[32];
	    num = 32;
	    if (octGetPoints(&srcItem, &num, points) < OCT_OK) {
		octFreeGenerator(&gen);
		ErrAdd(octErrorString());
		return FALSE;
	    }
	    octPutPoints(&destItem, num, points);

/*  This is the way it should work but it doesn't work all the time:
	    num = 0;
	    if (octGetPoints(&srcItem, &num, points) == OCT_TOO_SMALL) {
		if (!UMalloc(&points, num * sizeof(octPoint))) {
		    octFreeGenerator(&gen);
		    return FALSE;
		}
		octGetPoints(&srcItem, &num, points);
		octPutPoints(&destItem, num, points);
	    } else {
		char buf[512];
		PrintDebug("Warning: Path or polygon has no points!");
		sprintf(buf, "num = %d", num);
		PrintDebug(buf);
	    }
*/
	}
	if (!DeepCopyObjs(&destItem, &srcItem)) {
	    octFreeGenerator(&gen);
	    return FALSE;
	}
    }
    octFreeGenerator(&gen);
    CK_OCT(status);  /* check in case of OCT_ERROR */
    return (TRUE);
}

typedef enum IconType_e
	{pal = 0, univ, gal, star} IconType;
static char *iconNames[] =
	{PT_PERCENT"iNewPal", PT_PERCENT"iNewUniv",
         PT_PERCENT"iNewGal", PT_PERCENT"iNewStar"};

/* 8/24/89 7/27/89
Makes an interface facet and returns it opened.  If one already exists,
it is overwritten.
*/
static boolean
MkBaseIcon(iconFacetPtr, iconCellName, type)
octObject *iconFacetPtr;
char *iconCellName;
IconType type;
{
    octObject srcFacet = {OCT_UNDEFINED_OBJECT, 0};
    char buf[MAXPATHLEN];
    char *techDir;

    srcFacet.type = OCT_FACET;
    ERR_IF1(!UGetFullTechDir(&techDir));

    sprintf(buf, "%s/%s", techDir, iconNames[(int) type]);
    srcFacet.contents.facet.cell = buf;
    srcFacet.contents.facet.view = "schematic";
    srcFacet.contents.facet.facet = "interface";
    srcFacet.contents.facet.version = OCT_CURRENT_VERSION;
    srcFacet.contents.facet.mode = "r";
    CK_OCT(octOpenFacet(&srcFacet));

    iconFacetPtr->type = OCT_FACET;
    ERR_IF1(!StrDup(&(iconFacetPtr->contents.facet.cell), iconCellName));
    iconFacetPtr->contents.facet.view = "schematic";
    iconFacetPtr->contents.facet.facet = "interface";
    iconFacetPtr->contents.facet.version = OCT_CURRENT_VERSION;
    ERR_IF1(!ViKillBuf(iconFacetPtr));  /* kill VEM buffer before overwrite */
    iconFacetPtr->contents.facet.mode = "w";
    CK_OCT(octOpenFacet(iconFacetPtr));
    ERR_IF1(!DeepCopyObjs(iconFacetPtr, &srcFacet));
    CK_OCT(octFlushFacet(iconFacetPtr));
    return (TRUE);
}

/* Propagate the domain to the schematic icon for the galaxy. -BLE 1/26/95 */
static void
propagateDomain(galFacetPtr, iconFacetPtr)
octObject *galFacetPtr, *iconFacetPtr;
{
    const char* domain = 0;
    if ( GetStringProp(galFacetPtr, "domain", &domain) ) {
	if ( domain ) SetStringProp(iconFacetPtr, "domain", (char*)domain);
    }
    return;
}

/* 8/24/89
Makes an icon contents facet for stars.  It simply puts formal terminals
in the contents facet.
*/
static boolean
MkStarConFacet(conFacetPtr, cellName, termsPtr)
octObject *conFacetPtr;
char *cellName;
TermList *termsPtr;
{
    octObject term = {OCT_UNDEFINED_OBJECT, 0};
    int i;

    conFacetPtr->type = OCT_FACET;
    conFacetPtr->contents.facet.cell = cellName;
    conFacetPtr->contents.facet.view = "schematic";
    conFacetPtr->contents.facet.facet = "contents";
    conFacetPtr->contents.facet.version = OCT_CURRENT_VERSION;
    ERR_IF1(!ViKillBuf(conFacetPtr));  /* kill VEM buffer before overwrite */
    conFacetPtr->contents.facet.mode = "w";
    CK_OCT(octOpenFacet(conFacetPtr));
    for (i = 0; i < termsPtr->in_n; i++) {
	CK_OCT(ohCreateTerm(conFacetPtr, &term, termsPtr->in[i].name));
    }
    for (i = 0; i < termsPtr->out_n; i++) {
	CK_OCT(ohCreateTerm(conFacetPtr, &term, termsPtr->out[i].name));
    }
    CK_OCT(octCloseFacet(conFacetPtr));
    return (TRUE);
}

static boolean
MkBox(layerPtr, size)
octObject *layerPtr;
int size;
{
    Shape box;
    struct octPoint corners[2];
    static struct octPoint noTranslate = {0, 0};
    octObject dummy = {OCT_UNDEFINED_OBJECT, 0};

    box.type = OCT_BOX;
    box.points = corners;
    box.points_n = 2;

    corners[0].x = -50;
    corners[0].y = 50;
    corners[1].x = 50;
    corners[1].y = 50-size*25;

/* 11/11/93 added FALSE */
    ERR_IF1(!PutShape(layerPtr, &dummy, &box, &noTranslate, FALSE));
    return TRUE;
}

static boolean
MkLabel(facetPtr, name)
octObject *facetPtr;
char *name;
{
    octObject label = {OCT_UNDEFINED_OBJECT, 0},
	      labelLayer = {OCT_UNDEFINED_OBJECT, 0};
    static struct octBox labelBox = {
	{(octCoord) -50, (octCoord) -50},
	{(octCoord) 50, (octCoord) -30}
    };

    CK_OCT(ohGetOrCreateLayer(facetPtr, &labelLayer, "label"));
    CK_OCT(ohCreateLabel(&labelLayer, &label, name, labelBox, (octCoord) 20,
	OCT_JUST_BOTTOM, OCT_JUST_LEFT, OCT_JUST_LEFT));
    return (TRUE);
}

/* MkTerms  8/6/88
Make terminals of icon according to a TermList.  This function determines
the position (i.e. location) of each Term in the TermList.
*/
static boolean
MkTerms(facetPtr, termsPtr)
octObject *facetPtr;
TermList *termsPtr;
{
    Term *p;
    int i, numTerms, numIns, numOuts;

    /* init the mkTerm.c package */
    ERR_IF1(!MkTermInit(facetPtr));

    /* make input terminals */
    p = termsPtr->in;
    numIns = termsPtr->in_n;
    numOuts = termsPtr->out_n;
    numTerms = numIns + numOuts;

    /* Deal with special cases first */
    if (numTerms > MAX_NUM_TERMS) {
	ErrAdd("Too many terminals");
	return(FALSE);
    } else if (termsPtr->in_n == 1) {
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, 2, numIns));
    } else if (termsPtr->in_n == 2) {
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, 1, numIns)); p++;
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, 3, numIns));
    } else if (termsPtr->in_n == 3) {
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, 1, numIns)); p++;
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, 2, numIns)); p++;
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, 3, numIns));
    } else {
	for (i = 0; i < termsPtr->in_n; i++) {
	    ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, i, numIns));
	    p++;
	}
    }

    /* make output terminals */
    p = termsPtr->out;

    /* Deal with special cases first */
    if (numTerms > MAX_NUM_TERMS) {
	ErrAdd("Too many output terminals");
	return(FALSE);
    } else if (termsPtr->out_n == 1) {
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, 2,numOuts));
    } else if (termsPtr->out_n == 2) {
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, 1,numOuts)); p++;
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, 3,numOuts));
    } else if (termsPtr->out_n == 3) {
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, 1,numOuts)); p++;
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, 2,numOuts)); p++;
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, 3,numOuts));
    } else {
	for (i = 0; i < termsPtr->out_n; i++) {
	    ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, i, numOuts));
	    p++;
	}
    }
    return(TRUE);
}

/* 8/8/89
*/
boolean
MkPalIcon(facetPtr, iconFacetPtr)
octObject *facetPtr, *iconFacetPtr;
{
    char buf[MSG_BUF_MAX];
    octObject prop = {OCT_UNDEFINED_OBJECT, 0};

    sprintf(buf,"Making palette icon for '%s'", facetPtr->contents.facet.cell);
    PrintDebug(buf);
    ERR_IF1(!AskAboutIcon(facetPtr->contents.facet.cell));
    ERR_IF1(!MkBaseIcon(iconFacetPtr, facetPtr->contents.facet.cell, pal));
    ERR_IF1(!MkLabel(iconFacetPtr, BaseName(facetPtr->contents.facet.cell)));
    CK_OCT(CreateOrModifyPropStr(iconFacetPtr, &prop, "palette", ""));
    propagateDomain(facetPtr, iconFacetPtr);
    CK_OCT(octFlushFacet(iconFacetPtr));
    return (TRUE);
}


/* 8/1/89
*/
boolean
MkUnivIcon(facetPtr, iconFacetPtr)
octObject *facetPtr, *iconFacetPtr;
{
    char buf[MSG_BUF_MAX];
    octObject prop = {OCT_UNDEFINED_OBJECT, 0};

    sprintf(buf,"Making universe icon for '%s'",facetPtr->contents.facet.cell);
    PrintDebug(buf);
    ERR_IF1(!AskAboutIcon(facetPtr->contents.facet.cell));
    ERR_IF1(!MkBaseIcon(iconFacetPtr, facetPtr->contents.facet.cell, univ));
    ERR_IF1(!MkLabel(iconFacetPtr, BaseName(facetPtr->contents.facet.cell)));
    CK_OCT(CreateOrModifyPropStr(iconFacetPtr, &prop, "universe", ""));
    propagateDomain(facetPtr, iconFacetPtr);
    CK_OCT(octFlushFacet(iconFacetPtr));
    return (TRUE);
}

boolean
MkGalIcon(galFacetPtr, iconFacetPtr)
octObject *galFacetPtr, *iconFacetPtr;
{
    TermList terms;
    char buf[MSG_BUF_MAX];
    octObject prop = {OCT_UNDEFINED_OBJECT, 0};
    char* name = BaseName(galFacetPtr->contents.facet.cell);
    int maxNumTerms, size;

    sprintf(buf, "Making galaxy icon for '%s'", name);
    PrintDebug(buf);

    ERR_IF1(!AskAboutIcon(galFacetPtr->contents.facet.cell));
    ERR_IF1(!CompileGalStandalone(galFacetPtr));
    ERR_IF1(!MkGetTerms(name, &terms));
    if (terms.in_n < terms.out_n) maxNumTerms = terms.out_n;
    else maxNumTerms = terms.in_n;
    if (maxNumTerms <= 5) size = 4;
    else size = maxNumTerms-1;
    ERR_IF1(!MkBaseIcon(iconFacetPtr, galFacetPtr->contents.facet.cell, gal));
    /* make the box */
    CK_OCT(ohGetOrCreateLayer(iconFacetPtr, &galBoxLayer, "greenOutline"));
    ERR_IF1(!MkBox(&galBoxLayer, size));
    ERR_IF1(!MkLabel(iconFacetPtr, name));
    ERR_IF1(!MkTerms(iconFacetPtr, &terms));
    if (ohGetOrCreateLayer(iconFacetPtr, &iconBackgrLayer, "iconBackground")
	== OCT_OK) {
	ERR_IF1(!MkBox(&iconBackgrLayer, size));
    }
    CK_OCT(CreateOrModifyPropStr(iconFacetPtr, &prop, "galaxy", ""));
    propagateDomain(galFacetPtr, iconFacetPtr);
    CK_OCT(octFlushFacet(iconFacetPtr));
    return (TRUE);
}

/* 7/28/89 3/6/89 ...
Updates:
3/6/89 = return iconFacetPtr, name change
*/
boolean
MkStarIcon(nameplus, dir, domain, iconFacetPtr)
char *nameplus, *dir, *domain;
octObject *iconFacetPtr;
{
    TermList terms;
    char buf[MSG_BUF_MAX];
    char *fileName;
    const char *name;
    octObject iconConFacet = {OCT_UNDEFINED_OBJECT, 0},
	      prop = {OCT_UNDEFINED_OBJECT, 0};
    int maxNumTerms, size;

    /* nameplus may have extensions for the number of ports, etc. */
    /* Here, we find the class name. */
    name = callParseClass(nameplus);
    if (!strcmp(domain,NODOMAIN)) {
	terms.in_n = 0;
	terms.out_n = 0;
	ERR_IF1(!ConcatDirName(dir, nameplus, &fileName));
	ERR_IF1(!AskAboutIcon(fileName));
    } else {
        sprintf(buf, "MkStarIcon: unknown star '%s' (it needs to be loaded?)",
		name);
        ERR_IF2(!KcIsKnown(name), buf);
	sprintf(buf, "Making star icon for '%s'", name);
	PrintDebug(buf);
	ERR_IF1(!ConcatDirName(dir, nameplus, &fileName));
	ERR_IF1(!AskAboutIcon(fileName));
	ERR_IF1(!MkGetTerms(nameplus, &terms));
    }
    ERR_IF1(!MkStarConFacet(&iconConFacet, fileName, &terms));
    if (terms.in_n < terms.out_n) maxNumTerms = terms.out_n;
    else maxNumTerms = terms.in_n;
    if (maxNumTerms <= 5) size = 4;
    else size = maxNumTerms-1;
    ERR_IF1(!MkBaseIcon(iconFacetPtr, fileName, star));
    /* make the box */
    CK_OCT(ohGetOrCreateLayer(iconFacetPtr, &starBoxLayer, "blueOutline"));
    ERR_IF1(!MkBox(&starBoxLayer, size));
    ERR_IF1(!MkLabel(iconFacetPtr, name));
    ERR_IF1(!MkTerms(iconFacetPtr, &terms));
    if (ohGetOrCreateLayer(iconFacetPtr, &iconBackgrLayer, "iconBackground")
	== OCT_OK) {
	ERR_IF1(!MkBox(&iconBackgrLayer, size));
/* Not making shadows for stars
	if (ohGetOrCreateLayer(iconFacetPtr, &iconShadowLayer, "iconShadow")
	    == OCT_OK) {
	    ERR_IF1(!MkShadow(&iconShadowLayer, size));
	}
*/
    }
    CK_OCT(CreateOrModifyPropStr(iconFacetPtr, &prop, "star", ""));
    SetStringProp(iconFacetPtr, "domain", domain);
    CK_OCT(octFlushFacet(iconFacetPtr));
    return (TRUE);
}
