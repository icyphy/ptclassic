/* palette.c  edg
Version identification:
@(#)palette.c	1.16	02/08/99
Make palette of icons.
These routines should be in octIfc.c maybe, since it enforces policy.
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

/* Includes */

#include "local.h"
#include <stdio.h>

#include "palette.h"

#include "oct.h"
#include "rpc.h"
#include "oh.h"
#include "octMacros.h"
#include "err.h"
#include "mkIcon.h"
#include "util.h"
#include "octIfc.h"

static boolean
GOCPaletteFacet(palName, facetPtr)
char *palName;
octObject *facetPtr;
{
    octObject prop = {OCT_UNDEFINED_OBJECT, 0};
    octStatus status;

    facetPtr->type = OCT_FACET;
    facetPtr->contents.facet.cell = palName;
    facetPtr->contents.facet.view = "schematic";
    facetPtr->contents.facet.facet = "contents";
    facetPtr->contents.facet.version = OCT_CURRENT_VERSION;
    facetPtr->contents.facet.mode = "a";
    if ((status = octOpenFacet(facetPtr)) <= 0) {
	ErrAdd(octErrorString());
	return(FALSE);
    } else if (status == OCT_NEW_FACET) {
	CK_OCT(GetOrCreatePropStr(facetPtr, &prop, "VIEWTYPE", "SCHEMATIC"));
	FreeOctMembers(&prop);
	CK_OCT(GetOrCreatePropStr(facetPtr, &prop, "EDITSTYLE", "SCHEMATIC"));
	FreeOctMembers(&prop);
	CK_OCT(GetOrCreatePropStr(facetPtr, &prop, "TECHNOLOGY", UTechProp));
	FreeOctMembers(&prop);
    }
    return (TRUE);
}

/* 1/5/90 2/27/89
Function maps ..., (-99, 0) into 0, (1, 100) into 100, ...
*/
static octCoord
AlignFunction(bbx)
octCoord bbx;
{
    return ((bbx < 0) ? (bbx / 100) * 100 : ((bbx + 99) / 100) * 100);
}

/* 1/5/90 8/1/89
*/
static boolean
GOCCursorParams(palFacetPtr, cursorPtr, leftMar, width, dx, dy)
octObject *palFacetPtr, *cursorPtr;
octCoord *leftMar, *width, *dx, *dy;
{
#define widthDef 800  /* 5 icons per row */
#define dxDef 200
#define dyDef 150

    octObject obj = {OCT_UNDEFINED_OBJECT, 0};

    if (ohGetByPropName(cursorPtr, &obj, "leftMargin") == OCT_NOT_FOUND) {
	obj.contents.prop.type = OCT_INTEGER;
	obj.contents.prop.value.integer =
	    cursorPtr->contents.instance.transform.translation.x;
	CK_OCT(octCreate(cursorPtr, &obj));
    }
    *leftMar = obj.contents.prop.value.integer;
    CK_OCT(ohGetOrCreatePropInt(cursorPtr, &obj, "width", widthDef));
    *width = obj.contents.prop.value.integer;
    CK_OCT(ohGetOrCreatePropInt(cursorPtr, &obj, "dx", dxDef));
    *dx = obj.contents.prop.value.integer;
    CK_OCT(ohGetOrCreatePropInt(cursorPtr, &obj, "dy", dyDef));
    *dy = obj.contents.prop.value.integer;
    return (TRUE);
}

