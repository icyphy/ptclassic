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
 * Deleting a set of objects in Symbolic
 *
 * David Harrison
 * University of California,  Berkeley
 * 1989
 *
 * New delete command implemented using new group net maintenance
 * library.
 */

#include "internal.h"
#include "st.h"
#include "region.h"
#include "group.h"
#include "net.h"
#include "contact.h"
#include "delete.h"

#define DEL_SET_BAG	";;; Delete: Cut-Set Bag ;;;"

/* Forward declarations */
static void add_items();
static void delete_objs();
static void del_path();
static void del_inst();
static void del_term();
static void del_bagfunc();
static void del_net();
static void del_nomaint();

static int orphanTerm();

static void fterm_remove();

void symDeleteObjects(delSetBag, del_func)
octObject *delSetBag;		/* Deleted set bag              */
symDelItemFunc *del_func;	/* Item called before each item is deleted */
/*
 * Deletes all items in `delSetBag'.  `del_func' specifies a function
 * that is called just before each item in the set is deleted.
 * See the definition of `symDelItemFunc' for the form.  `item'
 * will be the object that is to be deleted and `data' will be
 * the user data in `del_func'.  If no del_func is supplied,
 * no callback is made.
 *
 * Steps toward deleting objects:
 *   - Identify cut set (paths with one side in deleted set the other
 *     outside the deleted set).  Also adds cut-set paths and connectors
 *     to deleted set.
 *   - Break nets across cut set paths
 *   - Add cut set paths to deleted set
 *   - Delete objects in set
 */
{
    octObject fct;
    octGenerator gen;
    octObject cut_set, extras, obj;

    octGetFacet(delSetBag, &fct);

    /* Make the cut set bag */
    cut_set.type = OCT_BAG;  cut_set.contents.bag.name = DEL_SET_BAG;
    SYMCK(octCreate(&fct, &cut_set));

    /* Find cut set */
    symCutSet(delSetBag, &cut_set);

    /* Add cut set paths and orphaned connectors to delete bag */
    extras.type = OCT_BAG;    extras.contents.bag.name = DEL_SET_BAG;
    SYMCK(octCreate(&fct, &extras));
    add_items(delSetBag, &cut_set, &extras);

    /* Break nets */
    symBreakSet(&cut_set, delSetBag, (st_table *) 0);

    /* Add extras into deleted set */
    SYMCK(octInitGenContents(&extras, OCT_ALL_MASK, &gen));
    while (octGenerate(&gen, &obj) == OCT_OK) {
	SYMCK(octAttachOnce(delSetBag, &obj));
    }
    SYMCK(octDelete(&extras));

    /* Delete the objects (and related nets */
    delete_objs(delSetBag, del_func);

    /* Remove cut set */
    SYMCK(octDelete(&cut_set));
}



static void add_items(del_bag, cut_set, extras)
octObject *del_bag;		/* Bag of items to be deleted */
octObject *cut_set;		/* Cut set paths for group    */
octObject *extras;		/* Bag for extra items        */
/*
 * This routine adds the paths in `cut_set' to `extras' (these
 * will later be added to the deleted set bag).  
 * It also examines the paths for any connectors which might
 * be orphaned when the delete is done.  These are also added
 * to the `extras' bag.
 */
{
    octObject path, term, inst;
    octGenerator gen, term_gen;

    SYMCK(octInitGenContents(cut_set, OCT_PATH_MASK, &gen));
    while (octGenerate(&gen, &path) == OCT_OK) {
	SYMCK(octAttachOnce(extras, &path));
	SYMCK(octInitGenContents(&path, OCT_TERM_MASK, &term_gen));
	while (octGenerate(&term_gen, &term) == OCT_OK) {
	    inst.objectId = term.contents.term.instanceId;
	    SYMCK(octGetById(&inst));
	    if ((octIsAttached(del_bag, &inst) != OCT_OK) &&
		symContactP(&inst) &&
		orphanTerm(&term, del_bag)) {
		/* Orphan connector -- add to deleted set */
		SYMCK(octAttachOnce(extras, &inst));
	    }
	}
    }
}



