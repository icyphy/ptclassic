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
 * Jump Terminals and Nets
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 *
 * Terminals on an instance can be annotated with a property
 * called LOCALNET or GLOBALNET.  This means that connections
 * to the terminal are made to a net named by the property.
 * This is used to support connections to a net without explicitly
 * connecting all of the elements of the net.
 * Further, if it is a GLOBALNET property, a formal terminal
 * is automatically added to the net with the same GLOBALNET
 * property attached to the terminal.  This has the effect
 * of connecting terminals across an entire design to one
 * net.  This file contains the routines to support this
 * policy.
 */

#include "internal.h"
#include "oh.h"
#include "gen.h"
#include "jump.h"

#define OCT_GLOBAL_NAME		"GLOBALNET"
#define OCT_LOCAL_NAME		"LOCALNET"

#define DEFAULT_NAME		";;; Global Net ;;;"

typedef enum sym_jt_query_defn {
    SYM_NOT_JT, SYM_GLOBAL_NET, SYM_LOCAL_NET
} symJTInfo;

/*
 * Forward declarations
 */

static symJTInfo symQJumpTerm
  ARGS((octObject *master, octObject *term, char **name));
  /* Examines a terminal for global or local net properties */



static void defJumpName();
static symJumpNameFunc jumpFunc = { defJumpName, (char *) 0 };

void symSetJumpNameFunc(new, old)
symJumpNameFunc *new;		/* New jump naming function */
symJumpNameFunc *old;		/* Old jump naming function */
/*
 * This routine sets the global net naming function to `new'
 * and returns the old function in `old' (for later restoration
 * if desired).  The form of the function is:
 *    void func(term, name, data)
 *    octObject *term;
 *    char name[SYM_MAX_MSG];
 *    char *data;
 * This function is called when the package needs to have a
 * unique name for a global net created according to jump
 * terminal policy.  `term' is the jump terminal involved.
 * The routine should write the name in `name'.  `data'
 * is the same as `data' in `new'.
 */
{
    if (old) {
	*old = jumpFunc;
    }
    jumpFunc = *new;
}

static void defJumpName(term, name, data)
octObject *term;		/* Jump terminal */
char name[SYM_MAX_MSG];		/* Returned name */
char *data;			/* User data     */
/*
 * This is the default naming function for global nets.
 * It is called when a jump terminal has a GLOBAL_NET property
 * without a name.  Generally, it is intended that the
 * user or program determine what global net is appropriate.
 * The default naming function returns a fixed name for this.
 */
{
    (void) strcpy(name, DEFAULT_NAME);
}


static octObject *symJumpTermBag(obj)
octObject *obj;
/*
 * Returns the jump term bag for the facet containing `obj'.
 * Some caching is done to improve performance.  Returns
 * zero if there is no jump term bag.
 */
{
    static octId old_id = oct_null_id;
    static octObject JTBag;
    octObject fct;

    octGetFacet(obj, &fct);
    if (!octIdsEqual(fct.objectId, old_id) || (octGetById(&JTBag) != OCT_OK)) {
	JTBag.type = OCT_BAG;
	JTBag.objectId = oct_null_id;
	JTBag.contents.bag.name = OCT_JUMPTERM_NAME;
	(void) octGetByName(&fct, &JTBag);
	old_id = fct.objectId;
    }
    if (!octIdIsNull(JTBag.objectId)) {
	return &JTBag;
    } else {
	return (octObject *) 0;
    }
}


void symAddToJTBag(term)
octObject *term;		/* Terminal to add to bag */
/*
 * Adds the given terminal to the jump terminals bag.  It
 * is assumed that symJumpTermDeepP(term) is true.
 */
{
    octObject *bag, newbag, fct;

    if (!(bag = symJumpTermBag(term))) {
	newbag.objectId = oct_null_id;
	newbag.type = OCT_BAG;
	newbag.contents.bag.name = OCT_JUMPTERM_NAME;
	octGetFacet(term, &fct);
	SYMCK(octCreate(&fct, &newbag));
	symAddToJTBag(term);
    } else {
	SYMCK(octAttachOnce(bag, term));
    }
}



