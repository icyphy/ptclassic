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
Version identification:
$Id$

Author: E. A. Lee

This file defines functions that are called from Tcl.
They are registered with Tcl calling registerTclFns().
*/

#include "ptk.h"
#include "local.h"
#include "rpc.h"
#include "exec.h"
#include "ganttIfc.h"
#include "handle.h"
#include "vemInterface.h"
#include "octIfc.h"

/*******************************************************************
 * Command to highlight an object given its name. Call as
 *	ptkHighlight <fullName> [color]
 *******************************************************************/
static int
ptkHighlight(dummy, interp, argc, argv)
    ClientData dummy;                   /* Not used. */
    Tcl_Interp *interp;                 /* Current interpreter. */
    int argc;                           /* Number of arguments. */
    char **argv;                        /* Argument strings. */
{
    char *name;
    static RgbValue color;
    extern char *incr();
    extern boolean FrameStarCall();

    if(argc < 2) {
        strcpy(interp->result,
            "incorrect usage: should be \"ptkHighlight <fullName> [color]\"");
        return TCL_ERROR;
    }
    if (argc > 5) {
	strcpy(interp->result,
	   "extra garbage at end of line removed");
	argc = 5;
    }
    color.red = (argc >= 3) ? (unsigned short)atoi(argv[2]) : 65535;
    color.green = (argc >= 4) ? (unsigned short)atoi(argv[3]) : 0;
    color.blue = (argc == 5) ? (unsigned short)atoi(argv[4]) : 0;
    name = incr(argv[1]);
    if (name == NULL) {
	strcpy(interp->result,"no such star: argv[1]");
	return TCL_ERROR;    
    }
    FrameStarCall(name, &color, 1);
    return TCL_OK;
}

/*******************************************************************
 * Command to clear all highlights
 *	ptkClearHighlights
 *******************************************************************/
static int
ptkClearHighlights(dummy, interp, argc, argv)
    ClientData dummy;                   /* Not used. */
    Tcl_Interp *interp;                 /* Current interpreter. */
    int argc;                           /* Number of arguments. */
    char **argv;                        /* Argument strings. */
{
    FindClear();
    return TCL_OK;
}

/*******************************************************************
 * The global "lastFacet" pointer is used by the highlighting
 * routines to begin the search for the object to be highlighted.
 * This routine sets that pointer, given a handle for the oct object.
 * Usage:
 *	ptkSetHighlightFacet <octHandle>
 *******************************************************************/
extern octObject lastFacet;

static int
ptkSetHighlightFacet(dummy, interp, argc, argv)
    ClientData dummy;                   /* Not used. */
    Tcl_Interp *interp;                 /* Current interpreter. */
    int argc;                           /* Number of arguments. */
    char **argv;                        /* Argument strings. */
{
    if(argc != 2) {
        strcpy(interp->result,
            "incorrect usage: should be \"ptkSetHighlightFacet <octHandle>\"");
        return TCL_ERROR;
    }
    FreeOctMembers(&lastFacet);
    if (ptkHandle2OctObj(argv[1], &lastFacet)) return TCL_OK;
    else {
	strcpy(interp->result,
	    "Failed to find oct object corresponding to handle");
	return TCL_ERROR;
    }
}

/*******************************************************************
 * Register the Tcl commands
 *******************************************************************/

void registerTclFns(interp)
    Tcl_Interp *interp;
{
    Tcl_CreateCommand(interp, "ptkHighlight", ptkHighlight, 0,
            (void (*)()) NULL);
    Tcl_CreateCommand(interp, "ptkClearHighlights", ptkClearHighlights, 0,
            (void (*)()) NULL);
    Tcl_CreateCommand(interp, "ptkSetHighlightFacet", ptkSetHighlightFacet, 0,
            (void (*)()) NULL);
}
