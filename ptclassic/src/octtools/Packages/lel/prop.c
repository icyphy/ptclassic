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
 * lelFindProp
 *
 * David Harrison
 * University of California, Berkeley
 * 1989
 *
 * This file contains an implementation of a routine for determining
 * the value of a property in a design heirarchy.
 */

#include "port.h"
#include "oct.h"
#include "oh.h"
#include "lel.h"

octStatus lelFindProp(inst, container, prop, facet, name)
octObject *inst, *container, *prop;
char *facet, *name;
/*
 * This routine looks for the property `prop' by `name' under
 * `container'.  If `container' is `inst' or an actual terminal
 * of `inst',  the filled in property is returned and the routine
 * returns OCT_OK.  If the property is not found,  the routine
 * searches the master (or formal terminal) of the instance for
 * the property and returns its value if found.  The `facet'
 * parameter specifies what facet of the master is opened for the
 * master or formal terminal search.  If no object is found,  the
 * routine returns OCT_NOT_FOUND.  In any case, the name field of
 * `prop' is modified to point to `name'.
 *
 * If `container' is the master of `inst' or a formal terminal of
 * the master of `inst',  the search is actually carried out
 * assuming `container' is the associated container on the
 * instance.  The semantics are the same as those described
 * above.
 *
 * If `container' is neither `inst', an actual terminal of
 * `inst',  the master of `inst',  or a formal terminal of the
 * master of `inst',  the routine behaves exactly like
 * octGetByName(container, prop).
 *
 * In effect,  lelFindProp evaluates a property in the context of
 * a design hierarchy.  It implements the policy that the value
 * of a property in a master cell is overridden by the value of
 * the same property on an instance of that cell.  If the instance
 * in the hierarchy is unknown, a zero may be specified for it.
 */
{
    octObject master;
    octObject term;

    prop->type = OCT_PROP;
    prop->contents.prop.name = name;

    if (inst && ((octIdsEqual(container->objectId, inst->objectId)) ||
	((container->type == OCT_TERM) &&
	 (octIdsEqual(container->contents.term.instanceId, inst->objectId))))) {
	/* Search on instance then on master */
	if (octGetByName(container, prop) != OCT_OK) {
	    /* Must search master */
	    master.type = OCT_FACET;
	    master.contents.facet.facet = facet;
	    master.contents.facet.mode = "r";
	    if (octOpenMaster(inst, &master) >= OCT_OK) {
		if (container->type == OCT_TERM) {
		    term = *container;
		    if (octGetByName(&master, &term) >= OCT_OK) {
			if (octGetByName(&term, prop) >= OCT_OK) {
			    return OCT_OK;
			}
		    }
		} else {
		    if (octGetByName(&master, prop) >= OCT_OK) {
			return OCT_OK;
		    }
		}
	    }
	    return OCT_NOT_FOUND;
	} else {
	    return OCT_OK;
	}
    } else if (inst && (container->type == OCT_FACET)) {
	/* Search directly on instance first */
	if (octGetByName(inst, prop) >= OCT_OK) {
	    return OCT_OK;
	}
    } else if (inst && (container->type == OCT_TERM) &&
	       octIdIsNull(container->contents.term.instanceId)) {
	/* Search on instance terminal first */
	term = *container;
	if (octGetByName(inst, &term) >= OCT_OK) {
	    if (octGetByName(&term, prop) >= OCT_OK) {
		return OCT_OK;
	    }
	}
    }
    /* Regular get by name */
    return octGetByName(container, prop);
}
