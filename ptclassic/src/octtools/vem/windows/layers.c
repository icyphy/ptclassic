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
 * VEM Oct Window Mangement Routines
 * Layer list maintenence
 *
 * David Harrison
 * University of California, Berkeley
 * 1988
 *
 * Each VEM graphics window keeps a list of layers to be drawn in
 * priority order.  The routines in this file maintain these lists.
 * A special flag,  VEM_LYRUPDATE,  is used to mark a window for
 * layer update.  If this flag is set,  _wnLyrUpdate() should be
 * called to update all layers.  The flag is set using the
 * wnLyrChange() routine.
 */

#include "wininternal.h"	/* Internal window defintions */

/* Forward declarations for lint */
static void add_layers();
static void add_insts();
static void mark_facet();



static int priocmp(a, b)
char *a, *b;
/*
 * Comparison function used for sorting list of layers.
 */
{
    int aval = (*((tkLayer *) a))->priority;
    int bval = (*((tkLayer *) b))->priority;

    if (aval == bval) return 0;
    else if (aval > bval) return 1;
    else return -1;
}

void _wnLyrUpdate(i_win, fct_num)
internalWin *i_win;		/* Window to update     */
int fct_num;			/* What facet to update */
/*
 * This routine updates the list of layers for a specified
 * window.  The list contains not only those layers in the
 * facet associated with the window but those of all cells
 * instantiated beneath it.  This is to support proper
 * priority redisplay.  After the list has been compiled,
 * it is sorted in layer priority order.
 */
{
    octObject fct;
    tkHandle tech;
    st_table *layer_items;
    st_table *masters;

    /* Hash table of layers encountered */
    layer_items = st_init_table(st_ptrcmp, st_ptrhash);
    
    /* Hash table of masters encountered */
    masters = st_init_table(st_numcmp, st_numhash);

    fct.objectId = i_win->fcts[fct_num].winFctId;
    VEM_OCTCKRET(octGetById(&fct));
    
    /* Add the layers at this level to table */
    if (bufTech(fct.objectId, &tech) == VEM_OK) {
	daReset(i_win->fcts[fct_num].layers);
	add_layers(&fct, tech, i_win->fcts[fct_num].layers, layer_items);
	(void) st_insert(masters, (char *) fct.objectId, (char *) 0);

	/* Begin recursive decent */
	add_insts(&fct, i_win->fcts[fct_num].layers, layer_items, masters,
		  ((i_win->opts.disp_options & VEM_EXPAND) ?
		  "contents" : "interface"));

	/* Sort the layers in priority order */
	daSort(i_win->fcts[fct_num].layers, priocmp);

	/* Turn off the out-of-date flag */
	i_win->fcts[fct_num].fctbits &= (~F_ULYR);
    }
    /* Remove hash tables */
    st_free_table(layer_items);
    st_free_table(masters);
}



static void add_layers(fct, tech, lyr_list, lyr_table)
octObject *fct;			/* What facet to examine                 */
tkHandle tech;			/* Technology of facet                   */
dary lyr_list;			/* Array of bufLayerItem structures      */
st_table *lyr_table;		/* Hash table of bufLayerItem structures */
/*
 * This routine examines all layers attached to `fct',  and adds them
 * to `lyr_table' (if they are not already present).  If `id_flag'
 * is non-zero,  the `lyrId' field of the structure is filled with
 * the appropriate value.  Otherwise,  it is set to oct_null_id.
 */
{
    octGenerator gen;
    octObject layer;
    tkLayer tk_layer;

    VEM_OCTCKRET(octInitGenContents(fct, OCT_LAYER_MASK, &gen));
    while (octGenerate(&gen, &layer) == OCT_OK) {
	tk_layer = tkLyr(tech, &layer);
	if (!st_is_member(lyr_table, (char *) tk_layer)) {
	    *daSetLast(tkLayer, lyr_list) = tk_layer;
	    (void) st_insert(lyr_table, (char *) tk_layer, (char *) 0);
	}
    }
}