static boolean
GOCCursor(palFacetPtr, cursorPtr, leftMar, width, dx, dy)
octObject *palFacetPtr, *cursorPtr;
octCoord *leftMar, *width, *dx, *dy;
{
/* this is the default cursor location for a new palette */
static struct octPoint defaultCursorPt = {500, -500};

    octGenerator genInst;
    struct octPoint cursorPt;
    octBox bb;
    char *techDir, buf[MAXPATHLEN], *cell;

    octInitGenContentsSpecial(palFacetPtr, OCT_INSTANCE_MASK, &genInst);
    while (octGenerate(&genInst, cursorPtr) == OCT_OK) {
	if (IsCursor(cursorPtr)) {
	    if (!GOCCursorParams(palFacetPtr, cursorPtr, leftMar, width,
	      dx, dy)) {
		octFreeGenerator(&genInst);
		return FALSE;
	    }
	    octFreeGenerator(&genInst);
	    return (TRUE);
	}
    }
    octFreeGenerator(&genInst);
    /* No cursor found, create one... */
    if (octBB(palFacetPtr, &bb) == OCT_OK) {
	/* It looks like RPC octBB() doesn't give the right BB for some reason,
	    at least some times (edg 1/5/90)'
	*/
	/* Locates cursor at an even (100 octUnits) point below the
	    lowerLeft corner of current bounding box.
	*/
	cursorPt.x = AlignFunction(bb.lowerLeft.x + 50);
	cursorPt.y = -AlignFunction(-bb.lowerLeft.y + 75);
    } else {
	cursorPt = defaultCursorPt;
    }
    ERR_IF1(!UGetFullTechDir(&techDir));
    sprintf(buf, "%s/%s", techDir, PT_PERCENT "cCursor");
    ERR_IF1(!StrDup(&cell, buf));
    CK_OCT(CreateInstance2(palFacetPtr, cursorPtr, "", cell, "schematic",
	"interface", cursorPt.x, cursorPt.y, OCT_NO_TRANSFORM));
    ERR_IF1(!GOCCursorParams(palFacetPtr, cursorPtr, leftMar, width, dx, dy));
    return (TRUE);
}

/* 7/31/89 3/4/89
*/
static boolean
AddIconToPal(iconFacetPtr, palFacetPtr)
octObject *iconFacetPtr, *palFacetPtr;
{
    octCoord leftMar, width, dx, dy;
    struct octPoint cursorPt;
    octObject cursorInst = {OCT_UNDEFINED_OBJECT, 0},
	      iconInst = {OCT_UNDEFINED_OBJECT, 0};

    ERR_IF1(!GOCCursor(palFacetPtr, &cursorInst, &leftMar, &width, &dx, &dy));
    cursorPt = cursorInst.contents.instance.transform.translation;

    /* place icon at current cursor position */
    CK_OCT(CreateInstance2(palFacetPtr, &iconInst, "",
	iconFacetPtr->contents.facet.cell, iconFacetPtr->contents.facet.view,
	iconFacetPtr->contents.facet.facet,
	cursorPt.x, cursorPt.y, OCT_NO_TRANSFORM));

    /* increment cursor */
    cursorPt.x += dx;
    if (cursorPt.x > leftMar + width) {
	/* do a "CR-LF" */
	cursorPt.x = leftMar;
	cursorPt.y -= dy;
    }
    cursorInst.contents.instance.transform.translation = cursorPt;
    CK_OCT(octModify(&cursorInst));
    return (TRUE);
}

/* 7/31/89 4/14/89 3/6/89
Change dir prefix of the cell name of an icon facet.
This allows you to change the way an instance is instantiated with
the dir parameter.
Note that iconPtr is closed and iconFullPtr is opened.
*/
static boolean
MkOctFullName(iconPtr, dir, iconFullPtr)
octObject *iconPtr, *iconFullPtr;
char *dir;
{
    char *buf, *baseName;

    baseName = BaseName(iconPtr->contents.facet.cell);
    ERR_IF1(!ConcatDirName(dir, baseName, &buf));
    *iconFullPtr = *iconPtr;
    iconFullPtr->contents.facet.cell = buf;
    iconFullPtr->contents.facet.mode = "r";
    CK_OCT(octOpenFacet(iconFullPtr));
    CK_OCT(octCloseFacet(iconPtr));
    return (TRUE);
}

