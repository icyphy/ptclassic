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
/*LINTLIBRARY*/
/*
 * Terminal Information
 *
 * David Harrison
 * University of California,  Berkeley
 * 1987
 *
 * This file exports routines for finding information about instance
 * actual terminals.  It is a replacement of older code.
 */

#include "port.h"
#include "utility.h"
#include "oct.h"
#include "oh.h"
#include "region.h"

#include "internal.h"
#include "gen.h"
#include "contact.h"
#include "terminfo.h"


/* A terminal generator actually looks like this: */

typedef struct term_internal_defn {
    struct octTransform inst_trans;
    octGenerator geo_gen;
} term_internal;



void symInitTerm(a_term, t_gen)
octObject *a_term;		/* Instance terminal  */
symTermGen *t_gen;		/* Returned generator */
/*
 * Starts a generation sequence for returning the implementations
 * of an actual terminal by examining the interface of the
 * instance master.  The routine will raise error conditions
 * if there are errors.
 */
{
    term_internal *real_gen;
    octObject term_inst, inst_master, f_term;
    octGenerator gen;

    term_inst.objectId = a_term->contents.term.instanceId;
    SYMCK(octGetById(&term_inst));

    symInterface(&term_inst, &inst_master);

    if ( ohGetTerminal(&inst_master, a_term->contents.term.formalExternalId, &f_term) < OCT_OK ) {
	errRaise(sym_pkg_name, SYM_OCTFAIL, "Cannot get %s (Xid %d)\nfrom %s\n",
		 ohFormatName( a_term ),
		 a_term->contents.term.formalExternalId, ohFormatName( &inst_master ) );
    }

    SYMCK(octInitGenContents(&f_term, OCT_GEO_MASK, &gen));
    
    real_gen = ALLOC(term_internal, 1);
    real_gen->inst_trans = term_inst.contents.instance.transform;
    real_gen->geo_gen = gen;
    *t_gen = (symTermGen) real_gen;
}



int symNextTerm(gen, lyr, geo)
symTermGen gen;			/* Terminal generation state */
octObject *lyr;			/* Filled in layer object    */
octObject *geo;			/* Filled in geometry        */
/*
 * This routine generates the next implementation geometry
 * found for the actual terminal specified in symInitTerm().
 * If non-zero,  `lyr' will be filled with the implementation
 * layer for `geo'.  `geo', which must be non-zero,  will be filled with
 * an implementation geometry properly transformed into the
 * instance coordinate system.  Returns zero if there
 * are no more geometries to generate.
 */
{
    octStatus ret_code;

    ret_code = octGenerate(&(((term_internal *) gen)->geo_gen), geo);
    if (ret_code == OCT_GEN_DONE) return 0;

    /* Transform geo */
    octTransformGeo(geo, &(((term_internal *) gen)->inst_trans));

    /* Look for layer if requested */
    if (lyr) {
	SYMCK(octGenFirstContainer(geo, OCT_LAYER_MASK, lyr));
    }
    return 1;
}



extern void symEndTerm(gen)
symTermGen gen;			/* Generator to free */
/*
 * Releases all resources associated with `gen'.  This should
 * be done after all generation sequences.
 */
{
    SYMCK(octFreeGenerator(&(((term_internal *)gen)->geo_gen)));
    FREE(gen);
}




