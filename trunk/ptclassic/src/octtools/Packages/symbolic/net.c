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
 * Net Manipulation
 *
 * David Harrison
 * University of California, Berkeley
 * 1989
 *
 * This file contains code that manipulates nets according to published
 * symbolic policy.  The operations supported are:
 *   merging:  merge two nets into one
 *   breaking: break one net into two
 *   creation: make new machine named net
 *   query:    determine if net is human named or has jump terminals on it
 */

#include "internal.h"
#include "oh.h"
#include "st.h"

#include "terminfo.h"
#include "jump.h"
#include "gen.h"
#include "net.h"

/* Forward declarations */
static octObject *merge_move();
static int visit_object();
static int visit_joined();
static int visit_actual();
static void clean_up();

#define NAMING_STRING		"NET%d"
#define DEF_GLOBAL_NAME		";;; Default Global Net ;;;"



static symMergeFunc mergeFunc = { (int (*)()) 0, (char *) 0 };

void symSetMergeFunc(new, old)
symMergeFunc *new;		/* New merge function */
symMergeFunc *old;		/* Old merge function */
/*
 * This routine sets the merge function for the symbolic policy
 * library.  This function is called only when the package cannot 
 * determine which of two nets should be preserved.  The function 
 * has the following form:
 *   int func(net1, net2, data)
 *   octObject *net1, *net2;
 *   char *data;
 * If the function returns a non-zero value, net2 will be merged
 * into net1.  If the function returns zero, net1 will be merged
 * into net2.  If `old' is non-zero, the old function will be
 * returned so that the old behaviour can be restored if desired.
 */
{
    if (old) {
	*old = mergeFunc;
    }
    mergeFunc = *new;
}



octObject *symMergeNets(first, second)
octObject *first;		/* First net  */
octObject *second;		/* Second net */
/*
 * This routine merges the two specified nets into one net.
 * The merged net will be one of the originally passed nets.
 * The other net will be destroyed.  The final net is returned
 * by the function.  The merge direction is decided by a number
 * of heuristics:
 *   1.  If one of the nets is a jump net (has jump terminals
 *       on it),  it will be the one preserved.
 *   2.  If one of the nets is human named (as opposed to machine
 *       generated), it will be the one preserved.
 *   3.  If one of the nets has annotation (any properties) and
 *       the other doesn't, it will be preserved.
 *   4.  If none of the above apply, merge_func will be called
 *       to allow the caller to decide.  If no merge_func is
 *       available, the first net will be preserved.
 */
{
    octObject *result = (octObject *)NULL;
    if ( octIdsEqual( first->objectId, second->objectId ) ) {
	return first;		/* Ids are equal, no need to merge. */
    }

    if (symJumpNetP(first)) {
	if (!symJumpNetP(second)) {
	    result = merge_move(first, second);
	} else {
	    /* Horrors! Trying to merge jump nets -- punt! */
	    symMsg(SYM_ERR, 
"Attempt to merge jump nets!  This operation\n\
is not supported.  The nets `%s' and `%s' will\n\
not be merged.  Manual repair is required!\n",
		   first->contents.net.name, second->contents.net.name);
	}
    } else if (symJumpNetP(second)) {
	result = merge_move(second, first);
    } else {
	if (symHumanNamedNetP(first)) {
	    if (symHumanNamedNetP(second)) {
		int no_first, no_second;

		no_first = symLessAttach(first, 1, OCT_PROP_MASK, 1);
		no_second = symLessAttach(second, 1, OCT_PROP_MASK, 1);
		if (no_first ^ no_second) {
		    /* Different */
		    if (no_first) {
			result = merge_move(second, first);
		    } else {
			result = merge_move(first, second);
		    }
		} else if (mergeFunc.func) {
		    /* Ask merge function */
		    if ((*mergeFunc.func)(first, second, mergeFunc.data)) {
			result = merge_move(first, second);
		    } else {
			result = merge_move(second, first);
		    }
		} else {
		    /* Merge one into two */
		    result = merge_move(first, second);
		}
	    } else {
		/* Preserve first */
		result = merge_move(first, second);
	    }
	} else {
	    /* Preserve second */
	    result = merge_move(second, first);
	}
    }
    return result;
}




