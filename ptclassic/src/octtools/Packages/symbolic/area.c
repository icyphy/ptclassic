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
/*
 * Region searching for symbolic
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 *
 * This file exports routines for locating symbolic objects in
 * a region of a symbolic facet.  It provides a convenient interface
 * above the low-level facilities provided by the region package.
 */

#include "port.h"
#include "utility.h"
#include "region.h"

#include "internal.h"
#include "terminfo.h"
#include "area.h"

/*
 * Symbolic region generator
 */

typedef enum sym_area_state_defn {
    AREA_GEN_DONE,		/* No more objects to generate */
    AREA_NORM_GEN,		/* Generate normal objects     */
    AREA_ATERM_GEN		/* Generate actual terminals   */
} symAreaState;

typedef struct sym_int_area_defn {
    symAreaState state;		/* Generation state                    */
    octObjectMask mask;		/* Mask passed to symAreaInit          */
    struct octBox area;		/* Area passed to symAreaInit          */
    regObjGen gen;		/* Underlying region generator         */
    octGenerator aterm_gen;	/* Generator for actual terminals      */
} symIntArea;



void symAreaInit(fct, mask, area, gen)
octObject *fct;			/* Symbolic facet             */
octObjectMask mask;		/* What objects to look for   */
struct octBox *area;		/* Area to look inside of     */
symAreaGen *gen;		/* Generator state (returned) */
/*
 * Initializes a generation sequence that returns symbolic objects
 * that intersect the box given by `area'.  Only those objects
 * whose type is one of those defined in `mask' will be returned.
 * The following types are supported:
 *   OCT_PATH_MASK:	 Symbolic segments
 *   OCT_INSTANCE_MASK:  Symbolic instances and connectors
 *   OCT_TERM_MASK:      Symbolic actual terminals
 * The objects in a generation sequence are returned by successive
 * calls to symAreaNext().  A sequence is terminated by symAreaEnd().
 */
{
    symIntArea *atg;
    octObjectMask real_mask;

    if (mask & (~(OCT_INSTANCE_MASK|OCT_PATH_MASK|OCT_TERM_MASK))) {
	symMsg(SYM_ERR, "Inappropriate object mask for symAreaInit");
    }
    atg = ALLOC(symIntArea, 1);
    atg->mask = mask & (OCT_INSTANCE_MASK|OCT_PATH_MASK|OCT_TERM_MASK);
    atg->area = *area;
    real_mask = mask;
    if (mask & OCT_TERM_MASK) {
	real_mask &= ~OCT_TERM_MASK;
	real_mask |= OCT_INSTANCE_MASK;
    }
    if (regObjStart(fct, area, real_mask, &(atg->gen), 0) == REG_OK) {
	atg->state = AREA_NORM_GEN;
    } else {
	atg->state = AREA_GEN_DONE;
    }
    *gen = (symAreaGen) atg;
}



int symAreaNext(gen, obj)
symAreaGen gen;			/* Generator         */
octObject *obj;			/* Object (returned) */
/*
 * Returns the next object in the area sequence defined
 * by symAreaInit().  If there are no more objects, the
 * routine will return zero.  In all cases, after generating
 * items using symAreaNext(), the user should call symAreaEnd()
 * to reclaim resources consumed by the generator handle.
 */
{
    symIntArea *atg = (symIntArea *) gen;
    symTermGen sgen;
    octObject lyr, geo;
    int result;

    switch (atg->state) {
    case AREA_GEN_DONE:
	return 0;
    case AREA_NORM_GEN:
	if (regObjNext(atg->gen, obj) == REG_OK) {
	    if ((atg->mask & OCT_TERM_MASK) && (obj->type == OCT_INSTANCE)) {
		/* Terminal generation */
		SYMCK(octInitGenContents(obj, OCT_TERM_MASK, &(atg->aterm_gen)));
		atg->state = AREA_ATERM_GEN;
	    }
	    if (atg->mask & OCT_INSTANCE_MASK) {
		return 1;
	    } else {
		/* Only looking for actual terminals */
		return symAreaNext(gen, obj);
	    }
	} else {
	    atg->state = AREA_GEN_DONE;
	    return 0;
	}
	/*NOTREACHED*/
    case AREA_ATERM_GEN:
	result = 0;
	while (!result && (octGenerate(&(atg->aterm_gen), obj) == OCT_OK)) {
	    symInitTerm(obj, &sgen);
	    while (!result && symNextTerm(sgen, &lyr, &geo)) {
		if ((geo.type == OCT_BOX) &&
		    SYMBOXINTERSECT(geo.contents.box, atg->area)) {
		    result = 1;
		}
	    }
	    symEndTerm(sgen);
	}
	if (!result) {
	    atg->state = AREA_NORM_GEN;
	    return symAreaNext(gen, obj);
	} else {
	    return result;
	}
    }
    /*NOTREACHED*/
    return 0;
}



void symAreaEnd(gen)
symAreaGen gen;			/* Generator */
/*
 * Frees up resources consumed by the generator `gen'.
 */
{
    symIntArea *atg = (symIntArea *) gen;

    if (atg->state == AREA_ATERM_GEN) {
	octFreeGenerator(&(atg->aterm_gen));
    }
    regObjFinish(atg->gen);
    FREE(atg);
}
