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

#if defined(PTK)
#include "ptk.h"  /* Interpreter name, window name, etc.  aok */
#include "ptkCalls.h"  /* C utilities for using tcl from pigi.  aok */
#endif

/* The vars below store the state of the last run command for re-runs */
static int lastIterate;
octObject lastFacet = {OCT_UNDEFINED_OBJECT};

static boolean
AskIterate(facetPtr, nPtr)
octObject *facetPtr;
int *nPtr;
{
    static dmTextItem item = {"Number of iterations", 1, 20, NULL, NULL};
    char buf[64];
    int oldN;

    if (GetIterateProp(facetPtr, &oldN) == -1) {
	oldN = 10;
    }
    sprintf(buf, "%d", oldN);
    item.value = buf;
    if (dmMultiText("Iterations", 1, &item) != VEM_OK) {
	    PrintCon("Aborted entry");
	    return (FALSE);
    }
    *nPtr = atoi(item.value);
    if (*nPtr != oldN) {
	/* set prop only if it changes, to avoid creation of OctChangeRecord */
	SetIterateProp(facetPtr, *nPtr);
    }
    lastIterate = *nPtr;
    return (TRUE);
}

#if defined(PTKold)
/* Command used by Tk to get the current number of iterations from
   deep in the heart of the OCT database */
int
GetIterateCmd (dummy, interp, argc, argv)
    ClientData dummy;                   /* Not used. */
    Tcl_Interp *interp;                 /* Current interpreter. */
    int argc;                           /* Number of arguments. */
    char **argv;                        /* Argument strings. */
{
    int oldN;
    char value[64];
    octObject facet;

    if (argc != 2) {
        Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
                " OctObjectHandle\"", (char *) NULL);
        return TCL_ERROR;
    }

    ptkHandle2OctObj(argv[1], &facet);

    if (GetIterateProp(&facet, &oldN) == -1) {
	oldN = 10;
    }

    sprintf(value, "%d", oldN);
    Tcl_SetResult (interp, value, TCL_VOLATILE);
    return TCL_OK;

}
#endif

#if defined(PTKold)
/* Command used by Tk to set the current number of iterations from
   deep in the heart of the OCT database */

int
SetIterateCmd (dummy, interp, argc, argv)
    ClientData dummy;                   /* Not used. */
    Tcl_Interp *interp;                 /* Current interpreter. */
    int argc;                           /* Number of arguments. */
    char **argv;                        /* Argument strings. */
{
    int IterationNumber;
    octObject facet;


    if (argc != 3) {
        Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
                " OctObjectHandle IterationValue \"", (char *) NULL);
        return TCL_ERROR;
    }

    ptkHandle2OctObj(argv[1], &facet);
    Tcl_GetInt(interp, argv[2], &IterationNumber);

    SetIterateProp(&facet, IterationNumber);
    lastIterate = IterationNumber;

    return(TCL_OK);
}
#endif

/* Run a facet through a pre-specified number of iterations */
static boolean
RunN(facetPtr,N)
octObject *facetPtr;
int N;
{
    lastFacet = *facetPtr;
    ERR_IF1(!CompileFacet(facetPtr));
    ERR_IF1(!KcRun(N));
    return (TRUE);
}

static boolean
Run(facetPtr)
octObject *facetPtr;
{
    int n;

    lastFacet = *facetPtr;
    if(!AskIterate(facetPtr, &n)) return (TRUE);
    ERR_IF1(!CompileFacet(facetPtr));
    ERR_IF1(!KcRun(n));
    return (TRUE);
}


#if defined(PTK)

static boolean
ptkRun(facetPtr)
octObject *facetPtr;
{
    int n;
    char facetHandle[64];

    lastFacet = *facetPtr;
    ptkOctObj2Handle( facetPtr, facetHandle );

    Tcl_VarEval(ptkInterp,"runwindow ",facetHandle,(char *)NULL);

    return (TRUE);
}

#endif

int
RunUniverse(name, iterations)
char* name;
int iterations;
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
		RunN(&facet, iterations);
	} else if (editStatus == 2) {
		PrintCon("Aborted entry");
	}
	ViDone();
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
#if defined(PTK)
     ptkRun(facet);
#else
    if (!Run(&facet)) {
	PrintErr(ErrGet());
        ViDone();
    }
#endif

    ViDone();
}

static boolean
ReRun()
{
    ERR_IF1(!CompileFacet(&lastFacet));
    ERR_IF1(!KcRun(lastIterate));
    return (TRUE);
}

/* 12/29/89
*/
int 
RpcReRun(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    ViInit("re-run");
    ErrClear();

    if(! KcSetKBDomain(DEFAULT_DOMAIN)) {
        PrintErr("Failed to set default domain.");
        ViDone();
    }

    if (lastFacet.type == OCT_UNDEFINED_OBJECT
	|| spot->facet != lastFacet.objectId) {
	PrintErr("Cannot re-run this facet, use 'run'");
        ViDone();
    }
    if (!ReRun()) {
	PrintErr(ErrGet());
        ViDone();
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
char *objName;
{
    FindAndMarkError(&lastFacet, objName);
}
