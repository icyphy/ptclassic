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
/*
 * flatten.c -- Routines to flatten layers across OCT hierarchy.
 *			Brian C. Richards, June 21, 1988
 */

#include "copyright.h"
#include "port.h"
#include "utility.h"
#include "st.h"
#include "oct.h"
#include "oh.h"
#include "fang.h"

#define foreach_obj(container, mask, gen, obj)\
    OH_ASSERT(octInitGenContents(container, mask, gen));\
    while (octGenerate(gen, obj) == OCT_OK)

/* Forward reference */
void initFlattenLayers();

static int		layer_id;
static int		saveFlatLayer();
static fa_box		*findFlatLayer();

void
flattenCell()
{
	layer_id = 0;
}


void
flattenSubcellLayer(facet, layer, flat_geos, subcell_geos)
	octObject	*facet;
	fa_geometry	*layer;
	fa_geometry	*flat_geos;
	fa_geometry	*subcell_geos;
{
	octGenerator	instGen;
	octObject	inst;
	octObject	box;
	fa_box		*first_box, *fang_box;
	static fa_box	*findFlatLayer();

	++layer_id;

	/* Initialize internal data structures. */
	initFlattenLayers();
	fa_init(flat_geos);
	fa_init(subcell_geos);
	box.type = OCT_BOX;

	/* Flatten the corresponding boxes for the current instance. */
	foreach_obj(facet, OCT_INSTANCE_MASK, &instGen, &inst) {
		first_box = findFlatLayer(inst.contents.instance.master);
		if (!first_box) {
			continue;
		}

		for (fang_box = first_box; fang_box; fang_box = fang_box->next)
		{
			/*
			 * Convert each fang box into an oct box, transform
			 * the box to the parent coordinate system, and
			 * store the flattened box back in a fang geometry.
			 */
			box.contents.box.lowerLeft.x = fang_box->left;
			box.contents.box.lowerLeft.y = fang_box->bottom;
			box.contents.box.upperRight.x = fang_box->right;
			box.contents.box.upperRight.y = fang_box->top;

			octTransformGeo(&box, &inst.contents.instance.transform);

			fa_add_box(subcell_geos,
					box.contents.box.lowerLeft.x,
					box.contents.box.lowerLeft.y,
					box.contents.box.upperRight.x,
					box.contents.box.upperRight.y);
		}
	}

	/* Flatten all related boxes. */
	fa_or(layer, subcell_geos, flat_geos);

	/* Save the boxes for higher level calls. */
	saveFlatLayer(facet->contents.facet.cell, flat_geos);

	return;
}

static st_table	*flat_table = NIL(st_table);	/* Symbol table of flattened layers. */

void
initFlattenLayers()
{
	if (!flat_table) {
		flat_table = st_init_table(strcmp, st_strhash);
	}
}

void
reinitFlattenLayers()
{
	static enum st_retval	freeFlatLayer();

	if (flat_table) {
		/* Free any existing symbol table entries. */
		(void) st_foreach(flat_table, freeFlatLayer, NIL(char));
		st_free_table(flat_table);
	}

	flat_table = st_init_table(strcmp, st_strhash);
}

/* ARGSUSED */
static enum st_retval
freeFlatLayer(key, value, arg)
	char	*key;
	char	*value;
	char	*arg;
{
	fa_box	*boxes = (fa_box *) value;

	fa_free_boxes(boxes);
	return (ST_CONTINUE);
}

static int
saveFlatLayer(name, geo)
	char		*name;
	fa_geometry	*geo;
{
	char	temp_name[100];
	fa_box	*fang_boxes;

	fa_to_boxes(geo, &fang_boxes);

	/* There are boxes, so save them in the flattened symbol table. */
	(void) sprintf(temp_name, "%d_%s", layer_id, name);
	if (st_insert(flat_table, util_strsav(temp_name), (char *) fang_boxes)) {
		(void) fprintf(stderr,
			"octmm internal error: saveFlatLayer: '%s' reused.\n",
			temp_name);
		return (-1);
	}

	return (0);
}

static fa_box *
findFlatLayer(name)
	char	*name;
{
	char	temp_name[100];
	char    *ptr;

	(void) sprintf(temp_name, "%d_%s", layer_id, name);
	if (st_lookup(flat_table, temp_name, &ptr) == 0) {
		(void) fprintf(stderr,
			       "octmm internal error: findFlatLayer: '%s' not found.\n",
			       temp_name);
		return (NIL(fa_box));
	}

	return ((fa_box *) ptr);
}