static int orphanTerm(term, del_bag)
octObject *term;		/* Terminal of connector */
octObject *del_bag;		/* Bag of deleted items  */
/*
 * Returns a non-zero value if the terminal is an orphan.  A terminal
 * is an orphan if it is connected only to paths that are on the
 * deleted set bag.
 */
{
    octGenerator gen;
    octObject path;

    SYMCK(octInitGenContainers(term, OCT_PATH_MASK, &gen));
    while (octGenerate(&gen, &path) == OCT_OK) {
	if (octIsAttached(del_bag, &path) != OCT_OK) {
	    SYMCK(octFreeGenerator(&gen));
	    return 0;
	}
    }
    return 1;
}




static void delete_objs(del_bag, del_func)
octObject *del_bag;		/* Deleted set bag    */
symDelItemFunc *del_func;	/* Callback function  */
/*
 * This routine actually carries out the deletion of the objects
 * in `del_bag'.  It also queues up for redisplay any necessary
 * regions.
 */
{
    octObject obj;
    octGenerator gen;

    SYMCK(octInitGenContents(del_bag, OCT_ALL_MASK, &gen));
    while (octGenerate(&gen, &obj) == OCT_OK) {
	/* To eliminate possible multiple deletes */
	if (octGetById(&obj) != OCT_OK) continue;
	if (del_func) {
	    (*del_func->func)(&obj, del_func->data);
	}
	switch (obj.type) {
	case OCT_PATH:
	    del_path(&obj);
	    break;
	case OCT_INSTANCE:
	    del_inst(&obj);
	    break;
	case OCT_TERM:
	    del_term(&obj);
	    break;
	case OCT_BAG:
	    del_bagfunc(&obj);
	    break;
	case OCT_NET:
	    del_net(&obj, del_func);
	    break;
	case OCT_LABEL:
	    del_nomaint(&obj);
	    break;
	}
    }
}


static void del_path(path)
octObject *path;		/* Path to delete  */
/*
 * This routine deletes `path' and performs some clean up activity
 * on the nets and connectors attached to it.  These two checks
 * are as follows:
 *   1.  The containing net is checked to see if its empty.  If
 *       so,  it is deleted.
 */
{
    octGenerator gen;
    octObject obj;

    /* Clean up nets and layers */
    SYMCK(octInitGenContainers(path, OCT_NET_MASK, &gen));
    while (octGenerate(&gen, &obj) == OCT_OK) {
	if ((obj.type == OCT_NET) &&
	    symLessAttach(&obj, 1, OCT_PATH_MASK, 2) &&
	    symLessAttach(&obj, 1, OCT_TERM_MASK, 2)) {
	    symNetRemove(&obj);
	}
    }
    SYMCK(octDelete(path));
}



