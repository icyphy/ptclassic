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
 * Copying a set of objects in symbolic
 *
 * David Harrison
 * University of California, Berkeley
 * 1989
 *
 * This is an implementation for copying a set of objects in the
 * symbolic editing style.  Connectivity is maintaing throughout
 * this operation.
 */

#include "internal.h"
#include "utility.h"
#include "oct.h"

#include "group.h"
#include "net.h"
#include "jump.h"
#include "delete.h"
#include "contact.h"
#include "segments.h"
#include "terminfo.h"
#include "copy.h"

#define COPY_SET_BAG	";;; Copy Cut Set ;;;"
#define ORPHAN_BAG_NAME	";;; Copy Orphans ;;;"
#define NEW_CUT_SET	";;; New Copy Cut Set ;;;"
#define OLD_NET_BAG	";;; Old Nets ;;;"
#define RECONNECT_BAG	";;; Reconnected items ;;;"

/*
 * Forward declarations
 */

static void handle_orphans();
static void add_ep_conn();
static void copy_set();
static void apply_transform();
static void copy_path();
static void handle_containers();
static void handle_contents();
static void copy_inst();
static void actual_terms();
static void copy_general();
static void find_nets();
static void copy_nets();
static void rename_net();
static void net_contents();
static void reconnect();



void symCopyObjects(facet, set, transform, result, confun, ci)
octObject *facet;		/* Facet for copy         */
octObject *set;			/* Bag of objects to copy */
struct octTransform *transform;	/* Transformation         */
octObject *result;		/* Bag of new objects     */
symConFunc *confun;		/* Connector function     */
symCopyItemFunc *ci;		/* Copy Item callback     */
/*
 * This routine copies the objects given by `set' to a new location
 * that is computed by applying `transform' to all objects in 
 * the set.  The new copies of the objects are attached to the
 * supplied bag `result'.  Connectivity is maintained.  Restriction:
 * no connects will be made to any objects already in the destination
 * area.
 *
 * `ci' is a function called when the actual copy of an object 
 * is performed.  The prototype is shown in the header.
 * `old' will be the old object, `new' will be the newly copied
 * object, and `data' will be the `data' supplied to this function.
 * If the function is not supplied, no callback will be done.
 * New connector objects may be added to the copy set.  These
 * will be denoted by having `old' be zero.
 * 
 * Implementation steps:
 *  - Identify cut set (paths with one side in set and the other
 *    outside the set).
 *  - Break connectivity across cut set.
 *  - Copy objects and connectivity through transform.
 *  - Find orphaned paths in new set (those that have less than
 *    two connections).  Add new connectors to their endpoints.
 *  - Reconnect original set.
 */
{
    octObject cut_set, new_cut_set;
    st_table *mapping;

    /* Find cut set */
    cut_set.type = OCT_BAG;  cut_set.contents.bag.name = COPY_SET_BAG;
    SYMCK(octCreate(facet, &cut_set));
    octMarkTemporary(&cut_set);
    symCutSet(set, &cut_set);
    
    /* Break connectivity */
    mapping = st_init_table(st_numcmp, st_numhash);
    symBreakSet(&cut_set, set, mapping);
    
    /* Copy through transform */
    new_cut_set.type = OCT_BAG;  new_cut_set.contents.bag.name = NEW_CUT_SET;
    SYMCK(octCreate(facet, &new_cut_set));
    octMarkTemporary(&new_cut_set);
    copy_set(facet, set, transform, &cut_set, &new_cut_set, result, ci);

    /* Handle orphan paths */
    handle_orphans(&new_cut_set, result, confun, ci);
    
    /* Reconnect original set */
    symUnbreakSet(&cut_set, set, mapping);

    /* Clean up */
    SYMCK(octDelete(&cut_set));
    SYMCK(octDelete(&new_cut_set));
    st_free_table(mapping);
}


