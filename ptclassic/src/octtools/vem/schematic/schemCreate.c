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
/*
 * Schematic interface code
 *
 * David Harrison
 * University of California,  Berkeley
 * 1988
 *
 * This is the new schematic interface built on top of the symbolic
 * library.
 */

#include "general.h"
#include "list.h"
#include "oct.h"
#include "message.h"
#include "commands.h"
#include "vemUtil.h"
#include "symbolic.h"
#include "se.h"
#include "opts.h"		/* For vemExpert() */

/*
 * Forward declarations
 */
static vemStatus schemCreateError();

extern vemStatus schemCreatePath();

vemStatus schemCreateCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* Argument list            */
/*
 * This is the creation command for schematic views.  This command
 * can be used to create new instances, paths, and formal terminals.
 * A syntax summary is given below:
 *	formal-terminal:	"text" (over implementation)
 *	instance:		points (over some other inst)
 *	instance:		points "inst-spec"
 *	path:			lines lines ... 
 */
{
    vemOneArg *arg1;
    octObject facet;
    lsGen lg;
    int len;

    (void)vemExpert( spot );

    seInit();

    len = lsLength(cmdList);
    if (len < 1) return schemCreateError();

    /* Check for editing style differences */
    if (vuCkStyle(spot, cmdList) != VEM_OK) return VEM_ARGRESP;

    /* Ready to try to parse */
    lg = lsStart(cmdList);
    if (lsNext(lg, (lsGeneric *) &arg1, LS_NH) == LS_OK) {
	switch (arg1->argType) {
	case VEM_TEXT_ARG:
	    /* Formal terminal */
	    lsFinish(lg);
	    if (len != 1) return schemCreateError();
	    facet.objectId = spot->theFct;
	    if (octGetById(&facet) != OCT_OK) {
		vemMsg(MSG_A, "The buffer is inaccessible:\n  %s\n",
		       octErrorString());
		return VEM_ARGRESP;
	    }
	    /* Same as symbolic */
	    return seParseFormalTerminal(spot, &facet, (octObject *) 0,
					 arg1->argData.vemTextArg);
	case VEM_POINT_ARG:
	    if (len > 2) {
		lsFinish(lg);
		return schemCreateError();
	    }
	    /* Same as schematic */
	    return seCreateInst(spot, cmdList, "interface");
	case VEM_LINE_ARG:
	    return schemCreatePath(spot, cmdList);
	case VEM_OBJ_ARG:
	case VEM_BOX_ARG:
	    return schemCreateError();
	}
    }
    return VEM_OK;
}



#define MSG(txt)	vemMessage(txt, MSG_NOLOG|MSG_DISP)

static vemStatus schemCreateError()
/*
 * Displays a message about the form of symbolic create command
 */
{
    MSG("Create format:\n");
    MSG("  Formal Terminal:  \"term name\" : create (over implementation)\n");
    MSG("  Instance:         point(s) \"cell:view {name}\" : create\n");
    MSG("                    point(s) : create (over another instance)\n");
    MSG("  Segments:         line(s) : create\n");
    return VEM_ARGRESP;
}
