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
 * Contact Maintenence
 *
 * David Harrison
 * University of California, Berkeley
 * 1986, 1987, 1988, 1989
 *
 * This file contains code for assisting other parts of the symbolic
 * package with contacts.
 */

#include "internal.h"
#include "utility.h"
#include "oct.h"
#include "tap.h"
#include "gen.h"
#include "contact.h"
#include "segments.h"

/*
 * Forward declarations 
 */

static int prop_check();



static octObject *symConnectorBag(obj)
octObject *obj;
/*
 * Returns the connectors bag for the facet containing `obj'.
 * Some caching is done to improve performance.  Returns zero
 * if there is no jump term bag in the facet.
 */
{
    static octId old_id = oct_null_id;
    static octObject ConnBag;
    octObject fct;

    if (obj->type != OCT_FACET) {
	octGetFacet(obj, &fct);
    } else {
	fct = *obj;
    }
    if (!octIdsEqual(fct.objectId, old_id) || (octGetById(&ConnBag) != OCT_OK)) {
	ConnBag.type = OCT_BAG;
	ConnBag.objectId = oct_null_id;
	ConnBag.contents.bag.name = OCT_CONTACT_NAME;
	(void) octGetByName(&fct, &ConnBag);
	old_id = fct.objectId;
    }
    if (!octIdIsNull(ConnBag.objectId)) {
	return &ConnBag;
    } else {
	return (octObject *) 0;
    }
}


void symAddToConnectorsBag(connector)
octObject *connector;
/*
 * Adds `connector' to the connectors bag of its parent facet.
 * If there is no connectors bag, one is created.
 */
{
    octObject *bag, newbag, fct;

    if (!(bag = symConnectorBag(connector))) {
	newbag.objectId = oct_null_id;
	newbag.type = OCT_BAG;
	newbag.contents.bag.name = OCT_CONTACT_NAME;
	octGetFacet(connector, &fct);
	SYMCK(octCreate(&fct, &newbag));
	symAddToConnectorsBag(connector);
    } else {
	SYMCK(octAttachOnce(bag, connector));
    }
}




int symContactP(obj)
octObject *obj;			/* Instance or terminal */
/*
 * This routine returns a non-zero status if `obj' is a connector
 * (or a connector terminal).  A quick check of the connectors
 * bag is made.  If that is inconclusive,  the CELLTYPE property of
 * the instance master is checked.  This routine only checks
 * for instances and instance actual terminals.
 */
{
    octGenerator gen;
    octObject inst, bag;
    int found = 0;

    switch (obj->type) {
    case OCT_TERM:
	/* Actual terminal */
	if (!octIdIsNull(inst.objectId = obj->contents.term.instanceId)) {
	    SYMCK(octGetById(&inst));
	} else {
	    symMsg(SYM_ERR, "Formal terminal passed to symContactP");
	    return 0;
	}
	break;
    case OCT_INSTANCE:
	/* Instance itself */
	inst = *obj;
	break;
    default:
	symMsg(SYM_ERR, "Bad object type to symContactP");
	return 0;
	
    }
    /* Quick bag check */
    SYMCK(octInitGenContainers(&inst, OCT_BAG_MASK, &gen));
    while (octGenerate(&gen, &bag) == OCT_OK) {
	if (bag.contents.bag.name && (STRCMP(bag.contents.bag.name,
					     OCT_CONTACT_NAME) == 0)) found++;
    }
    if (found > 1) {
	errRaise(sym_pkg_name, SYM_BADBAG,
		 "Too many connections to connectors bag");
	/*NOTREACHED*/
    }
    if (found) return found;
    else return prop_check(&inst);
}



static int prop_check(inst)
octObject *inst;		/* Instance to check */
/*
 * Opens the master of the instance and checks the CELLTYPE property
 * to see if the instance is a connector.  If so,  the routine returns
 * a non-zero status.
 */
{
    octObject instMaster, propObj;

    symInterface(inst, &instMaster);
    propObj.type = OCT_PROP;
    propObj.contents.prop.name = OCT_CONTACT_PROP_NAME;
    if (octGetByName(&instMaster, &propObj) < OCT_OK) {
	if ( symVerbose ) {
	    symMsg(SYM_WARN, "WARNING: cell %s:%s:%s has no CELLTYPE property\n",
		instMaster.contents.facet.cell,
		instMaster.contents.facet.view,
		instMaster.contents.facet.facet);
	}	
	return 0;
    }
    if (propObj.contents.prop.value.string &&
	STRCMP(propObj.contents.prop.value.string, OCT_CONTACT_TYPE_NAME) == 0) {
	return 1;
    }
    return 0;
}