static void handle_orphans(cut_set, new_set, confun, ci)
octObject *cut_set;		/* Bag of cut set paths  */
octObject *new_set;		/* Newly copied items    */
symConFunc *confun;		/* Connector function    */
symCopyItemFunc *ci;		/* Copy callback         */
/*
 * Examines the the paths in `cut_set' and finds paths
 * that have less than two end-point terminal connections.
 * Endpoint connectors are added to these paths.
 */
{
    octGenerator gen;
    octObject path;
    struct octPoint pnts[2];
    segTermInfo info[2];
    int count;
    int32 np = 2;

    SYMCK(octInitGenContents(cut_set, OCT_PATH_MASK, &gen));
    while (octGenerate(&gen, &path) == OCT_OK) {
	SYMCK(octGetPoints(&path, &np, pnts));
	symSegTerms(&path, pnts, info);
	count = 0;
	if (octIdIsNull(info[0].term.objectId)) count++;
	if (octIdIsNull(info[1].term.objectId)) count++;
	if (count) {
	    add_ep_conn(new_set, &path, pnts, info, confun, ci);
	}
    }
}


static void add_ep_conn(new_set, seg, pnts, info, confun, ci)
octObject *new_set;		/* Bag of newly copied items              */
octObject *seg;			/* Segment for adding endpoint connectors */
struct octPoint pnts[2];	/* Segment endpoints                      */
segTermInfo info[2];		/* Endpoint terminal information          */
symConFunc *confun;		/* Connector function                     */
symCopyItemFunc *ci;		/* Copy item callback                     */
/*
 * Puts a simple end-point connector at each unconnected end of `seg'.
 * This connector is obtained using `confun'.  The caller is notified
 * of this creation using `ci'.
 */
{
    octObject new_conn, at;
    tapLayerListElement elem;
    int i;

    for (i = 0;  i < 2;  i++) {
	if (octIdIsNull(info[i].term.objectId)) {
	    symSegDesc(seg, pnts+i, &elem);
	    new_conn.objectId = oct_null_id;
	    symConnector(seg, pnts[i].x, pnts[i].y, 1, &elem, confun, &new_conn);
	    if (!octIdIsNull(new_conn.objectId)) {
		SYMCK(octGenFirstContent(&new_conn, OCT_TERM_MASK, &at));
		SYMCK(octAttach(seg, &at));
		SYMCK(octAttach(new_set, &new_conn));
		if (ci) (*ci->func)((octObject *) 0, &new_conn, ci->data);
	    }
	}
    }
}


static void copy_set(facet, set, trans, cut, new_cut, new_set, ci)
octObject *facet;		/* Facet for copy         */
octObject *set;			/* Bag of objects to copy */
struct octTransform *trans;	/* Transformation         */
octObject *cut;			/* Bag of cut set paths   */
octObject *new_cut;		/* Bag of new cut set paths (RETURNED) */
octObject *new_set;		/* Bag of new objects (RETURNED) */
symCopyItemFunc *ci;		/* Copy item callback     */
/*
 * This routine applies `transform' to the geometric objects in
 * the set and then makes copies of them.  The new objects are
 * attached to `new_set'.  If the old object was part of `cut',
 * the new object is also attached to `new_cut'.  The connectivity
 * information for the objects is also copied.  New copies are
 * made of properties attached to the objects.
 *  - Copy objects through transform maintaining a mapping table
 *    between old paths and actual terminals and new paths and
 *    actual terminals (using the oct identifier as the key).
 *    Also attach the new objects to `new_cut' and `new_set'
 *    as appropriate.
 *  - Build a bag of nets attached to items in the original set.
 *  - Copy each net and attach to new objects based on the mapping
 *    table constructed in the first step.
 */
{
    st_table *mapping;
    octObject old_nets;

    mapping = st_init_table(st_numcmp, st_numhash);
    apply_transform(set, trans, cut, new_set, new_cut, mapping, ci);

    old_nets.type = OCT_BAG;  old_nets.contents.bag.name = OLD_NET_BAG;
    SYMCK(octCreate(facet, &old_nets));
    octMarkTemporary(&old_nets);
    find_nets(set, &old_nets);

    copy_nets(&old_nets, mapping, ci);

    /* Clean up */
    SYMCK(octDelete(&old_nets));
    st_free_table(mapping);
}



