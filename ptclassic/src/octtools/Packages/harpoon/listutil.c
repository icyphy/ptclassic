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
#include "fang.h"
#include "harpoon_int.h"

/*LINTLIBRARY*/

void harFreeList(listPtr)
harList *listPtr;
{
    harList list, nextList;

    HAR_START("harFreeList");

    for (list = *listPtr; list != HAR_EMPTY_LIST; list = nextList) {
	nextList = harRestOfList(list);
	switch (list->type) {
	    case HAR_NAMED_GEO:
		fa_free_geo(harFirstGeo(list));
		FREE(harFirstName(list));
		break;
	    case HAR_POINT_GEO:
		FREE(harFirstPerimArray(list));
		break;
	    default:
		errRaise(HAR_PKG_NAME, 0, "corrupt list");
	}
	FREE(list);
    }
    HAR_END();
}

fa_geometry *harGetNamedGeo(name, geoListPtr)
char *name;			/* name of desired geo */
harList *geoListPtr;		/* list of named geometries */
{
    struct harListItem *geoItem;	/* new item */

    HAR_START("harGetNamedGeo");

    while (*geoListPtr != HAR_EMPTY_LIST) {
	if (!strcmp(name, harFirstName(*geoListPtr))) {
	    HAR_END();
	    return(harFirstGeo(*geoListPtr));
	}
	geoListPtr = &harRestOfList(*geoListPtr);
    }
    geoItem = ALLOC(struct harListItem, 1);
    harRestOfList(geoItem) = *geoListPtr;
    *geoListPtr = geoItem;
    geoItem->type = HAR_NAMED_GEO;
    harFirstName(geoItem) = util_strsav(name);
    fa_init(harFirstGeo(geoItem));
    HAR_END();
    return(harFirstGeo(geoItem));
}