static octObject *merge_move(preserve, overwrite)
octObject *preserve;		/* Net to preserve through merging  */
octObject *overwrite;		/* Net to overwrite through merging */
/*
 * This routine merges the two specified nets into one net.  Objects
 * attached to the `overwrite' net are detached and moved to the 
 * `preserve' net.  The `overwrite' net is then destroyed.  This
 * routine will produce a warning when human named nets are merged.
 * Returns the preserved net.
 */
{
    octGenerator gen;
    octObject obj;

    if (symHumanNamedNetP(preserve) && symHumanNamedNetP(overwrite)) {
	symMsg(SYM_WARN, "Merging net `%s' into `%s'\n",
	       overwrite->contents.net.name, preserve->contents.net.name);
    }
    SYMCK(octInitGenContents(overwrite, OCT_ALL_MASK, &gen));
    while (octGenerate(&gen, &obj) == OCT_OK) {
	SYMCK(octAttach(preserve, &obj));
	SYMCK(octDetach(overwrite, &obj));
    }
    SYMCK(octDelete(overwrite));
    return preserve;
}



/*
 * Net breaking function
 */

int symBreakNet(obj, net, new_net, warn_p)
octObject *obj;			/* Object connected to net */
octObject *net;			/* Net itself              */
octObject *new_net;		/* Other portion of net    */
int warn_p;			/* Issue broken net warning */
/*
 * Removes the portion of `net' that is connected to `obj' and
 * places that portion in `new_net' (which should already have
 * been created).  The routine returns zero if it discovered
 * a jump terminal along the way thus indicating the break
 * should go the other way.
 */
{
    octGenerator gen;
    octObject n_obj;
    st_table *visited;
    int rv;

    visited = st_init_table(st_numcmp, st_numhash);
    if ( (rv = visit_object(obj, net, new_net, visited)) ) {
	/* Undo attachments, ask for reversal of sense */
	SYMCK(octInitGenContents(new_net, OCT_ALL_MASK, &gen));
	while (octGenerate(&gen, &n_obj) == OCT_OK) {
	    SYMCK(octAttach(net, &n_obj));
	    SYMCK(octDetach(new_net, &n_obj));
	}
    }
    st_free_table(visited);
    if (rv == 0) {
	/* Clean up the nets */
	clean_up(net, new_net, warn_p);
	return 1;
    } else {
	return 0;
    }
}

static int visit_object(obj, net, new_net, visited)
octObject *obj;			/* Visited object */
octObject *net;			/* Old net        */
octObject *new_net;		/* New net        */
st_table *visited;		/* Table of visited nodes */
/*
 * If `obj' is not in `visited' and `obj' is attached to
 * `net',  `obj' is removed from `net' and attached to
 * `new_net' and placed in `visited'.  Jump terminals
 * require special care.  If we meet a jump terminal, it
 * means we chose the direction of the break incorrectly.
 * This routine returns one if this occurred.  It still
 * breaks the nets but the sense of the break should
 * be reversed (this is done in net_break).
 */
{
    octObject new_object;
    octGenerator gen;
    int rv = 0;			/* Return value */

    if (!st_is_member(visited, (char *) obj->objectId)) {
	st_insert(visited, (char *) obj->objectId, (char *) 0);
	switch (obj->type) {
	case OCT_PATH:
	    SYMCK(octInitGenContents(obj, OCT_TERM_MASK, &gen));
	    while (octGenerate(&gen, &new_object) == OCT_OK) {
		if (!rv) rv |= visit_object(&new_object, net, new_net, visited);
	    }
	    break;
	case OCT_TERM:
	    if (obj->contents.term.instanceId) {
		/* Actual terminal */
		if (symJumpTermP(obj)) {
		    /* Ask to reverse sense */
		    return 1;
		}
		if (!rv) rv |= visit_actual(obj, net, new_net, visited);
	    } else {
		/* Formal terminal -- no visitation required */
	    }
	    break;
	default:
	    errRaise(sym_pkg_name, SYM_BADOBJ, "Unknown object: %s\n",
		     ohFormatName(obj));
	    /*NOTREACHED*/
	}
	if (octIsAttached(net, obj) == OCT_OK) {
	    SYMCK(octAttach(new_net, obj));
	    SYMCK(octDetach(net, obj));
	}
    }
    return rv;
}