static void apply_transform(set, trans, cut, new_set, new_cut, mapping, ci)
octObject *set;			/* Set of objects to copy */
struct octTransform *trans;	/* Transformation         */
octObject *cut;			/* Original cut set       */
octObject *new_set;		/* Set of copied objects (RETURNED) */
octObject *new_cut;		/* New cut set (RETURNED) */
st_table *mapping;		/* Mapping of paths,aterms (RETURNED) */
symCopyItemFunc *ci;		/* Copy item callback     */
/*
 * This routine steps through `set' applying `trans' to each object
 * and making a new copy of that object.  The new copy is attached
 * to `new_set'.  If the old object was attached to `cut', the
 * new object is attached to `new_cut'.  The routine also updates
 * `mapping' which is a table of octId's that map old paths and
 * actual terminals to their newly created counterparts.  Attachments
 * (both up and down) are examined for each object and are duplicated
 * or copied as appropriate.
 */
{
    octObject geo;
    octGenerator gen;

    SYMCK(octInitGenContents(set, OCT_GEO_MASK|OCT_INSTANCE_MASK, &gen));
    while (octGenerate(&gen, &geo) == OCT_OK) {
	switch (geo.type) {
	case OCT_PATH:
	    copy_path(&geo, trans, cut, new_set, new_cut, mapping, ci);
	    break;
	case OCT_INSTANCE:
	    copy_inst(&geo, trans, new_set, mapping, ci);
	    break;
	default:
	    copy_general(&geo, trans, new_set, ci);
	    break;
	}
    }
}



static void copy_path(path, trans, cut, new_set, new_cut, mapping, ci)
octObject *path;		/* Path to copy                */
struct octTransform *trans;	/* Transformation              */
octObject *cut;			/* Cut set                     */
octObject *new_set;		/* New set (MODIFIED)          */
octObject *new_cut;		/* New cut set (MODIFIED)      */
st_table *mapping;		/* Mapping between old and new */
symCopyItemFunc *ci;		/* Copy item callback          */
/*
 * This routine copies a path by transforming it through `trans'.
 * The new copy is attached to `new_set'.  If the old path was
 * attached to `cut', the new copy is attached to `new_cut'.
 * A new entry is put in `mapping' that maps the old path id
 * to the new path id.
 */
{
    octObject new_path, old_layer;
    struct octPoint *pnts = (struct octPoint *) 0;
    int32 num = 0;

    /* Get the old path points */
    if (octGetPoints(path, &num, pnts) == OCT_ERROR) {
	errRaise(sym_pkg_name, 0, "octGetPoints failed:\n  %s\n", octErrorString());
	/*NOTREACHED*/
    }
    pnts = ALLOC(struct octPoint, num);
    SYMCK(octGetPoints(path, &num, pnts));

    /* Get the old path layer */
    SYMCK(octGenFirstContainer(path, OCT_LAYER_MASK, &old_layer));

    /* Make new path */
    new_path = *path;
    octTransformGeo(&new_path, trans);
    octTransformPoints(num, pnts, trans);
    SYMCK(octCreate(&old_layer, &new_path));
    SYMCK(octPutPoints(&new_path, num, pnts));
    (void) free((char *) pnts);

    /* Update mapping table */
    (void) st_insert(mapping, (char *) path->objectId, (char *) new_path.objectId);
    /* Handle path attachments */
    handle_containers(path, &new_path);
    handle_contents(path, &new_path);

    /* Connect to appropriate bags */
    SYMCK(octAttach(new_set, &new_path));
    if (octIsAttached(cut, path) == OCT_OK) {
	SYMCK(octAttach(new_cut, &new_path));
    }

    /* Call user function (if applicable) */
    if (ci) (*ci->func)(path, &new_path, ci->data);
}



static int bag_p(bag)
octObject *bag;			/* Bag to examine   */
/*
 * Returns a non-zero status if it is alright to attach copied
 * items onto `bag'.  Basically, this routine looks at bag
 * names to determine this property.  Only certain policy
 * bags are automatically supported.  Other attachements
 * could be done with the copy item callback function.
 */
{
    if (bag->contents.bag.name) {
	if (STRCMP(bag->contents.bag.name, OCT_CONTACT_NAME) == 0) return 1;
	if (STRCMP(bag->contents.bag.name, OCT_INSTANCE_NAME) == 0) return 1;
	if (STRCMP(bag->contents.bag.name, OCT_JUMPTERM_NAME) == 0) return 1;
    }
    return 0;
}

