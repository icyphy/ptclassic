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
/*LINTLIBRARY*/
#include "copyright.h"
#include "port.h"
#include "utility.h"
#include "errtrap.h"
#include "oct.h"
#include "oh.h"
#include "region.h"
#include "tap.h"
#include "symlib.h"
#include "symlib_int.h"


static symStatus
sym_handle_segment_end_point(facet, pt, term, type, layer, width, direction)
octObject *facet;
struct octPoint *pt;
octObject *term;
int type;
octObject *layer;
octCoord width;
int direction;
{
    octObject inst, seg, oldinst;
    tapLayerListElement layerList[40];
    octStatus status;
    octGenerator gen;
    struct octBox bb;
    struct octPoint pnts[2];
    int count = 0;
    int32 np = 2;

    if (type == SYM_USE_EXISTING_OR_CREATE) {
	/* XXX this may cause problems, need ERR_IGNORE */
	if (sym_handle_segment_end_point(facet, pt, term, SYM_USE_EXISTING, layer, width, direction) == SYM_NO_EXIST) {
	    SYM_CHECK(sym_handle_segment_end_point(facet, pt, term, SYM_CREATE, layer, width, direction));
	}
	return SYM_OK;
    }

    if (type == SYM_USE_EXISTING) {
	if (octIdIsNull(term->objectId)) {
	    if (regFindActual(facet, pt, term, &bb) != REG_OK) {
		return SYM_NO_EXIST;
	    }
	    pt->x = SYM_AVEX(bb);
	    pt->y = SYM_AVEY(bb);
	} else {
	    if (SYM_FTERMP(term)) {
		errRaise(SYMLIB_PKG_NAME, SYM_ERROR, "can not use formal terminals for end points");
	    }
	    if (regFindImpl(term, &bb) != REG_OK) {
		errRaise(SYMLIB_PKG_NAME, SYM_ERROR, "can not find the implementation");
	    }
	    pt->x = SYM_AVEX(bb);
	    pt->y = SYM_AVEY(bb);
	}
	
	if (symConnectorp(term)) {
	    /* look for all paths attached to the terminal */
	    OCT_CHECK(octInitGenContainers(term, OCT_PATH_MASK, &gen));
	    while (octGenerate(&gen, &seg) == OCT_OK) {
		status = octGenFirstContainer(&seg, OCT_LAYER_MASK,
					      &(layerList[count].layer));
		switch (status) {
		    case OCT_GEN_DONE:
		    errRaise(SYMLIB_PKG_NAME, SYM_ERROR, "no layer");

		    case OCT_OK:
		    break;

		    default:
		    RAISE_ERROR;
		}
		
		layerList[count].width = seg.contents.path.width;
		OCT_CHECK(octGetPoints(&seg, &np, pnts));
		if (SYM_ABS(pnts[0].x - pnts[1].x) > SYM_ABS(pnts[0].y - pnts[1].y)) {
		    layerList[count].direction = TAP_HORIZONTAL;
		} else {
		    layerList[count].direction = TAP_VERTICAL;
		}
		count++;
	    }
	    layerList[count].layer = *layer;
	    layerList[count].width = width;
	    layerList[count].direction = (direction == SYM_HORIZONTAL) ? TAP_HORIZONTAL : TAP_VERTICAL;
	    count++;
	
	    if (tapGetConnector(count, layerList, &inst) != OCT_OK) {
		errRaise(SYMLIB_PKG_NAME, SYM_ERROR, "can not get a connector (USE_EXISTING)");
	    }
	    oldinst.objectId = term->contents.term.instanceId;
	    OCT_CHECK(octGetById(&oldinst));
	    inst.contents.instance.name = oldinst.contents.instance.name;
	    symReplaceInstance(&oldinst, &inst);
	    OCT_CHECK(octGenFirstContent(&inst, OCT_TERM_MASK, term));
	}
    } else if (type == SYM_CREATE) {
	/* create it */
	layerList[0].layer = *layer;
	layerList[0].direction = (direction == SYM_HORIZONTAL) ? TAP_HORIZONTAL : TAP_VERTICAL;
	layerList[0].width = width;
	if (tapGetConnector(1, layerList, &inst) != OCT_OK) {
	    errRaise(SYMLIB_PKG_NAME, SYM_ERROR, "can not get a connector (CREATE)");
	}
	inst.contents.instance.transform.translation.x = pt->x;
	inst.contents.instance.transform.translation.y = pt->y;
	symCreateInstance(facet, &inst);
	OCT_CHECK(octGenFirstContent(&inst, OCT_TERM_MASK, term));
    } else {
	errRaise(SYMLIB_PKG_NAME, SYM_ERROR, "bad type argument to symCreateSegment");
    }
    return SYM_OK;
}
    

void
symCreateSegment(layer, pt1, term1, type1, pt2, term2, type2, width, dir)
octObject *layer;
struct octPoint *pt1;
octObject *term1;
int type1;
struct octPoint *pt2;
octObject *term2;
int type2;
octCoord width;
int dir;
{
    octObject facet, net1, net2, seg;
    struct octPoint points[2];
    
    SYM_START;

    octGetFacet(layer, &facet);

    if ((pt1->x == pt2->x) && (pt1->y == pt2->y)) {
	switch (dir) {
	    case SYM_HORIZONTAL:
	    pt1->x--; pt2->x++;
	    break;

	    case SYM_VERTICAL:
	    pt1->y--; pt2->y++;
	    break;
	}
    }
    
    if (width < tapGetMinWidth(layer)) {
	width = tapGetMinWidth(layer);
    }
    
    /* handle end points */
    SYM_CHECK(sym_handle_segment_end_point(&facet, pt1, term1, type1, layer, width, dir));
    SYM_CHECK(sym_handle_segment_end_point(&facet, pt2, term2, type2, layer, width, dir));

    points[0] = *pt1;
    points[1] = *pt2;
        
    OCT_CHECK(ohCreatePath(layer, &seg, width));
    OCT_CHECK(octPutPoints(&seg, 2, points));

    symGetNet(term1, &net1);
    symGetNet(term2, &net2);
    sym_merge_nets(&net1, &net2);
    symGetNet(term1, &net1);
    symGetNet(term2, &net2);
    OCT_CHECK(octAttach(&seg, term1));
    OCT_CHECK(octAttach(&seg, term2));
    if (octIdIsNull(net1.objectId)) {
	if (octIdIsNull(net2.objectId)) {
	    symCreateNet(&facet, &net1);
	    OCT_CHECK(octAttachOnce(&net1, &seg));
	} else {
	    OCT_CHECK(octAttachOnce(&net2, &seg));
	}
    } else {
	OCT_CHECK(octAttachOnce(&net1, &seg));
    }	    

    SYM_END;

    return;
}