void symCreateJumpNet(term, net)
octObject *term;		/* Jump terminal  */
octObject *net;			/* Net (returned) */
/* 
 * This routine creates a new symbolic jump net and returns it
 * in `net'.  Jump nets inherit their name from the LOCAL_NET
 * or GLOBAL_NET property on the associated jump terminal (i.e. `term').
 * If it is a GLOBAL_NET, a dummy formal terminal is also created
 * without an implementation to propogate the net throughout the
 * heirarchy.
 */
{
    octObject inst, interface, facet;
    char query_name[SYM_MAX_MSG];

    /* Get interface formal terminal */
    inst.objectId = term->contents.term.instanceId;
    SYMCK(octGetById(&inst));
    symInterface(&inst, &interface);

    net->type = OCT_NET;
    net->objectId = oct_null_id;
    if (symQJumpTerm(&interface,term,&(net->contents.net.name)) != SYM_NOT_JT) {
	if (!net->contents.net.name || (strlen(net->contents.net.name) == 0)) {
	    /* Query for name */
	    (*jumpFunc.func)(term, query_name, jumpFunc.data);
	    net->contents.net.name = query_name;
	}
	octGetFacet(term, &facet);
	SYMCK(octCreate(&facet, net));
    } else {
	/* Error */
	symMsg(SYM_ERR, "Cannot create jump net: %s is not a jump terminal",
	       term->contents.term.name);
    }
}



int symJumpTermP(obj)
octObject *obj;			/* Object to check            */
/*
 * This routine returns a non-zero value if the given object
 * is a jump terminal by examining the jump terminal bag policy.
 * This routine tries to cache information to make it a little
 * faster.
 */
{
    octObject *JTBag;

    if ( (JTBag = symJumpTermBag(obj)) ) {
	return octIsAttached(JTBag, obj) == OCT_OK;
    } else {
	return 0;
    }
}



int symJumpTermDeepP(aterm)
octObject *aterm;		/* Object to check */
/*
 * This routine examines the given actual terminal to see if it
 * is a jump terminal.  Unlike `symJumpTermP',  this routine does
 * not assume the terminal has already been attached to the
 * jump terminal bag.  It looks inside the interface master
 * if required.
 */
{
    octObject inst, interface;
    char *name;
    int result = 0;

    if (!(result = symJumpTermP(aterm))) {
	if ((aterm->type != OCT_TERM) ||
	    octIdIsNull(aterm->contents.term.instanceId)) {
	    symMsg(SYM_ERR, "Bad object type passed to symJumpTermDeepP");
	    return 0;
	}
	inst.objectId = aterm->contents.term.instanceId;
	SYMCK(octGetById(&inst));
	symInterface(&inst, &interface);

	if (symQJumpTerm(&interface, aterm, &name) == SYM_NOT_JT) {
	    return 0;
	} else {
	    return 1;
	}
    } else {
	return result;
    }
}



static symJTInfo symQJumpTerm(master, term, name)
octObject *master;		/* Interface master for actual terminal */
octObject *term;		/* Actual terminal itself               */
char **name;			/* Global or local net name             */
/*
 * This routine looks up the actual terminal `term' in `master' and
 * examines it for the presence of a LOCAL_NET or GLOBAL_NET property.
 * If found, it will fill in the value of the property into `name'.
 * It returns SYM_NOT_JT if it is not a jump terminal, SYM_GLOBAL_NET
 * if it is a global net, and SYM_LOCAL_NET if it is a local net.
 */
{
    octObject ifterm, prop;
    octGenerator gen;
    symJTInfo result = SYM_NOT_JT;

    SYMCK(ohGetTerminal(master, term->contents.term.formalExternalId,
			&ifterm));
    SYMCK(octInitGenContents(&ifterm, OCT_PROP_MASK, &gen));
    while (octGenerate(&gen, &prop) == OCT_OK) {
	if (prop.contents.prop.name &&
	    (STRCMP(prop.contents.prop.name, OCT_GLOBAL_NAME) == 0)) {
	    *name = prop.contents.prop.value.string;
	    result = SYM_GLOBAL_NET;
	}
	if (prop.contents.prop.name &&
	    (STRCMP(prop.contents.prop.name, OCT_LOCAL_NAME) == 0)) {
	    *name = prop.contents.prop.value.string;
	    result = SYM_LOCAL_NET;
	}
    }
    return result;
}



int symJumpNetP(net)
octObject *net;			/* Net to check */
/*
 * This routine returns a non-zero value if the given net
 * has any jump terminals on it.  The routine caches information
 * to try to make it a little faster.
 */
{
    octObject path, term;
    octGenerator gen, sub_gen;

    if (!symJumpTermBag(net)) return 0;
    SYMCK(octInitGenContents(net, OCT_PATH_MASK|OCT_TERM_MASK, &gen));
    while (octGenerate(&gen, &path) == OCT_OK) {
	if (path.type == OCT_PATH) {
	    SYMCK(octInitGenContents(&path, OCT_TERM_MASK, &sub_gen));
	    while (octGenerate(&sub_gen, &term) == OCT_OK) {
		if (symJumpTermP(&term)) {
		    (void) octFreeGenerator(&sub_gen);
		    (void) octFreeGenerator(&gen);
		    return 1;
		}
	    }
	} else {
	    if (symJumpTermP(&path)) {
		(void) octFreeGenerator(&gen);
		return 1;
	    }
	}
    }
    return 0;
}




 