static void handle_containers(old, new)
octObject *old;			/* Old path         */
octObject *new;			/* New copy of path */
/*
 * This routine handles the attachments between the new
 * copy of an object and its containing objects.  In general,
 * all attachments are duplicated except: layers and facets (that attachment
 * is already there), terminals (formal terminals aren't copied),
 * nets (connectivity copy is done later), and certain bags (basically 
 * those locally created).
 */
{
    octGenerator gen;
    octObject obj;

    SYMCK(octInitGenContainers(old, OCT_ALL_MASK, &gen));
    while (octGenerate(&gen, &obj) == OCT_OK) {
	switch (obj.type) {
	case OCT_LAYER:
	case OCT_NET:
	case OCT_FACET:
	case OCT_TERM:
	case OCT_INSTANCE:
	case OCT_PATH:
	    /* These are ignored -- nets and terms are handled much later */
	    break;
	case OCT_BAG:
	    if (!bag_p(&obj)) {
		/* Don't copy attachment to this bag */
		break;
	    }
	    /* Intentional fall-through */
	default:
	    /* Make attachment */
	    SYMCK(octAttach(&obj, new));
	    break;
	}
    }
}


static void handle_contents(old, new)
octObject *old;			/* Old path */
octObject *new;			/* New path */
/*
 * This routine handles attachments between the new
 * copy of an object and the objects it contains.  In
 * general, all attachments are copied except properties
 * (new copies of the properties are attached) and terminals
 * (connectivity is restored later).
 */
{
    octGenerator gen;
    octObject obj;

    SYMCK(octInitGenContents(old, OCT_ALL_MASK, &gen));
    while (octGenerate(&gen, &obj) == OCT_OK) {
	switch (obj.type) {
	case OCT_PROP:
	    /* Make a copy of the property attached to the object */
	    SYMCK(octCreate(new, &obj));
	    /*
	     * Note: the attachments to the old property might have to
	     *       be copied here.  We will have to see.
	     */
	    break;
	case OCT_TERM:
	    /* Ignored -- connectivity information is restored later */
	    break;
	default:
	    /* Copy attachment */
	    SYMCK(octAttach(new, &obj));
	    break;
	}
    }
}



static void copy_inst(inst, trans, new_set, mapping, ci)
octObject *inst;		/* Instance to copy     */
struct octTransform *trans;	/* Transformation       */
octObject *new_set;		/* Bag for copied items */
st_table *mapping;		/* Mapping from old to new */
symCopyItemFunc *ci;		/* Copy item callback    */
/*
 * This routine copies an instance by transforming it through
 * `trans'.  The new copy is attached to `new_set'.  New entries
 * are put in `mapping' mapping the old actual terminals to
 * the new actual terminals (for use in connectivity copying).
 * Certain attachments are also duplicated.
 */
{
    octObject new_inst, fct;

    /* Make new one */
    octGetFacet(inst, &fct);
    new_inst = *inst;
    octTransformGeo(&new_inst, trans);
    SYMCK(octCreate(&fct, &new_inst));

    /* Handle attachments to instance */
    handle_containers(inst, &new_inst);
    handle_contents(inst, &new_inst);

    /* Handle actual terminals */
    actual_terms(inst, &new_inst, mapping);

    SYMCK(octAttach(new_set, &new_inst));
    if (ci) (*ci->func)(inst, &new_inst, ci->data);
}



static void actual_terms(old, new, mapping)
octObject *old;			/* Old instance */
octObject *new;			/* New instance */
st_table *mapping;		/* Mapping old to new */
/*
 * This routine handles maintenance of actual terminals on
 * a copied instance.  Appropriate copying of attachments
 * both up and down occur and the mapping table is
 * updated to reflect the mapping from the old terminal
 * to the new.  This relationship is by name (there isn't
 * any other way currently).
 */
{
    octGenerator gen;
    octObject term, new_term;

    SYMCK(octInitGenContents(old, OCT_TERM_MASK, &gen));
    while (octGenerate(&gen, &term) == OCT_OK) {
	new_term = term;
	SYMCK(octGetByName(new, &new_term));

	/* Handle attachments */
	handle_containers(&term, &new_term);
	handle_contents(&term, &new_term);

	/* Update mapping table */
	(void) st_insert(mapping, (char *) term.objectId,
			 (char *) new_term.objectId);
    }
}