static void add_insts(fct, lyr_list, lyr_tbl, mas_tbl, fct_name)
octObject *fct;			/* What facet to search          */
dary lyr_list;			/* Dynamic array of bufLayerItem */
st_table *lyr_tbl;		/* Hash table of bufLayerItem    */
st_table *mas_tbl;		/* Masters already searched      */
char *fct_name;			/* What facet to open            */
/*
 * This routine examines all instances in `fct'.  Those whose
 * master's have not been examined are examined using add_layers.
 */
{
    octGenerator gen;
    octObject inst, master;
    tkHandle tech;

    VEM_OCTCKRET(octInitGenContents(fct, OCT_INSTANCE_MASK, &gen));
    master.type = OCT_FACET;
    while (octGenerate(&gen, &inst) == OCT_OK) {
	if (strcmp(fct_name, "interface") == 0) 
	  master.contents.facet.facet = inst.contents.instance.facet;
	else
	  master.contents.facet.facet = fct_name;
	master.contents.facet.mode = "r";
	if (bufOpenMaster(&inst, &master, BUFREADONLY) == VEM_OK) {
	    /* Check to see if already examined */
	    if (!st_is_member(mas_tbl, (char *) master.objectId)) {
		if (bufTech(master.objectId, &tech) == VEM_OK) {
		    add_layers(&master, tech, lyr_list, lyr_tbl);
		    /* Recursive portion */
		    add_insts(&master, lyr_list, lyr_tbl, mas_tbl, fct_name);
		    /* Add to master table */
		    st_insert(mas_tbl, (char *) master.objectId, (char *) 0);
		}
	    }
	}
    }
}



/*ARGSUSED*/
static enum st_retval win_search(key, value, arg)
char *key, *value, *arg;
/*
 * This routine is passed to st_foreach to search for
 * a given facet id.  If it is found,  it is marked
 * as out of date.
 */
{
    register internalWin *win_info = (internalWin *) value;
    register int i;
    
    for (i = 0;  i < win_info->num_fcts;  i++) {
	if (win_info->fcts[i].winFctId == (octId) arg) {
	    win_info->fcts[i].fctbits |= F_ULYR;
	}
    }
    return ST_CONTINUE;
}

static vemStatus mark_inst(inst, arg)
octObject *inst;		/* What instance   */
char *arg;			/* Passed argument */
/*
 * This routine is passed to bufInstances to generate through
 * all instances of a buffer (upward in the heirarchy).
 * The passed argument is a list of masters we have already
 * examined.  If the instance master is not in this table,
 * it is marked recursively.
 */
{
    st_table *mastbl = (st_table *) arg;
    octObject master;

    octGetFacet(inst, &master);
    if (!st_is_member(mastbl, (char *) master.objectId)) {
	mark_facet(master.objectId, mastbl);
    }
    return VEM_OK;
}

static void mark_facet(fct_id, mastbl)
octId fct_id;			/* What facet to mark               */
st_table *mastbl;		/* Masters we have already examined */
{
    _wnForeach(WN_ACTIVE | WN_INACTIVE | WN_INTR, win_search, (char *) fct_id);
    st_insert(mastbl, (char *) fct_id,  (char *) 0);

    /* Now recursively traverse instances of `fct_id' */
    bufInstances(fct_id, mark_inst, (char *) mastbl);
}


void wnLyrChange(fct_id)
octId fct_id;			/* Facet identifier */
/*
 * This routine searches for `fct_id' and all its
 * ancestors in the design heirarchy and marks
 * those that match as requiring layer updates.
 */
{
    st_table *mastbl = (st_table *) 0;

    /* Hash table for already encountered masters */
    mastbl = st_init_table(st_numcmp, st_numhash);

    mark_facet(fct_id, mastbl);
    st_free_table(mastbl);
}