static int visit_joined(obj, net, new_net, visited)
octObject *obj;			/* Visited object */
octObject *net;			/* Old net        */
octObject *new_net;		/* New net        */
st_table *visited;		/* Table of visited nodes */
/*
 * This routine visits any actual terminals joined to `obj'
 * through the joined term mechanism.  This can be expensive
 * so it is done only for real instances.
 */
{
    octGenerator gen, subgen;
    octObject a_inst, bag, interface, f_term, v_term;
    int inst_flag;
    int rv = 0;			/* Return value */

    a_inst.objectId = obj->contents.term.instanceId;
    SYMCK(octGetById(&a_inst));
    /* Determine if real instance or connector */
    SYMCK(octInitGenContainers(&a_inst, OCT_BAG_MASK, &gen));
    inst_flag = 1;
    while (octGenerate(&gen, &bag) == OCT_OK) {
	if (bag.contents.bag.name &&
	    (STRCMP(bag.contents.bag.name, OCT_CONTACT_NAME) == 0)) {
	    inst_flag = 0;
	}
    }
    if (inst_flag) {
	/* Visit joined terminals -- if any */
	symInterface(&a_inst, &interface);
	f_term.type = OCT_TERM;
	f_term.contents.term.name = obj->contents.term.name;
	f_term.contents.term.instanceId = oct_null_id;
	if (octGetByName(&interface, &f_term) == OCT_OK) {
	    SYMCK(octInitGenContainers(&f_term, OCT_BAG_MASK, &gen));
	    while (octGenerate(&gen, &bag) == OCT_OK) {
		if (bag.contents.bag.name &&
		    (STRCMP(bag.contents.bag.name, OCT_JOIN_NAME) == 0)) {
		    SYMCK(octInitGenContents(&bag,OCT_TERM_MASK,&subgen));
		    while (octGenerate(&subgen, &v_term) == OCT_OK) {
			SYMCK(octGetByName(&a_inst, &v_term));
			if (!rv) {
			    rv |= visit_object(&v_term, net, new_net, visited);
			}
		    }
		}
	    }
	} else {
	    /* Bad formal terminal */
	    errRaise(sym_pkg_name, SYM_BADTERM,
		     "Actual/formal terminal mismatch\n");
	    /*NOTREACHED*/
	}
    }
    return rv;
}


static int visit_actual(obj, net, new_net, visited)
octObject *obj;			/* Visited object */
octObject *net;			/* Old net        */
octObject *new_net;		/* New net        */
st_table *visited;		/* Table of visited nodes */
/*
 * This routine visits an actual terminal.  Basically, this
 * touches all paths connected to it and any terminals
 * joined to it through joined terminal bags.
 */
{
    octGenerator gen;
    octObject new_object;
    int rv = 0;

    /* Visit all regular paths connected to it */
    SYMCK(octInitGenContainers(obj, OCT_PATH_MASK | OCT_TERM_MASK, &gen));
    while (octGenerate(&gen, &new_object) == OCT_OK) {
	if (!rv) rv |= visit_object(&new_object, net, new_net, visited);
    }
    /* Visit any joined terminals */
    if (!rv) rv |= visit_joined(obj, net, new_net, visited);
    return rv;
}



#ifdef JUMPTERM_VISIT_TEST
static int visit_jump_terms(obj, net, new_net, visited)
octObject *obj;			/* Visited object */
octObject *net;			/* Old net        */
octObject *new_net;		/* New net        */
st_table *visited;		/* Table of visited nodes */
/*
 * This routine visits related jump terminals to the jump
 * terminal `obj'.  This is a little tricky.  Jump terminals
 * generally are not on any net.  So, instead, we have to
 * generate through all actual terminals on `net' and
 * visit those that are classed as jump terminals themselves.
 */
{
    octObject path, term;
    octGenerator gen, sub_gen;

    SYMCK(octInitGenContents(net, OCT_PATH_MASK, &gen));
    while (octGenerate(&gen, &path) == OCT_OK) {
	SYMCK(octInitGenContents(&path, OCT_TERM_MASK, &sub_gen));
	while (octGenerate(&sub_gen, &term) == OCT_OK) {
	    if (symJumpTermP(&term)) {
		(void) visit_actual(&term, net, new_net, visited);
	    }
	}
    }
    /* Always returns one because this is a jump terminal */
    return 1;
}
#endif