static void copy_general(obj, trans, new_set, ci)
octObject *obj;			/* Object to copy */
struct octTransform *trans;	/* Transformation */
octObject *new_set;		/* New copy set   */
symCopyItemFunc *ci;		/* Copy item callback */
/*
 * This routine copies arbitrary geometry to a new location
 * computed by transforming the object through `trans'.  It
 * is assumed the object is geometric and is attached to
 * a layer.  The new copy of the object is attached to `new_set'.
 * It is also assumed the object has no associated points.
 */
{
    octObject new, old_layer;

    SYMCK(octGenFirstContainer(obj, OCT_LAYER_MASK, &old_layer));
    new = *obj;
    octTransformGeo(&new, trans);
    SYMCK(octCreate(&old_layer, &new));

    /* Handle attachments */
    handle_containers(obj, &new);
    handle_contents(obj, &new);

    SYMCK(octAttach(new_set, &new));
    if (ci) (*ci->func)(obj, &new, ci->data);
}



static void find_nets(set, nets)
octObject *set;			/* Bag of objects */
octObject *nets;		/* Resulting nets */
/*
 * This routine finds all nets associated with the items in `set'.
 * These nets are all attached to the bag `nets'.  Assumption:
 * all nets in the set can be found by looking upward off paths
 * and actual terminals (no indirect searches through containing
 * paths).
 */
{
    octObject obj, one_net, term;
    octGenerator gen, term_gen, net_gen;

    SYMCK(octInitGenContents(set, OCT_PATH_MASK|OCT_INSTANCE_MASK, &gen));
    while (octGenerate(&gen, &obj) == OCT_OK) {
	if (obj.type == OCT_PATH) {
	    /* Net directly above */
	    SYMCK(octGenFirstContainer(&obj, OCT_NET_MASK, &one_net));
	    SYMCK(octAttachOnce(nets, &one_net));
	} else {
	    /* Nets attached to actual terminals */
	    SYMCK(octInitGenContents(&obj, OCT_TERM_MASK, &term_gen));
	    while (octGenerate(&term_gen, &term) == OCT_OK) {
		SYMCK(octInitGenContainers(&term, OCT_NET_MASK, &net_gen));
		while (octGenerate(&net_gen, &one_net) == OCT_OK) {
		    SYMCK(octAttachOnce(nets, &one_net));
		}
	    }
	}
    }
}



static void copy_nets(nets, mapping, ci)
octObject *nets;		/* Set of nets to copy         */
st_table *mapping;		/* Mapping between old and new */
symCopyItemFunc *ci;		/* Copy item callback          */
/*
 * This routine copies all nets in the bag `nets'.  The attachments
 * to these copies are then carefully restored to new copies of
 * the underlying objects using the table `mapping' which maps
 * old terminals and paths to new terminals and paths.  Some
 * assumptions are made:
 *   - New nets are created attached to the facet
 *   - No duplication of upward attachments is done.
 */
{
    octObject one_net, new_net, fct;
    octGenerator gen;

    octGetFacet(nets, &fct);
    SYMCK(octInitGenContents(nets, OCT_NET_MASK, &gen));
    while (octGenerate(&gen, &one_net) == OCT_OK) {
	new_net = one_net;
	if (symJumpNetP(&one_net)) {
	    /* No new copy is made */
	} else {
	    SYMCK(octCreate(&fct, &new_net));
	    if (symHumanNamedNetP(&new_net)) rename_net(&new_net);
	    net_contents(&one_net, &new_net);
	}
	reconnect(&one_net, &new_net, mapping);
	if (ci && !octIdsEqual(one_net.objectId, new_net.objectId)) {
	    (*ci->func)(&one_net, &new_net, ci->data);
	}
    }
}



#define MAX_NET_NAME_LEN	2048
#define MAX_ID_LEN		20