void symSegTerms(segment, points, info)
octObject *segment;		/* Segment to examine             */
struct octPoint *points;	/* Segment endpoints              */
segTermInfo info[2];		/* Segment terminal info (RETURN) */
/*
 * This routine examines `segment' and returns information about
 * corresponding terminals in `info'.  If there is no corresponding
 * terminal,  the terminal object in `info' will have the octId
 * oct_null_id.  Does not guarantee the segment endpoints intersect
 * the terminals (just connectivity).
 */
{
    octGenerator gen;
    octObject dummy_term, lyr, geo;
    struct seg_term_info temp;
    symTermGen t_gen;
    int count, i, layers;

    /* Initialize information */
    for (i = 0;  i < 2;  i++) {
	info[i].flags = 0;  info[i].term.objectId = oct_null_id;
    }

    /* Look at terminals beneath segment */
    SYMCK(octInitGenContents(segment, OCT_TERM_MASK, &gen));
    for (count = 0;
	 (octGenerate(&gen, ((count < 2) ?
			      &(info[count].term) :
			      &dummy_term)) == OCT_OK);
	 count++)
      /* Null loop body */;
    if (count >= 3) {
	errRaise(sym_pkg_name, SYM_BADPOL, "Too many segment terminals");
	/*NOTREACHED*/
    }

    /* Find terminal information */
    for (i = 0;  i < count;  i++) {
	info[i].term_bb.lowerLeft.x = info[i].term_bb.lowerLeft.y = OCT_MAX_COORD;
	info[i].term_bb.upperRight.x = info[i].term_bb.upperRight.y = OCT_MIN_COORD;
	
	symInitTerm(&(info[i].term), &t_gen);
	layers = 0;
	while (symNextTerm(t_gen, &lyr, &geo)) {
	    if (geo.type == OCT_BOX) {
		/* update extent */
		LA(geo.contents.box.lowerLeft.x, info[i].term_bb.lowerLeft.x);
		LA(geo.contents.box.lowerLeft.y, info[i].term_bb.lowerLeft.y);
		GA(geo.contents.box.upperRight.x, info[i].term_bb.upperRight.x);
		GA(geo.contents.box.upperRight.y, info[i].term_bb.upperRight.y);

		/* assumes one box per layer -- could be bogus */
		layers++;
	    } else {
		/* Terminal implementation is not a box */
		errRaise(sym_pkg_name, SYM_BADPOL,
			 "Terminal %s: implementation is a %s. It should be a box",
			 ohFormatName(&(info[i].term)), ohFormatName(&geo));
		/*NOTREACHED*/
	    }
	}
	if (layers > 1) info[i].flags |= SEG_AT_MULTI;
	if (symContactP(&(info[i].term))) info[i].flags |= SEG_AT_CONNECTOR;
	symEndTerm(t_gen);
    }
	    
    /* Match terminal endpoints */
    if (count >= 1) {
	struct octPoint center;

	center.x =
	  (info[0].term_bb.lowerLeft.x + info[0].term_bb.upperRight.x)/2;
	center.y =
	  (info[0].term_bb.lowerLeft.y + info[0].term_bb.upperRight.y)/2;
	if (DIST(points[1], center) < DIST(points[0], center)) {
	    /* Swap terminal information */
	    temp = info[0];  info[0] = info[1];  info[1] = temp;
	}
    }
}



#ifdef PRT
/* Printing functions for debugging */

static char *sflags(flags)
int flags;
{
    static char sbuf[256];

    sbuf[0] = '\0';
    if (flags & SEG_AT_CONNECTOR) {
	strcat(sbuf, "|SEG_AT_CONNECTOR");
    }
    if (flags & SEG_AT_MULTI) {
	strcat(sbuf, "|SEG_AT_MULTI");
    }
    if (sbuf[0] == '|') {
	return &(sbuf[1]);
    } else {
	return &(sbuf[0]);
    }
}

int prt_segterminfo(ptr, val)
char *ptr;
char *val;
{
    char *start = ptr;
    segTermInfo *si = (segTermInfo *) val;

    ptr += prt_str(ptr, "[(seg_term_info) flags = ");
    ptr += prt_str(ptr, sflags(si->flags));
    ptr += prt_str(ptr, ", term = ");
    ptr += prt_oct(ptr, (char *) &(si->term));
    ptr += prt_str(ptr, ", term_bb = ");
    ptr += prt_box(ptr, &(si->term_bb));
    ptr += prt_str(ptr, "]");
    return (int) (ptr - start);
}

#endif /* PRT */