static void clean_up(main_net, break_net, warn_p)
octObject *main_net;		/* Original net */
octObject *break_net;		/* Broken net   */
int warn_p;			/* Issue warning */
/*
 * After symBreakNet() has been called,  this routine determines
 * what happens to annotation and the like between these two
 * nets.  Current operations:
 *   1.  If `main_net' has no paths or terminals,  all attachments
 *       to `break_net' are moved to `main_net' and `break_net'
 *       is destroyed.
 *   2.  If `break_net' has less than two paths and less than two
 *       terminals attached to it, it is destroyed.
 */
{
    octObject obj;
    octGenerator gen;

    if (symLessAttach(main_net, 1, OCT_PATH_MASK, 1) &&
	symLessAttach(main_net, 1, OCT_TERM_MASK, 2)) {
	/* Swap the nets */
	SYMCK(octInitGenContents(main_net, OCT_PATH_MASK|OCT_TERM_MASK, &gen));
	while (octGenerate(&gen, &obj) == OCT_OK) {
	    SYMCK(octDetach(main_net, &obj));
	}
	SYMCK(octInitGenContents(break_net, OCT_PATH_MASK|OCT_TERM_MASK, &gen));
	while (octGenerate(&gen, &obj) == OCT_OK) {
	    SYMCK(octAttach(main_net, &obj));
	    SYMCK(octDetach(break_net, &obj));
	}
    } 
    if (symLessAttach(break_net, 1, OCT_PATH_MASK, 1) &&
	symLessAttach(break_net, 1, OCT_TERM_MASK, 2)) {
	symNetRemove(break_net);
	break_net->objectId = oct_null_id;
    }
    if (symLessAttach(main_net, 1, OCT_PATH_MASK, 1) &&
	symLessAttach(main_net, 1, OCT_TERM_MASK, 2)) {
	symNetRemove(main_net);
    } else if (symHumanNamedNetP(main_net) && !octIdIsNull(break_net->objectId)) {
	if (warn_p) {
	    symMsg(SYM_WARN, "Net `%s' broken: broken portion given no name\n",
		   main_net->contents.net.name);
	}
	break_net->contents.net.name = (char *) 0;
	SYMCK(octModify(break_net));
    }
}


int symLocNet(obj, net)
octObject *obj;			/* Object to examine */
octObject *net;			/* Returned net      */
/*
 * This routine attempts to find the net for the object `obj'.
 * The object is expected to be either a path or a terminal.
 * Returns zero if there is no net associated with the object.
 */
{
    octObject net_or_path;
    octGenerator gen;
    int ret = 0;

    if (obj->type == OCT_TERM) {
	SYMCK(octInitGenContainers(obj, OCT_NET_MASK|OCT_PATH_MASK, &gen));
	/* This is still a little bogus if cell is not up to spec ... */
	ret = 0;
	while (octGenerate(&gen, &net_or_path) == OCT_OK) {
	    if (net_or_path.type == OCT_PATH) {
		ret = symLocNet(&net_or_path, net);
	    } else {
		*net = net_or_path;
		ret = 1;
	    }
	}
    } else if (obj->type == OCT_PATH) {
	if (octGenFirstContainer(obj, OCT_NET_MASK, net) == OCT_OK) {
	    ret = 1;
	} else {
	    ret = 0;
	}
    }
    return ret;
}



void symCreateNamedNet(obj, net)
octObject *obj, *net;
/*
 * Creates a new machine named net in the same facet as `obj'.
 */
{
    octObject	facet;
    
    if (obj->type != OCT_FACET) {
	octGetFacet(obj, &facet);
    } else {
	facet = *obj;
    }
    net->type = OCT_NET;
    net->contents.net.name = NULL;
    net->contents.net.width = 0;
    net->objectId = oct_null_id;
    SYMCK(octCreate(&facet, net));
}



int symHumanNamedNetP(net)
octObject *net;
/* 
 * Returns 1 if the net is not machine named
 */
{
    int	netNum;
    
    if ((net->contents.net.name == (char *) 0) ||
	(strlen(net->contents.net.name) == 0) ||
	(sscanf(net->contents.net.name, NAMING_STRING, &netNum) == 1)) {
	return 0;
    } else {
	return 1;
    }
}



void symNetRemove(net)
octObject *net;			/* Net to remove */
/*
 * This routine removes a net.  It will also delete any properties
 * and formal terminals attached to the net.  
 */
{
    octObject obj;
    octGenerator gen;

    /* Remove certain objects under net */
    SYMCK(octInitGenContents(net, OCT_PROP_MASK|OCT_TERM_MASK, &gen));
    while (octGenerate(&gen, &obj) == OCT_OK) {
	if ((obj.type == OCT_PROP) ||
	    ((obj.type == OCT_TERM) &&
	     octIdIsNull(obj.contents.term.instanceId))) {
	    SYMCK(octDelete(&obj));
	}
    }
    if (symHumanNamedNetP(net)) {
	symMsg(SYM_WARN, "Deleting net `%s'\n", net->contents.net.name);
    }
    SYMCK(octDelete(net));
}

