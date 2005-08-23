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
/* misc.c  edg
Version identification:
@(#)misc.c	2.44	11/29/96
*/

/*
        Added ShowFacetNum to show the "handle" of the current facet.
        this function is for debugging of the tcl/tk interface.
        - aok  2/22/93
*/

/* Standard includes */
#include "local.h"		/* include compat.h, sol2compat.h, ansi.h */
#include <stdio.h>
#include <string.h>

/* Octtools includes */
#include "oct.h"		/* define octObject */
#include "list.h"		/* define lsList */
#include "rpc.h"		/* define remote procedure calls */
#include "utility.h"		/* define util_csystem */

/* Pigilib includes */
#include "misc.h"
#include "vemInterface.h"
#include "util.h"
#include "octIfc.h"
#include "err.h"
#include "main.h"
#include "compile.h"
#include "kernelCalls.h"
#include "icon.h"
#include "xfunctions.h"

/* for ShowFacetNum */
#include "ptk.h"
#include "handle.h"

#define dmWidth 40

/* uses Tycho to display the man page for the current star. */
int 
Man(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    octObject mFacet = {OCT_UNDEFINED_OBJECT, 0},
	      inst = {OCT_UNDEFINED_OBJECT, 0},
	      facet = {OCT_UNDEFINED_OBJECT, 0};
    vemStatus status;
    char *fullName, codeFile[512], domain[64];

    ViInit("Man");
    ErrClear();

    /* get current facet */
    facet.objectId = spot->facet;
    if (octGetById(&facet) != OCT_OK) {
        PrintErr(octErrorString());
        ViDone();
    }
    status = vuFindSpot(spot, &inst, OCT_INSTANCE_MASK);
    if (status == VEM_NOSELECT) {
        PrintCon("Aborted");
        ViDone();
    } else if (status != VEM_OK) {
        PrintErr("Cursor must be over an icon instance");
        ViDone();
    } else {
        if (IsGal(&inst) || IsUniv(&inst) || IsPal(&inst)) {
            PrintErr("The Man command is currently only supported for Stars.");
            ViDone();
        } else {
            if (IsStar(&inst)) {
                if (!MyOpenMaster(&mFacet, &inst, "interface", "r")) {
                    PrintErr(ErrGet());
                    ViDone();
                }
                octFullName(&mFacet, &fullName);

                /* Figure out source file name */
                if (!IconFileToSourceFile(fullName, codeFile, domain)) {
                    PrintErr(ErrGet());
                    ViDone();
                }

                TCL_CATCH_ERR( Tcl_VarEval (ptkInterp,
                        "::tycho::EditPtlang::gendoc ",
                        codeFile, (char*) NULL) );
            } else {
                PrintErr("The Man command is currently only supported for Stars.");
                ViDone();
            }
        }
    }
    ViDone();
}


/* ShowFacetNum calls a tcl function showfacetnum.  this function
   is redefinable by the user.
   This is a helping function for the development of the TCL/TK interface
   to Ptolemy - aok */
int
ShowFacetNum (spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    octObject facet = {OCT_UNDEFINED_OBJECT, 0},
	      inst = {OCT_UNDEFINED_OBJECT, 0};
    char FacetName[POCT_FACET_HANDLE_LEN],
	 InstanceName[POCT_FACET_HANDLE_LEN];
    vemStatus status;

    ViInit("show facet number");
    ErrClear();

    /* Get the facet based on the "spot" */
    facet.objectId = spot->facet;
    if (octGetById(&facet) != OCT_OK) {
        PrintErr(octErrorString());
        ViDone();
    }

    ptkOctObj2Handle ( &facet, FacetName );

    /* Get the instance under the cursor */
    status = vuFindSpot(spot, &inst, OCT_INSTANCE_MASK);
    if (status == VEM_NOSELECT) {
        PrintCon("Aborted");
        ViDone();
    } else if (status != VEM_OK) {
        /* cursor not over an instance... */
        strcpy(InstanceName, "NIL");
    } else {
        /* cursor is over some type of instance... */
	ptkOctObj2Handle ( &inst, InstanceName );
    }

    Tcl_VarEval( ptkInterp, "info commands ptkShowFacetNum", (char *) NULL);

    /* Create a ptkShowFacetNum if it doesn't exist. */
    if ( strlen(ptkInterp->result) == 0 ) {
        Tcl_VarEval( ptkInterp, "proc ptkShowFacetNum { facet instance }",
        " { puts  \"facet, instance : $facet $instance\"  } ", (char *) NULL );
    }
    
    Tcl_VarEval( ptkInterp, "ptkShowFacetNum ", FacetName, " ", 
                 InstanceName, (char *) NULL);

    ViDone();

}




/***********************************************************************
PrintFacet(): calls "ptkPrintFacet" utility to save PostScript to printer
or file.
***********************************************************************/
int
PrintFacet(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
	octObject facet = {OCT_UNDEFINED_OBJECT, 0};
	char *fullName;

	ViInit("print facet");
	ErrClear();

	/* Get facet information. */
	facet.objectId = spot->facet;
	if (octGetById(&facet) != OCT_OK) {
		PrintErr(octErrorString());
	}
	else {
		octFullName(&facet, &fullName);
		TCL_CATCH_ERR( Tcl_VarEval( ptkInterp, "ptkPrintFacet ",
					    fullName, (char *) NULL) );
	}

	ViDone();
}


