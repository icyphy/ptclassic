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
 * Joined Terminals
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 *
 * This file contains utility routines for dealing with joined
 * terminals.  Joined terminals are separate terminals that
 * are electrically equivalent.  The routines included here
 * test to see if a terminal is joined with others and
 * allow the user to find out what other terminals are
 * joined.
 */

#include "internal.h"

#include "gen.h"
#include "inst.h"
#include "oh.h"
#include "joined.h"


static octObject *get_joined_bag(aterm)
octObject *aterm;		/* Actual terminal */
/*
 * This routine returns the joined terminal bag in the interface
 * master of `aterm' that contains `aterm'.  One level of
 * cache is maintained for performance.  It will return
 * (octObject *) 0 if there is no joined terminal bag.
 * The returned object is locally owned and should be
 * copied if the caller wants to preserve it.
 */
{
    static octId old_id = oct_null_id;
    static octObject JTB;
    octObject inst, interface, fterm;
    
    if (!octIdsEqual(aterm->objectId, old_id)) {
	inst.objectId = aterm->contents.term.instanceId;
	SYMCK(octGetById(&inst));
	symInterface(&inst, &interface);
	SYMCK(ohGetTerminal(&interface, aterm->contents.term.formalExternalId,
			    &fterm));
	JTB.type = OCT_BAG;
	JTB.contents.bag.name = OCT_JOIN_NAME;
	if (octGetContainerByName(&fterm, &JTB) == OCT_OK) {
	    old_id = aterm->objectId;
	} else {
	    return (octObject *) 0;
	}
    }
    return &JTB;
}




int symNumJoined(term)
octObject *term;		/* Actual terminal */
/*
 * This routine returns the number of other terminals that
 * are electrically equivalent to `term'.  The specified
 * terminal must be an actual terminal of a real instance.
 * The routine returns zero if there are no electrically
 * equivalent terminals.
 */
{
    octObject *jtd, obj;
    octGenerator gen;
    int result;

    if (symInstanceP(term)) {
	if ( (jtd = get_joined_bag(term)) ) {
	    result = 0;
	    SYMCK(octInitGenContents(jtd, OCT_TERM_MASK, &gen));
	    while (octGenerate(&gen, &obj) == OCT_OK) {
		result++;
	    }
	    if (result > 0) {
		/* One of those is `term' */
		result = result - 1;
	    }
	    return result;
	} else {
	    return 0;
	}
    } else {
	symMsg(SYM_ERR, "Bad object type to symNumJoined");
	return 0;
    }
}



int symListJoined(term, num, joined)
octObject *term;		/* Actual terminal              */
int num;			/* Number of spaces in `joined' */
octObject joined[];		/* Array of terminals           */
/* 
 * This routine finds all terminals that are electrically equivalent
 * to `term' and puts them in `joined'.  The number of terminals
 * actually placed in `joined' is returned by the function.  The
 * number of joined terminals can be determined by calling
 * symNumJoined().  It will return SYM_LIST_TOO_SMALL if `joined'
 * is too small to contain all of the terminals.
 */
{
    octGenerator gen;
    octObject inst, interface, aterm, fterm, *jtb;
    int spot;

    if (symInstanceP(term)) {
	if ( (jtb = get_joined_bag(term)) ) {
	    /* Obtain instance */
	    inst.objectId = term->contents.term.instanceId;
	    SYMCK(octGetById(&inst));

	    /* Obtain the interface master */
	    symInterface(&inst, &interface);

	    /* Generate through all terminals */
	    spot = 0;
	    SYMCK(octInitGenContents(&inst, OCT_TERM_MASK, &gen));
	    while (octGenerate(&gen, &aterm) == OCT_OK) {
		SYMCK(ohGetTerminal(&interface,
				    aterm.contents.term.formalExternalId,
				    &fterm));
		if ((octIsAttached(jtb, &fterm) == OCT_OK) &&
		    !octIdsEqual(aterm.objectId, term->objectId)) {
		    if (spot < num) {
			joined[spot++] = aterm;
		    } else {
			symMsg(SYM_ERR,
			       "Array passed to symListJoined is too small");
			return SYM_LIST_TOO_SMALL;
		    }
		}
	    }
	    return spot;
	} else {
	    /* No joined terminals */
	    return 0;
	}
    } else {
	symMsg(SYM_ERR, "Bad object type passed to symListJoined");
	return 0;
    }
}
