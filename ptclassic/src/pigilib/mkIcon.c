/* mkIcon.c  edg
Version identification:
$Id$
*/

/*
Copyright (c) 1989 The Regents of the University of California.
			All Rights Reserved.
*/

/* Includes */
#include <stdio.h>
#include <sys/file.h>
#include "local.h"
#include "rpc.h"
#include "err.h"
#include "mkTerm.h"
#include "octIfc.h"
#include "octMacros.h"
#include "util.h"
#include "oh.h"
#include "vemInterface.h"
#include "list.h"
#include "mkIcon.h"

#define ERRBUF_MAX 1000

/* 8/24/89
Check to see if icon already exists.
*/
static boolean
AskAboutIcon(iconCellName)
char *iconCellName;
{
    octObject iconFacet;

    iconFacet.type = OCT_FACET;
    iconFacet.contents.facet.cell = iconCellName;
    iconFacet.contents.facet.view = "schematic";
    iconFacet.contents.facet.facet = "interface";
    iconFacet.contents.facet.version = OCT_CURRENT_VERSION;
    iconFacet.contents.facet.mode = "r";
    if (octOpenFacet(&iconFacet) >= OCT_OK) {
	CK_OCT(octCloseFacet(&iconFacet));
	ERR_IF2(dmConfirm("Icon already exists, overwrite?", "", "OK", "Abort")
	    == VEM_FALSE, "AskAboutIcon: aborted");
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
    octObject srcItem, destItem;
    octStatus status;
    int32 num;

    octInitGenContents(srcPtr, OCT_ALL_MASK, &gen);
    while ((status = octGenerate(&gen, &srcItem)) == OCT_OK) {
	destItem = srcItem;
	CK_OCT(octCreate(destPtr, &destItem));
	/* copy points of paths and polygons... */
	if (srcItem.type == OCT_PATH || srcItem.type == OCT_POLYGON) {

/* This temporary fix is limited to 32 points: 10/18/89 */
	    octPoint points[32];
	    num = 32;
	    CK_OCT(octGetPoints(&srcItem, &num, points));
	    octGetPoints(&srcItem, &num, points);
	    octPutPoints(&destItem, num, points);

/*  This is the way it should work but it doesn't work all the time:
	    num = 0;
	    if (octGetPoints(&srcItem, &num, points) == OCT_TOO_SMALL) {
		ERR_IF1(!UMalloc(&points, num * sizeof(octPoint)));
		octGetPoints(&srcItem, &num, points);
		octPutPoints(&destItem, num, points);
	    } else {
		char buf[512];
		PrintDebug("Warning: Path or polygon has no points!");
		PrintDebug(sprintf(buf, "num = %d", num));
	    }
*/
	}
	ERR_IF1(!DeepCopyObjs(&destItem, &srcItem));
    }
    CK_OCT(status);  /* check in case of OCT_ERROR */
    octFreeGenerator(&gen);
    return (TRUE);
}

typedef enum IconType_e {star = 0, gal, univ, pal} IconType;
static char *iconNames[] = {"%iNewStar", "%iNewGal", "%iNewUniv", "%iNewPal"};

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
    octObject srcFacet;
    char buf[FILENAME_MAX];
    char *techDir;

    srcFacet.type = OCT_FACET;
    ERR_IF1(!UGetFullTechDir(&techDir));
    srcFacet.contents.facet.cell =
	sprintf(buf, "%s/%s", techDir, iconNames[(int) type]);
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
    octObject term;
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
MkLabel(facetPtr, name)
octObject *facetPtr;
char *name;
{
    octObject label, labelLayer;
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
the position (ie. location) of each Term in the TermList.
*/
static boolean
MkTerms(facetPtr, termsPtr)
octObject *facetPtr;
TermList *termsPtr;
{
    Term *p;

    /* init the mkTerm.c package */
    ERR_IF1(!MkTermInit(facetPtr));
    /* make input terminals */
    p = termsPtr->in;
    switch (termsPtr->in_n) {
    case 0:
	break;
    case 1:
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, l0));
	break;
    case 2:
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, l1u)); p++;
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, l1d));
	break;
    case 3:
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, l1u)); p++;
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, l0)); p++;
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, l1d));
	break;
    case 4:
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, l2u)); p++;
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, l1u)); p++;
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, l0)); p++;
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, l1d));
	break;
    case 5:
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, l2u)); p++;
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, l1u)); p++;
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, l0)); p++;
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, l1d)); p++;
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, l2d));
	break;
    case 6:
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, l3u)); p++;
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, l2u)); p++;
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, l1u)); p++;
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, l0)); p++;
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, l1d)); p++;
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, l2d));
	break;
    case 7:
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, l3u)); p++;
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, l2u)); p++;
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, l1u)); p++;
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, l0)); p++;
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, l1d)); p++;
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, l2d)); p++;
	ERR_IF1(!MkTerm(p->name, TRUE, p->type, p->multiple, l3d));
	break;
    default:
	ErrAdd("Too many input terminals");
	return(FALSE);
    }
    /* make output terminals */
    p = termsPtr->out;
    switch (termsPtr->out_n) {
    case 0:
	break;
    case 1:
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, r0));
	break;
    case 2:
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, r1u)); p++;
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, r1d));
	break;
    case 3:
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, r1u)); p++;
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, r0)); p++;
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, r1d));
	break;
    case 4:
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, r2u)); p++;
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, r1u)); p++;
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, r0)); p++;
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, r1d));
	break;
    case 5:
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, r2u)); p++;
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, r1u)); p++;
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, r0)); p++;
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, r1d)); p++;
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, r2d));
	break;
    case 6:
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, r3u)); p++;
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, r2u)); p++;
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, r1u)); p++;
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, r0)); p++;
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, r1d)); p++;
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, r2d));
	break;
    case 7:
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, r3u)); p++;
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, r2u)); p++;
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, r1u)); p++;
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, r0)); p++;
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, r1d)); p++;
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, r2d)); p++;
	ERR_IF1(!MkTerm(p->name, FALSE, p->type, p->multiple, r3d));
	break;
    default:
	ErrAdd("Too many output terminals");
	return(FALSE);
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
    octObject prop;

    PrintDebug(sprintf(buf, "Making palette icon for '%s'",
	facetPtr->contents.facet.cell));
    ERR_IF1(!AskAboutIcon(facetPtr->contents.facet.cell));
    ERR_IF1(!MkBaseIcon(iconFacetPtr, facetPtr->contents.facet.cell, pal));
    ERR_IF1(!MkLabel(iconFacetPtr, BaseName(facetPtr->contents.facet.cell)));
    CK_OCT(ohCreateOrModifyPropStr(iconFacetPtr, &prop, "palette", ""));
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
    octObject prop;

    PrintDebug(sprintf(buf, "Making universe icon for '%s'",
	facetPtr->contents.facet.cell));
    ERR_IF1(!AskAboutIcon(facetPtr->contents.facet.cell));
    ERR_IF1(!MkBaseIcon(iconFacetPtr, facetPtr->contents.facet.cell, univ));
    ERR_IF1(!MkLabel(iconFacetPtr, BaseName(facetPtr->contents.facet.cell)));
    CK_OCT(ohCreateOrModifyPropStr(iconFacetPtr, &prop, "universe", ""));
    CK_OCT(octFlushFacet(iconFacetPtr));
    return (TRUE);
}