int
ERFilterDesign(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    char buf[512];

    ViInit("equirriple FIR design");
    ErrClear();
    sprintf(buf, "xterm -display %s -name Equirriple_FIR_design -e optfir &",
            xDisplay);
    PrintDebug(buf);
    if (util_csystem(buf)) {
        strcpy(buf, "Error invoking optfir program.");
	PrintErr(buf);
    }
    ViDone();
}

int
WFilterDesign(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    char buf[512];

    ViInit("window FIR design");
    ErrClear();
    sprintf(buf, "xterm -display %s -name Equirriple_FIR_design -e wfir &",
            xDisplay);
    PrintDebug(buf);
    if (util_csystem(buf)) {
        PrintErr("Error invoking wfir program.");
    }
    ViDone();
}


int 
Profile(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    static dmTextItem item = {"Star or Target name", 1, 40, NULL, NULL};
    octObject facet = {OCT_UNDEFINED_OBJECT, 0},
	      inst = {OCT_UNDEFINED_OBJECT, 0};
    vemStatus status;

    ViInit("profile");
    ErrClear();
    /* get current facet */
    facet.objectId = spot->facet;
    if (octGetById(&facet) != OCT_OK) {
	PrintErr(octErrorString());
    	ViDone();
    }
    /* get name of instance under cursor */
    status = vuFindSpot(spot, &inst, OCT_INSTANCE_MASK);
    if (status == VEM_NOSELECT) {
	PrintCon("Aborted");
    } else if (status != VEM_OK) {
	/* No inst under cursor */
	if (dmMultiText("Profile", 1, &item) != VEM_OK) {
	    PrintCon("Aborted entry");
	}
	else if (!KcProfile(item.value)) {
	    PrintErr(ErrGet());
	}
    } else if (IsStar(&inst)) {
	if(setCurDomainInst(&inst) == NULL) {
	    PrintErr("Domain error in instance.");
	}
	    /* autoload the star if necessary */
	else if (!AutoLoadCk(&inst) ||
	    !KcProfile(AkoName(inst.contents.instance.master))) {
	    PrintErr(ErrGet());
	}
    } else if (IsGal(&inst)) {
	if (!CompileGalInst(&inst,&facet) ||
	    !KcProfile(AkoName(inst.contents.instance.master))) {
	    PrintErr(ErrGet());
	}
    } else {
	/* other kinds of objects, try to display them all */
	char propval[80];
	propval[0] = '?'; propval[1] = 0;
	clr_accum_string();
	if (IsDelay(&inst)) {
	    accum_string ("Delay: value = '");
	    GetStringizedProp(&inst, "delay", propval, 80);
	    accum_string (propval);
	    accum_string ("'");
	}
	else if (IsBus(&inst)) {
	    accum_string ("Bus Marker: width = '");
	    GetStringizedProp(&inst, "buswidth", propval, 80);
	    accum_string (propval);
	    accum_string ("'");
	}
	else if (IsIoPort(&inst)) {
	    accum_string ("Galaxy ");
	    accum_string (IsInputPort(&inst) ? "In" : "Out");
	    accum_string ("put port");
	}
	else if (IsUniv(&inst)) {
	    accum_string ("Universe instance\n");
	}
	else if (IsPal(&inst)) {
	    accum_string ("Palette instance\n");
	}
	else {
	    accum_string ("Unknown instance type\n");
	}
	pr_accum_string();
    }
    ViDone();
}

/* 6/27/89 = tries to open facet read-only first */
/* 10/21/93  Now uses Wei-Jen's nifty Tk code */
int 
RpcOpenFacet(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    ViInit("open-facet");
    ErrClear();
    TCL_CATCH_ERR( 
	Tcl_VarEval( ptkInterp, "::tycho::Oct::openFacet", (char *) NULL) );
    ViDone();
}

/* Load the star pointed to */
static int 
DoRpcLoadStar(spot, cmdList, permB) /* ARGSUSED */
RPCSpot	*spot;
lsList	cmdList;
int	permB;
{
    int		linkCnt = 0;
    char	linkArgs[1024];
    octObject	inst = {OCT_UNDEFINED_OBJECT, 0};
    vemStatus	status;
    RPCArg	*theArg;

    ViInit("load-star");

    /* see if any arguments were given */
    linkArgs[0] = '\0';
    while ( lsDelBegin(cmdList, (lsGeneric *) &theArg) == VEM_OK ) {
	switch ( theArg->argType ) {
	case VEM_TEXT_ARG:
	    strcpy( linkArgs, theArg->argData.string);
	    strcat( linkArgs, " ");
	    ++linkCnt;
	    break;
	case VEM_OBJ_ARG:
	    PrintErr("Instance args not yet implemented");
	    ViDone();
	    break;
	default:
	    PrintErr("Only strings and instances allowed as link arguments");
	    ViDone();
	}
    }

    /* set the current domain */
    if (!setCurDomainS(spot)) {
        PrintErr("Invalid domain found");
	ViDone();
    }
    /* get name of instance under cursor */
    status = vuFindSpot(spot, &inst, OCT_INSTANCE_MASK);
    if (status == VEM_NOSELECT) {
	PrintCon("Aborted");
    } else if (status != VEM_OK) {
	PrintErr("Cursor must be over an icon instance");
    } else if (!IsStar(&inst)) {
	PrintErr("Instance is not a star");
    } else {
	if (!LoadTheStar(&inst, permB, linkArgs))
		PrintErr(ErrGet());
    }
    ViDone();
}


/* Load the star pointed to */
int 
RpcLoadStar(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    return DoRpcLoadStar(spot, cmdList, FALSE);
}

int 
RpcLoadStarPerm(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    return DoRpcLoadStar(spot, cmdList, TRUE);
}
