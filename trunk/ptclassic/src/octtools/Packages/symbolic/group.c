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
 * Group netlisting operations
 *
 * David Harrison
 * University of California,  Berkeley
 *
 * This file implements functions that are used in symbolic editing
 * when dealing with groups of items with connectivity information.
 * These functions are used to move, copy, and delete objects
 * efficiently and correctly.
 */

#include "list.h"
#include "st.h"
#include "oct.h"
#include "oh.h"

#include "internal.h"
#include "net.h"
#include "contact.h"
#include "group.h"

/* Forward declarations */

/*static octStatus find_cut_set();*/
static void inst_cut_set();
static void path_cut_set();



void symCutSet(grp_bag, cut_set)
octObject *grp_bag;		/* Bag of items forming set      */
octObject *cut_set;		/* Returned bag of cut set paths */
/*
 * This routine locates those paths which are on the boundary of
 * a set of objects given by `grp_bag'.  In other words,  those paths
 * with one end-point in the set and the other outside the set.
 * This is done by examining each object in the set.  Objects
 * other than instances and paths are ignored.  For each instance,
 * all paths connected to its actual terminals are examined.  If
 * the path is not in the group bag,  it is eligible for
 * the cut set.  For each path,  the instances connected to its
 * end-points are examined.  If any of them are in the group
 * set bag,  the path is placed in the cut set.  `cut_set' is
 * assumed to already exist.
 */
{
    octObject obj;
    octGenerator gen;

    SYMCK(octInitGenContents(grp_bag, OCT_ALL_MASK, &gen));
    while (octGenerate(&gen, &obj) == OCT_OK) {
	if (obj.type == OCT_INSTANCE) {
	    inst_cut_set(&obj, grp_bag, cut_set);
	} else if (obj.type == OCT_PATH) {
	    path_cut_set(&obj, grp_bag, cut_set);
	}
    }
}



static void inst_cut_set(inst, grp_bag, cut_set)
octObject *inst;		/* Instance to check */
octObject *grp_bag;		/* Initial group     */
octObject *cut_set;		/* Cut set bag       */
/*
 * This routine examines all of the actual terminals of
 * `inst' and generates all paths which are connected
 * to them.  Each path is then subjected to the path
 * cut set test (see path_cut_set).
 */
{
    octObject a_term, path;
    octGenerator gen, sub_gen;

    SYMCK(octInitGenContents(inst, OCT_TERM_MASK, &gen));
    while (octGenerate(&gen, &a_term) == OCT_OK) {
	SYMCK(octInitGenContainers(&a_term, OCT_PATH_MASK, &sub_gen));
	while (octGenerate(&sub_gen, &path) == OCT_OK) {
	    path_cut_set(&path, grp_bag, cut_set);
	}
    }
}



static void path_cut_set(path, grp_bag, cut_set)
octObject *path;		/* Path to check */
octObject *grp_bag;		/* Initial group */
octObject *cut_set;		/* Cut set bag   */
/*
 * This routine locates all instances connected to `path'.  If
 * any of theses instances are NOT connected to `grp_bag' or
 * if the path itself is not connected to `grp_bag',  the
 * path is added to `cut_set'.  Also,  if the path has at least
 * one unconnected end-point it is suitable.  
 */
{
    octObject a_term, inst;
    octGenerator gen;
    int count = 0, flag = 0;

    if (octIsAttached(grp_bag, path) == OCT_OK) {
	/* Only on cut set if at least one end-point isn't in group */
	SYMCK(octInitGenContents(path, OCT_TERM_MASK, &gen));
	while (octGenerate(&gen, &a_term) == OCT_OK) {
	    inst.objectId = a_term.contents.term.instanceId;
	    SYMCK(octGetById(&inst));
	    if (octIsAttached(grp_bag, &inst) != OCT_OK) {
		flag = 1;
	    }
	    count++;
	}
    } else {
	/* Automatically on cut set */
	flag = 1;
    }
    if (flag || (count < 2)) {
	/* Path is suitable */
	SYMCK(octAttachOnce(cut_set, path));
    }
}



