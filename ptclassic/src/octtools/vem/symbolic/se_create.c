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
/*
 * Create command for symbolic
 *
 * David Harrison
 * University of California,  Berkeley
 * 1988, 1990
 *
 * This file dispatches to the appropriate create commands based on
 * the argument types found on the arg list.
 *
 * Work to be done:
 *  - becomes part of the vem specific symbolic commands
 *  - set up warning and merge functions
 */

#include "general.h"
#include "symbolic.h"
#include "message.h"
#include "commands.h"
#include "region.h"
#include "vemUtil.h"
#include "se.h"

/* Forward declarations */
static vemStatus seCreateError();

#define TEMP_BAG_NAME	";;; Terminal Implementation ;;;"



vemStatus seCreateCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* Argument list            */
/*
 * This is the creation command for symbolic views.  This command
 * can be used to create new instances, paths, and formal terminals.
 * A syntax summary is given below:
 *	formal-terminal:	"text" (over implementation)
 *      formal-terminal:	objects "text"
 *	instance:		points (over some other inst)
 *	instance:		points "layer"
 *	path:			lines lines ... (over layer)
 *	path:			lines lines ... "layer"
 */
{
    vemOneArg *arg1, *arg2;
    octObject facet, bag;
    lsGen lg;
    int len;
    (void)vemExpert( spot );

    seInit();

    len = lsLength(cmdList);
    if (len < 1) return seCreateError();

    /* Check for editing style differences */
    if (vuCkStyle(spot, cmdList) != VEM_OK) return VEM_ARGRESP;

    /* Ready to try to parse */
    lg = lsStart(cmdList);
    if (lsNext(lg, (lsGeneric *) &arg1, LS_NH) == LS_OK) {
	switch (arg1->argType) {
	case VEM_TEXT_ARG:
	case VEM_OBJ_ARG:
	    /* Formal terminal */
	    facet.objectId = spot->theFct;
	    if (octGetById(&facet) != OCT_OK) {
		vemMsg(MSG_A, "The buffer is inaccessible:\n  %s\n",
		       octErrorString());
		return VEM_ARGRESP;
	    }
	    if (len == 1) {
		/* Standard terminal creation */
		lsFinish(lg);
		if (arg1->argType == VEM_TEXT_ARG) {
		    return seParseFormalTerminal(spot, &facet,
						 (octObject *) 0,
						 arg1->argData.vemTextArg);
		} else {
		    return seCreateError();
		}
	    } else if (len == 2) {
		/* Advanced terminal creation */
		if (lsNext(lg, (lsGeneric *) &arg2, LS_NH) == LS_OK) {
		    if (arg2->argType != VEM_TEXT_ARG) {
			return seCreateError();
		    }
		    lsFinish(lg);
		    bag.objectId = arg1->argData.vemObjArg.theBag;
		    if (octGetById(&bag) == OCT_OK) {
			return seParseFormalTerminal(spot, &facet,
						     &bag,
						     arg2->argData.vemTextArg);
		    } else {
			vemMsg(MSG_A, "Cannot get objects: %s\n",
				octErrorString());
			return VEM_CORRUPT;
		    }
		} else {
		    lsFinish(lg);
		    vemMsg(MSG_C, "Cannot get second argument\n");
		    return VEM_CORRUPT;
		}
	    } else {
		return seCreateError();
	    }
	    break;
	case VEM_POINT_ARG:
	    if (len > 2) {
		lsFinish(lg);
		return seCreateError();
	    }
	    return seCreateInst(spot, cmdList, "contents");
	case VEM_LINE_ARG:
	    return seCreatePath(spot, cmdList);
	case VEM_BOX_ARG:
	    return seCreateError();
	}
    }
    return VEM_OK;
}



#define MSG(txt)	vemMsg(MSG_C, txt)

static vemStatus seCreateError()
/*
 * Displays a message about the form of symbolic create command
 */
{
    MSG("Create format:\n");
    MSG("  Formal Terminal:  \"term name\" : create (over implementation)\n");
    MSG("                    object(s) \"term name\" : create\n");
    MSG("  Instance:         point(s) \"cell:view {name}\" : create\n");
    MSG("                    point(s) : create (over another instance)\n");
    MSG("  Segments:         line(s) \"layername\" : create\n");
    MSG("                    line(s) : create (over desired layer)\n");
    return VEM_ARGRESP;
}



vemStatus seParseFormalTerminal(spot, facet, implementation, name)
vemPoint *spot;			/* Where command was invoked       */
octObject *facet;		/* Target facet		           */
octObject *implementation;	/* Optional bag of implementations */
char *name;			/* Terminal name                   */
/*
 * This command creates a new formal terminal in a symbolic cell.
 * There are two forms for this command:
 *   "name" : create
 *   objects "name" : create
 * The first form creates a formal terminal whose implementation is
 * the actual terminal under the spot where the command was invoked.
 * The second form creates a formal terminal whose implementation
 * is all of the geometry in the object set.  Specifically,
 * if `implementation' is non-zero,  it will be used as the
 * implementation.  The name may also be specified as 
 * "name termtype direction" where termtype and direction are
 * optional annotations.  This routine parses this specification
 * and calls the terminal creation command.
 */
{
    int okay;
    char termname[1024], termtype[32], direction[32];
    octObject termImpl, bag;
    vemStatus result;

    if (bufWritable(spot->theFct) != VEM_OK) {
	vemMsg(MSG_C, "Buffer is read only.\n");
	return VEM_CORRUPT;
    }

    if (!implementation) {
	/* Find the actual terminal under spot -- make temporary bag */
	if (seFindActual(facet, spot->theWin, &(spot->thePoint), &termImpl)) {
	    bag.type = OCT_BAG;
	    bag.contents.bag.name = TEMP_BAG_NAME;
	    VEM_OCTCKRVAL(vuCreateTemporary(facet, &bag), VEM_CORRUPT);
	    VEM_OCTCKRVAL(octAttach(&bag, &termImpl), VEM_CORRUPT);
	} else {
	    /* Can't find implementation */
	    vemMsg(MSG_C, "No implementation for the terminal was found.\n");
	    return VEM_CORRUPT;
	}
    }

    okay = sscanf(name, "%s %s %s", termname, termtype, direction);
    if (okay == 3) {
	result = seCreateFormalTerminal(facet,
					(implementation ? implementation : &bag),
					termname, termtype, direction);
    } else {
	result = seCreateFormalTerminal(facet,
					(implementation ? implementation : &bag),
					termname, (char *) 0, (char *) 0);
    }
    if (!implementation) VEM_OCTCKRVAL(octDelete(&bag), VEM_CORRUPT);
    return result;
}