boolean
MkGalIcon(galFacetPtr, iconFacetPtr)
octObject *galFacetPtr, *iconFacetPtr;
{
    TermList terms;
    char buf[MSG_BUF_MAX];
    octObject prop;

    PrintDebug(sprintf(buf, "Making galaxy icon for '%s'",
	galFacetPtr->contents.facet.cell));
    ERR_IF1(!AskAboutIcon(galFacetPtr->contents.facet.cell));
    ERR_IF1(!GetGalTerms(galFacetPtr, &terms));
    ERR_IF1(!MkBaseIcon(iconFacetPtr, galFacetPtr->contents.facet.cell, gal));
    ERR_IF1(!MkLabel(iconFacetPtr, BaseName(galFacetPtr->contents.facet.cell)));
    ERR_IF1(!MkTerms(iconFacetPtr, &terms));
    CK_OCT(ohCreateOrModifyPropStr(iconFacetPtr, &prop, "galaxy", ""));
    CK_OCT(octFlushFacet(iconFacetPtr));
    return (TRUE);
}

/* 7/28/89 3/6/89 ...
Updates:
3/6/89 = return iconFacetPtr, name change
*/
boolean
MkStarIcon(name, dir, iconFacetPtr)
char *name, *dir;
octObject *iconFacetPtr;
{
    TermList terms;
    char buf[MSG_BUF_MAX];
    char *fileName;
    octObject iconConFacet, prop;

    ERR_IF2(!KcIsKnown(name),
	sprintf(buf, "MkStarIcon: unknown star '%s' (it needs to be loaded?)",
	name));
    PrintDebug(sprintf(buf, "Making star icon for '%s'", name));
    ERR_IF1(!ConcatDirName(dir, name, &fileName));
    ERR_IF1(!AskAboutIcon(fileName));
    ERR_IF1(!KcGetTerms(name, &terms));
    ERR_IF1(!MkStarConFacet(&iconConFacet, fileName, &terms));
    ERR_IF1(!MkBaseIcon(iconFacetPtr, fileName, star));
    ERR_IF1(!MkLabel(iconFacetPtr, name));
    ERR_IF1(!MkTerms(iconFacetPtr, &terms));
    CK_OCT(ohCreateOrModifyPropStr(iconFacetPtr, &prop, "star", ""));
    CK_OCT(octFlushFacet(iconFacetPtr));
    return (TRUE);
}