static void del_inst(inst)
octObject *inst;		/* Instance to delete */
/*
 * This routine deletes `inst' and performs some cleanup checks
 * on the nets and paths connected to it.  These include:
 *   1.  A check on each terminal for empty nets.
 *   2.  A check on each terminal for formal terminals.
 *   3.  Check for empty bags above the instance.
 */
{
    octGenerator t_gen, nt_gen;
    octObject a_term, n_or_t, bag;

    SYMCK(octInitGenContents(inst, OCT_TERM_MASK, &t_gen));
    while (octGenerate(&t_gen, &a_term) == OCT_OK) {
	/* Check for nets, terminals, and containing bags */
	SYMCK(octInitGenContainers(&a_term,
				OCT_NET_MASK | OCT_BAG_MASK | OCT_TERM_MASK,
				&nt_gen));
	while (octGenerate(&nt_gen, &n_or_t) == OCT_OK) {
	    if (n_or_t.type == OCT_NET) {
		/* Detach it from net so it will get joined terminals */
		SYMCK(octDetach(&n_or_t, &a_term));
		if (symLessAttach(&n_or_t, 1, OCT_ALL_MASK, 2)) {
		    symNetRemove(&n_or_t);
		}
	    } else if ((n_or_t.type == OCT_TERM) &&
		       octIdIsNull(n_or_t.contents.term.instanceId)) {
		fterm_remove(&n_or_t);
	    } else if ((n_or_t.type == OCT_BAG)) {
		SYMCK(octDetach(&n_or_t, &a_term));
		if (symLessAttach(&n_or_t, 1, OCT_ALL_MASK, 2) &&
		    n_or_t.contents.bag.name &&
		    (STRCMP(n_or_t.contents.bag.name, OCT_JUMPTERM_NAME) == 0)) {
		    SYMCK(octDelete(&n_or_t));
		}
	    }
	}
    }
    /* Handle INSTANCES and CONNECTORS bag */
    SYMCK(octInitGenContainers(inst, OCT_BAG_MASK, &t_gen));
    while (octGenerate(&t_gen, &bag) == OCT_OK) {
	if (symLessAttach(&bag, 1, OCT_ALL_MASK, 2) &&
	    bag.contents.bag.name &&
	    ((STRCMP(bag.contents.bag.name, OCT_INSTANCE_NAME) == 0) ||
	     (STRCMP(bag.contents.bag.name, OCT_CONTACT_NAME) == 0))) {
	    SYMCK(octDelete(&bag));
	}
    }
    SYMCK(octDelete(inst));
}



static void del_term(term)
octObject *term;		/* Terminal to delete   */
/*
 * Deletes `term' if appropriate and causes necessary redisplays.
 * Only formal terminals may be deleted.
 */
{
    if (term->contents.term.instanceId) {
	symMsg(SYM_ERR, "`%s': Actual terminals cannot be deleted.\n",
	       term->contents.term.name);
	return;
    }
    SYMCK(octDelete(term));
    symMsg(SYM_INFO, "Deleted formal terminal `%s'\n",
	   term->contents.term.name);
}



static void del_bagfunc(bag)
octObject *bag;			/* Bag to delete        */
/*
 * If the bag is not one created by VEM (all of which are temporary),
 * it is deleted.
 */
{
    if (!octIsTemporary(bag)) {
	SYMCK(octDelete(bag));
	symMsg(SYM_INFO, "Deleted bag named `%s'\n", bag->contents.bag.name);
    }
}


static void del_net(net, cb)
octObject *net;
symDelItemFunc *cb;
/*
 * generate all of the paths on the net and then pass them
 * (in a bag) to delete_objs.
 */
{
    octObject facet, bag, path;
    octGenerator gen;

    octGetFacet(net, &facet);
    bag.type = OCT_BAG;
    bag.contents.bag.name = ";;; deletion bag ;;;";
    SYMCK(octCreate(&facet, &bag));
    octMarkTemporary(&bag);	/* just in case this routine dies */

    SYMCK(octInitGenContents(net, OCT_PATH_MASK, &gen));
    while (octGenerate(&gen, &path) == OCT_OK) {
	SYMCK(octAttach(&bag, &path));
    }
    symDeleteObjects(&bag, cb);
    SYMCK(octDelete(&bag));
}



static void del_nomaint(obj)
octObject *obj;
/*
 * Deletes an object without performing any maintenence on it.
 * If it has a size,  the region is updated beneath it.
 */
{
    SYMCK(octDelete(obj));
}


static void fterm_remove(term)
octObject *term;		/* Terminal to remove */
/*
 * This routine removes a formal terminal.  Attached properties
 * are also removed.
 */
{
    octObject obj;
    octGenerator gen;

    /* Remove certain objects under net */
    SYMCK(octInitGenContents(term, OCT_PROP_MASK, &gen));
    while (octGenerate(&gen, &obj) == OCT_OK) {
	SYMCK(octDelete(&obj));
    }
    symMsg(SYM_INFO, "Deleting formal terminal `%s'\n",
	   term->contents.term.name);
    SYMCK(octDelete(term));
}
