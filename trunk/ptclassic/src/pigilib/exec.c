/* 
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY
*/
/* exec.c  edg
Version identification:
$Id$
*/


/* Includes */
#include "local.h"
#include <stdio.h>
#include <strings.h>
#include "rpc.h"
#include "vemInterface.h"
#include "compile.h"
#include "octIfc.h"
#include "oh.h"
#include "edit.h"
#include "err.h"
#include "util.h"
#include "ganttIfc.h"

#include "ptk.h"  /* Interpreter name, window name, etc.  aok */
#include "handle.h"

/* The vars below store the state of the last run command for re-runs */
static int lastIterate;
octObject lastFacet = {OCT_UNDEFINED_OBJECT};

/* Run a universe given only the full name of the facet.
   The number of iterations is gotten from the oct facet.  */
int
RunUniverse(name)
char* name;
{
	octObject facet;
	int editStatus;

	ViInit(name);
	ErrClear();

	if( ohOpenFacet(&facet, name, "schematic", "contents", "r") <= 0) {
		PrintErr(octErrorString());
		ViDone();
	}

	if (!IsUnivFacet(&facet)) {
		PrintErr("Schematic is not a universe");
		ViDone();
	}
	if(! KcSetKBDomain(DEFAULT_DOMAIN)) {
		PrintErr("Failed to set default domain.");
		ViDone();
	}

	if (!(editStatus = EditFormalParams(&facet))) {
	    PrintErr(ErrGet());
            ViDone();
        }

	if(editStatus == 1) {
		ptkRun(&facet,TRUE);
	} else if (editStatus == 2) {
		PrintCon("Aborted entry");
	}
	ViDone();
}

/*
 * Basic interface to ptcl.
 * If now == TRUE, the universe is run right away.
 * Otherwise, we wait for the user to press the GO button.
 */
static boolean
ptkRun(facetPtr,now)
octObject *facetPtr;
boolean now;
{
    int n;
    char* name;
    char octHandle[64];

    lastFacet = *facetPtr;
    ptkOctObj2Handle(facetPtr,octHandle);

    name = BaseName(facetPtr->contents.facet.cell);

    TCL_CATCH_ERR1(
	Tcl_VarEval(ptkInterp,"ptkRunControl ", name, " ", octHandle,
	(char *)NULL));

    if (now) {
	/* Run the universe right away */
        TCL_CATCH_ERR1(
	    Tcl_VarEval(ptkInterp,"ptkGo ", name, " ", octHandle,
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
    octObject facet;

    ViInit("run");
    ErrClear();
    FindClear();
    /* get current facet */
    facet.objectId = spot->facet;
    if (octGetById(&facet) != OCT_OK) {
	PrintErr(octErrorString());
    	ViDone();
    }
    if (!IsUnivFacet(&facet)) {
	PrintErr("Schematic is not a universe");
        ViDone();
    }

    if(! KcSetKBDomain(DEFAULT_DOMAIN)) {
        PrintErr("Failed to set default domain.");
        ViDone();
    }
    ptkRun(facet,FALSE);
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
char *objName;
{
    FindAndMarkError(&lastFacet, objName);
}
