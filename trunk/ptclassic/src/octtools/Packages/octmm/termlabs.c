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
#include "copyright.h"
#include "port.h"
#include "oct.h"
#include "oh.h"

/* Forward references */
int map_term
	ARGS((octObject *facet, octObject *fterm, double scale));

void map_terminals_to_labels(prog_name, facet, scale)
char *prog_name;
octObject *facet;
double scale;
{
    octObject interface, fterm, fterm1;
    octGenerator gen;
    octStatus status;
    int not_found = 1;

    /*
     *  Generate over all formal terminals
     */
    OH_ASSERT(octInitGenContents(facet, OCT_TERM_MASK, &gen));
    while ((status = octGenerate(&gen, &fterm)) == OCT_OK) {

	not_found = map_term(facet, &fterm, 1.0);
	if (not_found) {
	    /* If no geometry found, check the interface facet */
	    status = ohOpenFacet(&interface, facet->contents.facet.cell,
			    facet->contents.facet.view, "interface", "r");
	    if (status >= OCT_OK && status != OCT_NEW_FACET) {
		status = ohGetByTermName(&interface, &fterm1, 
					    fterm.contents.term.name);
		if (status >= OCT_OK) {
		    not_found = map_term(facet, &fterm1, scale);
		}
	    }
	}
	if (not_found) {
	    (void) fprintf(stderr, 
		"%s: (warning only) no implementation for %s found\n",
		    prog_name, fterm.contents.term.name);
	}
    }
    OH_ASSERT(status == OCT_GEN_DONE ? OCT_OK : status);
}

    
/*
 *  fterm is a formal terminal in either the contents or interface
 *	
 */

int
map_term(facet, fterm, scale)
octObject *facet;
octObject *fterm;
double scale;
{
    octObject geo, layer, label;
    octGenerator gen;
    octStatus status;
    struct octBox bb;
    int not_found;
    char *name;


    /*
     *  Generate over all geometries attached to this formal terminal
     */
    not_found = 1;
    OH_ASSERT(octInitGenContents(fterm, OCT_GEO_MASK, &gen));
    while ((status = octGenerate(&gen, &geo)) == OCT_OK) {

	/* allow the geo to disappear when it is deleted from the layer */
	OH_ASSERT(octDetach(fterm, &geo));

	if (not_found) {
	    if (octBB(&geo, &bb) < 0) {
		octError("Getting bbox of implementing geometry");
		continue;
	    }
	    if (octGenFirstContainer(&geo, OCT_LAYER_MASK, &layer) < 0) {
		octError("Getting layer of implementing geometry");
		continue;
	    }
	    bb.lowerLeft.x *= scale;
	    bb.lowerLeft.y *= scale;
	    bb.upperRight.x *= scale;
	    bb.upperRight.y *= scale;
	    /*
	    cx = scale * (bb.lowerLeft.x + bb.upperRight.x) / 2;
	    cy = scale * (bb.lowerLeft.y + bb.upperRight.y) / 2;
	    */
	    name = fterm->contents.term.name;

	    /* Get the layer in the original contents facet */
	    OH_ASSERT(octGetOrCreate(facet, &layer));

	    OH_ASSERT(ohCreateLabel(&layer, &label, name, bb, 10, OCT_JUST_CENTER, OCT_JUST_LEFT, OCT_JUST_LEFT));
	    not_found = 0;
	}
    }
    OH_ASSERT(status == OCT_GEN_DONE ? OCT_OK : status);
    return not_found;
}
