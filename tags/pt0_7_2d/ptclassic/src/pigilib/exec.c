/* 
Copyright (c) 1990-1997 The Regents of the University of California.
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

/* exec.c  edg
Version identification:
@(#)exec.c	1.43	02/04/97
*/

/* Standard includes */
#include "local.h"		/* include compat.h, sol2compat.h, ansi.h */
#include <stdio.h>
#include <string.h>

/* Octtools includes */
#include "oct.h"		/* define octObject data & octXXX functions */
#include "list.h"		/* define lsList */
#include "rpc.h"		/* define RPC data structures */
#include "oh.h"

/* Pigilib includes */
#include "exec.h"
#include "vemInterface.h"
#include "compile.h"
#include "octIfc.h"
#include "edit.h"
#include "err.h"
#include "util.h"
#include "ganttIfc.h"
#include "kernelCalls.h"	/* define KcDisplaySchedule */

#include "ptk.h"		/* Interpreter name, window name, etc.  aok */
#include "handle.h"

/* The vars below store the state of the last run command for re-runs */
octObject lastFacet = {OCT_UNDEFINED_OBJECT, 0};

/* Run a universe given only the full name of the facet.
   The number of iterations is gotten from the oct facet.  */
int
RunUniverse(name)
char* name;
{
	octObject facet = {OCT_UNDEFINED_OBJECT, 0};
	char facetHandle[POCT_FACET_HANDLE_LEN];

	ViInit(name);
	ErrClear();

	if( ohOpenFacet(&facet, name, "schematic", "contents", "r") <= 0) {
		PrintErr(octErrorString());
	}
	else if (!IsUnivFacet(&facet)) {
		PrintErr("Schematic is not a universe");
	}
	else {
	    /* Create a Tk window to handle editing and then do run command */
	    ptkOctObj2Handle(&facet, facetHandle);
	    TCL_CATCH_ERR( Tcl_VarEval(ptkInterp, "ptkEditStrings ",
	           " \"Run universe with the following parameters\" ",
	           " \"ptkSetRunUniverse ", facetHandle, " {%s} \" ",
	           " \"[lindex [ptkGetParams ", facetHandle, " NIL] 1]\" ",
	           " Params ",
	           (char *)NULL) );
	}

	ViDone();
}

/*
 * Basic interface to ptcl.
 * If now == TRUE, the universe is run right away.
 * Otherwise, we wait for the user to press the GO button.
 */
/* used externally by SetRunUniverse */
boolean
ptkRun(facetPtr,now)
octObject *facetPtr;
boolean now;
{
    char* name;
    char octHandle[POCT_FACET_HANDLE_LEN];
    char buf[512];

    /* Reset lastFacet to the current facet */
    if ( lastFacet.objectId != facetPtr->objectId ) FreeOctMembers(&lastFacet);
    lastFacet = *facetPtr;
    ptkOctObj2Handle(facetPtr, octHandle);
    name = BaseName(facetPtr->contents.facet.cell);

    /* It would be nice if we could just fix this for the user by
     * copying the facet name and chopping off the trailing /, but
     * this would result in yet another memory leak, since we would
     * never know when to delete the copy.  An alternative would be
     * to just change the value of the facet,  and chop of the trailing /
     * there, but this seems a little risky. Instead we check here 
     * and in the pigi startup script.
     */
    if (strcmp(name,"") == 0) {
        sprintf(buf, "Can't call ptkRunControl, the facet %s had a trailing /",
		facetPtr->contents.facet.cell);
	PrintErr(buf);
	return FALSE;
    }

    TCL_CATCH_ERR1(
	Tcl_VarEval(ptkInterp, "ptkRunControl ", name, " ", octHandle,
	(char *)NULL));

    if (now) {				/* Run the universe right away */
	TCL_CATCH_ERR1(
	    Tcl_VarEval(ptkInterp, "ptkGo ", name, " ", octHandle,
	    (char *)NULL));
    }

    return (TRUE);
}

/* 11/7/89 4/24/88
*/
int 
RpcRun(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    octObject facet = {OCT_UNDEFINED_OBJECT, 0};

    ViInit("open run window");
    ErrClear();
    FindClear();

    /* get current facet */
    facet.objectId = spot->facet;
    if (octGetById(&facet) != OCT_OK) {
	PrintErr(octErrorString());
    }
    else if (!IsUnivFacet(&facet)) {
	PrintErr("Schematic is not a universe");
	FreeOctMembers(&facet);
    }
    else {
	/* Do not free facet: it will be saved as lastFacet */
	ptkRun(&facet, FALSE);
    }

    ViDone();
}

int 
RpcDisplaySchedule(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    ViInit("display-schedule");
    ErrClear();
    FindClear();
    KcDisplaySchedule();
    ViDone();
}

/* mark stars involved in errors detected by kernel */
void
PigiErrorMark(objName)
const char* objName;
{
    FindAndMark(&lastFacet, objName, 0);
}

/* mark stars with a pattern given the full name */
void
PigiMark(objName)
const char* objName;
{
    FindAndMark(&lastFacet, objName, 1);
}

/* mark stars with a pattern given the full name and the color to mark */
void
PigiMarkColor(objName,color)
const char* objName;
const char* color;
{
    FindAndMarkColor(&lastFacet, objName, 1, color);
}
