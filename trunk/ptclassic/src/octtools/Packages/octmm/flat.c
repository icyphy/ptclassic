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
#include "oct.h"
#include "oh.h"
#include "tr.h"

/* Forward reference */
octStatus flatten_instance
	ARGS((octObject *facet, octObject *inst, tr_stack *stack));

octStatus flatten_contacts(facet)
octObject *facet;
{
    octStatus s;
    octObject bag, inst;
    octGenerator gen;
    tr_stack *stack = tr_create();

    s = ohGetByBagName(facet, &bag, "CONNECTORS");
    if (s == OCT_NOT_FOUND) return OCT_OK;
    else if (s != OCT_OK) return s;

    OH_ASSERT(octInitGenContents(&bag, OCT_INSTANCE_MASK, &gen));
    while ((s = octGenerate(&gen, &inst)) == OCT_OK) {
	tr_push(stack);
	tr_add_transform(stack, &inst.contents.instance.transform, 0);
	s = flatten_instance(facet, &inst, stack);
	if (s != OCT_OK) return s;
	tr_pop(stack);
	if ((s = octDelete(&inst)) != OCT_OK) return s;
    }
    tr_free(stack);
    return OCT_OK;
}

octStatus flatten_instance(facet, inst, stack)
octObject *facet;
octObject *inst;
tr_stack *stack;
{
    octObject master, layer, new_layer, geo;
    octGenerator gen, gen1;
    octPoint *point, *pts;
    octStatus s;
    int i;
    int32 num;

    s = ohOpenMaster(&master, inst, "contents", "r");
    if (s < OCT_OK) return s;

    s = octInitGenContents(&master, OCT_LAYER_MASK, &gen);
    while ((s = octGenerate(&gen, &layer)) == OCT_OK) {

	s = ohGetOrCreateLayer(facet, &new_layer, layer.contents.layer.name);
	if (s != OCT_OK) return s;

	s = octInitGenContents(&layer, OCT_GEO_MASK, &gen1);
	while ((s = octGenerate(&gen1, &geo)) == OCT_OK) {
	    switch(geo.type) {
		case OCT_BOX:
		    point = &geo.contents.box.lowerLeft;
		    tr_transform(stack, &(point->x), &(point->y));
		    point = &geo.contents.box.upperRight;
		    tr_transform(stack, &(point->x), &(point->y));
		    if ((s = octCreate(&new_layer, &geo)) != OCT_OK) return s;
		    break;
		
		case OCT_CIRCLE:	/* better than nothing ... */
		    point = &geo.contents.circle.center;
		    tr_transform(stack, &(point->x), &(point->y));
		    if ((s = octCreate(&new_layer, &geo)) != OCT_OK) return s;
		    break;

		case OCT_PATH:
		    if ((s = octCreate(&new_layer, &geo)) != OCT_OK) return s;
		    ohGetPoints(&geo, &num, &pts);
		    for(i = 0; i < num; i++) {
			tr_transform(stack, &(pts[i].x), &(pts[i].y));
		    }
		    if ((s = octPutPoints(&geo, num, pts)) != OCT_OK) return s;
		    break;

		case OCT_POLYGON:
		    if ((s = octCreate(&new_layer, &geo)) != OCT_OK) return s;
		    ohGetPoints(&geo, &num, &pts);
		    for(i = 0; i < num; i++) {
			tr_transform(stack, &(pts[i].x), &(pts[i].y));
		    }
		    if ((s = octPutPoints(&geo, num, pts)) != OCT_OK) return s;
		    break;

		case OCT_LABEL:
		    point = &geo.contents.label.region.lowerLeft;
		    tr_transform(stack, &(point->x), &(point->y));
		    point = &geo.contents.label.region.upperRight;
		    tr_transform(stack, &(point->x), &(point->y));
		    if ((s = octCreate(&new_layer, &geo)) != OCT_OK) return s;
		    break;
	    }
	}
	if (s != OCT_GEN_DONE) return s;
    }
    if (s != OCT_GEN_DONE) return s;
    return OCT_OK;
}