void symBreakSet(cut_set, grp_bag, mapping)
octObject *cut_set;		/* Bag of paths on the cut set */
octObject *grp_bag;		/* Original set of objects     */
st_table *mapping;		/* Path to term mapping        */
/*
 * This routine generates through paths on the cut set and
 * breaks the nets appropriately.  Breaking a net causes
 * an additional net to be created.  The connections to
 * this net will be inward (toward the group) by default
 * but may be reversed if jump terminals are involved.  There are
 * some nasty assumptions about the correctness of the symbolic
 * policy in the cell here.  If `mapping' is non-zero, the routine 
 * fills the hash table with mappings between path ids in the cut 
 * set to the terminal ids they were severed from.  This information
 * is used by symUnbreakSet() to reconnect the group.  This hash
 * table should be initialized using st_numhash and st_numcmp.
 */
{
    octObject term, path, inst, net, new_net;
    octGenerator gen, sub_gen;
    int included_flag;
    lsList term_list;

    SYMCK(octInitGenContents(cut_set, OCT_PATH_MASK, &gen));
    while (octGenerate(&gen, &path) == OCT_OK) {
	included_flag = (octIsAttached(grp_bag, &path) == OCT_OK);
	SYMCK(octInitGenContents(&path, OCT_TERM_MASK, &sub_gen));
	while (octGenerate(&sub_gen, &term) == OCT_OK) {
	    SYMCK(octGenFirstContainer(&path, OCT_NET_MASK, &net));
	    inst.objectId = term.contents.term.instanceId;
	    SYMCK(octGetById(&inst));
	    if (included_flag ^ (octIsAttached(grp_bag, &inst) == OCT_OK)) {
		/* Disconnect */
		if (mapping) {
		    if (!st_lookup(mapping, (char *) path.objectId,
				   (char **) &term_list)) {
			/* Not there -- add */
			term_list = lsCreate();
			(void) st_insert(mapping, (char *) path.objectId,
					 (char *) term_list);
		    }
		    lsNewEnd(term_list, (lsGeneric) term.objectId, LS_NH);
		}
		SYMCK(octDetach(&path, &term));
		/* Attempt to break in one direction */
		symCreateNamedNet(&path, &new_net);
		if (!symBreakNet((included_flag ? &path : &term),
			       &net, &new_net, (mapping == (st_table *) 0))) {
		    /* Attempt to break the other way */
		    (void) symBreakNet((included_flag ? &term : &path),
				       &net, &new_net,
				       (mapping == (st_table *) 0));
		} 
	    }
	}
    }
}



void symUnbreakSet(cut_set, grp_bag, mapping)
octObject *cut_set;		/* Bag of paths on cut set */
octObject *grp_bag;		/* Original set of objects */
st_table *mapping;		/* Reconnection table      */
/*
 * This routine reconnects a group that was cut using symBreakSet().
 * The paths in the cut set are reattached to the terminals they
 * were originally connected to and nets are merged appropriately.
 * `mapping' is a table generated by symBreakSet() that maps the
 * paths to their terminals.  The hash table should be destroyed
 * using st_free_table() after use (any internally allocated
 * items will be freed).
 */
{
    octObject path, term, path_net, term_net, inst;
    octGenerator gen;
    lsList term_list;
    lsGen term_gen;

    SYMCK(octInitGenContents(cut_set, OCT_PATH_MASK, &gen));
    while (octGenerate(&gen, &path) == OCT_OK) {
	if (st_lookup(mapping, (char *) path.objectId,
		      (char **) &term_list)) {
	    term_gen = lsStart(term_list);
	    while (lsNext(term_gen, (lsGeneric *) &term.objectId, LS_NH) == LS_OK) {
		SYMCK(octGetById(&term));
		/* Find path net */
		if (!symLocNet(&path, &path_net)) {
		    errRaise(sym_pkg_name, SYM_NONET, "Path `%s' has no net",
			     ohFormatName(&path));
		    /*NOTREACHED*/
		}
		/* Find terminal net */
		if (symLocNet(&term, &term_net)) {
		    if (!octIdsEqual(path_net.objectId, term_net.objectId)) {
			/* Merge the two nets */
			if (octIsAttached(grp_bag, &path) == OCT_OK) {
			    (void) symMergeNets(&term_net, &path_net);
			} else {
			    (void) symMergeNets(&path_net, &term_net);
			}
		    }
		} else {
		    /*
		     * This may happen -- it means the other side really
		     * doesn't have a net.  No merge is required, but we
		     * may need to add the terminal to the net if the
		     * instance isn't a connector.
		     */
		    inst.objectId = term.contents.term.instanceId;
		    SYMCK(octGetById(&inst));
		    if (!symContactP(&inst)) {
			SYMCK(octAttach(&path_net, &term));
		    }
		}
		/* Join the path and terminal */
		SYMCK(octAttach(&path, &term));
	    }
	    lsFinish(term_gen);
	    lsDestroy(term_list, (void (*)()) 0);
	} else {
	    errRaise(sym_pkg_name, SYM_MAPFAIL,
		     "Bad mapping table for symUnbreakSet");
	    /*NOTREACHED*/
	}
    }
}