/*ARGSUSED*/
static int symDCF(ne, elems, inst, why, data)
int ne;				/* Number of connections   */
tapLayerListElement elems[];	/* Connection descriptions */
octObject *inst;		/* Returned instances      */
char why[SYM_MAX_MSG];		/* Error message           */
char *data;			/* User data (not used)    */
/*
 * This is a default connector generation function for use when
 * creating or modifying segments.  It uses tapGetConnector()
 * to find an appropriate connector based on the description
 * passed in `ne' and `elems'.
 */
{
    if (tapGetConnector(ne, elems, inst) == 0) {
	int i;
	char buf[256];

	inst->objectId = oct_null_id;
	(void) sprintf(why, "No appropriate connector found: %d layers\n", ne);
	for ( i = 0; i < ne; i++ ) {
	    (void) sprintf( buf, "%s(%ld) ",
			   elems[i].layer.contents.layer.name,
			   (long)elems[i].width );
	    (void) strcat( why, buf );
	}
	return 0;
    } else {
	return 1;
    }
}

static symConFunc symDefConFuncDefn = { symDCF, (char *) 0 };
symConFunc *symDefConFunc = &symDefConFuncDefn;



void symConnector(parent, x, y, ne, elems, confun, connector)
octObject *parent;		/* Parent object          */
octCoord x, y;			/* Location               */
int ne;				/* Number of connections  */
tapLayerListElement elems[];	/* Connection description */
symConFunc *confun;		/* Connector function     */
octObject *connector;		/* Connector (returned)   */
/*
 * Makes and places a connector at the location given by
 * `x,y'.  Generates the connector automatically using
 * the descriptions given in `ne' and `elems' and the
 * connector generation function `confun'.  The connector
 * is returned in `connector'.
 */
{
    char message[SYM_MAX_MSG];
    octObject fct;

    message[0] = '\0';
    connector->objectId = oct_null_id;
    if ((*confun->func)(ne, elems, connector, message, confun->data) == 0) {
	if (strlen(message)) {
	    /* Function expects us to report error */
	    symMsg(SYM_ERR, "Cannot create connector:\n  %s", message);
	}
    } else {
	/* Place the instance */
	connector->contents.instance.name = OCT_CONN_INST_NAME;
	connector->contents.instance.transform.translation.x = x;
	connector->contents.instance.transform.translation.y = y;
	if (parent->type != OCT_FACET) {
	    octGetFacet(parent, &fct);
	    parent = &fct;
	}
	if (octCreate(parent, connector) >= OCT_OK) {
	    symAddToConnectorsBag(connector);
	} else {
	    connector->objectId = oct_null_id;
	    symMsg(SYM_ERR,
		   "can't create instance of connector:\n  %s",
		   octErrorString());
	}
    }
}



int symUpgrade(term, elem, confun, new_conn)
octObject *term;		/* Actual terminal of connector */
tapLayerListElement *elem;	/* New segment description      */
symConFunc *confun;		/* Connector function           */
octObject *new_conn;		/* New connector (returned)     */
/*
 * This routine examines `term' to see if it can be upgraded
 * to a new connector that will allow `elem' to connect to
 * it.  If so, it fills in the instance in `new_conn' and
 * returns a non-zero value.  If not, it returns zero.
 * Note that the connector is not actually placed.  This
 * must be done with symInstance().
 *
 * Precondition: symContactP(term) is true.
 */
{
    char message[SYM_MAX_MSG];
    octGenerator gen;
    octObject seg;
    int count, idx, result;
    tapLayerListElement *full_desc;

    /* First pass counts the segment connections */
    count = 0;
    SYMCK(octInitGenContainers(term, OCT_PATH_MASK, &gen));
    while (octGenerate(&gen, &seg) == OCT_OK) {
	count++;
    }

    /* Second pass builds new connector description */
    full_desc = ALLOC(tapLayerListElement, count+1);
    idx = 0;
    SYMCK(octInitGenContainers(term, OCT_PATH_MASK, &gen));
    while (octGenerate(&gen, &seg) == OCT_OK) {
	symSegDesc(&seg, (struct octPoint *) 0, full_desc+idx);
	idx++;
    }
    full_desc[count] = *elem;

    /* Connector function */
    result =(*confun->func)(count+1, full_desc, new_conn, message, confun->data);
    FREE(full_desc);
    return result;
}
