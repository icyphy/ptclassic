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
 * Formal Terminal Maintenence
 *
 * David Harrison
 * University of California, Berkeley
 * 1988
 *
 * This file contains routines for handling symbolic formal terminals.
 */

#include "region.h"
#include "internal.h"
#include "net.h"
#include "contact.h"
#include "formal.h"
#include "gen.h"



void symNewFormal(facet, fterm, implBag, termtype, termdir, term_func)
octObject *facet;		/* Facet where terminal is created           */
octObject *fterm;		/* Returned formal terminal (name specified) */
octObject *implBag;		/* Implementation definitions                */
char *termtype;			/* Terminal type (none if zero)              */
char *termdir;			/* Terminal direction (none if zero)         */
symTermFunc *term_func;		/* Terminal creation function (none if zero) */
/*
 * This routine creates a new symbolic formal terminal `fterm' whose
 * implementation is all of the actual terminals and segments in
 * `implBag'.  TERMTYPE and  TERMDIRECTION properties will be added 
 * to the terminal with the values provided in `termtype' and `termdir'.  
 * The caller should delete the implementation bag if there is no
 * further use for it.  Any previous implementations for the terminal
 * are removed.  If supplied, `term_func' will be called
 * when items are attached or detached from the new terminal.  The
 * form of the function is given in the header file.  `fterm' will
 * be the terminal, `obj' will be the item that is about to be
 * attached or detached from the terminal, and `detach' will be
 * non-zero if the item is going to be detached (otherwise attached).
 */
{
    octGenerator gen, subgen, wipegen;
    octObject impl, tempterm, termNet, tempNet, prop;

    /* Examine all terminals on the implementation bag for correctness */
    SYMCK(octInitGenContents(implBag, OCT_TERM_MASK, &gen));
    while (octGenerate(&gen, &impl) == OCT_OK) {
	/* Check to see if its already a formal */
	SYMCK(octInitGenContainers(&impl, OCT_TERM_MASK, &subgen));
	while (octGenerate(&subgen, &tempterm) == OCT_OK) {
	    if (term_func) {
		(*term_func->func)(&tempterm, &impl, 1, term_func->data);
	    }
	}
	SYMCK(octInitGenContainers(&impl, OCT_TERM_MASK, &wipegen));
	while (octGenerate(&wipegen, &tempterm) == OCT_OK) {
	    SYMCK(octDelete(&tempterm));
	}
    }

    /* Create new formal terminal */
    if (octGetByName(facet, fterm) == OCT_OK) {
	/* Already formal with this name -- remove old implementations */
	SYMCK(octInitGenContents(fterm, OCT_GEO_MASK|OCT_TERM_MASK, &gen));
	while (octGenerate(&gen, &impl) == OCT_OK) {
	    if (term_func) {
		(*term_func->func)(fterm, &impl, 1, term_func->data);
	    }
	    SYMCK(octDetach(fterm, &impl));
	}
	/* Detach from all major nets */
	SYMCK(octInitGenContainers(fterm, OCT_NET_MASK, &gen));
	while (octGenerate(&gen, &impl) == OCT_OK) {
	    SYMCK(octDetach(&impl, fterm));
	}
    } else {
	SYMCK(octCreate(facet, fterm));
    }

    /* Add properties if specified */
    if (termtype) {
	prop.type = OCT_PROP;
	prop.contents.prop.name = SYM_TERM_TYPE;
	prop.contents.prop.type = OCT_STRING;
	prop.contents.prop.value.string = termtype;
	SYMCK(octCreate(fterm, &prop));
    }

    if (termdir) {
	prop.type = OCT_PROP;
	prop.contents.prop.name = SYM_TERM_DIR;
	prop.contents.prop.type = OCT_STRING;
	prop.contents.prop.value.string = termdir;
	SYMCK(octCreate(fterm, &prop));
    }

    /* Attach implementations to formal terminal */
    termNet.objectId = oct_null_id;
    SYMCK(octInitGenContents(implBag, OCT_TERM_MASK|OCT_PATH_MASK,
			     &gen));
    while (octGenerate(&gen, &impl) == OCT_OK) {
	if (term_func) {
	    (*term_func->func)(fterm, &impl, 0, term_func->data);
	}
	SYMCK(octAttach(fterm, &impl));
	/* Check to make sure it has the same net */
	if (regFindNet(&impl, &tempNet) == REG_OK) {
	    if (!octIdIsNull(termNet.objectId)) {
		if (!octIdsEqual(termNet.objectId, tempNet.objectId)) {
		    /* Fatal error */
		    errRaise(sym_pkg_name, SYM_MULNET,
			     "while creating terminal `%s':\n  All implementations of a formal terminal must be on the same net.",
			     fterm->contents.term.name);
		    /*NOTREACHED*/
		}
	    } else {
		termNet = tempNet;
	    }
	}
    }
    /* Do the net maintenence */
    if (octIdIsNull(termNet.objectId)) {
	/* Make a new one */
	symCreateNamedNet(facet, &termNet);
    }
    /* Attach all implementations that aren't already attached */
    SYMCK(octInitGenContents(implBag, OCT_TERM_MASK, &gen));
    while (octGenerate(&gen, &impl) == OCT_OK) {
	if (!symContactP(&impl)) {
	    if (octIsAttached(&termNet, &impl) != OCT_OK) {
		SYMCK(octAttach(&termNet, &impl));
	    }
	}
    }
    SYMCK(octAttach(&termNet, fterm));
}



int symGetTermProp(term, prop)
octObject *term;		/* Formal or actual terminal */
octObject *prop;		/* Property itself           */
/*
 * This routine tries to find the property `prop' on the
 * terminal `term'.  If it fails and the terminal is
 * an actual terminal, it will continue the search on
 * the corresponding terminal on the interface master.
 * If the property is found, it is returned in `prop'
 * and the routine returns a non-zero status.  If the 
 * property can't be found, the routine returns zero.  
 */
{
    octGenerator gen;
    octObject inst, master, formal, actual;

    if (term->type == OCT_TERM) {
	if (octGetByName(term, prop) == OCT_OK) {
	    return 1;
	} else if (octIdIsNull(term->contents.term.instanceId)) {
	    /* Formal terminal */
	    if (octInitGenContents(term, OCT_TERM_MASK, &gen) == OCT_OK) {
		while (octGenerate(&gen, &actual) == OCT_OK) {
		    if (symGetTermProp(&actual, prop)) {
			octFreeGenerator(&gen);
			return 1;
		    }
		}
	    }
	} else {
	    /* Actual terminal */
	    inst.objectId = term->contents.term.instanceId;
	    if (octGetById(&inst) == OCT_OK) {
		symInterface(&inst, &master);
		if (octGetByExternalId(&master,
				       term->contents.term.formalExternalId,
				       &formal) == OCT_OK) {
		    return symGetTermProp(&formal, prop);
		}
	    }
	}
    }
    return 0;
}
