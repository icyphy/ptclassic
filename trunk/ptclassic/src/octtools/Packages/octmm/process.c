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
#include "utility.h"
#include "oct.h"
#include "st.h"
#include "oh.h"
#include "vov.h"
#include "errtrap.h"
#include "octmm.h"

extern char *optProgName;

extern octStatus flatten_contacts();	/* from flat.c */
extern int force_remake;		/* from main.c */
extern int process_subcells;		/* from main.c */
extern int remove_connectors;		/* from main.c */

static st_table *cells_processed = 0;
static void process_recur();


void
process(old_cell, old_view, new_view, scale, filename)
char *old_cell;
char *old_view;
char *new_view;
char *scale;
char *filename;
{
    cells_processed = st_init_table(strcmp, st_strhash);
    process_recur(old_cell, old_view, new_view, scale, filename);
    st_free_table(cells_processed);
}


void
myModifyInstance(instance, new_cell, new_view)
octObject *instance;
char *new_cell;
char *new_view;
{
    octObject new_inst;

    new_inst = *instance;
    new_inst.contents.instance.master = new_cell;
    new_inst.contents.instance.view = new_view;

    ohReplaceInstance(&new_inst, instance);
    *instance = new_inst;

    return;
}


/*
 *  process: process a cell
 *
 *  That is, 
 *	1. copy the (old_cell, old_view) to (old_cell, new_view)
 *	2. Recursively process all instances in (old_cell, new_view);
 *	3. Re-create each instance
 *	4. Perform the mask operations on this cell
 */

static void process_recur(old_cell, old_view, new_view, scale, filename)
char *old_cell;
char *old_view;
char *new_view;
char *scale;
char *filename;
{
    char *insert_name, *inst_cell, *inst_view;
    octObject inst, facet, master, layer;
    octStatus status = OCT_OK;
    octGenerator gen;
    octId *inst_array;
    int i, ninst;

    /*
     *  Copy the old cell and then open it for changing
     */
    if (strcmp(old_view, new_view) != 0) {
	octObject new, old;

	old.type = new.type = OCT_FACET;
	old.contents.facet.cell = old_cell;
	new.contents.facet.cell = old_cell;
	old.contents.facet.view = old_view;
	new.contents.facet.view = new_view;
	old.contents.facet.facet = "contents";
	new.contents.facet.facet = "contents";
	old.contents.facet.version = OCT_CURRENT_VERSION;
	new.contents.facet.version = OCT_CURRENT_VERSION;
	old.contents.facet.mode = "r";
	new.contents.facet.mode = "w";
	VOVinputFacet( &old );
	VOVoutputFacet( &new );
	if (octCopyFacet(&new, &old) < OCT_OK) {
	    errRaise( "octmm", 1, "Cannot copy %s into %s", ohFormatName(&old),ohFormatName(&new) );
	}
    }
    
    OH_ASSERT(ohOpenFacet(&facet, old_cell, new_view, "contents", "a"));

    VOVinputFacet( &facet );
    VOVoutputFacet( &facet );

    /* My guess is that this is do nothing code - cxh */
    if (status == OCT_INCONSISTENT) {
	ohPrintInconsistent(&facet, stdout);
	exit(-1);
    }

    if (process_subcells) {

	/*
	 *  Blow away contacts 
	 */
	if (remove_connectors) {
	    OH_ASSERT(flatten_contacts(&facet));
	}

	/* 
	 *  Gen all instances into an array to avoid suprises when re-creating
	 */
	ohContentsToArray(&facet, OCT_INSTANCE_MASK, &inst_array, &ninst);

	/* 
	 *  Recursively process instances, and then re-create the instances
	 */
	for(i = 0; i < ninst; i++) {
	    OH_ASSERT(ohGetById(&inst, inst_array[i]));
	    inst_cell = inst.contents.instance.master;
	    inst_view = inst.contents.instance.view;
	    if (st_lookup(cells_processed, inst_cell, NIL(char *)) == 0) {
		if (force_remake) {
		    process_recur(inst_cell, inst_view, new_view, 
					scale, filename);
		} else {
		    status = ohOpenFacet(&master, inst_cell, new_view, 
						"contents","r");
		    VOVinputFacet( &master );
		    if (status == OCT_NO_EXIST) {
			process_recur(inst_cell, inst_view, new_view, 
					    scale, filename);
		    } else if (status == OCT_OLD_FACET || OCT_ALREADY_OPEN) {
			(void) st_insert(cells_processed, inst_cell, NIL(char));
		    } else {
			errRaise( "octmm", 1, "Cannot open %s", ohFormatName(&master) );
		    }
		}
	    }
	    /* Now re-create the instance with the new view */
	    myModifyInstance(&inst, inst_cell, new_view);
	}
	FREE(inst_array);
    }

    /*
     *  Now do the mask-modification stuff
     */
    oct_mask_mod(optProgName, filename, &facet, scale);

    /*
     * garbage collect layers
     */
    OH_ASSERT(octInitGenContents(&facet, OCT_LAYER_MASK, &gen));
    while (octGenerate(&gen, &layer) ==  OCT_OK) {
	if (ohCountContents(&layer, OCT_GEO_MASK) == 0) {
	    OH_ASSERT(octDelete(&layer));
	}
    }
    
    OH_ASSERT(octFlushFacet(&facet));

    insert_name = util_strsav(old_cell);
    (void) st_insert(cells_processed, insert_name, NIL(char));

    return;
}