static void rename_net(net)
octObject *net;			/* Net to rename */
/*
 * Renames the net uniquely.  Uses a static string.
 */
{
    char new_name[MAX_NET_NAME_LEN];
    int32 xid;

    if (net->contents.net.name &&
	((strlen(net->contents.net.name)+MAX_ID_LEN) > MAX_NET_NAME_LEN)) {
	errRaise(sym_pkg_name, 0, "Net name too long");
	/*NOTREACHED*/
    }
    octExternalId(net, &xid);
    (void) sprintf(new_name, "%s_%ld", net->contents.net.name, (long)xid);
    net->contents.net.name = new_name;
    SYMCK(octModify(net));
}



static void net_contents(old, new)
octObject *old;			/* Old net */
octObject *new;			/* New net */
/*
 * This routine handles copying the contents of `old' to `new'.
 * Any properties are fully copied.  Terminals and paths are 
 * ignored (they are handled by reconnect).  Other items are
 * attached but not copied.
 */
{
    octGenerator gen;
    octObject obj;

    SYMCK(octInitGenContents(old, OCT_ALL_MASK, &gen));
    while (octGenerate(&gen, &obj) == OCT_OK) {
	switch (obj.type) {
	case OCT_PROP:
	    /* Make a copy of the property attached to the object */
	    SYMCK(octCreate(new, &obj));
	    /*
	     * Note: the attachments to the old property might have to
	     *       be copied here.  We will have to see.
	     */
	    break;
	case OCT_TERM:
	case OCT_PATH:
	    /* Ignored -- connectivity information is restored later */
	    break;
	default:
	    /* Copy attachment */
	    SYMCK(octAttach(new, &obj));
	    break;
	}
    }


}


static void reconnect(old, new, mapping)
octObject *old;			/* Old net */
octObject *new;			/* New net */
st_table *mapping;		/* Mapping between old and new */
/*
 * This routine attaches objects to the new net by examining
 * the objects attached to the old net and looking them
 * up in the `mapping' table.  It also attaches new paths to
 * their actual terminal endpoints by examining this table.
 * There is an assumption that all paths in the copy set
 * were attached to nets before the copy.
 */
{
    octObject obj, new_obj, term, new_term, temp_bag;
    octGenerator gen, term_gen;

    /*
     * In the special case where old == new (the jump terminal case), 
     * we create a temporary bag to contain the new objects that is 
     * then moved to the net after the process.
     */
    if (octIdsEqual(old->objectId, new->objectId)) {
	temp_bag.type = OCT_BAG;
	temp_bag.contents.bag.name = RECONNECT_BAG;
	SYMCK(octCreate(old, &temp_bag));
	octMarkTemporary(&temp_bag);
	new = &temp_bag;
    }
    SYMCK(octInitGenContents(old, OCT_PATH_MASK|OCT_TERM_MASK, &gen));
    while (octGenerate(&gen, &obj) == OCT_OK) {
	/* Look it up in the old table if not formal terminal */
	if ((obj.type != OCT_TERM) || !octIdIsNull(obj.contents.term.instanceId)) {
	    if ((st_lookup(mapping, (char *) obj.objectId,
			   (char **) &new_obj.objectId))) {
		SYMCK(octGetById(&new_obj));
		SYMCK(octAttach(new, &new_obj));
		
		/* If it is a path make endpoint attachments */
		if (obj.type == OCT_PATH) {
		    SYMCK(octInitGenContents(&obj, OCT_TERM_MASK, &term_gen));
		    while (octGenerate(&term_gen, &term) == OCT_OK) {
			if (st_lookup(mapping, (char *) term.objectId,
				      (char **) &new_term.objectId)) {
			    SYMCK(octGetById(&new_term));
			    SYMCK(octAttach(&new_obj, &new_term));
			} else {
			    errRaise(sym_pkg_name, 0, "No corresponding path terminal");
			    /*NOTREACHED*/
			}
		    }
		}
	    } else if (new != &temp_bag) {
		/* Only in the non-jumpterm case is this an error */
		errRaise(sym_pkg_name, 0, "No corresponding net object");
		/*NOTREACHED*/
	    }
	}
    }
    if (new == &temp_bag) {
	/* Now patch it up */
	SYMCK(octInitGenContents(&temp_bag, OCT_ALL_MASK, &gen));
	while (octGenerate(&gen, &obj) == OCT_OK) {
	    SYMCK(octAttach(old, &obj));
	}
	SYMCK(octDelete(&temp_bag));
    }
}
