/* Version Identification:
 * $Id$
 */
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
#ifndef HARPOON_H
#define HARPOON_H

#include "ansi.h"

extern char harPkgName[];
#define HAR_PKG_NAME	harPkgName

struct harListItem {
    struct harListItem *next;
    int type;
    union {
	struct {
	    char *name;
	    fa_geometry geo;
	} namedGeo;
	struct {
	    int count;
	    struct octPoint *array;
	} perimGeo;
    } element;
};
typedef struct harListItem *harList;

#define HAR_EMPTY_LIST		((harList) 0)
#define harRestOfList(list)	((list)->next)

#define harFirstGeo(list)	(&(list)->element.namedGeo.geo)
#define harFirstName(list)	((list)->element.namedGeo.name)

#define harFirstPerimCount(list)	((list)->element.perimGeo.count)
#define harFirstPerimArray(list)	((list)->element.perimGeo.array)

#define HAR_NO_TRANSFORM	((struct octTransform *) 0)

#define HAR_FILLED_BOX		(OCT_BOX + OCT_MAX_TYPE)
#define HAR_UNFILLED_BOX	(OCT_BOX)
#define HAR_FILLED_POLYGON	(OCT_POLYGON + OCT_MAX_TYPE)
#define HAR_UNFILLED_POLYGON	(OCT_POLYGON)

#define HAR_INFINITY		(-1)

/* functions to convert from oct to fang */
EXTERN void harAddObjectToFang
	ARGS((octObject *obj, fa_geometry *geo, octTransform *xfrm));
EXTERN void harAddContentsToFang
	ARGS((octObject *cont, fa_geometry *geo, octTransform *xfrm));
EXTERN void harAddFacetToFangList
	ARGS((octObject *facet, harList *geos, octTransform *xfrm));

/* functions to convert from fang to oct */
EXTERN void harFangToContents
	ARGS((fa_geometry *geo, octObject *cont, int objType));
EXTERN void harBoxesToContents
	ARGS((fa_box *boxes, octObject *cont));
EXTERN void harRegionsToContents
	ARGS((fa_region *reg, octObject *cont));
EXTERN void harFangListToFacet
	ARGS(( harList *geos, octObject *facet, int objType));

/* functions to convert from fang to the perimList form */
EXTERN void harFangToPerimList
	ARGS((fa_geometry *geo, harList *perims, int ObjType));
EXTERN void harBoxesToPerimList
	ARGS((fa_box *boxes, harList *perims));
EXTERN void harRegionsToPerimList
	ARGS((fa_region *regs, harList *perims));
EXTERN void harFacetToPerimList
	ARGS((octObject *facet, octCoord grow, octCoord minEdge, harList *perims));

/* miscellaneous */
EXTERN void harAddTransformedFang
	ARGS((fa_geometry *add, octTransform *xfrm, fa_geometry *sum));
EXTERN fa_geometry *harGetNamedGeo
	ARGS((char *name, harList *list));
EXTERN void harFreeList
	ARGS((harList *list));

#endif /* HARPOON_H */
