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
/* Automatic Generation of Interfaces
 *
 * David Harrison
 * University of California,  Berkeley
 *
 * This file provides an interface to the _vulcan_ protection frame
 * generator.
 */

#include "internal.h"
#include "oct.h"
#include "vulcan.h"
#include "oh.h"
#include "gen.h"

static int defGen();

static symGenFunc genFunc = { defGen, (char *) 0 };

void symSetGenFunc(new, old)
symGenFunc *new;		/* New interface generation function */
symGenFunc *old;		/* Old interface generation function */
/*
 * This routine set the interface generation function for the symbolic
 * policy library.  The function is called when the package requires
 * an interface for a cell and none exists.  See symGenFunc for the
 * form.  `inFacet' is the contents facet used as input.  `outFacet'
 * is the facet where the interface should be written.  Both facets
 * should already be open.  `data' is the same as that provided in `new'.
 * The routine should return zero if it is unsuccessful.
 * If `old' is non-zero, it is filled with the old function so that
 * it can be restored at a later time.  The default interface generation
 * function uses vulcan(1) to produce an interface.
 */
{
    if (old) {
	*old = genFunc;
    }
    genFunc = *new;
}



static void vulMsgHandler(msg_type, message)
vulcanMessageType msg_type;	/* What kind of message */
char *message;			/* Message itself       */
/*
 * This routine is passed to vulcan as a message handler
 * before calling the protection frame generator.  It
 * is used to display vulcan messages to the console window.
 */
{
    switch (msg_type) {
    case VULCAN_INFORMATION:
    case VULCAN_DEBUGGING:
	symMsg(SYM_INFO, "vulcan: %s\n", message);
	break;
    case VULCAN_PARTIAL:
    case VULCAN_WARNING:
	symMsg(SYM_WARN, "vulcan: %s\n", message);
	break;
    case VULCAN_SEVERE:
	symMsg(SYM_ERR, "vulcan: %s\n", message);
	break;
    case VULCAN_FATAL:
	/* For completeness */
	symMsg(SYM_ERR, "vulcan: (fatal): %s\n", message);
	break;
	
    }
}



/*ARGSUSED*/
static int defGen(in, out, data)
octObject *in;			/* Input facet  */
octObject *out;			/* Output facet */
char *data;			/* User data    */
/*
 * This is the default interface generation function.  It
 * uses vulcan to produce an interface.
 */
{
    vulcanMessageHandler(vulMsgHandler);

    if (vulcan(in, out) != VULCAN_INFORMATION) {
	return 0;
    } else {
	return 1;
    }
}



void symInterface(inst, interface)
octObject *inst;		/* Instance            */
octObject *interface;		/* Resulting interface */
/*
 * This routine returns the interface facet of the cell specified
 * in `inst'.  If no interface is found,  the routine will
 * attempt to generate one.  If that is not successful,  the
 * routine will raise a fatal exception error.
 */
{
    octObject confacet;

    interface->type = OCT_FACET;
    interface->contents.facet.facet = "interface";
    interface->contents.facet.mode = "r";
    interface->contents.facet.version = OCT_CURRENT_VERSION;
    
    if (octOpenMaster(inst, interface) < OCT_OK) {
	symMsg(SYM_INFO, "instance `%s' has no interface:\n  %s\n\
Attemping to make one\n", ohFormatName(inst), octErrorString());

	/* Open both contents for examination */
	confacet = *interface;
	confacet.contents.facet.facet = "contents";
	if (octOpenMaster(inst, &confacet) < OCT_OK) {
	    errRaise(sym_pkg_name, SYM_NOMASTER,
		     "No master for `%s':\n  %s",
		     ohFormatName(inst), octErrorString());
	    /*NOTREACHED*/
	}
	/* Open interface for writing */
	*interface = confacet;
	interface->contents.facet.facet = "interface";
	interface->contents.facet.mode = "w";
	if (octOpenRelative(&confacet, interface, OCT_SIBLING) < OCT_OK) {
	    errRaise(sym_pkg_name, SYM_PERM,
		     "Cannot open interface of `%s':\n  %s",
		     ohFormatName(inst), octErrorString());
	    /*NOTREACHED*/
	}
	if (!(*genFunc.gen_func)(&confacet, interface, genFunc.data)) {
	    errRaise(sym_pkg_name, SYM_NOINTR,
		     "Could not create interface for `%s'",
		     ohFormatName(inst));
	    /*NOTREACHED*/
	}
    }
}