/* 8/8/89
Inputs: dir = directory which contains *facetPtr (both schematic and icon)
    that will be used to instantiate the icon into the facet.  It may be 
    empty to create a relative pathname.
*/
boolean
MkPalIconInPal(facetPtr, dir, palName)
octObject *facetPtr;
char *dir, *palName;
{
    octObject iconFacet = {OCT_UNDEFINED_OBJECT, 0},
	      iconFullFacet = {OCT_UNDEFINED_OBJECT, 0},
	      palFacet = {OCT_UNDEFINED_OBJECT, 0};

    ERR_IF1(!MkPalIcon(facetPtr, &iconFacet));
    ERR_IF1(!MkOctFullName(&iconFacet, dir, &iconFullFacet));
    ERR_IF1(!GOCPaletteFacet(palName, &palFacet));
    ERR_IF1(!AddIconToPal(&iconFullFacet, &palFacet));
    CK_OCT(octCloseFacet(&palFacet));
    CK_OCT(octCloseFacet(&iconFullFacet));
    return (TRUE);
}

/* 7/31/89
Inputs: dir = directory which contains *facetPtr (both schematic and icon)
    that will be used to instantiate the icon into the facet.  It may be 
    empty to create a relative pathname.
*/
boolean
MkUnivIconInPal(facetPtr, dir, palName)
octObject *facetPtr;
char *dir, *palName;
{
    octObject iconFacet = {OCT_UNDEFINED_OBJECT, 0},
	      iconFullFacet = {OCT_UNDEFINED_OBJECT, 0},
	      palFacet = {OCT_UNDEFINED_OBJECT, 0};

    ERR_IF1(!MkUnivIcon(facetPtr, &iconFacet));
    ERR_IF1(!MkOctFullName(&iconFacet, dir, &iconFullFacet));
    ERR_IF1(!GOCPaletteFacet(palName, &palFacet));
    ERR_IF1(!AddIconToPal(&iconFullFacet, &palFacet));
    CK_OCT(octCloseFacet(&palFacet));
    CK_OCT(octCloseFacet(&iconFullFacet));
    return (TRUE);
}

/* 7/31/89 4/14/89 3/6/89
Inputs: dir = directory which contains *galFacetPtr (both schematic and icon)
    that will be used to instantiate the icon into the facet.  It may be 
    empty to create a relative pathname.
*/
boolean
MkGalIconInPal(galFacetPtr, dir, palName)
octObject *galFacetPtr;
char *dir, *palName;
{
    octObject iconFacet = {OCT_UNDEFINED_OBJECT, 0},
	      iconFullFacet = {OCT_UNDEFINED_OBJECT, 0},
	      palFacet = {OCT_UNDEFINED_OBJECT, 0};

    ERR_IF1(!MkGalIcon(galFacetPtr, &iconFacet));
    ERR_IF1(!MkOctFullName(&iconFacet, dir, &iconFullFacet));
    ERR_IF1(!GOCPaletteFacet(palName, &palFacet));
    ERR_IF1(!AddIconToPal(&iconFullFacet, &palFacet));
    CK_OCT(octCloseFacet(&palFacet));
    CK_OCT(octCloseFacet(&iconFullFacet));
    return (TRUE);
}

/* 7/28/89 3/6/89
Inputs: dir = directory to place icon facet in.  If left blank,
then icon will be instantiated using a relative pathname and icon
will be placed in current VEM directory.  Else, use dir and absolute
pathname.
*/
boolean
MkStarIconInPal(starName, dir, domain, palName)
char *starName, *dir, *domain, *palName;
{
    octObject iconFacet = {OCT_UNDEFINED_OBJECT, 0},
	      palFacet = {OCT_UNDEFINED_OBJECT, 0};

    ERR_IF1(!MkStarIcon(starName, dir, domain, &iconFacet));
    ERR_IF1(!GOCPaletteFacet(palName, &palFacet));
    ERR_IF1(!AddIconToPal(&iconFacet, &palFacet));
    CK_OCT(octCloseFacet(&palFacet));
    CK_OCT(octCloseFacet(&iconFacet));
    return (TRUE);
}
